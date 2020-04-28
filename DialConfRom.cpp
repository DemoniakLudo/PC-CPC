#include  <afxwin.h>
#include  <afxdlgs.h>

#include  "Types.h"
#include  "Config.h"
#include  "Resource.h"
#include  "DialConfRom.h"


CDialConfRom::CDialConfRom( int n, CWnd * pParent /*=NULL*/ )
    : CDialog( CDialConfRom::IDD, pParent )
{
    NumRom = n;
}


void CDialConfRom::DoDataExchange( CDataExchange * pDX )
{
    CDialog::DoDataExchange( pDX );
    DDX_Control(pDX, IDC_ACTIVE, m_Active);
    DDX_Control(pDX, IDC_CHOIX_ROM, m_ComboChoixRom);
}


BEGIN_MESSAGE_MAP( CDialConfRom, CDialog )
    ON_BN_CLICKED(IDOK, OnBnClickedOk)
    ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
    ON_CBN_SELCHANGE(IDC_CHOIX_ROM, OnSelchangeChoixRom)
    ON_BN_CLICKED(IDC_ACTIVE, OnClickedActive)
END_MESSAGE_MAP()


BOOL CDialConfRom::OnInitDialog( void )
{
    char Tmp[ 16 ];

    CDialog::OnInitDialog();
    if ( NumRom == -1 )
        strcpy( Tmp, "Lower" );
    else
        sprintf( Tmp, "Upper %3d", NumRom );

    SetWindowText( Tmp );
    InitCombo();
    return( TRUE );
}


void CDialConfRom::GetPathAndFile( char * FullName )
{
    strcpy( Path, FullName );
    char * p = &Path[ strlen( Path ) - 1 ];
    while ( p > Path && * p != '\\' )
        p--;

    if ( * p == '\\' )
        p++;

    strcpy( Name, p );
    * p = 0;
}


void CDialConfRom::InitCombo( void )
{
    WIN32_FIND_DATA fData;

    int n = NumRom;
    if ( n > -1 && ! LocRomExt[ n ][ 0 ] )
        for ( n = 0; n < MAX_ROM_EXT; n++ )
            if ( LocRomExt[ n ][ 0 ] )
                break;

    strcpy( Path, n > -1 ? LocRomExt[ n ] : LocRomInf );
    char * p = &Path[ strlen( Path ) - 1 ];
    while ( p > Path && * p != '\\' )
        p--;

    if ( * p == '\\' )
        p++;

    if ( n == NumRom )
        {
        strcpy( Name, p );
        m_Active.SetCheck( 1 );
        m_ComboChoixRom.AddString( Name );
        }
    else
        {
        * Name = 0;
        m_Active.SetCheck( 0 );
        }
    strcpy( p, "*.*" );
    HANDLE f = FindFirstFile( Path, &fData );
    * p = 0;
    if ( f != INVALID_HANDLE_VALUE )
        {
        do
            {
            if ( ! ( fData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) )
                {
                char * n = fData.cFileName;
                if ( strcmp( n, Name ) )
                    m_ComboChoixRom.AddString( n );
                }
            }
        while( FindNextFile( f, &fData ) );
        }
    FindClose( f );
    m_ComboChoixRom.AddString( "..." );
    if ( n == NumRom )
        m_ComboChoixRom.SetCurSel( 0 );
}


void CDialConfRom::OnSelchangeChoixRom( void )
{
    static char Tmp[ 128 ];
    int i = m_ComboChoixRom.GetCurSel();
    m_ComboChoixRom.GetLBText( i, Tmp );
    if ( * Tmp != '.' )
        {
        strcpy( Name, Tmp );
        m_Active.SetCheck( 1 );
        }
    else
        {
        CFileDialog f( TRUE );
        if ( f.DoModal() == IDOK )
            {
            GetPathAndFile( ( char * )( const char * )f.GetPathName() );
            m_ComboChoixRom.AddString( Name );
            m_ComboChoixRom.SetCurSel( m_ComboChoixRom.GetCount() - 1 );
            m_Active.SetCheck( 1 );
            }
        }
}


void CDialConfRom::OnClickedActive( void )
{
    if ( ! m_Active.GetCheck() )
        {
        m_ComboChoixRom.SetCurSel( -1 );
        * Name = 0;
        }
}


void CDialConfRom::OnBnClickedOk( void )
{
    Valid = TRUE;
    CDialog::OnOK();
}


void CDialConfRom::OnBnClickedCancel( void )
{
    Valid = FALSE;
    CDialog::OnOK();
}
