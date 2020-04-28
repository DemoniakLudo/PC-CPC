#include  <afxwin.h>
#include  <afxdlgs.h>
#include  <afxcmn.h>

#include  "Types.h"
#include  "Config.h"
#include  "Resource.h"
#include  "EditConfig.h"
#include  "ChoixCouleur.h"



BEGIN_MESSAGE_MAP( MyTabCtrl, CTabCtrl )
    ON_NOTIFY_REFLECT( TCN_SELCHANGE, OnSelChange )
END_MESSAGE_MAP()


MyTabCtrl::MyTabCtrl( void )
{
    SelTab = 0;
    MaxWidth = 0;
    MaxHeight = 0;
    PtrDial[ CONF_TAB_MEM ] = &m_TabConfMem;
    PtrDial[ CONF_TAB_SCR ] = &m_TabConfScr;
    PtrDial[ CONF_TAB_KBD ] = &m_TabConfKbd;
    PtrDial[ CONF_TAB_SND ] = &m_TabConfSnd;
}


BOOL MyTabCtrl::CreateTab( CDialog *pDlg, UINT nId, BOOL Enable )
{
    if ( ! pDlg->Create( nId, m_Parent ) )
        return FALSE;

    CRect r;
    pDlg->GetWindowRect( &r);
    MaxWidth = max( r.Width(), MaxWidth );
    MaxHeight = max( r.Height(), MaxHeight );
    pDlg->EnableWindow( Enable );
    pDlg->ShowWindow( Enable ? SW_SHOW : SW_HIDE );
    return TRUE;
}


void MyTabCtrl::PreSubclassWindow( void )
{
    CRect r;
    CTabCtrl::PreSubclassWindow();
    m_Parent = GetParent();

    InsertItem( CONF_TAB_MEM, "Roms / Ram" );
    InsertItem( CONF_TAB_SCR, "Video" );
    InsertItem( CONF_TAB_KBD, "Clavier" );
    InsertItem( CONF_TAB_SND, "Son" );

    CreateTab( PtrDial[ CONF_TAB_MEM ], IDD_CONF_MEM, TRUE );
    CreateTab( PtrDial[ CONF_TAB_SCR ], IDD_CONF_SCR, FALSE );
    CreateTab( PtrDial[ CONF_TAB_KBD ], IDD_CONF_KBD, FALSE );
    CreateTab( PtrDial[ CONF_TAB_SND ], IDD_CONF_SND, FALSE );

    GetWindowRect( &r );
    m_Parent->ScreenToClient( &r );
    AdjustRect( FALSE, &r );
    r.right = MaxWidth + r.left;
    r.bottom = MaxHeight + r.top;
    AdjustRect( TRUE, &r );
    MoveWindow( &r );
    AdjustRect( FALSE, &r );
    for ( int i = ( sizeof PtrDial / sizeof PtrDial[ 0 ] ); i--; )
        PtrDial[ i ]->MoveWindow( r.left, r.top, r.Width(), r.Height(), FALSE );
    SelectTab( 0 );
}


void MyTabCtrl::SelectTab( int newTab )
{
    SetCurSel( newTab );
    if ( SelTab != GetCurSel() )
        {
        PtrDial[ SelTab ]->EnableWindow( FALSE );
        PtrDial[ SelTab ]->ShowWindow( SW_HIDE );
        SelTab = GetCurSel();
        }
    PtrDial[ SelTab ]->EnableWindow( TRUE );
    PtrDial[ SelTab ]->ShowWindow( SW_SHOW );
}


void MyTabCtrl::OnSelChange( NMHDR * pNMHDR, LRESULT * pResult )
{
    SelectTab( GetCurSel() );
    * pResult = 0;
}


void CEditConfig::DoDataExchange( CDataExchange* pDX )
{
    CDialog::DoDataExchange( pDX );
    DDX_Control( pDX, IDC_LOGLEVEL, m_LogLevel );
    DDX_Control( pDX, IDC_ONGLET_CONF, m_OngletConf );
}

BEGIN_MESSAGE_MAP( CEditConfig, CDialog )
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED( IDC_EXPORT_CONFIG, OnBnClickedExportConfig )
    ON_BN_CLICKED( IDC_IMPORT_CONFIG, OnBnClickedImportConfig )
    ON_BN_CLICKED( IDC_VALIDER, OnBnClickedValider )
    ON_BN_CLICKED( IDC_ANNULER, OnBnClickedAnnuler )
    ON_CBN_SELCHANGE( IDC_LOGLEVEL, OnCbnSelchangeLoglevel )
END_MESSAGE_MAP()


BOOL CEditConfig::OnInitDialog( void )
{
    CDialog::OnInitDialog();

    SetIcon( m_hIcon, TRUE );
    SetIcon( m_hIcon, FALSE );
    m_LogLevel.AddString( "0 - Rien" );
    m_LogLevel.AddString( "1 - Erreurs" );
    m_LogLevel.AddString( "2 - Warnings" );
    m_LogLevel.AddString( "3 - Informations" );
    m_LogLevel.AddString( "4 - Débug" );
    m_LogLevel.AddString( "5 - Débug full" );
    m_LogLevel.SetCurSel( LogLevel );
    Valid = 0;
    m_OngletConf.SelectTab( FirstTab );
    return( TRUE );
}


void CEditConfig::OnPaint( void )
{
    if ( IsIconic() )
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


HCURSOR CEditConfig::OnQueryDragIcon( void )
{
    return ( HCURSOR ) m_hIcon;
}


void CEditConfig::OnBnClickedExportConfig( void )
{
    CFileDialog f( FALSE, 0, CONFIG_NAME );
    if ( f.DoModal() == IDOK )
        SaveConfig( ( char * )( const char * )f.GetPathName() );
}


void CEditConfig::OnBnClickedImportConfig( void )
{
    CFileDialog f( TRUE, 0, CONFIG_NAME );
    if ( f.DoModal() == IDOK )
        ReadConfig( ( char * )( const char * )f.GetPathName(), FALSE );
}


void CEditConfig::OnBnClickedValider( void )
{
    Valid = 1;
    CDialog::OnOK();
}


void CEditConfig::OnBnClickedAnnuler( void )
{
    Valid = 0;
    CDialog::OnOK();
}


void CEditConfig::OnCbnSelchangeLoglevel()
{
    LogLevel = m_LogLevel.GetCurSel();
}

