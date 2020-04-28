#include  <afxcmn.h>

#include  "Resource.h"
#include  "ChoixCouleur.h"


CChoixCouleur::CChoixCouleur( unsigned int * palette, CWnd * pParent )
    : CDialog( IDD_CHOIXCOULEUR, pParent)
{
    memcpy( RgbCPC, palette, sizeof( RgbCPC ) );
}


void CChoixCouleur::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_NUMCOL, m_NumCol);
    DDX_Control(pDX, IDC_COLCHOIX, m_ColChoix);
    DDX_Control(pDX, IDC_COL31, m_Col[ 31 ] );
    DDX_Control(pDX, IDC_COL30, m_Col[ 30 ] );
    DDX_Control(pDX, IDC_COL29, m_Col[ 29 ] );
    DDX_Control(pDX, IDC_COL28, m_Col[ 28 ] );
    DDX_Control(pDX, IDC_COL27, m_Col[ 27 ] );
    DDX_Control(pDX, IDC_COL26, m_Col[ 26 ] );
    DDX_Control(pDX, IDC_COL25, m_Col[ 25 ] );
    DDX_Control(pDX, IDC_COL24, m_Col[ 24 ] );
    DDX_Control(pDX, IDC_COL23, m_Col[ 23 ] );
    DDX_Control(pDX, IDC_COL22, m_Col[ 22 ] );
    DDX_Control(pDX, IDC_COL21, m_Col[ 21 ] );
    DDX_Control(pDX, IDC_COL20, m_Col[ 20 ] );
    DDX_Control(pDX, IDC_COL19, m_Col[ 19 ] );
    DDX_Control(pDX, IDC_COL18, m_Col[ 18 ] );
    DDX_Control(pDX, IDC_COL17, m_Col[ 17 ] );
    DDX_Control(pDX, IDC_COL16, m_Col[ 16 ] );
    DDX_Control(pDX, IDC_COL15, m_Col[ 15 ] );
    DDX_Control(pDX, IDC_COL14, m_Col[ 14 ] );
    DDX_Control(pDX, IDC_COL13, m_Col[ 13 ] );
    DDX_Control(pDX, IDC_COL12, m_Col[ 12 ] );
    DDX_Control(pDX, IDC_COL11, m_Col[ 11 ] );
    DDX_Control(pDX, IDC_COL10, m_Col[ 10 ] );
    DDX_Control(pDX, IDC_COL08, m_Col[ 8 ] );
    DDX_Control(pDX, IDC_COL09, m_Col[ 9 ] );
    DDX_Control(pDX, IDC_COL07, m_Col[ 7 ] );
    DDX_Control(pDX, IDC_COL06, m_Col[ 6 ] );
    DDX_Control(pDX, IDC_COL05, m_Col[ 5 ] );
    DDX_Control(pDX, IDC_COL04, m_Col[ 4 ] );
    DDX_Control(pDX, IDC_COL03, m_Col[ 3 ] );
    DDX_Control(pDX, IDC_COL02, m_Col[ 2 ] );
    DDX_Control(pDX, IDC_COL01, m_Col[ 1 ] );
    DDX_Control(pDX, IDC_COL00, m_Col[ 0 ] );
    DDX_Control(pDX, IDC_SLIDER_R, m_SlideR);
    DDX_Control(pDX, IDC_SLIDER_V, m_SlideV);
    DDX_Control(pDX, IDC_SLIDER_B, m_SlideB);
    DDX_Control(pDX, IDC_EDIT_R, m_EditR);
    DDX_Control(pDX, IDC_EDIT_V, m_EditV);
    DDX_Control(pDX, IDC_EDIT_B, m_EditB);
}


BEGIN_MESSAGE_MAP(CChoixCouleur, CDialog)
    ON_BN_CLICKED(IDC_COL00, OnCol00)
    ON_BN_CLICKED(IDC_COL01, OnCol01)
    ON_BN_CLICKED(IDC_COL02, OnCol02)
    ON_BN_CLICKED(IDC_COL03, OnCol03)
    ON_BN_CLICKED(IDC_COL04, OnCol04)
    ON_BN_CLICKED(IDC_COL05, OnCol05)
    ON_BN_CLICKED(IDC_COL06, OnCol06)
    ON_BN_CLICKED(IDC_COL07, OnCol07)
    ON_BN_CLICKED(IDC_COL08, OnCol08)
    ON_BN_CLICKED(IDC_COL09, OnCol09)
    ON_BN_CLICKED(IDC_COL10, OnCol10)
    ON_BN_CLICKED(IDC_COL11, OnCol11)
    ON_BN_CLICKED(IDC_COL12, OnCol12)
    ON_BN_CLICKED(IDC_COL13, OnCol13)
    ON_BN_CLICKED(IDC_COL14, OnCol14)
    ON_BN_CLICKED(IDC_COL15, OnCol15)
    ON_BN_CLICKED(IDC_COL16, OnCol16)
    ON_BN_CLICKED(IDC_COL17, OnCol17)
    ON_BN_CLICKED(IDC_COL18, OnCol18)
    ON_BN_CLICKED(IDC_COL19, OnCol19)
    ON_BN_CLICKED(IDC_COL20, OnCol20)
    ON_BN_CLICKED(IDC_COL21, OnCol21)
    ON_BN_CLICKED(IDC_COL22, OnCol22)
    ON_BN_CLICKED(IDC_COL23, OnCol23)
    ON_BN_CLICKED(IDC_COL24, OnCol24)
    ON_BN_CLICKED(IDC_COL25, OnCol25)
    ON_BN_CLICKED(IDC_COL26, OnCol26)
    ON_BN_CLICKED(IDC_COL27, OnCol27)
    ON_BN_CLICKED(IDC_COL28, OnCol28)
    ON_BN_CLICKED(IDC_COL29, OnCol29)
    ON_BN_CLICKED(IDC_COL30, OnCol30)
    ON_BN_CLICKED(IDC_COL31, OnCol31)
    ON_BN_CLICKED(ID_VALIDE, OnValide)
    ON_EN_CHANGE(IDC_EDIT_R, OnChangeEditR)
    ON_EN_CHANGE(IDC_EDIT_V, OnChangeEditV)
    ON_EN_CHANGE(IDC_EDIT_B, OnChangeEditB)
    ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_R, OnReleasedcaptureSliderR)
    ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_V, OnReleasedcaptureSliderV)
    ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_B, OnReleasedcaptureSliderB)
    ON_WM_PAINT()
END_MESSAGE_MAP()


BOOL CChoixCouleur::OnInitDialog( void )
{
    CDialog::OnInitDialog();
    m_SlideR.SetRange( 0, 255 );
    m_SlideV.SetRange( 0, 255 );
    m_SlideB.SetRange( 0, 255 );
    Couleur = 0;
    AffichePalette( true );
    return( TRUE );
}


void CChoixCouleur::OnPaint( void )
{
    AffichePalette( false );
    CDialog::OnPaint();
}


void CChoixCouleur::GetRgbCPC( int c, int &r, int &v, int &b )
{
    int x = RgbCPC[ c ];
    r = ( x >> 16 ) & 0xFF;
    v = ( x >> 8 ) & 0xFF;
    b = x & 0xFF;
}


void CChoixCouleur::SetPalette( int r, int v, int b, bool SetEdit )
{
    RgbCPC[ Couleur ] = b + ( v << 8 ) + ( r << 16 );
    AffichePalette( SetEdit );
}


//
// Affichage de la palette CPC (32 couleurs)
//
void CChoixCouleur::AffichePalette( bool SetEdit )
{
    CRect rect;
    CDC * TempDC;
    int tmpr, tmpv, tmpb;

    for ( int i = 0; i < 32; i++ )
        {
        m_Col[ i ].GetClientRect( rect );
        TempDC = m_Col[ i ].GetDC();
        GetRgbCPC( i, tmpr, tmpv, tmpb );
        TempDC->FillSolidRect( rect, RGB( tmpr, tmpv, tmpb ) );
        m_Col[ i ].ReleaseDC( TempDC );
        }
    m_ColChoix.GetClientRect( rect );
    TempDC = m_ColChoix.GetDC();
    GetRgbCPC( Couleur, r, v, b );
    edit = false;
    m_SlideR.SetPos( r );
    if ( SetEdit )
        m_EditR.SetWindowText( itoa( r, Tmp, 10 ) );

    m_SlideV.SetPos( v );
    if ( SetEdit )
        m_EditV.SetWindowText( itoa( v, Tmp, 10 ) );

    m_SlideB.SetPos( b );
    if ( SetEdit )
        m_EditB.SetWindowText( itoa( b, Tmp, 10 ) );

    TempDC->FillSolidRect( rect, RGB( r, v, b ) );
    m_ColChoix.ReleaseDC( TempDC );
    wsprintf( Tmp, "%02d :", Couleur );
    m_NumCol.SetWindowText( Tmp );
    edit = true;
}


void CChoixCouleur::OnCol00( void )
{
    Couleur = 0;
    AffichePalette( true );
}


void CChoixCouleur::OnCol01( void )
{
    Couleur = 1;
    AffichePalette( true );
}


void CChoixCouleur::OnCol02( void )
{
    Couleur = 2;
    AffichePalette( true );
}


void CChoixCouleur::OnCol03( void )
{
    Couleur = 3;
    AffichePalette( true );
}


void CChoixCouleur::OnCol04( void )
{
    Couleur = 4;
    AffichePalette( true );
}


void CChoixCouleur::OnCol05( void )
{
    Couleur = 5;
    AffichePalette( true );
}


void CChoixCouleur::OnCol06( void )
{
    Couleur = 6;
    AffichePalette( true );
}


void CChoixCouleur::OnCol07( void )
{
    Couleur = 7;
    AffichePalette( true );
}


void CChoixCouleur::OnCol08( void )
{
    Couleur = 8;
    AffichePalette( true );
}


void CChoixCouleur::OnCol09( void )
{
    Couleur = 9;
    AffichePalette( true );
}


void CChoixCouleur::OnCol10( void )
{
    Couleur = 10;
    AffichePalette( true );
}


void CChoixCouleur::OnCol11( void )
{
    Couleur = 11;
    AffichePalette( true );
}


void CChoixCouleur::OnCol12( void )
{
    Couleur = 12;
    AffichePalette( true );
}


void CChoixCouleur::OnCol13( void )
{
    Couleur = 13;
    AffichePalette( true );
}


void CChoixCouleur::OnCol14( void )
{
    Couleur = 14;
    AffichePalette( true );
}


void CChoixCouleur::OnCol15( void )
{
    Couleur = 15;
    AffichePalette( true );
}


void CChoixCouleur::OnCol16( void )
{
    Couleur = 16;
    AffichePalette( true );
}


void CChoixCouleur::OnCol17( void )
{
    Couleur = 17;
    AffichePalette( true );
}


void CChoixCouleur::OnCol18( void )
{
    Couleur = 18;
    AffichePalette( true );
}


void CChoixCouleur::OnCol19( void )
{
    Couleur = 19;
    AffichePalette( true );
}


void CChoixCouleur::OnCol20( void )
{
    Couleur = 20;
    AffichePalette( true );
}


void CChoixCouleur::OnCol21( void )
{
    Couleur = 21;
    AffichePalette( true );
}


void CChoixCouleur::OnCol22( void )
{
    Couleur = 22;
    AffichePalette( true );
}


void CChoixCouleur::OnCol23( void )
{
    Couleur = 23;
    AffichePalette( true );
}


void CChoixCouleur::OnCol24( void )
{
    Couleur = 24;
    AffichePalette( true );
}


void CChoixCouleur::OnCol25( void )
{
    Couleur = 25;
    AffichePalette( true );
}


void CChoixCouleur::OnCol26( void )
{
    Couleur = 26;
    AffichePalette( true );
}


void CChoixCouleur::OnCol27( void )
{
    Couleur = 27;
    AffichePalette( true );
}


void CChoixCouleur::OnCol28( void )
{
    Couleur = 28;
    AffichePalette( true );
}


void CChoixCouleur::OnCol29( void )
{
    Couleur = 29;
    AffichePalette( true );
}


void CChoixCouleur::OnCol30( void )
{
    Couleur = 30;
    AffichePalette( true );
}


void CChoixCouleur::OnCol31( void )
{
    Couleur = 31;
    AffichePalette( true );
}


void CChoixCouleur::OnChangeEditR( void )
{
    if ( edit )
        {
        m_EditR.GetWindowText( Tmp, sizeof( Tmp ) );
        r = atoi( Tmp );
        if ( r > -1 && r < 256 )
            SetPalette( r, v, b, false );
        }
}


void CChoixCouleur::OnChangeEditV( void )
{
    if ( edit )
        {
        m_EditV.GetWindowText( Tmp, sizeof( Tmp ) );
        v = atoi( Tmp );
        if ( v > -1 && v < 256 )
            SetPalette( r, v, b, false );
        }
}


void CChoixCouleur::OnChangeEditB( void )
{
    if ( edit )
        {
        m_EditB.GetWindowText( Tmp, sizeof( Tmp ) );
        b = atoi( Tmp );
        if ( b > -1 && b < 256 )
            SetPalette( r, v, b, false );
        }
}


void CChoixCouleur::OnReleasedcaptureSliderR( NMHDR * pNMHDR, LRESULT * pResult )
{
    r = m_SlideR.GetPos();
    SetPalette( r, v, b, true );
    * pResult = 0;
}


void CChoixCouleur::OnReleasedcaptureSliderV( NMHDR * pNMHDR, LRESULT * pResult )
{
    v = m_SlideV.GetPos();
    SetPalette( r, v, b, true );
    * pResult = 0;
}


void CChoixCouleur::OnReleasedcaptureSliderB( NMHDR * pNMHDR, LRESULT * pResult )
{
    b = m_SlideB.GetPos();
    SetPalette( r, v, b, true );
    * pResult = 0;
}


void CChoixCouleur::OnCancel( void )
{
    CDialog::OnCancel();
    Retour = FALSE;
}


void CChoixCouleur::OnValide( void )
{
    CDialog::OnOK();
    Retour = TRUE;
}

