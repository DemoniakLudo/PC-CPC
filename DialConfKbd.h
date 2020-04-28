#ifndef DIALCONFKBD
#define DIALCONFKBD
#include "afxwin.h"


class CDialConfKbd : public CDialog
{
public :
    CDialConfKbd(CWnd* pParent = NULL);
    BOOL IsReset( void ) { return( DmdReset ); }

    enum { IDD = IDD_CONF_KBD };

    CButton m_Key1;
    CButton m_Key2;
    CButton m_Key3;
    CButton m_Key4;
    CButton m_Key6;
    CButton m_Key5;
    CButton m_Key7;
    CButton m_Key8;
    CButton m_Key9;
    CButton m_Key0;
    CButton m_KeyCrochetO;
    CButton m_KeyMoins;
    CButton m_KeyA;
    CButton m_KeyZ;
    CButton m_KeyQ;
    CButton m_KeyW;
    CButton m_KeyM;
    CButton m_KeyPipe;
    CButton m_KeyEtoile;
    CButton m_KeyPct;
    CButton m_KeyDieze;
    CButton m_KeyVirgule;
    CButton m_KeyPtVirgule;
    CButton m_KeySlash;
    CButton m_KeyPlus;
    CButton m_KeyDollar;
    CButton m_KeybFR;
    CButton m_KeybUK;

protected :
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog( void );
    virtual BOOL OnCommand( WPARAM wParam, LPARAM lParam );
    virtual void OnOK( void ) {}

    DECLARE_MESSAGE_MAP()

private :
    void OnKeybFr( void );
    void OnKeybUk( void );

    BOOL DmdReset;
};


#endif
