#ifndef DIALCONFSND_H
#define DIALCONFSND_H


class CDialConfSnd : public CDialog
{
public :
    CDialConfSnd(CWnd* pParent = NULL);
    BOOL IsReset( void ) { return( DmdReset ); }

    enum { IDD = IDD_CONF_SND };

    CButton         m_SndOn;
    CStatic         m_TxtFreq;
    CSliderCtrl     m_FreqWav;

protected :
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog( void );
    virtual void OnOK( void ) {}
    afx_msg void OnBnClickedSndOn( void );
    afx_msg void OnCustomdrawFreqWav( NMHDR * pNMHDR, LRESULT * pResult );

    DECLARE_MESSAGE_MAP()

private :
    char Tmp[ 128 ];
    BOOL DmdReset;
};


#endif