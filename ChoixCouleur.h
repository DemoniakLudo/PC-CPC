#ifndef CHOIXCOULEUR_H
#define CHOIXCOULEUR_H

#include  <afxcmn.h>
#include  <afxext.h>
//#include  <atlimage.h>


class CChoixCouleur : public CDialog
{
public:
// Dialog Data
    //{{AFX_DATA(CSNAEditorDlg)
    enum { IDD = IDD_CHOIXCOULEUR };
    CChoixCouleur( unsigned int * palette, CWnd * pParent = NULL );
    BOOL GetRetour( void ) { return( Retour ); }
    int GetCouleur( void ) { return( Couleur ); }
    unsigned int * GetPalette( void ) { return( RgbCPC ); }

protected:
    virtual void DoDataExchange( CDataExchange * pDX );
    virtual BOOL OnInitDialog( void );
    afx_msg void OnCol00( void );
    afx_msg void OnCol01( void );
    afx_msg void OnCol02( void );
    afx_msg void OnCol03( void );
    afx_msg void OnCol04( void );
    afx_msg void OnCol05( void );
    afx_msg void OnCol06( void );
    afx_msg void OnCol07( void );
    afx_msg void OnCol08( void );
    afx_msg void OnCol09( void );
    afx_msg void OnCol10( void );
    afx_msg void OnCol11( void );
    afx_msg void OnCol12( void );
    afx_msg void OnCol13( void );
    afx_msg void OnCol14( void );
    afx_msg void OnCol15( void );
    afx_msg void OnCol16( void );
    afx_msg void OnCol17( void );
    afx_msg void OnCol18( void );
    afx_msg void OnCol19( void );
    afx_msg void OnCol20( void );
    afx_msg void OnCol21( void );
    afx_msg void OnCol22( void );
    afx_msg void OnCol23( void );
    afx_msg void OnCol24( void );
    afx_msg void OnCol25( void );
    afx_msg void OnCol26( void );
    afx_msg void OnCol27( void );
    afx_msg void OnCol28( void );
    afx_msg void OnCol29( void );
    afx_msg void OnCol30( void );
    afx_msg void OnCol31( void );
    afx_msg void OnChangeEditB( void );
    afx_msg void OnChangeEditR( void );
    afx_msg void OnChangeEditV( void );
    afx_msg void OnReleasedcaptureSliderB( NMHDR * pNMHDR, LRESULT * pResult );
    afx_msg void OnReleasedcaptureSliderR( NMHDR * pNMHDR, LRESULT * pResult );
    afx_msg void OnReleasedcaptureSliderV( NMHDR * pNMHDR, LRESULT * pResult );
    afx_msg void OnPaint( void );
    afx_msg void OnValide( void );
    virtual void OnCancel( void );
    virtual void OnOK( void ) {}
    DECLARE_MESSAGE_MAP()

private :
    void AffichePalette( bool SetEdit );
    void GetRgbCPC( int c, int &r, int &v, int &b );
    void SetPalette( int r, int v, int b, bool SetEdit );

    CStatic m_NumCol;
    CStatic m_ColChoix;
    CStatic m_Col[ 32 ];
    CSliderCtrl m_SlideR, m_SlideV, m_SlideB;
    CEdit m_EditR, m_EditV, m_EditB;
    unsigned int RgbCPC[ 32 ];
    int Couleur, r, v, b;
    BOOL Retour, edit;
    char Tmp[ 16 ];
};


#endif
