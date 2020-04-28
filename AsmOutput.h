#ifndef ASMOUTPUT_H
#define ASMOUTPUT_H

#include  <afxwin.h>

#include  "Resource.h"
#include  "DebugDlg.h"
#include  "types.h"

class AsmOutput : public CDialog
{
public:
    enum { IDD = IDD_ASM_OUT };
    AsmOutput( CWnd * pParent = NULL );
    afx_msg void OnClose();
    void AddString( char * msg );
    void Assemble( char * Nom );
    CMyListBox m_Sortie;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();
    DECLARE_MESSAGE_MAP()

private:
    void AddList( char * Txt, char * arg1, int arg2, char * arg3, int itemData );

    CFont font8;
    int Close;
};

#endif
