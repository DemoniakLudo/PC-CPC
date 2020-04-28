#ifndef ACTION_REPLAY_H
#define ACTION_REPLAY_H


void StartActionReplay( void );


class CActionReplay;

class CLightListBox : public CListBox
{
public :
    void SetParent( CActionReplay * p ) { parent = p; }

protected:
    virtual int VKeyToItem(UINT nKey, UINT nIndex);

private:
    CActionReplay * parent;
};


class CActionReplay : public CDialog
{
public :
    CActionReplay( CWnd * pParent = NULL );
    enum { IDD = IDD_ACTION_REPLAY };
    void ListUp( int index );
    void ListDown( int index );
    void ListPgUp( int index );
    void ListPgDown( int index );

protected :
    virtual void DoDataExchange( CDataExchange * pDX );
    virtual BOOL OnInitDialog( void );
    virtual void OnCancel( void );
    afx_msg void OnVScroll( UINT nSBCode, UINT nPos, CScrollBar * pScrollBar );
    afx_msg void OnTimer( UINT TimerVal );
    afx_msg void OnBnClickedAction( void );
    afx_msg void OnBnClickedMemBank( void );
    afx_msg void OnBnClickedBpCherche( void );
    afx_msg void OnBnClickedBpPoke( void );
    DECLARE_MESSAGE_MAP()

private :
    void InitMem( void );
    void RedrawListe( int sel );
    char * CreateMemData( int adr, BYTE * data );

    CScrollBar  m_ScrollMem;
    CLightListBox m_ListeMem;
    CListBox m_ListResult;
    CStatic m_NbItems;
    CButton m_MemBank;
    CButton m_BpValInf, m_BpValSup, m_BpValEqu, m_BpValDif;
    CFont font8;
    int memOffset;
    int IncMemOffset;
    char tmp[ 32 ];
};

#endif
