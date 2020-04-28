#ifndef DIALCONFSCR_H
#define DIALCONFSCR_H
#include "afxwin.h"


class CDialConfScr : public CDialog
{
public :
    CDialConfScr(CWnd* pParent = NULL);   // constructeur standard
    BOOL IsReset( void ) { return( DmdReset ); }

    enum { IDD = IDD_CONF_SCR };

    CEdit           m_ResoY;
    CEdit           m_ResoX;
    CButton         m_BpDoubleReso;
    CButton         m_BpDoubleLignes;
    CButton         m_BpBorderless;
    CButton         m_BpPalEmul;
    CButton         m_BpScrCoul;
    CButton         m_BpScrMono;
    CButton         m_Depth8;
    CButton         m_Depth32;

protected :
    virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge de DDX/DDV
    virtual BOOL OnInitDialog( void );
    virtual void OnOK( void ) {}

    afx_msg void OnBorderless( void );
    afx_msg void OnDoublelines( void );
    afx_msg void OnDoublereso( void );
    afx_msg void OnPalEmul( void );
    afx_msg void OnBnClickedEditpalcolor( void );
    afx_msg void OnBnClickedEditpalmono( void );
    afx_msg void OnBpScrCoul( void );
    afx_msg void OnBpScrMono( void );
    afx_msg void OnUpdateResox( void );
    afx_msg void OnUpdateResoy( void );
    afx_msg void OnDepth8( void );
    afx_msg void OnDepth32( void );
    DECLARE_MESSAGE_MAP()

private :
    BOOL DmdReset;
    char Tmp[ 64 ];
};


#endif
