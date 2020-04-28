#include  <afxwin.h>

#include  "Types.h"
#include  "Config.h"
#include  "Resource.h"
#include  "DialConfKbd.h"


CDialConfKbd::CDialConfKbd(CWnd* pParent /*=NULL*/)	: CDialog(CDialConfKbd::IDD, pParent)
{
}


void CDialConfKbd::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_KEY_FE_08, m_Key1);
    DDX_Control(pDX, IDC_KEY_FD_08, m_Key2);
    DDX_Control(pDX, IDC_KEY_FD_07, m_Key3);
    DDX_Control(pDX, IDC_KEY_FE_07, m_Key4);
    DDX_Control(pDX, IDC_KEY_FD_06, m_Key5);
    DDX_Control(pDX, IDC_KEY_FE_06, m_Key6);
    DDX_Control(pDX, IDC_KEY_FD_05, m_Key7);
    DDX_Control(pDX, IDC_KEY_FE_05, m_Key8);
    DDX_Control(pDX, IDC_KEY_FD_04, m_Key9);
    DDX_Control(pDX, IDC_KEY_FE_04, m_Key0);
    DDX_Control(pDX, IDC_KEY_FD_03, m_KeyCrochetO);
    DDX_Control(pDX, IDC_KEY_FE_03, m_KeyMoins);
    DDX_Control(pDX, IDC_KEY_F7_08, m_KeyA);
    DDX_Control(pDX, IDC_KEY_F7_07, m_KeyZ);
    DDX_Control(pDX, IDC_KEY_DF_08, m_KeyQ);
    DDX_Control(pDX, IDC_KEY_7F_08, m_KeyW);
    DDX_Control(pDX, IDC_KEY_DF_03, m_KeyM);
    DDX_Control(pDX, IDC_KEY_FB_03, m_KeyPipe);
    DDX_Control(pDX, IDC_KEY_FD_02, m_KeyEtoile);
    DDX_Control(pDX, IDC_KEY_EF_03, m_KeyPct);
    DDX_Control(pDX, IDC_KEY_F7_02, m_KeyDieze);
    DDX_Control(pDX, IDC_KEY_BF_04, m_KeyVirgule);
    DDX_Control(pDX, IDC_KEY_7F_04, m_KeyPtVirgule);
    DDX_Control(pDX, IDC_KEY_7F_03, m_KeySlash);
    DDX_Control(pDX, IDC_KEY_BF_03, m_KeyPlus);
    DDX_Control(pDX, IDC_KEY_BF_02, m_KeyDollar);
    DDX_Control(pDX, IDC_KEYB_FR, m_KeybFR);
    DDX_Control(pDX, IDC_KEYB_UK, m_KeybUK);
}


BEGIN_MESSAGE_MAP(CDialConfKbd, CDialog)
END_MESSAGE_MAP()


BOOL CDialConfKbd::OnInitDialog( void )
{
    CDialog::OnInitDialog();
    DmdReset = FALSE;
    OnKeybFr();
    m_KeybFR.SetCheck( 1 );
    return( TRUE );
}


BOOL CDialConfKbd::OnCommand( WPARAM wParam, LPARAM lParam )
{
    WORD code = HIWORD( wParam );
    if ( code == BN_CLICKED )
        {
        WORD nID = LOWORD( wParam );

        if ( nID == IDC_KEYB_FR )
            OnKeybFr();

        if ( nID == IDC_KEYB_UK )
            OnKeybUk();
        }
    return( TRUE );
}


void CDialConfKbd::OnKeybFr( void )
{
    m_Key1.SetWindowText( "1\n&&" );
    m_Key2.SetWindowText( "2\né" );
    m_Key3.SetWindowText( "3\n\"" );
    m_Key4.SetWindowText( "4\n'" );
    m_Key5.SetWindowText( "5\n(" );
    m_Key6.SetWindowText( "6\n]" );
    m_Key7.SetWindowText( "7\nè" );
    m_Key8.SetWindowText( "8\n!" );
    m_Key9.SetWindowText( "9\nç" );
    m_Key0.SetWindowText( "0\nà" );
    m_KeyCrochetO.SetWindowText( "[\n)" );
    m_KeyMoins.SetWindowText( "_\n-" );
    m_KeyA.SetWindowText( "A" );
    m_KeyZ.SetWindowText( "Z" );
    m_KeyQ.SetWindowText( "Q" );
    m_KeyW.SetWindowText( "W" );
    m_KeyM.SetWindowText( "M" );
    m_KeyPipe.SetWindowText( "|\n^" );
    m_KeyEtoile.SetWindowText( "<\n*" );
    m_KeyPct.SetWindowText( "%\nù" );
    m_KeyDieze.SetWindowText( ">\n#" );
    m_KeyVirgule.SetWindowText( "?\n," );
    m_KeyPtVirgule.SetWindowText( ".\n;" );
    m_KeySlash.SetWindowText( "/\n:" );
    m_KeyPlus.SetWindowText( "+\n=" );
    m_KeyDollar.SetWindowText( "@\\\n $" );
}


void CDialConfKbd::OnKeybUk( void )
{
    m_Key1.SetWindowText( "!\n1" );
    m_Key2.SetWindowText( "\"\n2" );
    m_Key3.SetWindowText( "#\n3" );
    m_Key4.SetWindowText( "$\n4" );
    m_Key5.SetWindowText( "%\n5" );
    m_Key6.SetWindowText( "&&\n6" );
    m_Key7.SetWindowText( "'\n7" );
    m_Key8.SetWindowText( "(\n8" );
    m_Key9.SetWindowText( ")\n9" );
    m_Key0.SetWindowText( "_\n0" );
    m_KeyCrochetO.SetWindowText( "=\n-" );
    m_KeyMoins.SetWindowText( "£\n^" );
    m_KeyA.SetWindowText( "Q" );
    m_KeyZ.SetWindowText( "W" );
    m_KeyQ.SetWindowText( "A" );
    m_KeyW.SetWindowText( "Z" );
    m_KeyM.SetWindowText( "*\n:" );
    m_KeyPipe.SetWindowText( "|\n@" );
    m_KeyEtoile.SetWindowText( "{\n[" );
    m_KeyPct.SetWindowText( "+\n;" );
    m_KeyDieze.SetWindowText( "}\n]" );
    m_KeyVirgule.SetWindowText( "M" );
    m_KeyPtVirgule.SetWindowText( "<\n," );
    m_KeySlash.SetWindowText( ">\n." );
    m_KeyPlus.SetWindowText( "?\n/" );
    m_KeyDollar.SetWindowText( "`\n\\" );
}

