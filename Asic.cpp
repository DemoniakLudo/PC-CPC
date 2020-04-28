/******************************************************************************/
/* Configuration pour l'archivage des diff�rents �l�ments du fichier source   */
/******************************************************************************/
// !CONFIG!=/L/* /R/* /W"* Nom : "
// D�finition du syst�me       !CONFIG!=/V1!EMULATEUR CPC!
// D�finition du sous syst�me  !CONFIG!=/V2!PC-CPC!
// D�finition du sous ensemble !CONFIG!=/V3!Chips!
// D�finition du module        !CONFIG!=/V4!ASIC!
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
*  Fichier     : ASIC.C                | Version : 0.1ao
* ------------------------------------------------------------------------------
*  Date        : 22/05/2007            | Auteur  : L.DEPLANQUE
* ------------------------------------------------------------------------------
*  Description : Emulation de l'ASIC du CPC+
*
* ------------------------------------------------------------------------------
*  Historique  :
*           Date           |         Auteur          |       Description
* ------------------------------------------------------------------------------
*  22/05/2007              | L.DEPLANQUE             | creation
* ------------------------------------------------------------------------------
********************************************************** !END! **************/



#include  "Types.h"

#include  "Config.h"
#include  "Vga.h"
#include  "Log.h"

#define     MODULENAME      "ASIC"

static int PosDelockAsic;

/********************************************************* !NAME! **************
* Nom : AsicOn
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Indique si l'ASIC est actif ou non
*
********************************************************** !0! ****************/
BOOL AsicOn;

/********************************************************* !NAME! **************
* Nom : MemASIC
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Mapping m�moire des registres de l'ASIC
*
********************************************************** !0! ****************/
UBYTE MemASIC[ 0x4000 ];

/********************************************************* !NAME! **************
* Nom : DelockAsic
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : V�rification s�quence de "delockage" de l'asic
*
* R�sultat    : /
*
* Variables globales modifi�es : AsicOn
*
********************************************************** !0! ****************/
void DelockAsic( int Val ) {
	static int ChaineDelockAsic[] = {
		0xFF, 0x00, 0xFF, 0x77, 0xB3, 0x51, 0xA8, 0xD4, 0x62,
		0x39, 0x9C, 0x46, 0x2B, 0x15, 0x8A, 0xCD, 0xEE
	};

	if ( ! AsicOn ) {
		if ( Val == ChaineDelockAsic[ PosDelockAsic++ ] ) {
			if ( PosDelockAsic == 17 ) {
				AsicOn = TRUE;
				Log( MODULENAME, "Asic unlocked", LOG_DEBUG );
			}
		}
		else
			PosDelockAsic = 0;
	}
}

/********************************************************* !NAME! **************
* Nom : SetAsicColor
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Positionne les couleurs dans les registres de l'ASIC en fonction
*               des couleurs du CPC OLD
*
* R�sultat    : /
*
* Variables globales modifi�es : MemASIC
*
********************************************************** !0! ****************/
void SetAsicColor( int Pen, int Val ) {
	static int ColAsic[ 32 ] = {
		0x666, 0x666, 0xF06, 0xFF6, 0x006, 0x0F6, 0x606, 0x6F6,
		0x0F6, 0xFF6, 0xFF0, 0xFFF, 0x0F0, 0x0FF, 0x6F0, 0x6FF,
		0x006, 0xF06, 0xF00, 0xF0F, 0x000, 0x00F, 0x600, 0x60F,
		0x066, 0xF66, 0xF60, 0xF6F, 0x060, 0x06F, 0x660, 0x66F
	};

	MemASIC[ 0x2400 + ( Pen << 1 ) ] = ( UBYTE )ColAsic[ Val ];
	MemASIC[ 0x2401 + ( Pen << 1 ) ] = ( UBYTE )( ColAsic[ Val ] >> 8 );
}


/********************************************************* !NAME! **************
* Nom : SyncAsic
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Ex�cution d'un cycle de l'ASIC
*
* R�sultat    : /
*
* Variables globales modifi�es : /
*
********************************************************** !0! ****************/
void SyncAsic( void ) {
	static UBYTE OldMemAsic[ 0x4000 ];

	if ( AsicOn ) {
		// Couleurs
		for ( int i = 0; i < 64; i += 2 ) {
			int r = ( MemASIC[ 0x2400 + i ] >> 4 ) * 17;
			int b = ( MemASIC[ 0x2400 + i ] & 0x0F ) * 17;
			int v = ( MemASIC[ 0x2401 + i ] & 0x0F ) * 17;
			TabCoul[ i >> 1 ] = Monochrome ? v << 8 : ( r << 16 ) + ( v << 8 ) + b;
		}
		/*
		for ( i = 0; i < 0x4000; i++ )
		{
		if ( OldMemAsic[ i ] != MemASIC[ i ] )
		{
		char tmp[ 128 ];
		sprintf( tmp, "MemAsic[ #%04X ] = #%02X", i + 0x4000, MemASIC[ i ] );
		Log( MODULENAME, tmp, LOG_DEBUG );
		OldMemAsic[ i ] = MemASIC[ i ];
		}
		}
		*/
	}
}


/********************************************************* !NAME! **************
* Nom : ResetAsic
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : R�initialisation de l'ASIC
*
* R�sultat    : /
*
* Variables globales modifi�es : PosDelockAsic, AsicOn
*
********************************************************** !0! ****************/
void ResetAsic( void ) {
	PosDelockAsic = 0;
	AsicOn = FALSE;
}
