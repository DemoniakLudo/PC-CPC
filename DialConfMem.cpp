#include  <afxwin.h>
#include  <afxdlgs.h>
#include  <afxcmn.h>

#include  "Types.h"
#include  "Config.h"
#include  "Resource.h"
#include  "DialConfRom.h"
#include  "DialConfMem.h"


CDialConfMem::CDialConfMem(CWnd* pParent /*=NULL*/) : CDialog(CDialConfMem::IDD, pParent)
{
    NumRomSup = 0;
}


void CDialConfMem::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_ROM_MULTIFACE, m_RomMulti);
    DDX_Control(pDX, IDC_MULTIFACE, m_ChoixMuliface);
    DDX_Control(pDX, IDC_BP_MULTIFACE, m_BpMultiface);
    DDX_Control(pDX, IDC_SLIDER_RAM, m_SliderRam);
    DDX_Control(pDX, IDC_VAL_RAM, m_ValRam);
    DDX_Control(pDX, IDC_LISTROM, m_ListRom);
    DDX_Control(pDX, IDC_MACHINE, m_ComboMachine);
    DDX_Control(pDX, IDC_MODELE, m_ComboModele);
}


BEGIN_MESSAGE_MAP(CDialConfMem, CDialog)
    ON_BN_CLICKED(IDC_BP_MULTIFACE, OnBpMultiface)
    ON_BN_CLICKED(IDC_MULTIFACE, OnMultiface)
    ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_RAM, OnNMCustomdrawSliderRam)
    ON_LBN_SELCHANGE(IDC_LISTROM, OnLbnSelchangeListrom)
    ON_CBN_SELCHANGE(IDC_MACHINE, OnSelchangeMachine)
    ON_CBN_SELCHANGE(IDC_MODELE, OnSelchangeModele)
END_MESSAGE_MAP()


BOOL CDialConfMem::OnInitDialog( void )
{
    CDialog::OnInitDialog();

    m_ChoixMuliface.SetCheck( Multiface );
    m_BpMultiface.EnableWindow( Multiface );
    m_RomMulti.EnableWindow( Multiface );
    m_RomMulti.SetWindowText( LocRomMulti );
    OnMultiface();
    m_SliderRam.SetRange( 0, 2 );
    m_SliderRam.SetPos( Use512Ko ? 2 : Use128Ko ? 1 : 0 );
    DmdReset = FALSE;
    InitListeRoms();
    m_ComboMachine.AddString( "Isp" );
    m_ComboMachine.AddString( "Triumph" );
    m_ComboMachine.AddString( "Saisho" );
    m_ComboMachine.AddString( "Solavox" );
    m_ComboMachine.AddString( "Awa" );
    m_ComboMachine.AddString( "Schneider" );
    m_ComboMachine.AddString( "Orion" );
    m_ComboMachine.AddString( "Amstrad" );
    m_ComboMachine.SetCurSel( ComputerName );
    m_ComboModele.AddString( "Plus Tard...");
    m_ComboModele.SetCurSel( 0 );
    return( TRUE );
}


void CDialConfMem::InitListeRoms( void )
{
    int i;

    for ( i = m_ListRom.GetCount(); i--; )
        m_ListRom.DeleteString( i );

    sprintf( Tmp, "Lower     : %s", GetName( LocRomInf ) );
    m_ListRom.AddString( Tmp );
    for ( i = 0; i < MAX_ROM_EXT; i++ )
        {
        sprintf( Tmp, "Upper %3d : %s", i, GetName( LocRomExt[ i ] ) );
        m_ListRom.AddString( Tmp );
        }
}


void CDialConfMem::OnBpMultiface( void )
{
    CFileDialog f( TRUE );
    if ( f.DoModal() == IDOK )
        {
        m_RomMulti.SetWindowText( f.GetPathName() );
        m_RomMulti.GetWindowText( LocRomMulti, sizeof( LocRomMulti ) );
        DmdReset = TRUE;
        }
}


void CDialConfMem::OnMultiface( void )
{
    Multiface = m_ChoixMuliface.GetCheck();
    m_BpMultiface.EnableWindow( Multiface );
    m_RomMulti.EnableWindow( Multiface );
    DmdReset = TRUE;
}


void CDialConfMem::OnNMCustomdrawSliderRam(NMHDR *pNMHDR, LRESULT *pResult)
{
    int Memory = m_SliderRam.GetPos();
    int a = Memory == 2;
    int b = Memory >= 1;
    if ( a != Use512Ko || b != Use128Ko )
        {
        Use512Ko = a;
        Use128Ko = b;
        DmdReset = TRUE;
        }
    sprintf( Tmp, "total : %3d Ko", Use512Ko ? 576 : Use128Ko ? 128 : 64 );
    m_ValRam.SetWindowTextA( Tmp );
    * pResult = 0;
}


void CDialConfMem::OnLbnSelchangeListrom()
{
    int i = m_ListRom.GetCurSel() - 1;
    CDialConfRom r( i );
    r.DoModal();
    if ( r.IsValid() )
        {
        sprintf( Tmp, "%s%s", r.GetPath(), r.GetName() );
        if ( i > -1 )
            strcpy( LocRomExt[ i ], Tmp );
        else
            strcpy( LocRomInf, Tmp );

        InitListeRoms();
        DmdReset = TRUE;
        }
}

void CDialConfMem::OnSelchangeMachine()
{
    ComputerName = m_ComboMachine.GetCurSel();
    DmdReset = TRUE;
}

void CDialConfMem::OnSelchangeModele()
{
}
