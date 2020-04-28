/******************************************************************************/
/* Configuration pour l'archivage des différents éléments du fichier source   */
/******************************************************************************/
// !CONFIG!=/L/* /R/* /W"* Nom : "
// Définition du système       !CONFIG!=/V1!EMULATEUR CPC!
// Définition du sous système  !CONFIG!=/V2!PC-CPC!
// Définition du sous ensemble !CONFIG!=/V3!Chips!
// Définition du module        !CONFIG!=/V4!VGA!
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
*  Fichier     : VGA.C                 | Version : 0.1aq
* ------------------------------------------------------------------------------
*  Date        : 05/11/2002            | Auteur  : L.DEPLANQUE
* ------------------------------------------------------------------------------
*  Description : Emulation du circuit VGA
*
* ------------------------------------------------------------------------------
*  Historique  :
*           Date           |         Auteur          |       Description
* ------------------------------------------------------------------------------
*  05/11/2002              | L.DEPLANQUE             | creation
* ------------------------------------------------------------------------------
*  03/08/2004              | L.DEPLANQUE             | Version 0.1ab :
*                          |                         | Correction du mapping
*                          |                         | mémoire lors de la
*                          |                         | sélection de la valeur
*                          |                         | 0xC3 sur le Gate array.
*                          |                         | Gestion de 16 roms
*                          |                         | d'extensions.
* ------------------------------------------------------------------------------
*  06/12/2004              | L.DEPLANQUE             | Version 0.1ae :
*                          |                         | Ajout de la rom
*                          |                         | d'extension de gestion
*                          |                         | compactage/décompactage
* ------------------------------------------------------------------------------
*  19/10/2005              | L.DEPLANQUE             | Version 0.1ah:
*                          |                         | rom de gestion compactage/
*                          |                         | décompactage utilisée en
*                          |                         | tant que rom externe
* ------------------------------------------------------------------------------
*  08/12/2005              | L.DEPLANQUE             | Version 0.1ai:
*                          |                         | Modification paramètres
*                          |                         | couleurs pour sauvegarde
*                          |                         | BMP
* ------------------------------------------------------------------------------
*  16/12/2005              | L.DEPLANQUE             | Version 0.1aj:
*                          |                         | Prise en compte des
*                          |                         | en-têtes Amsdos dans les
*                          |                         | roms externes
* ------------------------------------------------------------------------------
*  06/10/2006              | L.DEPLANQUE             | Version 0.1am:
*                          |                         | Correction d'un bug
*                          |                         | dans la gestion des roms
*                          |                         | externes (pb multiface).
*                          |                         | Gestion 512K de ram.
*                          |                         | Adoucissement couleurs
* ------------------------------------------------------------------------------
*  22/03/2007              | L.DEPLANQUE             | Version 0.1an:
*                          |                         | Mémorisation nouveau
*                          |                         | mode demandé avant
*                          |                         | affectation à la
*                          |                         | prochaine HBL
* ------------------------------------------------------------------------------
*  30/05/2007              | L.DEPLANQUE             | Version 0.1ao:
*                          |                         | Prise en compte palette
*                          |                         | CPC+
*                          |                         | Ajout variable activation
*                          |                         | page mémoire ASIC
* ------------------------------------------------------------------------------
*  18/04/2010              | L.DEPLANQUE             | Version 0.1aq:
*                          |                         | Prise en compte des
*                          |                         | composantes R,V,B pour
*                          |                         | définition couleurs en 
*                          |                         | mode monochrome
* ------------------------------------------------------------------------------
********************************************************** !END! **************/


#include  <windows.h>

#include  "types.h"
#include  "multiface.h"
#include  "NewDisc.h"
#include  "Plateform.h"
#include  "SnapShot.h"
#include  "config.h"
#include  "log.h"
#include  "z80.h"
#include  "vga.h"
#include  "crtc.h"
#include  "asic.h"


#define     MODULENAME      "VGA"

/********************************************************* !NAME! **************
* Nom : MemCPC
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Mémoire du CPC
*
********************************************************** !0! ****************/
UBYTE MemCPC[ 0x90000 ];

/********************************************************* !NAME! **************
* Nom : TabPOKE
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Mapping des écritures vers mémoire CPC
*
********************************************************** !0! ****************/
UBYTE * TabPOKE[ 4 ];

/********************************************************* !NAME! **************
* Nom : TabPEEK
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Mapping des lectures depuis mémoire CPC
*
********************************************************** !0! ****************/
UBYTE * TabPEEK[ 4 ];

/********************************************************* !NAME! **************
* Nom : TabCoul
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Couleurs R,V,B
*
********************************************************** !0! ****************/
int TabCoul[ 32 ];


/********************************************************* !NAME! **************
* Nom : Bloc
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Numéro du bloc de ram sélectionnée (512K)
*
********************************************************** !0! ****************/
static int Bloc;

/********************************************************* !NAME! **************
* Nom : MemoMode
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Nouveau mode écran sélectionné
*
********************************************************** !0! ****************/
int MemoMode;

/********************************************************* !NAME! **************
* Nom : DelayGa
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Delai d'attente avant changement ink
*
********************************************************** !0! ****************/
int DelayGa = 0;

/********************************************************* !NAME! **************
* Nom : AsicPage
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Indique si la page ASIC est commutée en #4000
*
********************************************************** !0! ****************/
static int AsicPage;

/********************************************************* !NAME! **************
* Nom : ROMINF
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Rom inférieure
*
********************************************************** !0! ****************/
static UBYTE ROMINF[ 0x4000 ];

/********************************************************* !NAME! **************
* Nom : ROMEXT
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Roms d'extensions : 00 = RomSup, 07 = Rom disque (Amsdos)
*
********************************************************** !0! ****************/
static UBYTE ROMEXT[ MAX_ROM_EXT ][ 0x4000 ];

/********************************************************* !NAME! **************
* Nom : AdjRam
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Configuration de la ram en fonction du registre de sélection
*
********************************************************** !0! ****************/
static int AdjRam[ 8 ][ 4 ][ 8 ] = {
	// C0       C1       C2       C3       C4       C5       C6       C7
	0x00000, 0x00000, 0x10000, 0x00000, 0x00000, 0x00000, 0x00000, 0x00000,
	0x04000, 0x04000, 0x14000, 0x0C000, 0x10000, 0x14000, 0x18000, 0x1C000,
	0x08000, 0x08000, 0x18000, 0x08000, 0x08000, 0x08000, 0x08000, 0x08000,
	0x0C000, 0x1C000, 0x1C000, 0x1C000, 0x0C000, 0x0C000, 0x0C000, 0x0C000,

	// C8       C9       CA       CB       CC       CD       CE       CF
	0x00000, 0x00000, 0x20000, 0x00000, 0x00000, 0x00000, 0x00000, 0x00000,
	0x04000, 0x04000, 0x24000, 0x0C000, 0x20000, 0x24000, 0x28000, 0x2C000,
	0x08000, 0x08000, 0x28000, 0x08000, 0x08000, 0x08000, 0x08000, 0x08000,
	0x0C000, 0x2C000, 0x2C000, 0x2C000, 0x0C000, 0x0C000, 0x0C000, 0x0C000,

	// D0       D1       D2       D3       D4       D5       D6       D7
	0x00000, 0x00000, 0x30000, 0x00000, 0x00000, 0x00000, 0x00000, 0x00000,
	0x04000, 0x04000, 0x34000, 0x0C000, 0x30000, 0x34000, 0x38000, 0x3C000,
	0x08000, 0x08000, 0x38000, 0x08000, 0x08000, 0x08000, 0x08000, 0x08000,
	0x0C000, 0x3C000, 0x3C000, 0x3C000, 0x0C000, 0x0C000, 0x0C000, 0x0C000,

	// D8       D9       DA       DB       DC       DD       DE       DF
	0x00000, 0x00000, 0x40000, 0x00000, 0x00000, 0x00000, 0x00000, 0x00000,
	0x04000, 0x04000, 0x44000, 0x0C000, 0x40000, 0x44000, 0x48000, 0x4C000,
	0x08000, 0x08000, 0x48000, 0x08000, 0x08000, 0x08000, 0x08000, 0x08000,
	0x0C000, 0x4C000, 0x4C000, 0x4C000, 0x0C000, 0x0C000, 0x0C000, 0x0C000,

	// E0       E1       E2       E3       E4       E5       E6       E7
	0x00000, 0x00000, 0x50000, 0x00000, 0x00000, 0x00000, 0x00000, 0x00000,
	0x04000, 0x04000, 0x54000, 0x0C000, 0x50000, 0x54000, 0x58000, 0x5C000,
	0x08000, 0x08000, 0x58000, 0x08000, 0x08000, 0x08000, 0x08000, 0x08000,
	0x0C000, 0x5C000, 0x5C000, 0x5C000, 0x0C000, 0x0C000, 0x0C000, 0x0C000,

	// E8       E9       EA       EB       EC       ED       EE       EF
	0x00000, 0x00000, 0x60000, 0x00000, 0x00000, 0x00000, 0x00000, 0x00000,
	0x04000, 0x04000, 0x64000, 0x0C000, 0x60000, 0x64000, 0x68000, 0x6C000,
	0x08000, 0x08000, 0x68000, 0x08000, 0x08000, 0x08000, 0x08000, 0x08000,
	0x0C000, 0x6C000, 0x6C000, 0x6C000, 0x0C000, 0x0C000, 0x0C000, 0x0C000,

	// F0       F1       F2       F3       F4       F5       F6       F7
	0x00000, 0x00000, 0x70000, 0x00000, 0x00000, 0x00000, 0x00000, 0x00000,
	0x04000, 0x04000, 0x74000, 0x0C000, 0x70000, 0x74000, 0x78000, 0x7C000,
	0x08000, 0x08000, 0x78000, 0x08000, 0x08000, 0x08000, 0x08000, 0x08000,
	0x0C000, 0x7C000, 0x7C000, 0x7C000, 0x0C000, 0x0C000, 0x0C000, 0x0C000,

	// F8       F9       FA       FB       FC       FD       FE       FF
	0x00000, 0x00000, 0x80000, 0x00000, 0x00000, 0x00000, 0x00000, 0x00000,
	0x04000, 0x04000, 0x84000, 0x0C000, 0x80000, 0x84000, 0x88000, 0x8C000,
	0x08000, 0x08000, 0x88000, 0x08000, 0x08000, 0x08000, 0x08000, 0x08000,
	0x0C000, 0x8C000, 0x8C000, 0x8C000, 0x0C000, 0x0C000, 0x0C000, 0x0C000,

	// 0123     0127     4567     0327     0423     0523     0623     0723
};


/********************************************************* !NAME! **************
* Nom : SetMemCPC
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Mapping de la mémoire du CPC en fonction de la sélection des
*               roms et rams
*
* Résultat    : /
*
* Variables globales modifiées : TabPOKE, TabPEEK
*
********************************************************** !0! ****************/
void SetMemCPC( void ) {
	TabPOKE[ 0 ] = MultifaceEnable && ( ! ( Cpc.VGARom & ROMINF_OFF ) ) ? MULTIFACE_ROM : &MemCPC[ AdjRam[ Bloc ][ 0 ][ Cpc.VGARam ] ];
	TabPEEK[ 0 ] = ( Cpc.VGARom & ROMINF_OFF ) ? &MemCPC[ AdjRam[ Bloc ][ 0 ][ Cpc.VGARam ] ] : MultifaceEnable ? MULTIFACE_ROM : ROMINF;
	TabPOKE[ 1 ] = TabPEEK[ 1 ] = UseCpcPlus && AsicPage ? MemASIC : &MemCPC[ AdjRam[ Bloc ][ 1 ][ Cpc.VGARam ] ];
	TabPOKE[ 2 ] = TabPEEK[ 2 ] = &MemCPC[ AdjRam[ Bloc ][ 2 ][ Cpc.VGARam ] ];
	TabPOKE[ 3 ] = &MemCPC[ AdjRam[ Bloc ][ 3 ][ Cpc.VGARam ] ];
	TabPEEK[ 3 ] = ( Cpc.VGARom & ROMSUP_OFF ) ? &MemCPC[ AdjRam[ Bloc ][ 3 ][ Cpc.VGARam ] ] : ROMEXT[ Cpc.NumRom ];
}


/********************************************************* !NAME! **************
* Nom : CheckAmsdos
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Vérifie si le buffer passé en paramètre correspond
*               à une en-tête Amsdos
*
* Résultat    : TRUE si en-tête Amsdos, FALSE sinon
*
* Variables globales modifiées : /
*
********************************************************** !0! ****************/
static BOOL CheckAmsdos( UBYTE * Buf ) {
	int i, Checksum = 0;
	BOOL ModeAmsdos = FALSE;
	StAmsdos * pEntete = ( StAmsdos * )Buf;

	for ( i = 0; i < 67; i++ )
		Checksum += Buf[ i ];

	if ( ( pEntete->CheckSum == ( USHORT )Checksum ) && Checksum )
		ModeAmsdos = TRUE;

	return( ModeAmsdos );
}


static int MemoColor = 0xFF;


void VGASyncColor( void ) {
	if ( MemoColor < 0x20 ) {
		if ( ! DelayGa-- ) {
			TabCoul[ Cpc.InkReg ] = Monochrome ? RgbCPCMono[ MemoColor ] :
			RgbCPCColor[ MemoColor ];
			Cpc.InkData[ Cpc.InkReg ] = ( UBYTE )MemoColor;
			if ( UseCpcPlus )
				SetAsicColor( Cpc.InkReg, MemoColor );

			MemoColor = 0xFF;
			DelayGa = 0;
		}
	}
}


/********************************************************* !NAME! **************
* Nom : WriteVGA
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Ecriture d'un registre du VGA
*
* Résultat    : /
*
* Variables globales modifiées : Cpc.InkReg, TabCoul, Cpc.InkData, Cpc.VGARom,
*                                MemoMode, CntHSync, IRQ, Cpc.VGARam
*
********************************************************** !0! ****************/
void WriteVGA( int val ) {
	int newVal = val & 0x1F;
	switch( val >> 6 ) {
	case 0 :
		Cpc.InkReg = min( newVal, 16 );
		break;

	case 1 :
		MemoColor = newVal;
		break;

	case 2 :
		if ( ( val & 32 ) && AsicOn )
			AsicPage = ( ( val & 0x18 ) == 0x18 );
		else {
			MemoMode = val & 3;
			Cpc.VGARom = val;
			if ( val & 16 ) {
				Cpc.CntHSync = Cpc.IRQ = 0;
			}
		}
		SetMemCPC();
		break;

	case 3 :
		Cpc.VGARam = ( Use128Ko || Use512Ko ) * ( val & 7 );
		Bloc = Use512Ko * ( ( val >> 3 ) & 7 );
		SetMemCPC();
		break;
	}
}


/********************************************************* !NAME! **************
* Nom : WriteROM
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Sélection du numéro de rom supérieure
*
* Résultat    : /
*
* Variables globales modifiées : Cpc.NumRom
*
********************************************************** !0! ****************/
void WriteROM( int val ) {
	Log( MODULENAME, "Sélection rom externe %d", val, LOG_DEBUG_FULL );
	Cpc.NumRom = val;
	SetMemCPC();
}


/********************************************************* !NAME! **************
* Nom : ReadRom
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Lecture d'une rom
*
* Résultat    : TRUE si ok, FALSE sinon
*
* Variables globales modifiées : ROMEXT
*
********************************************************** !0! ****************/
BOOL ReadRom( char * NomFic, int numRom ) {
	static UBYTE RomLoc[ 0x4080 ];
	DWORD BytesRead;

	HANDLE fp = CreateFile( NomFic, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL );
	if ( fp != INVALID_HANDLE_VALUE ) {
		ReadFile( fp, RomLoc, sizeof( RomLoc ), &BytesRead, NULL );
		if ( CheckAmsdos( RomLoc ) )
			memcpy( RomLoc, &RomLoc[ 0x80 ], 0x4000 );

		memcpy( ROMEXT[ numRom ], RomLoc, sizeof( ROMEXT[ numRom ] ) );
		CloseHandle( fp );
		return TRUE;
	}
	Log( MODULENAME, "Fichier %s non trouvé.", NomFic, LOG_ERROR );
	return( FALSE );
}


/********************************************************* !NAME! **************
* Nom : InitMemCPC
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Initialisation : lecture des roms du cpc
*
* Résultat    : TRUE si ok, FALSE sinon
*
* Variables globales modifiées : ROMINF, ROMEXT
*
********************************************************** !0! ****************/
BOOL InitMemCPC( void ) {
	HANDLE fp;
	BOOL ErreurExt = FALSE;
	DWORD BytesRead;
	int i;

	fp = CreateFile( LocRomInf, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL );
	if ( fp != INVALID_HANDLE_VALUE ) {
		ReadFile( fp, ROMINF, sizeof( ROMINF ), &BytesRead, NULL );
		CloseHandle( fp );

		// Si rom Anglaise, passe en clavier Anglais (QWERTY)
		if ( ROMINF[ 0x3A00 ] == 0x7C )
			SetKeybEn();

		for ( i = 0; i < MAX_ROM_EXT; i++ ) {
			if ( LocRomExt[ i ][ 0 ] ) {
				if ( ! ReadRom( LocRomExt[ i ], i ) ) {
					ErreurExt = TRUE;
					break;
				}
			}
			else
				memcpy( ROMEXT[ i ], ROMEXT[ 0 ], sizeof( ROMEXT[ 0 ] ) );
		}
		if ( ! ErreurExt ) {
			WriteVGA( 0x89 );
			WriteVGA( 0xC0 );
			return( TRUE );
		}
	}
	else
		Log( MODULENAME, "Fichier %s non trouvé.", LocRomInf, LOG_ERROR );

	return( FALSE );
}
