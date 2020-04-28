#include  <afxwin.h>
#include  <afxcmn.h>

#include  "Types.h"
#include  "Config.h"
#include  "Resource.h"
#include  "DialConfSnd.h"


static int Freq[ 6 ] = { 22050, 31250, 44100, 62500, 88200, 125000 };


CDialConfSnd::CDialConfSnd(CWnd* pParent /*=NULL*/) : CDialog(CDialConfSnd::IDD, pParent)
{
}


void CDialConfSnd::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_SND_ON, m_SndOn);
    DDX_Control(pDX, IDC_TXT_FREQ, m_TxtFreq);
    DDX_Control(pDX, IDC_FREQ_WAV, m_FreqWav);
}


BEGIN_MESSAGE_MAP(CDialConfSnd, CDialog)
    ON_BN_CLICKED(IDC_SND_ON, OnBnClickedSndOn)
    ON_NOTIFY(NM_CUSTOMDRAW, IDC_FREQ_WAV, OnCustomdrawFreqWav)
END_MESSAGE_MAP()


BOOL CDialConfSnd::OnInitDialog( void )
{
    CDialog::OnInitDialog();
    m_FreqWav.SetRange( 0, 5 );
    for ( int i = 0; i < 6; i++ )
        if ( SndFreqReplay == Freq[ i ] )
            m_FreqWav.SetPos( i );

    DmdReset = FALSE;
    m_SndOn.SetCheck( SndEnabled );
    return( TRUE );
}


void CDialConfSnd::OnBnClickedSndOn( void )
{
    SndEnabled = m_SndOn.GetCheck();
}


void CDialConfSnd::OnCustomdrawFreqWav( NMHDR * pNMHDR, LRESULT * pResult )
{
    SndFreqReplay = Freq[ m_FreqWav.GetPos() ];
    sprintf( Tmp, "Fréquence de replay : %d Hz", SndFreqReplay );
    m_TxtFreq.SetWindowTextA( Tmp );
    DmdReset = TRUE;
    * pResult = 0;
}
