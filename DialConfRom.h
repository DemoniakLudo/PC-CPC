#ifndef DIALCONFROM_H
#define DIALCONFROM_H


class CDialConfRom : public CDialog
{
public :
    CDialConfRom( int n, CWnd * pParent = NULL );

    enum { IDD = IDD_CONF_CHOIXROM };

    BOOL IsValid( void ) { return( Valid ); }
    char * GetPath( void ) { return( * Name ? Path : Name ); }
    char * GetName( void ) { return( Name ); }

    CButton m_Active;
    CComboBox m_ComboChoixRom;

protected :
    DECLARE_MESSAGE_MAP()

    virtual void DoDataExchange( CDataExchange * pDX );
    virtual BOOL OnInitDialog( void );
    afx_msg void OnBnClickedOk( void );
    afx_msg void OnBnClickedCancel( void );
    afx_msg void OnSelchangeChoixRom( void );
    afx_msg void OnClickedActive( void );

private :
    void InitCombo( void );
    void GetPathAndFile( char * FullName );

    BOOL Valid;
    int NumRom;
    char Path[ LEN_PATH ];
    char Name[ LEN_PATH ];
};


#endif
