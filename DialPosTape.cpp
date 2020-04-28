// DialPosTape.cpp : implementation file
//

#include  <afxwin.h>

#include  "DialPosTape.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialPosTape dialog


CDialPosTape::CDialPosTape( int Pos, CWnd * pParent /*=NULL*/ )
    : CDialog( CDialPosTape::IDD, pParent )
{
    //{{AFX_DATA_INIT(CDialPosTape)
        // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
    PosFileTape = Pos;
}


void CDialPosTape::DoDataExchange( CDataExchange * pDX )
{
    CDialog::DoDataExchange( pDX );
    //{{AFX_DATA_MAP(CDialPosTape)
        // NOTE: the ClassWizard will add DDX and DDV calls here
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP( CDialPosTape, CDialog )
    //{{AFX_MSG_MAP(CDialPosTape)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialPosTape message handlers

BOOL CDialPosTape::OnInitDialog()
{
    CDialog::OnInitDialog();
    return( TRUE );
}


void CDialPosTape::OnOK()
{
    CDialog::OnOK();
}
