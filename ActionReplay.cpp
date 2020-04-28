#include  <afxwin.h>

#include  "Types.h"
#include  "VGA.h"
#include  "Resource.h"
#include  "Plateform.h"
#include  "ActionReplay.h"


#define IDT_TIMER   1

#define LIGNE_MEM   20

#define MEM_SIZE    0x20000

#define MAX_ITEMS   4000


static UBYTE CopyMem[ MEM_SIZE ];
static UBYTE SeekMem[ MEM_SIZE ];
static UBYTE MoveMem[ MEM_SIZE ];
static UBYTE NotMatch[ MEM_SIZE ];



int CLightListBox::VKeyToItem( UINT nKey, UINT nIndex )
{
    switch( nKey )
        {
        case VK_PRIOR :
            parent->ListPgUp( nIndex );
            break;

        case VK_NEXT :
            parent->ListPgDown( nIndex );
            break;

        case VK_UP :
            if ( nIndex > 0 )
                SetCurSel( nIndex - 1 );
            else
                parent->ListUp( nIndex );
            break;

        case VK_DOWN :
            if ( nIndex < LIGNE_MEM - 1 )
                SetCurSel( nIndex + 1 );
            else
                parent->ListDown( nIndex );
            break;
        }
    return -2;
}


void StartActionReplay( void )
{
    CActionReplay a;
    a.DoModal();
}


CActionReplay::CActionReplay(CWnd* pParent /*=NULL*/) : CDialog(CActionReplay::IDD, pParent)
{
}


void CActionReplay::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LISTE_MEM, m_ListeMem);
    DDX_Control(pDX, IDC_SCROLL, m_ScrollMem);
    DDX_Control(pDX, IDC_MEM_BANK, m_MemBank);
    DDX_Control(pDX, IDC_VAL_INF, m_BpValInf);
    DDX_Control(pDX, IDC_VAL_SUP, m_BpValSup);
    DDX_Control(pDX, IDC_VAL_EQU, m_BpValEqu);
    DDX_Control(pDX, IDC_VAL_DIF, m_BpValDif);
    DDX_Control(pDX, IDC_LIST_RESULT, m_ListResult);
    DDX_Control(pDX, IDC_NB_ITEMS, m_NbItems);
}


BOOL CActionReplay::OnInitDialog( void )
{
    CDialog::OnInitDialog();
    font8.CreatePointFont( 85, "Courier New", this->GetDC() );
    m_ListeMem.SetParent( this );
    m_ListeMem.SetFont( &font8 );
    m_ListResult.SetFont( &font8 );
    m_ScrollMem.SetScrollRange( 0, 0x10000 - LIGNE_MEM );
    memOffset = 0;
    InitMem();
    m_BpValInf.SetCheck( 1 );
    SetTimer( IDT_TIMER, 50, NULL );
    m_NbItems.SetWindowTextA( "" );
    return( TRUE );
}


BEGIN_MESSAGE_MAP(CActionReplay, CDialog)
    ON_WM_VSCROLL()
    ON_WM_TIMER()
    ON_BN_CLICKED(IDC_ACTION, OnBnClickedAction)
    ON_BN_CLICKED(IDC_MEM_BANK, OnBnClickedMemBank)
    ON_BN_CLICKED(IDC_BP_CHERCHE, OnBnClickedBpCherche)
    ON_BN_CLICKED(IDC_BP_POKE, OnBnClickedBpPoke)
END_MESSAGE_MAP()


void CActionReplay::OnTimer( UINT TimerVal )
{
    BOOL redraw = FALSE;
    for ( int i = 0; i < LIGNE_MEM; i++ )
        {
        int adr = i + memOffset + ( m_MemBank.GetCheck() ? 0x10000 : 0 );
        if ( MoveMem[ adr ] != MemCPC[ adr ] )
            {
            MoveMem[ adr ] = MemCPC[ adr ];
            redraw = TRUE;
            }
        }
    if ( redraw )
        RedrawListe( m_ListeMem.GetCurSel() );
}

void CActionReplay::InitMem( void )
{
    memset( NotMatch, 0, sizeof( NotMatch ) );
    memcpy( CopyMem, MemCPC, MEM_SIZE );
    RedrawListe( -1 );
}


char * CActionReplay::CreateMemData( int adr, BYTE * Data )
{
    static char * Bank[ 8 ] = { "C0", "C0", "C0", "C0", "C4", "C5", "C6", "C7" };
    static char s[ 32 ];

    sprintf( s
           , "%s %04X %02X %02X"
           , Bank[ adr >> 14 ]
           , adr < 0x10000 ? adr : ( ( adr & 0x3FFF ) | 0x4000 )
           , CopyMem[ adr ]
           , Data[ adr ]
           );
    return( s );
}


void CActionReplay::RedrawListe( int sel )
{
    m_ListeMem.ResetContent();
    for ( int i = 0; i < LIGNE_MEM; i++ )
        m_ListeMem.AddString( CreateMemData( i + memOffset + ( m_MemBank.GetCheck() ? 0x10000 : 0 ), MemCPC ) );

    m_ListeMem.SetCurSel( sel );
}


void CActionReplay::ListUp( int index )
{
    if ( memOffset > 0 )
        memOffset--;
    m_ScrollMem.SetScrollPos( memOffset );
    RedrawListe( index );
}


void CActionReplay::ListDown( int index )
{
    if ( memOffset < 0x10000 - LIGNE_MEM )
        memOffset++;
    m_ScrollMem.SetScrollPos( memOffset );
    RedrawListe( index );
}


void CActionReplay::ListPgUp( int index )
{
    memOffset -= LIGNE_MEM;
    if ( memOffset < 0 )
        memOffset = 0;
    m_ScrollMem.SetScrollPos( memOffset );
    RedrawListe( index );
}


void CActionReplay::ListPgDown( int index )
{
    memOffset += LIGNE_MEM;
    if ( memOffset > 0x10000 - LIGNE_MEM )
        memOffset = 0x10000 - LIGNE_MEM;
    m_ScrollMem.SetScrollPos( memOffset );
    RedrawListe( index );
}


void CActionReplay::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar * pScrollBar )
{
    if ( pScrollBar == &m_ScrollMem )
        {
        switch( nSBCode )
            {
            case SB_TOP :
                memOffset = 0;
                break;

            case SB_BOTTOM :
                memOffset = 0x10000 - LIGNE_MEM;
                break;

            case SB_LINEUP :
                if ( memOffset > 0 )
                    memOffset--;
                break;

            case SB_LINEDOWN :
                if ( memOffset < 0x10000 - LIGNE_MEM )
                    memOffset++;
                break;

            case SB_PAGEUP :
                memOffset -= LIGNE_MEM;
                if ( memOffset < 0 )
                    memOffset = 0;
                break;

            case SB_PAGEDOWN :
                memOffset += LIGNE_MEM;
                if ( memOffset > 0x10000 - LIGNE_MEM )
                    memOffset = 0x10000 - LIGNE_MEM;
                break;

            case SB_THUMBPOSITION :
            case SB_THUMBTRACK :
                memOffset = nPos & 0xFFFF;
                break;
            }
        pScrollBar->SetScrollPos( memOffset );
        RedrawListe( -1 );
        }
    CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}


void CActionReplay::OnBnClickedAction( void )
{
    memOffset = 0;
    m_ScrollMem.SetScrollPos( 0 );
    m_ListResult.ResetContent();
    InitMem();
}


void CActionReplay::OnBnClickedMemBank()
{
    RedrawListe( -1 );
}


void CActionReplay::OnCancel( void )
{
    KillTimer( IDT_TIMER );
    CDialog::OnCancel();
}

void CActionReplay::OnBnClickedBpCherche( void )
{
    int cnt = 0, MemoPause = Pause;

    Pause = 1;
    memcpy( SeekMem, MemCPC, MEM_SIZE );
    m_ListResult.ResetContent();
    for ( int i = 0; i < MEM_SIZE; i++ )
        {
        if ( (  ( SeekMem[ i ] < CopyMem[ i ] && m_BpValInf.GetCheck() )
             || ( SeekMem[ i ] > CopyMem[ i ] && m_BpValSup.GetCheck() )
             || ( SeekMem[ i ] == CopyMem[ i ] && m_BpValEqu.GetCheck() )
             || ( SeekMem[ i ] != CopyMem[ i ] && m_BpValDif.GetCheck() )
             )
             && ! NotMatch[ i ]
           )
            {
            m_ListResult.AddString( CreateMemData( i, SeekMem ) );
            if (cnt++ > MAX_ITEMS )
                break;
            }
        else
            NotMatch[ i ] = 1;

        CopyMem[ i ] = SeekMem[ i ];
        }
    if ( cnt < MAX_ITEMS )
        sprintf( tmp, "Nombre d'éléments : %d", cnt );
    else
        strcpy( tmp, "Trops d'éléments !!!" );

    m_NbItems.SetWindowTextA( tmp );
    Pause = MemoPause;
}


void CActionReplay::OnBnClickedBpPoke( void )
{
    int i = m_ListResult.GetCurSel();
    if ( i > -1 )
        {
        m_ListResult.GetText( i, tmp );
        }
}
