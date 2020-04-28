#include  <afxwin.h>

#include  "Types.h"
#include  "Config.h"
#include  "Resource.h"
#include  "DialConfScr.h"
#include  "ChoixCouleur.h"


CDialConfScr::CDialConfScr(CWnd* pParent /*=NULL*/) : CDialog(CDialConfScr::IDD, pParent)
{
}


void CDialConfScr::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_RESOX, m_ResoX);
    DDX_Control(pDX, IDC_RESOY, m_ResoY);
    DDX_Control(pDX, IDC_DOUBLELINES, m_BpDoubleLignes);
    DDX_Control(pDX, IDC_BORDERLESS, m_BpBorderless);
    DDX_Control(pDX, IDC_PALEMUL, m_BpPalEmul);
    DDX_Control(pDX, IDC_SCR_COUL, m_BpScrCoul);
    DDX_Control(pDX, IDC_SCR_MONO, m_BpScrMono);
}


BEGIN_MESSAGE_MAP(CDialConfScr, CDialog)
    ON_BN_CLICKED(IDC_BORDERLESS, OnBorderless)
    ON_BN_CLICKED(IDC_DOUBLELINES, OnDoublelines)
    ON_BN_CLICKED(IDC_PALEMUL, OnPalEmul)
    ON_BN_CLICKED(IDC_EDITPALCOLOR, OnBnClickedEditpalcolor)
    ON_BN_CLICKED(IDC_EDITPALMONO, OnBnClickedEditpalmono)
    ON_BN_CLICKED(IDC_SCR_COUL, OnBpScrCoul)
    ON_BN_CLICKED(IDC_SCR_MONO, OnBpScrMono)
    ON_EN_UPDATE(IDC_RESOX, OnUpdateResox)
    ON_EN_UPDATE(IDC_RESOY, OnUpdateResoy)

END_MESSAGE_MAP()


BOOL CDialConfScr::OnInitDialog( void )
{
    CDialog::OnInitDialog();

    itoa( TailleScrX, Tmp, 10 );
    m_ResoX.SetWindowText( Tmp );
    itoa( TailleScrY, Tmp, 10 );
    m_ResoY.SetWindowText( Tmp );
    m_BpDoubleReso.SetCheck( 1 );
    m_BpDoubleLignes.SetCheck( DoubleLines );
    m_BpPalEmul.SetCheck( PalEmul );
    m_BpBorderless.SetCheck( BorderLess );
    m_BpScrCoul.SetCheck( ! Monochrome );
    m_BpScrMono.SetCheck( Monochrome );
    OnBorderless();

    DmdReset = FALSE;
    return( TRUE );
}

void CDialConfScr::OnBnClickedEditpalcolor( void )
{
    CChoixCouleur x( RgbCPCColor, NULL );
    x.DoModal();
    if ( x.GetRetour() )
        memcpy( RgbCPCColor, x.GetPalette(), sizeof( RgbCPCMono ) );
}


void CDialConfScr::OnBnClickedEditpalmono( void )
{
    CChoixCouleur x( RgbCPCMono, NULL );
    x.DoModal();
    if ( x.GetRetour() )
        memcpy( RgbCPCMono, x.GetPalette(), sizeof( RgbCPCMono ) );
}


void CDialConfScr::OnDoublelines( void )
{
    DoubleLines = m_BpDoubleLignes.GetCheck();
}


void CDialConfScr::OnPalEmul( void )
{
    PalEmul = m_BpPalEmul.GetCheck();
}


void CDialConfScr::OnBorderless( void )
{
    BorderLess = m_BpBorderless.GetCheck();
    m_ResoX.EnableWindow( BorderLess );
    m_ResoY.EnableWindow( BorderLess );
    DmdReset = TRUE;
}



void CDialConfScr::OnBpScrCoul( void )
{
    Monochrome = FALSE;
}


void CDialConfScr::OnBpScrMono( void )
{
    Monochrome = TRUE;
}


void CDialConfScr::OnUpdateResox( void )
{
    m_ResoX.GetWindowText( Tmp, sizeof( Tmp ) );
    TailleScrX = atoi( Tmp );
}


void CDialConfScr::OnUpdateResoy( void )
{
    m_ResoY.GetWindowText( Tmp, sizeof( Tmp ) );
    TailleScrY = atoi( Tmp );
}


