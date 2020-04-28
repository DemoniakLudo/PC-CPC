#include  <windows.h>

#include  "Types.h"
#include  "AY8912.h"
#include  "Config.h"
#include  "SnapShot.h"
#include  "Log.h"
#include  "WriteAvi.h"

#define     MODULENAME      "AY8912"

#define     MAGIC_NUMBER        ( 0x7A120000 / SndFreqReplay )
#define     SND_BUFSIZE         256
#define     SND_NB_BUFFERS      32

extern int CptInstr;

static int Exit;

static HWAVEOUT hWave = 0;

static UBYTE Buf[ SND_NB_BUFFERS ][ SND_BUFSIZE ];

static WAVEHDR Header[ SND_NB_BUFFERS ];

static BOOL WritePSGReg13, IsSound;

static int stepE, stepN, EnvStep;
static int Hold, Alternate, Attack, Holding;
static int Rng, OutNoise, CntEnv, CntNoise;
static int VolTable[ 16 ] = {30, 406, 788, 1177, 1577, 1991, 2426, 2890, 3395, 3959, 4606, 5370, 6300, 7463, 8958, 10922 };
static struct {
	int Cnt, Step, Out;
} TabTone[ 3 ];

void Write8912( int Reg, int Val )
{
	int tmp;
	Cpc.PsgData[ Reg ] = Reg < 8 && Reg > 10 && Reg != 6 ? Val : Val & 0x1F;
	switch( Reg ) {
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
		tmp = Cpc.PsgData[ Reg & 6 ] + ( ( Cpc.PsgData[ 1 + ( Reg & 6 ) ] & 0x0F ) << 8 );
		TabTone[ Reg >> 1 ].Step = tmp ? tmp : 1;
		break;

	case 6:
		stepN = Val ? Val : 1;
		break;

	case 11:
	case 12:
		stepE = ( Cpc.PsgData[ 11 ] + ( Cpc.PsgData[ 12 ] << 8 ) ) << 1;
		stepE = stepE ? stepE : 2;
		break;

	case 13:
		EnvStep = 0x0F;
		Holding = 0;
		Attack = Val & 0x04 ? 0x0F : 0x00;
		Hold = ( Val & 0x08 >> 3 ) | Val & 0x01;
		Alternate = Val & 0x08 ? Val & 0x02 ? 0x0F : 0x00 : Attack;
		break;
	}
}

BOOL IsWritePSGReg13( void ) {
	BOOL Ret = WritePSGReg13;
	WritePSGReg13 = FALSE;
	return( Ret );
}

void Reset8912( void ) {
	int i;
	Rng = OutNoise = 1;
	memset( TabTone, 0, sizeof( TabTone ) );
	CntNoise = CntEnv = 0;
	IsSound = 1;
	for ( i = 0; i < 14; i++ )
		Write8912( i, 0 );
}

static UBYTE BufLoc[ SND_BUFSIZE ];

void CreateSoundBuffer( void ) {
	int i, c;

	for ( i = 0; i < SND_BUFSIZE; i++ ) {
		int vol = 0x8000;
		if ( ++CntNoise >= stepN ) {
			CntNoise = 0;
			OutNoise ^= ( ( Rng + 1 ) & 2 ) >> 1;
			Rng = ( Rng ^ ( Rng & 1 ? 0x24000 : 0 ) ) >> 1;
		}
		if ( ! Holding ) {
			if ( ++CntEnv >= stepE ) {
				CntEnv = 0;
				if ( --EnvStep < 0 ) {
					Attack ^= Alternate;
					EnvStep = Hold ? 0x00 : 0x0F;
					Holding = Hold;
				}
			}
		}
		for ( c = 0; c < 3; c++ ) {
			if ( ++TabTone[ c ].Cnt >= TabTone[ c ].Step ) {
				TabTone[ c ].Cnt = 0;
				TabTone[ c ].Out ^= 1;
			}
			if ( ( TabTone[ c ].Out | ( ( Cpc.PsgData[ 7 ] >> c ) & 1 ) ) & ( OutNoise | ( ( Cpc.PsgData[ 7 ] >> ( c + 3 ) ) & 1 ) ) )
				vol += VolTable[ ( Cpc.PsgData[ 8 + c ] & 16 ) ? ( EnvStep ^ Attack ) : Cpc.PsgData[ 8 + c ] ];
		}
		BufLoc[ i ] = IsSound ? vol >> 8 : 0x80;
	}
}

static DWORD WINAPI ThreadFunc( void * Param ) {
	MSG Msg;

	Param = Param;      // Eviter un warning à la compilation
	while( ! Exit && GetMessage( &Msg, 0, 0, 0 ) && Msg.message != MM_WOM_CLOSE ) {
		if ( Msg.message == MM_WOM_DONE ) {
			WAVEHDR * h = ( WAVEHDR * )Msg.lParam;
			if ( h->dwFlags & WHDR_PREPARED )
				waveOutUnprepareHeader( ( HWAVEOUT )Msg.wParam, h, sizeof( WAVEHDR ) );

#ifndef USE_SOUND_HBL
			CreateSoundBuffer();
#endif
			memcpy( ( UBYTE * )h->lpData, BufLoc, SND_BUFSIZE );
			if ( WriteAviStarted() )
				WriteAviSound( BufLoc, SND_BUFSIZE );

			h->dwFlags = 0;
			waveOutPrepareHeader( ( HWAVEOUT )Msg.wParam, h, sizeof( WAVEHDR ) );
			waveOutWrite( ( HWAVEOUT )Msg.wParam, h, sizeof( WAVEHDR ) );
		}
	}
	return( 0 );
}

static BOOL InitWave( void ) {
	WAVEFORMATEX Format;
	DWORD ThreadID = 0;
	HANDLE hThread;
	int j;

	Exit = 0;
	hThread = CreateThread( 0, 0, ( LPTHREAD_START_ROUTINE )ThreadFunc, 0, 0, &ThreadID );
	if ( ! hThread )
		return( FALSE );

	SetThreadPriority( hThread, THREAD_PRIORITY_TIME_CRITICAL );
	CloseHandle( hThread );
	Sleep( 100 );
	Format.wFormatTag = WAVE_FORMAT_PCM;
	Format.nChannels = 1;
	Format.nSamplesPerSec = SndFreqReplay;
	Format.wBitsPerSample = 8;
	Format.nBlockAlign = 1;
	Format.nAvgBytesPerSec = SndFreqReplay;
	Format.cbSize = 0;
	if ( waveOutOpen( &hWave, WAVE_MAPPER, &Format, ThreadID, 0, CALLBACK_THREAD ) ) {
		PostThreadMessage( ThreadID, MM_WOM_CLOSE, ( WPARAM )hWave, 0 );
		return( FALSE );
	}
	for ( j = 0; ! Exit && j < SND_NB_BUFFERS; j++ ) {
		memset( &Header[ j ], 0, sizeof( WAVEHDR ) );
		memset( &Buf[ j ], 128, SND_BUFSIZE );
		Header[ j ].lpData = ( char * )&Buf[ j ];
		Header[ j ].dwBufferLength = SND_BUFSIZE;
		Header[ j ].dwUser = j;
		waveOutPrepareHeader( hWave, &Header[ j ], sizeof( WAVEHDR ) );
		waveOutWrite( hWave, &Header[ j ], sizeof( WAVEHDR ) );
	}
	return( TRUE );
}

void EndSound( void ) {
	int j;

	Exit = 1;
	Sleep( 50 );
	if ( hWave ) {
		waveOutReset( hWave );
		for( j = 0; j < SND_NB_BUFFERS; j++ )
			if ( Header[ j ].dwFlags & WHDR_PREPARED )
				waveOutUnprepareHeader( hWave, &Header[ j ], sizeof( WAVEHDR ) );

		do {
			waveOutReset( hWave );
		}
		while( waveOutClose( hWave ) == WAVERR_STILLPLAYING );
	}
	hWave = 0;
}

void SoundEnable( BOOL e ) {
	IsSound = e;
}

BOOL InitSound( void ) {
	EndSound();
	return( InitWave() );
}

void SetSound( int Freq, int Volume ) {
}
