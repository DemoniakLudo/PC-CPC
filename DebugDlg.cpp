#include  <afxwin.h>
#include  <afxdlgs.h>

#include  "Types.h"
#include  "Resource.h"
#include  "SnapShot.h"
#include  "Plateform.h"
#include  "DebugDlg.h"
#include  "CRTC.h"
#include  "VGA.h"
#include  "Z80.h"
#include  "AY8912.h"
#include  "Config.h"
#include  "Desasm.h"
#include  "BreakPoints.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define     LIGNE_ASM       34
#define     LIGNE_STACK     13
#define     LIGNE_DUMP      16
#define     PAGE_DUMP       16 * 16

#define     ASM_SIZE        0x10000 - LIGNE_ASM
#define     STACK_SIZE      0x10000 - LIGNE_STACK
#define     DUMP_SIZE       0x10000 - PAGE_DUMP

#define     TAILLE_REDUIT   640
#define     TAILLE_EXPAND   842

extern void CycleZ80( UBYTE *, BOOL Debug );


extern int CptInstr;

static char Tmp[ 256 ];


void ShowDebug( HWND hWnd )
{
    SoundEnable( FALSE );
    DebugDlg d;
    DebugMode = 0;
    d.DoModal();
    BringWindowToTop( hWnd );
    SoundEnable( SndEnabled );
}


static char * GetFlags( void )
{
    static char SetFlags[ 9 ] = "SZ1H1VNC";

    for ( int i = 0; i < 8; i++ )
        Tmp[ i ] = FLAGS & (128 >> i ) ? SetFlags[ i ] : '.';

    Tmp[ 8 ] = 0;
    return( Tmp );
}


void CMyListBox::DrawItem( LPDRAWITEMSTRUCT lpDIS )
{
    memset( Text, 0, maxLen );
    if ( lpDIS->itemID != ( UINT )-1 )
        {
        int Val = ( int )GetItemData( lpDIS->itemID );
        SetBkColor( lpDIS->hDC, Val & ITEM_PC ? 0x800000 : Val & ITEM_BREAK ? 0x0000FF : 0xFFFFFF );
        SetTextColor( lpDIS->hDC, Val & ITEM_BREAK ? 0xFFFF00 : Val & ITEM_PC ? 0xFFFFFF : 0x000000 );
        GetText( lpDIS->itemID, Text );
        TextOut( lpDIS->hDC, lpDIS->rcItem.left + 2, lpDIS->rcItem.top, Text, maxLen );
        }
}


DebugDlg::DebugDlg(CWnd* pParent /*=NULL*/) : CDialog(DebugDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(DebugDlg)
    //}}AFX_DATA_INIT
    IsLoaded = IsValidated = FALSE;
    AsmOffset = RegPC;
    StackOffset = RegSP;
    DumpOffset = 0;
}


void DebugDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_CRTC_INDEX, m_Crtc_Index);
    DDX_Control(pDX, IDC_CRTC_REG00, m_Crtc_Reg00);
    DDX_Control(pDX, IDC_CRTC_REG01, m_Crtc_Reg01);
    DDX_Control(pDX, IDC_CRTC_REG02, m_Crtc_Reg02);
    DDX_Control(pDX, IDC_CRTC_REG03, m_Crtc_Reg03);
    DDX_Control(pDX, IDC_CRTC_REG04, m_Crtc_Reg04);
    DDX_Control(pDX, IDC_CRTC_REG05, m_Crtc_Reg05);
    DDX_Control(pDX, IDC_CRTC_REG06, m_Crtc_Reg06);
    DDX_Control(pDX, IDC_CRTC_REG07, m_Crtc_Reg07);
    DDX_Control(pDX, IDC_CRTC_REG08, m_Crtc_Reg08);
    DDX_Control(pDX, IDC_CRTC_REG09, m_Crtc_Reg09);
    DDX_Control(pDX, IDC_CRTC_REG10, m_Crtc_Reg10);
    DDX_Control(pDX, IDC_CRTC_REG11, m_Crtc_Reg11);
    DDX_Control(pDX, IDC_CRTC_REG12, m_Crtc_Reg12);
    DDX_Control(pDX, IDC_CRTC_REG13, m_Crtc_Reg13);
    DDX_Control(pDX, IDC_CRTC_REG14, m_Crtc_Reg14);
    DDX_Control(pDX, IDC_CRTC_REG15, m_Crtc_Reg15);
    DDX_Control(pDX, IDC_CRTC_REG16, m_Crtc_Reg16);
    DDX_Control(pDX, IDC_CRTC_REG17, m_Crtc_Reg17);
    DDX_Control(pDX, IDC_CRTC_REG18, m_Crtc_Reg18);
    DDX_Control(pDX, IDC_CRTC_REG19, m_Crtc_Reg19);
    DDX_Control(pDX, IDC_CRTC_REG20, m_Crtc_Reg20);
    DDX_Control(pDX, IDC_CRTC_REG21, m_Crtc_Reg21);
    DDX_Control(pDX, IDC_CRTC_REG22, m_Crtc_Reg22);
    DDX_Control(pDX, IDC_Z80_FLAGS, m_Z80_FLAGS);
    DDX_Control(pDX, IDC_Z80_AF, m_Z80_AF);
    DDX_Control(pDX, IDC_Z80_BC, m_Z80_BC);
    DDX_Control(pDX, IDC_Z80_DE, m_Z80_DE);
    DDX_Control(pDX, IDC_Z80_HL, m_Z80_HL);
    DDX_Control(pDX, IDC_Z80_AF_, m_Z80_AF_);
    DDX_Control(pDX, IDC_Z80_BC_, m_Z80_BC_);
    DDX_Control(pDX, IDC_Z80_DE_, m_Z80_DE_);
    DDX_Control(pDX, IDC_Z80_HL_, m_Z80_HL_);
    DDX_Control(pDX, IDC_Z80_SP, m_Z80_SP);
    DDX_Control(pDX, IDC_Z80_PC, m_Z80_PC);
    DDX_Control(pDX, IDC_Z80_IX, m_Z80_IX);
    DDX_Control(pDX, IDC_Z80_IY, m_Z80_IY);
    DDX_Control(pDX, IDC_Z80_IR, m_Z80_IR);
    DDX_Control(pDX, IDC_Z80_IM, m_Z80_IM);
    DDX_Control(pDX, IDC_Z80_IFF1, m_Z80_IFF1);
    DDX_Control(pDX, IDC_Z80_IFF2, m_Z80_IFF2);
    DDX_Control(pDX, IDC_Z80_T, m_Z80_T);
    DDX_Control(pDX, IDC_VGA_ROM, m_VGA_Rom);
    DDX_Control(pDX, IDC_VGA_RAM, m_VGA_Ram);
    DDX_Control(pDX, IDC_VGA_PEN, m_VGA_Pen);
    DDX_Control(pDX, IDC_COL00, m_VGA_Col00);
    DDX_Control(pDX, IDC_COL01, m_VGA_Col01);
    DDX_Control(pDX, IDC_COL02, m_VGA_Col02);
    DDX_Control(pDX, IDC_COL03, m_VGA_Col03);
    DDX_Control(pDX, IDC_COL04, m_VGA_Col04);
    DDX_Control(pDX, IDC_COL05, m_VGA_Col05);
    DDX_Control(pDX, IDC_COL06, m_VGA_Col06);
    DDX_Control(pDX, IDC_COL07, m_VGA_Col07);
    DDX_Control(pDX, IDC_COL08, m_VGA_Col08);
    DDX_Control(pDX, IDC_COL09, m_VGA_Col09);
    DDX_Control(pDX, IDC_COL10, m_VGA_Col10);
    DDX_Control(pDX, IDC_COL11, m_VGA_Col11);
    DDX_Control(pDX, IDC_COL12, m_VGA_Col12);
    DDX_Control(pDX, IDC_COL13, m_VGA_Col13);
    DDX_Control(pDX, IDC_COL14, m_VGA_Col14);
    DDX_Control(pDX, IDC_COL15, m_VGA_Col15);
    DDX_Control(pDX, IDC_COL16, m_VGA_Col16);
    DDX_Control(pDX, IDC_SHOW_COMP, m_BpShowComp);
    DDX_Control(pDX, IDC_DESASM, m_Desasm);
    DDX_Control(pDX, IDC_STACK, m_Stack);
    DDX_Control(pDX, IDC_DUMP, m_Dump);
    DDX_Control(pDX, IDC_SCROLL, m_ScrollDesasm);
    DDX_Control(pDX, IDC_SCROLL_STACK, m_ScrollStack);
    DDX_Control(pDX, IDC_SCROLL_DUMP, m_ScrollDump);
    DDX_Control(pDX, IDC_ASCII, m_Ascii);
    DDX_Control(pDX, IDC_NUMROM, m_NumRom);
    DDX_Control(pDX, IDC_CNTHSYNC, m_CntHSync);
}


BEGIN_MESSAGE_MAP(DebugDlg, CDialog)
    //{{AFX_MSG_MAP(DebugDlg)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_WM_VSCROLL()
    ON_BN_CLICKED(IDC_BPCLEART, OnBpClearT)
    ON_BN_CLICKED(IDC_SHOW_COMP, OnBpShowComp)
    ON_BN_CLICKED(IDC_ASCII, OnBnClickedAscii)
    ON_LBN_DBLCLK(IDC_DESASM, OnLbnDblclkDesasm)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// DebugDlg message handlers

BOOL DebugDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    SetIcon( m_hIcon, TRUE );
    SetIcon( m_hIcon, FALSE );

    font9.CreatePointFont( 90, "Courier New", this->GetDC() );
    m_Desasm.SetFont( &font9 );
    m_Desasm.SetMaxLen( 48 );
    font8.CreatePointFont( 80, "Courier New", this->GetDC() );
    m_Z80_FLAGS.SetFont( &font8 );
    m_Stack.SetFont( &font8 );
    m_Dump.SetFont( &font8 );
    m_ScrollDesasm.SetScrollRange( 0, ASM_SIZE );
    m_ScrollStack.SetScrollRange( 0, STACK_SIZE );
    m_ScrollDump.SetScrollRange( 0, DUMP_SIZE );

    WINDOWPLACEMENT plc;
    GetWindowPlacement( &plc );
    SetWindowPos( NULL, 0, 0, TAILLE_REDUIT, plc.rcNormalPosition.bottom - plc.rcNormalPosition.top, SWP_NOMOVE );
    CenterWindow();

    return( TRUE );
}


void DebugDlg::OnPaint()
{
    UpdateDatas();
    if (IsIconic())
        {
        CPaintDC dc( this );
        SendMessage( WM_ICONERASEBKGND, ( WPARAM )dc.GetSafeHdc(), 0 );
        CRect rect;
        GetClientRect( &rect );
        dc.DrawIcon( ( rect.Width() - GetSystemMetrics( SM_CXICON ) + 1 ) / 2
                   , ( rect.Height() - GetSystemMetrics( SM_CYICON ) + 1 ) / 2
                   , m_hIcon
                   );
        }
    else
        CDialog::OnPaint();
}


HCURSOR DebugDlg::OnQueryDragIcon()
{
    return( ( HCURSOR )m_hIcon );
}


void DebugDlg::SetHex( CEdit * Ctrl, int valeur, int digits )
{
    Tmp[ 0 ] = '#';
    if ( digits == 4 )
        {
        Hex4( &Tmp[ 1 ], valeur );
        Tmp[ 5 ] = 0;
        }
    else
        {
        Hex2( &Tmp[ 1 ], valeur );
        Tmp[ 3 ] = 0;
        }
    Ctrl->SetFont( &font8 );
    Ctrl->SetWindowText( Tmp );
}


void DebugDlg::SetColor( CStatic * s, int i )
{
    CRect rect;
    int c = TabCoul[ i ];
    CDC * d = s->GetDC();
    s->GetClientRect( &rect );
    d->FillSolidRect( rect, ( ( c & 0xFF ) << 16 ) + ( c & 0xFF00 ) +  ( ( c & 0xFF0000 ) >> 16 ) );
    ReleaseDC( d );
}


void DebugDlg::UpdateDatas( void )
{
    static CStatic * TabVgaCol[ 17 ] =
        {
        &m_VGA_Col00, &m_VGA_Col01, &m_VGA_Col02, &m_VGA_Col03,
        &m_VGA_Col04, &m_VGA_Col05, &m_VGA_Col06, &m_VGA_Col07,
        &m_VGA_Col08, &m_VGA_Col09, &m_VGA_Col10, &m_VGA_Col11,
        &m_VGA_Col12, &m_VGA_Col13, &m_VGA_Col14, &m_VGA_Col15,
        &m_VGA_Col16
        };
    static CEdit * TabCrtcReg[ 18 ] =
        {
        &m_Crtc_Reg00, &m_Crtc_Reg01, &m_Crtc_Reg02, &m_Crtc_Reg03,
        &m_Crtc_Reg04, &m_Crtc_Reg05, &m_Crtc_Reg06, &m_Crtc_Reg07,
        &m_Crtc_Reg08, &m_Crtc_Reg09, &m_Crtc_Reg10, &m_Crtc_Reg11,
        &m_Crtc_Reg12, &m_Crtc_Reg13, &m_Crtc_Reg14, &m_Crtc_Reg15,
        &m_Crtc_Reg16, &m_Crtc_Reg17
        };
    int i;

    Drawing = TRUE;

    // Z80
    m_Z80_FLAGS.SetWindowText( GetFlags() );
    SetHex( &m_Z80_AF, RegAF );
    SetHex( &m_Z80_AF_, Reg_AF );
    SetHex( &m_Z80_BC, RegBC );
    SetHex( &m_Z80_BC_, Reg_BC );
    SetHex( &m_Z80_DE, RegDE );
    SetHex( &m_Z80_DE_, Reg_DE );
    SetHex( &m_Z80_HL, RegHL );
    SetHex( &m_Z80_HL_, Reg_HL );
    SetHex( &m_Z80_IR, RegIR );
    SetHex( &m_Z80_PC, RegPC );
    SetHex( &m_Z80_SP, RegSP );
    SetHex( &m_Z80_IX, RegIX );
    SetHex( &m_Z80_IY, RegIY );
    m_Z80_IFF1.SetWindowText( itoa( Cpc.IFF1, Tmp, 10 ) );
    m_Z80_IFF2.SetWindowText( itoa( Cpc.IFF2, Tmp, 10 ) );
    m_Z80_IM.SetWindowText( itoa( Cpc.InterruptMode, Tmp, 10 ) );
    m_Z80_T.SetWindowText( itoa( CptInstr, Tmp, 10 ) );

    m_BpShowComp.GetWindowText( Tmp, sizeof( Tmp ) );
    if ( * Tmp == '<' )
        {
        // Gate Array
        SetHex( &m_VGA_Rom, Cpc.VGARom, 2 );
        SetHex( &m_VGA_Ram, Cpc.VGARam, 2 );
        SetHex( &m_VGA_Pen, Cpc.InkReg, 2 );

        for ( i = 0; i < 17; i++ )
            SetColor( TabVgaCol[ i ], i );

        SetHex( &m_NumRom, Cpc.NumRom, 2 );
        SetHex( &m_CntHSync, Cpc.CntHSync, 2 );

        // CRTC
        m_Crtc_Index.SetWindowText( itoa( Cpc.CRTCIndex, Tmp, 10 ) );
        for ( i = 0; i < 18; i++ )
            SetHex( TabCrtcReg[ i ], Cpc.CRTCReg[ i ], 2 );

        SetHex( &m_Crtc_Reg18, Cpc.HCC, 2 );
        SetHex( &m_Crtc_Reg19, Cpc.VCC, 2 );
        SetHex( &m_Crtc_Reg20, Cpc.VtAdj, 2 );
        SetHex( &m_Crtc_Reg21, Cpc.VLC, 2 );
        SetHex( &m_Crtc_Reg22, Cpc.SyncCount, 2 );
        }

    // Assembleur
    m_Desasm.ResetContent();
    int NewOffset = AsmOffset;
    m_ScrollDesasm.SetScrollPos( AsmOffset );
    for ( i = 0; i < LIGNE_ASM; i++ )
        {
        int val = 0;
        int Adr = NewOffset;
        NewOffset = SetLigne( ( USHORT )NewOffset, Tmp );
        if ( ! i )
            IncAsmOffset = NewOffset;

        m_Desasm.AddString( Tmp );

        if ( CheckBreakPoint( Adr ) )
            val |= ITEM_BREAK;

        if ( RegPC == Adr )
            val |= ITEM_PC;

        m_Desasm.SetItemData( i, val );
        }

    // Pile
    m_Stack.ResetContent();
    USHORT Adr = ( USHORT )StackOffset;
    m_ScrollStack.SetScrollPos( Adr );
    strcpy( Tmp, "#XXXX  #    " );
    for ( i = 0; i < LIGNE_STACK; i++ )
        {
        Hex4( &Tmp[ 1 ], Adr );
        Hex4( &Tmp[ 7 ], Peek8Ext( Adr ) + 256 * Peek8Ext( ( USHORT )( Adr + 1 ) ) );
        m_Stack.AddString( Tmp );
        if ( Adr == RegSP )
            m_Stack.SetSel( i );
        Adr += 2;
        }

    // Dump
    m_Dump.ResetContent();
    Adr = ( USHORT )DumpOffset;
    m_ScrollDump.SetScrollPos( Adr );
    strcpy( Tmp, "XXXX-                                " );
    for ( i = 0; i < LIGNE_DUMP; i++ )
        {
        Hex4( Tmp, Adr );
        for ( int j = 0; j < 16; j++ )
            if ( m_Ascii.GetCheck() )
                Tmp[ 5 + j ] = Peek8Ext( Adr++ );
            else
                Hex2( &Tmp[ 5 + ( j << 1 ) ], Peek8Ext( Adr++ ) );

        m_Dump.AddString( Tmp );
        }
    Drawing = FALSE;
}


void DebugDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    if ( pScrollBar == &m_ScrollDesasm )
        {
        switch( nSBCode )
            {
            case SB_BOTTOM :
                AsmOffset = ASM_SIZE;
                if ( AsmOffset < 0 )
                    AsmOffset = 0;

                break;

            case SB_LINEDOWN :
                AsmOffset = IncAsmOffset < ASM_SIZE ? IncAsmOffset : ASM_SIZE;
                break;

            case SB_LINEUP :
                if ( AsmOffset )
                    AsmOffset--;
                else
                    AsmOffset = 0;

                break;

            case SB_PAGEUP :
                AsmOffset -= LIGNE_ASM;
                if ( AsmOffset < 0 )
                    AsmOffset = 0;

                break;

            case SB_PAGEDOWN :
                AsmOffset += LIGNE_ASM;
                if ( AsmOffset + LIGNE_ASM >= ASM_SIZE )
                    {
                    AsmOffset = ASM_SIZE;
                    if ( AsmOffset < 0 )
                        AsmOffset = 0;
                    }
                break;

            case SB_THUMBPOSITION :
            case SB_THUMBTRACK :
                AsmOffset = nPos & 0xFFFF;
                break;

            case SB_TOP :
                AsmOffset = 0;
                break;
            }
        pScrollBar->SetScrollPos( AsmOffset );
        UpdateDatas();
        }
    if ( pScrollBar == &m_ScrollStack )
        {
        switch( nSBCode )
            {
            case SB_BOTTOM :
                StackOffset = STACK_SIZE;
                if ( StackOffset < 0 )
                    StackOffset = 0;

                break;

            case SB_LINEDOWN :
                StackOffset = StackOffset + 2 < 0x10000 ? StackOffset + 2 : StackOffset;
                break;

            case SB_LINEUP :
                StackOffset = StackOffset -2 >= 0 ? StackOffset - 2 : 0;
                break;

            case SB_PAGEUP :
                StackOffset -= 2 * LIGNE_STACK;
                if ( StackOffset < 0 )
                    StackOffset = 0;

                break;

            case SB_PAGEDOWN :
                StackOffset += 2 * LIGNE_STACK;
                if ( StackOffset + 2 * LIGNE_STACK >= STACK_SIZE )
                    {
                    StackOffset = STACK_SIZE;
                    if ( StackOffset < 0 )
                        StackOffset = 0;
                    }
                break;

            case SB_THUMBPOSITION :
            case SB_THUMBTRACK :
                StackOffset = nPos & 0xFFFE + ( StackOffset & 1 );
                break;

            case SB_TOP :
                StackOffset = 0;
                break;
            }
        pScrollBar->SetScrollPos( StackOffset );
        UpdateDatas();
        }
    if ( pScrollBar == &m_ScrollDump )
        {
        switch( nSBCode )
            {
            case SB_BOTTOM :
                DumpOffset = DUMP_SIZE;
                if ( DumpOffset < 0 )
                    DumpOffset = 0;

                break;

            case SB_LINEDOWN :
                DumpOffset = DumpOffset + 16 < 0x10000 ? DumpOffset + 16 : DumpOffset;
                break;

            case SB_LINEUP :
                DumpOffset = DumpOffset -16 >= 0 ? DumpOffset - 16 : 0;
                break;

            case SB_PAGEUP :
                DumpOffset -= PAGE_DUMP;
                if ( DumpOffset < 0 )
                    DumpOffset = 0;

                break;

            case SB_PAGEDOWN :
                DumpOffset += PAGE_DUMP;
                if ( DumpOffset + PAGE_DUMP >= DUMP_SIZE )
                    {
                    DumpOffset = DUMP_SIZE;
                    if ( DumpOffset < 0 )
                        DumpOffset = 0;
                    }
                break;

            case SB_THUMBPOSITION :
            case SB_THUMBTRACK :
                DumpOffset = nPos & 0xFFF0;
                break;

            case SB_TOP :
                DumpOffset = 0;
                break;
            }
        pScrollBar->SetScrollPos( DumpOffset );
        UpdateDatas();
        }
    CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}


void DebugDlg:: ExecOneCycle( BOOL Principal )
{
    USHORT StartPC = RegPC;
    USHORT EndPC = RegPC;
    UBYTE op1 = Peek8Ext( EndPC++ );
    UBYTE op2 = Peek8Ext( EndPC );
    BOOL BreakCond = TRUE;
    if ( Principal )
        {
        if ( op1 == 0xED && ( op2 & 0xF4 ) == 0xB0 ) // Continuer tant que LDIR/LDDR/CPIR/CPDR pas fini
            {
            EndPC++;
            BreakCond = FALSE;
            }
        if ( op1 == 0xCD || ( op1 & 0xC7 ) == 0xC4 ) // Continuer tant que CALL pas fini
            {
            EndPC += 2;
            BreakCond = FALSE;
            }
        if ( op1 == 0x76 ) // Continuer tant que HALT pas fini
            BreakCond = FALSE;
        }
    int oldCptInstr = CptInstr;
    do
        {
        CycleZ80( ( UBYTE * )"", TRUE );
        if ( RegPC == EndPC || CptInstr - oldCptInstr > 10000000 || ( op1 == 0x76 && RegPC != StartPC ) ) // 1 seconde = sécurité
            BreakCond = TRUE;
        }
    while( ! BreakCond );
    AsmOffset = RegPC;
    StackOffset = RegSP;
    UpdateDatas();
    UpdateScreen();
}


BOOL DebugDlg:: PreTranslateMessage( MSG * pMsg )
{
    char * Nom;

    if ( ! Drawing )
        switch( pMsg->message )
            {
            case WM_KEYDOWN :
                switch( pMsg->wParam )
                    {
                    case VK_F4 :
                        Nom = OpenNewFile( "Snapshot files (*.SNA)|*.SNA||"
                                         , "*.SNA"
                                         , DirSnapRead
                                         , NomSnapRead
                                         , FALSE
                                         );
                        if ( * Nom )
                            LireSnap( Nom );
                        break;

                    case VK_F5 :
                        Nom = OpenNewFile( "Snapshot files (*.SNA)|*.SNA||"
                                         , "*.SNA"
                                         , DirSnapWrite
                                         , NomSnapWrite
                                         , TRUE
                                         );
                        if ( * Nom )
                            SauveSnap( Nom );
                        break;

                    case VK_F7 :
                        ExecOneCycle( FALSE );
                        break;

                    case VK_F8 :
                        ExecOneCycle( TRUE );
                        break;
                }
                break;

            case WM_RBUTTONDBLCLK :
                break;
            }
    return CDialog::PreTranslateMessage( pMsg );
}


void DebugDlg::OnBpClearT( void )
{
    CptInstr = 0;
    UpdateDatas();
}


void DebugDlg::OnBpShowComp( void )
{
    WINDOWPLACEMENT plc;

    GetWindowPlacement( &plc );
    int TailleY = plc.rcNormalPosition.bottom - plc.rcNormalPosition.top;
    m_BpShowComp.GetWindowText( Tmp, sizeof( Tmp ) );
    SetWindowPos( NULL, 0, 0, * Tmp == '>' ? TAILLE_EXPAND : TAILLE_REDUIT, TailleY, SWP_NOMOVE );
    * Tmp ^= 2;
    m_BpShowComp.SetWindowText( Tmp );
    UpdateDatas();
}


void DebugDlg::OnBnClickedAscii()
{
    UpdateDatas();
}


void DebugDlg::OnLbnDblclkDesasm()
{
    m_Desasm.GetText( m_Desasm.GetCurSel(), Tmp );
    int deci = 0;
    for ( int lg = 4; lg--; )
        {
        char c = Tmp[ 3 - lg ];
        deci += isdigit( c ) ? c - '0' : c - 'A' + 10;
        if ( lg )
            deci <<= 4;
        }
    AddOrToogleBreakPoint( ( USHORT )deci );
    UpdateDatas();
}
