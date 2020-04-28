#ifndef DIALCONFMEM_H
#define DIALCONFMEM_H


class CDialConfMem : public CDialog
{
public :
    CDialConfMem(CWnd* pParent = NULL);   // constructeur standard
    BOOL IsReset( void ) { return( DmdReset ); }

    enum { IDD = IDD_CONF_MEM };

    CSpinButtonCtrl m_SpinRomSup;
    CEdit           m_RomMulti;
    CButton         m_ChoixMuliface;
    CButton         m_BpMultiface;
    CSliderCtrl     m_SliderRam;
    CStatic         m_ValRam;
    CListBox        m_ListRom;
    CComboBox       m_ComboMachine;
    CComboBox       m_ComboModele;

protected :
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog( void );
    virtual void OnOK( void ) {}
    afx_msg void OnBpMultiface( void );
    afx_msg void OnMultiface( void );
    afx_msg void OnNMCustomdrawSliderRam( NMHDR * pNMHDR, LRESULT * pResult );
    afx_msg void OnLbnSelchangeListrom( void );
    afx_msg void OnSelchangeMachine( void );
    afx_msg void OnSelchangeModele( void );
    DECLARE_MESSAGE_MAP()

private :
    void InitListeRoms( void );

    char Tmp[ 128 ];
    int NumRomSup;
    BOOL DmdReset;
};


#endif
