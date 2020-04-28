/******************************************************************************/
/* Configuration pour l'archivage des différents éléments du fichier source   */
/******************************************************************************/
// !CONFIG!=/L/* /R/* /W"* Nom : "
// Définition du système       !CONFIG!=/V1!EMULATEUR CPC!
// Définition du sous système  !CONFIG!=/V2!PC-CPC!
// Définition du sous ensemble !CONFIG!=/V3!Chips!
// Définition du module        !CONFIG!=/V4!PPI 8255!
/******************************************************************************/

/********************************************************* !NAME! **************
* !./FLE!
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\Fichiers
********************************************************** !0! *****************
* ------------------------------------------------------------------------------
*          SYSTEME         |      SOUS SYSTEME       |      SOUS ENSEMBLE
* ------------------------------------------------------------------------------
*  EMULATEUR CPC           | PC-CPC                  | Chips
* ------------------------------------------------------------------------------
*  Fichier     : PPI.C                 | Version : 0.1at
* ------------------------------------------------------------------------------
*  Date        : 05/11/2002            | Auteur  : L.DEPLANQUE
* ------------------------------------------------------------------------------
*  Description : Emulation du PPI 8255
*
* ------------------------------------------------------------------------------
*  Historique  :
*           Date           |         Auteur          |       Description
* ------------------------------------------------------------------------------
*  05/11/2002              | L.DEPLANQUE             | creation
* ------------------------------------------------------------------------------
*  06/11/2002              | L.DEPLANQUE             | Précisions bits du port
*                          |                         | B (Busy, 50Hz refresh)
* ------------------------------------------------------------------------------
*  07/11/2002              | L.DEPLANQUE             | Ajout gestion lecture et
*                          |                         | écriture cassette.:
*                          |                         | fonctions WriteCas() et
*                          |                         | ReadCas().
* ------------------------------------------------------------------------------
*  13/11/2002              | L.DEPLANQUE             | Optimisation WriteCas() 
*                          |                         | et ReadCas() : supprime
*                          |                         | test pointeur du fichier
*                          |                         | car inclus dans la
*                          |                         | variable Mode
* ------------------------------------------------------------------------------
*  03/12/2002              | L.DEPLANQUE             | Suppression de la
*                          |                         | génération de sons lors
*                          |                         | de la lecture/écriture
*                          |                         | cassette
* ------------------------------------------------------------------------------
*  09/01/2003              | L.DEPLANQUE             | Version 0.1u :
*                          |                         | corection d'un bugg :
*                          |                         | inversion des bits 
*                          |                         | PRINTER_BUSY et
*                          |                         | REFRESH_HZ
* ------------------------------------------------------------------------------
*  09/01/2004              | L.DEPLANQUE             | Version 0.1x :
*                          |                         | corection d'un bugg :
*                          |                         | problème d'écriture du
*                          |                         | buffer cassette dans le
*                          |                         | fichier correspondant
*                          |                         | (écrivait une taille=0)
* ------------------------------------------------------------------------------
*  02/02/2004              | L.DEPLANQUE             | Ajout de l'émission d'un
*                          |                         | son lors de la lecture
*                          |                         | ou l'écriture sur
*                          |                         | cassette.
* ------------------------------------------------------------------------------
*  30/11/2004              | L.DEPLANQUE             | Version 0.1ae :
*                          |                         | corection d'un bugg :
*                          |                         | Suppression de l'envoi
*                          |                         | d'un son dans le canal
*                          |                         | 0 pour simulation son
*                          |                         | cassette
* ------------------------------------------------------------------------------
*  02/06/2005              | L.DEPLANQUE             | Version 0.1ag :
*                          |                         | Gestion des fichiers .WAV
*                          |                         | pour la lecture depuis
*                          |                         | la cassette
* ------------------------------------------------------------------------------
*  06/12/2006              | L.DEPLANQUE             | Version 0.1am :
*                          |                         | Variable "VBL" remplacée
*                          |                         | par "VSync" et déplacée
*                          |                         | dans le module CRTC
* ------------------------------------------------------------------------------
*  13/05/2007              | L.DEPLANQUE             | Version 0.1an :
*                          |                         | Correction d'un petit bugg
*                          |                         | qui réinitialisait le
*                          |                         | canal sonore à chaque
*                          |                         | appel de la fonction
*                          |                         | SetSoundCas
* ------------------------------------------------------------------------------
*  12/06/2007              | L.DEPLANQUE             | Version 0.1ao :
*                          |                         | Optimisations mineures
*                          |                         | Suppression du bit
*                          |                         | PRINTER_BUSY
* ------------------------------------------------------------------------------
*  31/01/2011              | L.DEPLANQUE             | Version 0.1at :
*                          |                         | Prise en compte "bug" du
*                          |                         | CPC+ sur port contrôle
*                          |                         | PPI (pas de raz du port
*                          |                         | C quand écriture ctrl)
* ------------------------------------------------------------------------------
********************************************************** !END! **************/


#include  <windows.h>

#include  "Types.h"
#include  "Config.h"
#include  "SnapShot.h"
#include  "AY8912.h"
#include  "Crtc.h"
#include  "Ppi.h"
#include  "Z80.h"
#include  "Log.h"


#define     MODULENAME      "PPI"


#define REFRESH_HZ      0x10    // Screen refresh = 50Hz

#define SIZE_BUF_TAPE   0x800   // Taille buffer tampon cassette
// !!! Doit être une puissance de 2 !!!

/********************************************************* !NAME! **************
* Nom : modePSG
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Mode du PSG
*
********************************************************** !0! ****************/
static int modePSG;

/********************************************************* !NAME! **************
* Nom : clav
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Matrice du clavier (16 * 8 bits)
*
********************************************************** !0! ****************/
int clav[ 16 ];

/********************************************************* !NAME! **************
* Nom : MaskPortC
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Masques calculés pour masquage entrées/sorties
*
********************************************************** !0! ****************/
static int MaskPortC;

/********************************************************* !NAME! **************
* Nom : StWave
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Structures
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : En-tête d'un fichier WAV
*
********************************************************** !0! ****************/
typedef struct
{
	char    Tag1[ 4 ];
	int     Size1;
	char    Tag2[ 14 ];
	short   Mode;
	int     Freq;
	int     BytePerSec;
	short   NbBytes;
	short   Format;
	char    Tag3[ 4 ];
	int     Size2;
} StWave;

/********************************************************* !NAME! **************
* Nom : TpsTapeIn
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Nombre de cycles Z80 à attendre avant appel routine lecture
*               cassette
*
********************************************************** !0! ****************/
int TpsTapeIn = 0;

/********************************************************* !NAME! **************
* Nom : TpsTapeOut
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Nombre de cycles Z80 à attendre avant appel routine écriture
*               cassette
*
********************************************************** !0! ****************/
int TpsTapeOut = 0;

/********************************************************* !NAME! **************
* Nom : fCasIn
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Handle fichier lecture cassette
*
********************************************************** !0! ****************/
static HANDLE fCasIn = INVALID_HANDLE_VALUE;

/********************************************************* !NAME! **************
* Nom : fCasOut
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Handle fichier écriture cassette
*
********************************************************** !0! ****************/
static HANDLE fCasOut = INVALID_HANDLE_VALUE;

/********************************************************* !NAME! **************
* Nom : BitTapeIn
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Bit de lecture cassette pour le port F5xx
*
********************************************************** !0! ****************/
static UBYTE BitTapeIn = 0;

/********************************************************* !NAME! **************
* Nom : BufTape
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Buffer fichier cassette
*
********************************************************** !0! ****************/
static UBYTE BufTape[ SIZE_BUF_TAPE ];

/********************************************************* !NAME! **************
* Nom : PosBufTape
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Index sur buffer fichier cassette
*
********************************************************** !0! ****************/
static int PosBufTape;

/********************************************************* !NAME! **************
* Nom : FreqCpcIn
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Fréquence du son joué lors de la lecture cassette
*
********************************************************** !0! ****************/
static int FreqCpcIn;

/********************************************************* !NAME! **************
* Nom : FreqCpcOut
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Fréquence du son joué lors de l'écriture cassette
*
********************************************************** !0! ****************/
static int FreqCpcOut;

/********************************************************* !NAME! **************
* Nom : NbOctetsOut
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Nombre d'octets écris dans le fichier Wav
*
********************************************************** !0! ****************/
static unsigned int NbOctetsOut;

/********************************************************* !NAME! **************
* Nom : MemoSoundCasOff 
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Indique que la sortie son cassette est 'Off'
*
********************************************************** !0! ****************/
static int MemoSoundCasOff = 0;

/********************************************************* !NAME! **************
* Nom : CloseTapOut
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Fermeture fichier cassette en écriture
*
* Résultat    : /
*
* Variables globales modifiées : fCasOut
*
********************************************************** !0! ****************/
void CloseTapOut( void ) {
	StWave EnteteWaveOut;
	DWORD BytesWritten;

	if ( fCasOut != INVALID_HANDLE_VALUE ) {
		if ( PosBufTape ) {
			WriteFile( fCasOut, BufTape, PosBufTape, &BytesWritten, NULL );
			NbOctetsOut += PosBufTape;
		}

		SetFilePointer( fCasOut, 0, NULL, FILE_BEGIN );
		ReadFile( fCasOut, &EnteteWaveOut, sizeof( EnteteWaveOut ), &BytesWritten, NULL );
		SetFilePointer( fCasOut, 0, NULL, FILE_BEGIN );
		EnteteWaveOut.Size1 = NbOctetsOut + sizeof( EnteteWaveOut );
		EnteteWaveOut.Size2 = NbOctetsOut;
		WriteFile( fCasOut, &EnteteWaveOut, sizeof( EnteteWaveOut ), &BytesWritten, NULL );
		CloseHandle( fCasOut );
		fCasOut = INVALID_HANDLE_VALUE;
	}
	PosBufTape = 0;
	if ( UseSoundCas ) {
		MemoSoundCasOff = 1;
		SetSound( 0, 0 );
	}
	TpsTapeOut = 0;
}

/********************************************************* !NAME! **************
* Nom : CloseTapIn
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Fermeture fichier cassette en lecture
*
* Résultat    : /
*
* Variables globales modifiées : fCasIn
*
********************************************************** !0! ****************/
void CloseTapIn( void ) {
	if ( fCasIn != INVALID_HANDLE_VALUE ) {
		CloseHandle( fCasIn );
		fCasIn = INVALID_HANDLE_VALUE;
	}
	if ( UseSoundCas ) {
		MemoSoundCasOff = 1;
		SetSound( 0, 0 );
	}
	TpsTapeIn = 0;
}

/********************************************************* !NAME! **************
* Nom : OpenTapWrite
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Ouverture fichier cassette en écriture
*
* Résultat    : /
*
* Variables globales modifiées : fCas
*
********************************************************** !0! ****************/
void OpenTapWrite( char * Nom ) {
	StWave EnteteWaveOut;
	DWORD BytesWritten;

	Log( "Plateform", "Ecriture fichier cassette : %s", Nom, LOG_INFO );
	CloseTapOut();
	fCasOut = CreateFile( Nom, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL );
	if ( fCasOut != INVALID_HANDLE_VALUE ) {
		memset( &EnteteWaveOut, 0, sizeof( EnteteWaveOut ) );
		memcpy( EnteteWaveOut.Tag1, "RIFF", 4 );
		memcpy( EnteteWaveOut.Tag2, "WAVEfmt ", 8 );
		EnteteWaveOut.Tag2[ 8 ] = 0x10;
		EnteteWaveOut.Tag2[ 12 ] = 0x1;
		memcpy( EnteteWaveOut.Tag3, "data", 4 );
		EnteteWaveOut.Mode = 1;
		EnteteWaveOut.Format = 8;
		EnteteWaveOut.Freq = FreqWavOut;
		EnteteWaveOut.NbBytes = 1;
		EnteteWaveOut.BytePerSec = FreqWavOut;
		FreqCpcOut = FreqWavOut;
		TpsTapeOut = 1000000 / FreqCpcOut;
		WriteFile( fCasOut
			, &EnteteWaveOut
			, sizeof( EnteteWaveOut )
			, &BytesWritten
			, NULL
			);
		NbOctetsOut = 0;
	}
}

/********************************************************* !NAME! **************
* Nom : OpenTapRead
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Ouverture fichier cassette en lecture
*
* Résultat    : /
*
* Variables globales modifiées : fCas
*
********************************************************** !0! ****************/
void OpenTapRead( char * Nom ) {
	StWave Entete;

	Log( "Plateform", "Lecture fichier cassette : %s", Nom, LOG_INFO );
	CloseTapIn();
	fCasIn = CreateFile( Nom, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL );
	if ( fCasIn != INVALID_HANDLE_VALUE ) {
		DWORD BytesRead;
		ReadFile( fCasIn, &Entete, sizeof( Entete ), &BytesRead, NULL );
		if ( Entete.Mode == 1 && Entete.NbBytes == 1 && Entete.Format == 8 ) {
			TpsTapeIn = 1000000 / Entete.Freq;
			FreqCpcIn = Entete.Freq;
		}
		else {
			// Format wav non reconnu
			Log( MODULENAME, "Format de fichier .WAV non supporté.", LOG_ERROR );
			CloseTapIn();
		}
	}
}

/********************************************************* !NAME! **************
* Nom : SetPosTapeIn
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Positionne le fichier cassette entrée à une position précise
*
* Résultat    : /
*
* Variables globales modifiées : /
*
********************************************************** !0! ****************/
void SetPosTapeIn( int Pos ) {
	if ( ( fCasIn != INVALID_HANDLE_VALUE ) && FreqCpcIn )
		SetFilePointer( fCasIn, sizeof( StWave ) + Pos * FreqCpcIn, NULL, FILE_BEGIN );
}

/********************************************************* !NAME! **************
* Nom : GetPosTapeIn
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Retourne la position du fichier cassette entrée
*
* Résultat    : La position convertie en secondes ou -1
*
* Variables globales modifiées : /
*
********************************************************** !0! ****************/
int GetPosTapeIn( void ) {
	if ( ( fCasIn != INVALID_HANDLE_VALUE ) && FreqCpcIn ) {
		DWORD Pos = SetFilePointer( fCasIn, 0, NULL, FILE_CURRENT );
		return( ( Pos - sizeof( StWave ) ) / FreqCpcIn );
	}
	return( -1 );
}

/********************************************************* !NAME! **************
* Nom : SetSoundCas
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Arrête le son de la cassette si le moteur est OFF
*
* Résultat    : /
*
* Variables globales modifiées : /
*
********************************************************** !0! ****************/
static void SetSoundCas( void ) {
	if ( ! ( Cpc.PPI[ 2 ] & 0x10 ) && ! MemoSoundCasOff )
	{
		MemoSoundCasOff = 1;
		SetSound( 0, 0 );
	}
}

/********************************************************* !NAME! **************
* Nom : SoundCas
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Joue un son en fonction des bits lecture/écriture cassette
*
* Résultat    : /
*
* Variables globales modifiées : /
*
********************************************************** !0! ****************/
static void SoundCas( int BitCas, int FreqCPC ) {
	static int ValBit = 0;
	static int CntBit = 0;
	static int CntFreq = 0;

	if ( BitCas != ValBit ) {
		ValBit = BitCas;
		CntFreq += 32;
	}
	CntBit += 64;
	if ( CntBit >= FreqCPC ) {
		MemoSoundCasOff = 0;
		SetSound( CntFreq, 32 );
		CntBit = 0;
		CntFreq = 0;
	}
}

/********************************************************* !NAME! **************
* Nom : WriteCas
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Ecriture d'un bit dans le fichier cassette
*
* Résultat    : /
*
* Variables globales modifiées : /
*
********************************************************** !0! ****************/
void WriteCas( void ) {
	DWORD BytesWritten;
	int BitTapeOut = Cpc.PPI[ 2 ] & 0x20;
	if ( Cpc.PPI[ 2 ] & 0x10 ) {
		if ( UseSoundCas )
			SoundCas( BitTapeOut, FreqCpcOut );

		BufTape[ PosBufTape++ ] = ( UBYTE )( 0x40 + ( BitTapeOut << 2 ) );
		PosBufTape &= ( SIZE_BUF_TAPE - 1 );
		if ( ! PosBufTape ) {
			NbOctetsOut += SIZE_BUF_TAPE;
			if ( fCasOut != INVALID_HANDLE_VALUE )
				WriteFile( fCasOut, BufTape, SIZE_BUF_TAPE, &BytesWritten, NULL );
		}
	}
}

/********************************************************* !NAME! **************
* Nom : ReadCas
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Lecture d'un bit depuis le fichier cassette
*
* Résultat    : /
*
* Variables globales modifiées : BitTapeIn
*
********************************************************** !0! ****************/
void ReadCas( void ) {
	DWORD BytesRead;

	BitTapeIn = 0;
	if ( Cpc.PPI[ 2 ] & 0x10 ) {
		if ( ! PosBufTape && ( fCasIn != INVALID_HANDLE_VALUE ) )
			ReadFile( fCasIn, BufTape, SIZE_BUF_TAPE, &BytesRead, NULL );

		if ( BufTape[ PosBufTape++ ] > 0x85 )
			BitTapeIn = 0x80;

		PosBufTape &= ( SIZE_BUF_TAPE - 1 );
		if ( UseSoundCas )
			SoundCas( BitTapeIn, FreqCpcIn );
	}
}


/********************************************************* !NAME! **************
* Nom : UpdatePsg
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Mise à jour des registres du PSG en fonction du port A du PPI
*
* Résultat    : /
*
* Variables globales modifiées : RegPSGSel
*
********************************************************** !0! ****************/
static void UpdatePsg( void ) {
	Cpc.PsgIndex = modePSG == 3 ? Cpc.PPI[ 0 ] & 0x0F : Cpc.PsgIndex;
	if ( modePSG == 2 )
		Write8912( Cpc.PsgIndex, Cpc.PPI[ 0 ] );
}

/********************************************************* !NAME! **************
* Nom : WritePPI
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Ecriture d'un registre du PPI
*
* Résultat    : /
*
* Variables globales modifiées : Cpc.PPI, modePSG
*
********************************************************** !0! ****************/
void WritePPI( int adr, int val )
{
	int numReg = ( adr >> 8 ) & 3;
	Cpc.PPI[ numReg ] = val;
	switch( numReg ) {
	case 0 : // 0xF4xx
		UpdatePsg();
		break;

	case 1 : // 0xF5xx
		Log( MODULENAME, "Tentative écriture port PPI F500 (PC=%04X)", RegPC, LOG_INFO );
		break;

	case 2 : // 0xF6xx
		Cpc.PsgData[ 14 ] = clav[ val & 0x0F ];
		modePSG = val >> 6;
		UpdatePsg();
		break;


	case 3 : // 0xF7xx
		if ( val & 0x80 ) {
			Cpc.PPI[ 0 ] = 0;
			MaskPortC = ( val & 0x09 ) == 0x09 ? 0x00 : val & 0x08 ? 0x0F : val & 0x01 ? 0xF0 : 0xFF;
		}
		else
			Cpc.PPI[ 2 ] ^= ( - ( val & 1 ) ^ Cpc.PPI[ 2 ] ) & ( 1 << ( ( val >> 1 ) & 0x07 ) );

		if ( UseSoundCas )
			SetSoundCas();

		break;
	}
}


/********************************************************* !NAME! **************
* Nom : ReadPPI
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Lecture d'un registre du PPI
*
* Résultat    : Le registre du PPI selectionné
*
* Variables globales modifiées : /
*
********************************************************** !0! ****************/
int ReadPPI( int adr )
{
	switch( ( adr >> 8 ) & 3 )
	{
	case 0 : // 0xF4xx
		if ( modePSG == 1 )
			return( Cpc.PsgData[ Cpc.PsgIndex ] );

		Log( MODULENAME, "Lecture port F400 et modePsg <> 1", LOG_INFO );
		return( 0xFF );

	case 1 : // 0xF5xx
		return( ( ComputerName << 1 )
			| REFRESH_HZ
			| VSync
			| BitTapeIn
			); // Port B toujours en lecture

	case 2 : // 0xF6xx
		return( Cpc.PPI[ 2 ] & MaskPortC );
	}
	return( 0xFF );
}
