#ifndef EDITCONFIG_H
#define EDITCONFIG_H

#include  <afxcmn.h>

#include  "DialConfMem.h"
#include  "DialConfScr.h"
#include  "DialConfKbd.h"
#include  "DialConfSnd.h"


enum { CONF_TAB_MEM = 0, CONF_TAB_SCR ,CONF_TAB_KBD, CONF_TAB_SND };


class MyTabCtrl : public CTabCtrl {
public :
    MyTabCtrl( void );
    BOOL ResetKbd( void ) { return( m_TabConfKbd.IsReset() ); }
    BOOL ResetMem( void ) { return( m_TabConfMem.IsReset() ); }
    BOOL ResetScr( void ) { return( m_TabConfScr.IsReset() ); }
    BOOL ResetSnd( void ) { return( m_TabConfSnd.IsReset() ); }
    void SelectTab( int newTab );

protected:
    virtual void PreSubclassWindow( void );
    afx_msg void OnSelChange( NMHDR * pNMHDR, LRESULT * pResult );
    DECLARE_MESSAGE_MAP()

private :
    BOOL CreateTab( CDialog *, UINT, BOOL );

    CWnd * m_Parent;
    CDialConfMem m_TabConfMem;
    CDialConfScr m_TabConfScr;
    CDialConfKbd m_TabConfKbd;
    CDialConfSnd m_TabConfSnd;
    CDialog * PtrDial[ 4 ];
    int SelTab, MaxWidth, MaxHeight;
};


class CEditConfig : public CDialog
{
public :
    CEditConfig( int Tab, CWnd * pParent = NULL ) : CDialog( CEditConfig::IDD, pParent ) { FirstTab = Tab; }
    BOOL IsValid( void ) { return( Valid != 0 ); }
    BOOL ResetKbd( void ) { return( m_OngletConf.ResetKbd() ); }
    BOOL ResetMem( void ) { return( m_OngletConf.ResetMem() ); }
    BOOL ResetScr( void ) { return( m_OngletConf.ResetScr() ); }
    BOOL ResetSnd( void ) { return( m_OngletConf.ResetSnd() ); }

    enum { IDD = IDD_EDIT_CONFIG };
    CButton         m_BpOk;
    CButton         m_BpCancel;
    CComboBox       m_LogLevel;
    MyTabCtrl       m_OngletConf;

protected:
    HICON m_hIcon;

    virtual void DoDataExchange( CDataExchange* pDX );
    virtual BOOL OnInitDialog( void );
    afx_msg void OnPaint( void );
    afx_msg HCURSOR OnQueryDragIcon( void );
    virtual void OnOK( void ) {}
    afx_msg void OnBnClickedExportConfig( void );
    afx_msg void OnBnClickedImportConfig( void );
    afx_msg void OnBnClickedValider( void );
    afx_msg void OnBnClickedAnnuler( void );
    afx_msg void OnCbnSelchangeLoglevel( void );
    DECLARE_MESSAGE_MAP()

private :
    int FirstTab, Valid, Reset;
};

#endif
