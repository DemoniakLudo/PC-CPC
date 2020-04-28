#if !defined(AFX_DIALPOSTAPE_H__0380D030_9FB4_4887_B28F_71D69E919CEA__INCLUDED_)
#define AFX_DIALPOSTAPE_H__0380D030_9FB4_4887_B28F_71D69E919CEA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialPosTape.h : header file
//


#include  "resource.h"


/////////////////////////////////////////////////////////////////////////////
// CDialPosTape dialog

class CDialPosTape : public CDialog
{
// Construction
public:
    CDialPosTape( int Pos, CWnd * pParent = NULL );   // standard constructor
    int GetPosFileTape( void ) { return( PosFileTape ); }

// Dialog Data
    //{{AFX_DATA(CDialPosTape)
    enum { IDD = IDD_POS_TAPE };
        // NOTE: the ClassWizard will add data members here
    //}}AFX_DATA


// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CDialPosTape)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL


// Implementation
protected:

    // Generated message map functions
    //{{AFX_MSG(CDialPosTape)
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

private :
    int PosFileTape;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALPOSTAPE_H__0380D030_9FB4_4887_B28F_71D69E919CEA__INCLUDED_)
