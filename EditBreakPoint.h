#pragma once
#include "afxwin.h"


// Boîte de dialogue EditBreakPoint

class EditBreakPoint : public CDialog
{
    DECLARE_DYNAMIC(EditBreakPoint)

public:
    EditBreakPoint(CWnd* pParent = NULL);   // constructeur standard


// Données de boîte de dialogue
    enum { IDD = IDD_DIAL_BREAK };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge de DDX/DDV

    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnEnChangeCondition();
    afx_msg void OnBnClickedEnable();
    afx_msg void OnBnClickedCondEnable();
    CButton m_BpEnable;
    CButton m_BpEnableCondition;
    CListBox m_ListConditions;
    CEdit m_Adr;
    CEdit m_Condition;
    afx_msg void OnLbnSelchangeListBreak();
};
