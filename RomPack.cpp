/******************************************************************************/
/* Configuration pour l'archivage des différents éléments du fichier source   */
/******************************************************************************/
// !CONFIG!=/L/* /R/* /W"* Nom : "
// Définition du système       !CONFIG!=/V1!EMULATEUR CPC!
// Définition du sous système  !CONFIG!=/V2!PC-CPC!
// Définition du sous ensemble !CONFIG!=/V3!Accessoires!
// Définition du module        !CONFIG!=/V4!RomPack!
/******************************************************************************/

/********************************************************* !NAME! **************
* !./FLE!
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\Fichiers
********************************************************** !0! *****************
* ------------------------------------------------------------------------------
*          SYSTEME         |      SOUS SYSTEME       |      SOUS ENSEMBLE
* ------------------------------------------------------------------------------
*  EMULATEUR CPC           | PC-CPC                  | Accessoires
* -----------------------------------------------------------------------------
*  Fichier     : ROMPACK.C             | Version : 0.1at
* ------------------------------------------------------------------------------
*  Date        : 30/11/2005            | Auteur  : L.DEPLANQUE
* ------------------------------------------------------------------------------
*  Description : Gestion des copdes op. utilisées par la rom pack
*
* ------------------------------------------------------------------------------
*  Historique  :
*           Date           |         Auteur          |       Description
* ------------------------------------------------------------------------------
*  30/11/2005              | L.DEPLANQUE             | creation
* ------------------------------------------------------------------------------
*  18/12/2005              | L.DEPLANQUE             | Version 0.1aj :
*                          |                         | Ajout instruction ED_09
* ------------------------------------------------------------------------------
*  16/02/2006              | L.DEPLANQUE             | Version 0.1ak :
*                          |                         | Ajout instruction ED_0A
* ------------------------------------------------------------------------------
*  27/06/2006              | L.DEPLANQUE             | Version 0.1al :
*                          |                         | Ajout instruction ED_0B
*                          |                         | à ED_16
* ------------------------------------------------------------------------------
*  20/09/2006              | L.DEPLANQUE             | Version 0.1am :
*                          |                         | Ajout instruction ED_17
*                          |                         | à ED_1C
* ------------------------------------------------------------------------------
*  22/03/2007              | L.DEPLANQUE             | Version 0.1an :
*                          |                         | Ajout instruction ED_1D
*                          |                         | à ED_1F
*                          |                         | Correction nombre de 
*                          |                         | cycles nops retournés
*                          |                         | par les instructions
*                          |                         | ED_00 à ED_1C : 2 nops
*                          |                         | au lieu de 4
* ------------------------------------------------------------------------------
*  16/04/2010              | L.DEPLANQUE             | Version 0.1ap :
*                          |                         | Ajout instruction ED_1F
*                          |                         | (Assemble)
* ------------------------------------------------------------------------------
*  07/03/2011              | L.DEPLANQUE             | Version 0.1at :
*                          |                         | Suppression des RET
*                          |                         | inutiles et dangereux.
*                          |                         | Correction problème
*                          |                         | perte Cpc.VGARom.
*                          |                         | Ajout instructions ED_20
*                          |                         | et ED_21
*                          |                         | Utilisation RomPack
*                          |                         | conditionée par variable
* ------------------------------------------------------------------------------
********************************************************** !END! **************/


#include  "Types.h"

#include  <afxwin.h>

#include  "Z80.h"
#include  "Vga.h"
#include  "Pack.h"
#include  "RomPack.h"
#include  "Plateform.h"
#include  "SnapShot.h"
#include  "Config.h"
#include  "Log.h"
#include  "SauveBmp.h"
#include  "WriteAvi.h"
#include  "WriteYM.h"
#include  "NewDisc.h"
#include  "Desasm.h"
#include  "AsmOutput.h"


#define     MODULENAME      "Rompack"


/********************************************************* !NAME! **************
* Nom : RomPack
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Utilisation ou non RomPack
*
********************************************************** !0! ****************/
int RomPack = 0;


/********************************************************* !NAME! **************
* Nom : ConvLigne
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Indique si la conversion d'adresse écran doit se faire par
*               lignes ou par colonne
*
********************************************************** !0! ****************/
static BOOL ConvLigne = FALSE;


/********************************************************* !NAME! **************
* Nom : ConvCoord
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Indique si il y a lieu de faire une conversion des coordonnées
*               des fenêtres à compacter
*
********************************************************** !0! ****************/
static BOOL ConvCoord = FALSE;


/********************************************************* !NAME! **************
* Nom : SymbVidFile
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Pointeur de fichier vers vidéo symbos
*
********************************************************** !0! ****************/
static HANDLE SymbVidFile = INVALID_HANDLE_VALUE;


/********************************************************* !NAME! **************
* Nom : SymbVidBuffer
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Buffer de lecture fichier vidéo symbos
*
********************************************************** !0! ****************/
static UBYTE SymbVidBuffer[ 0x8000 ];


/********************************************************* !NAME! **************
* Nom : SymbVidSizeX
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Taille en X vidéo symbos
*
********************************************************** !0! ****************/
static int SymbVidSizeX;


/********************************************************* !NAME! **************
* Nom : SymbVidSizeY
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Taille en Y vidéo symbos
*
********************************************************** !0! ****************/
static int SymbVidSizeY;


/********************************************************* !NAME! **************
* Nom : SymbVidBufSize
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Taille du buffer frame vidéo symbos
*
********************************************************** !0! ****************/
static int SymbVidBufSize;


/********************************************************* !NAME! **************
* Nom : OldVgaRom
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Sauvegarde de la variable Cpc.VGARom
*
********************************************************** !0! ****************/
static int OldVgaRom;


/********************************************************* !NAME! **************
* Nom : Nom
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Nom d'un fichier à ouvrir
*
********************************************************** !0! ****************/
static char Nom[ LEN_PATH ];


/********************************************************* !NAME! **************
* Nom : BufMem
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Copie d'une partie de la mémoire du CPC
*
********************************************************** !0! ****************/
static UBYTE BufMem[ 0x4000 ];


static void GetName( int reg, int lg, char * ptr )
{
	int i;

	for ( i = 0; i < lg; i++ )
		ptr[ i ] = Peek8Ext( ( USHORT )( reg++ ) );

	ptr[ i ] = 0;
}


static void DrawLigneHor( UBYTE * BufMem, int x1, int x2, int y, int col )
{
	static UBYTE TabOct[ 4 ] = { 0x00, 0x80, 0x08, 0x88 };

	if ( x1 > x2 )
	{
		int tmp = x1;
		x1 = x2;
		x2 = tmp;
	}
	int adr = ( y >> 3 ) * 80 + ( y & 7 ) * 0x800;
	for ( int x = x1; x <= x2; x++ )
	{
		int oct = x >> 2;
		int pix = TabOct[ col ] >> ( x & 3 );
		int masque = ( TabOct[ 3 ] >> ( x & 3 ) ) ^ 0xFF;
		BufMem[ adr + oct ] &= masque;
		BufMem[ adr + oct ] |= pix;
	}
}


static void  FillTriangle( int x1, int y1, int x2, int y2, int x3, int y3, int col )
{
	int tmp;

	if ( y1 > y2 )
	{
		tmp = y1;
		y1 = y2;
		y2 = tmp;
		tmp = x1;
		x1 = x2;
		x2 = tmp;
	}
	if ( y2 > y3 )
	{
		tmp = y2;
		y2 = y3;
		y3 = tmp;
		tmp = x2;
		x2 = x3;
		x3 = tmp;
	}
	if ( y1 > y3 )
	{
		tmp = y1;
		y1 = y3;
		y3 = tmp;
		tmp = x1;
		x1 = x3;
		x3 = tmp;
	}
	int dx1 = x3 - x1;
	int dy1 = y3 - y1;
	int sgn1 = dx1 == 0 ? 0 : dx1 > 0 ? 1 : -1;
	dx1 = abs( dx1 );
	int err1 = 0;
	int dx2 = x2 - x1;
	int dy2 = y2 - y1;
	int sgn2 = dx2 == 0 ? 0 : dx2 > 0 ? 1 : -1;
	dx2 = abs( dx2 );
	int err2 = 0;
	int xl = x1;
	int xr = ( y1 == y2 ) ? x2 : x1;

	MemCpcToHost( 0xC000, BufMem, 0x4000 );
	for ( int y = y1; y < y3; y++ )
	{
		DrawLigneHor( BufMem, xl, xr, y, col );
		err1 += dx1;
		while( err1 > dy1 )
		{
			xl += sgn1;
			err1 -= dy1;
		}
		if ( y == y2 )
		{
			dx2 = x3 - x2;
			dy2 = y3 - y2;
			sgn2 = dx2 == 0 ? 0 : dx2 > 0 ? 1 : -1;
			dx2 = abs( dx2 );
			err2 = 0;
		}
		err2 += dx2;
		while( err2 > dy2 )
		{
			xr += sgn2;
			err2 -= dy2;
		}
	}
	MemHostToCpc( BufMem, 0xC000, 0x4000 );
}


/********************************************************* !NAME! **************
* Nom : ED_00
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\\/FLE!, ligne : !./LN!
*
* Description : Emulation spéciale code op. ED 00 (info PC-CPC)
*
* Résultat    : 4
*
* Variables globales modifiées : 
*
********************************************************** !0! ****************/
int ED_00( void )
{
	if ( RomPack )
	{
		Log( MODULENAME, "ED_00 (InitRomPack)", LOG_DEBUG );
		RegA = 0xAA;
	}
	return( 2 );
}


/********************************************************* !NAME! **************
* Nom : ED_01
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\\/FLE!, ligne : !./LN!
*
* Description : Emulation spéciale code op. ED 01 (PackScr)
*
* Résultat    : 4
*
* Variables globales modifiées : 
*
********************************************************** !0! ****************/
int ED_01( void )
{
	int ad;

	if ( RomPack )
	{
		Log( MODULENAME, "ED_01 (PackScr)", LOG_DEBUG );
		OldVgaRom = Cpc.VGARom;
		WriteVGA( Cpc.VGARom | ROMSUP_OFF );
		ad = PackScreen( RegHL, ConvLigne );
		Poke8Ext( RegIX, ( UBYTE )ad );
		Poke8Ext( ( USHORT )( RegIX + 1 ), ( UBYTE )( ad >> 8 ) );
		WriteVGA( OldVgaRom );
	}
	return( 2 );
}


/********************************************************* !NAME! **************
* Nom : ED_02
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\\/FLE!, ligne : !./LN!
*
* Description : Emulation spéciale code op. ED 02 (PackWin)
*
* Résultat    : 4
*
* Variables globales modifiées : 
*
********************************************************** !0! ****************/
int ED_02( void )
{
	int ad;

	if ( RomPack )
	{
		Log( MODULENAME, "ED_02 (PackWin)", LOG_DEBUG );
		OldVgaRom = Cpc.VGARom;
		WriteVGA( Cpc.VGARom | ROMSUP_OFF );
		ad = PackWindow( RegE, RegD, RegC, RegB, RegHL, ConvLigne, ConvCoord );
		Poke8Ext( RegIX, ( UBYTE )ad );
		Poke8Ext( ( USHORT )( RegIX + 1 ), ( UBYTE )( ad >> 8 ) );
		WriteVGA( OldVgaRom );
	}
	return( 2 );
}


/********************************************************* !NAME! **************
* Nom : ED_03
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\\/FLE!, ligne : !./LN!
*
* Description : Emulation spéciale code op. ED 03 (DepkScr)
*
* Résultat    : 4
*
* Variables globales modifiées : 
*
********************************************************** !0! ****************/
int ED_03( void )
{
	if ( RomPack )
	{
		Log( MODULENAME, "ED_03 (DepkScr)", LOG_DEBUG );
		OldVgaRom = Cpc.VGARom;
		WriteVGA( Cpc.VGARom | ROMSUP_OFF );
		DepackScreen( RegHL, ConvLigne );
		WriteVGA( OldVgaRom );
	}
	return( 2 );
}


/********************************************************* !NAME! **************
* Nom : ED_04
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\\/FLE!, ligne : !./LN!
*
* Description : Emulation spéciale code op. ED 01 (DepkWin)
*
* Résultat    : 4
*
* Variables globales modifiées : 
*
********************************************************** !0! ****************/
int ED_04( void )
{
	if ( RomPack )
	{
		Log( MODULENAME, "ED_04 (DepkWin)", LOG_DEBUG );
		OldVgaRom = Cpc.VGARom;
		WriteVGA( Cpc.VGARom | ROMSUP_OFF );
		DepackWindow( RegHL, ConvLigne );
		WriteVGA( OldVgaRom );
	}
	return( 2 );
}


/********************************************************* !NAME! **************
* Nom : ED_05
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\\/FLE!, ligne : !./LN!
*
* Description : Emulation spéciale code op. ED 05 (PackWinDC)
*
* Résultat    : 4
*
* Variables globales modifiées : 
*
********************************************************** !0! ****************/
int ED_05( void )
{
	int ad;

	if ( RomPack )
	{
		Log( MODULENAME, "ED_05 (PackWinDC)", LOG_DEBUG );
		OldVgaRom = Cpc.VGARom;
		WriteVGA( Cpc.VGARom | ROMSUP_OFF );
		ad = PackWinDC( RegHL, FALSE, ConvLigne, ConvCoord );
		Poke8Ext( RegIX, ( UBYTE )ad );
		Poke8Ext( ( USHORT )( RegIX + 1 ), ( UBYTE )( ad >> 8 ) );
		Poke8Ext( ( USHORT )( RegIX + 2 ), ( UBYTE )( ad >> 16 ) );
		Poke8Ext( ( USHORT )( RegIX + 3 ), ( UBYTE )( ad >> 24 ) );
		WriteVGA( OldVgaRom );
	}
	return( 2 );
}


/********************************************************* !NAME! **************
* Nom : ED_06
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\\/FLE!, ligne : !./LN!
*
* Description : Emulation spéciale code op. ED 06 (PackBuff)
*
* Résultat    : 4
*
* Variables globales modifiées : 
*
********************************************************** !0! ****************/
int ED_06( void )
{
	int ad;

	if ( RomPack )
	{
		Log( MODULENAME, "ED_06 (PackBuff)", LOG_DEBUG );
		OldVgaRom = Cpc.VGARom;
		WriteVGA( Cpc.VGARom | ROMSUP_OFF );
		ad = PackBuffer( RegHL, RegDE, RegBC );
		Poke8Ext( RegIX, ( UBYTE )ad );
		Poke8Ext( ( USHORT )( RegIX + 1 ), ( UBYTE )( ad >> 8 ) );
		WriteVGA( OldVgaRom );
	}
	return( 2 );
}


/********************************************************* !NAME! **************
* Nom : ED_07
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\\/FLE!, ligne : !./LN!
*
* Description : Emulation spéciale code op. ED 07 (DepkBuff)
*
* Résultat    : 4
*
* Variables globales modifiées : 
*
********************************************************** !0! ****************/
int ED_07( void )
{
	int ad;

	if ( RomPack )
	{
		Log( MODULENAME, "ED_07 (DepkBuff)", LOG_DEBUG );
		OldVgaRom = Cpc.VGARom;
		WriteVGA( Cpc.VGARom | ROMSUP_OFF );
		ad = DepackBuffer( RegHL, RegDE );
		Poke8Ext( RegIX, ( UBYTE )ad );
		Poke8Ext( ( USHORT )( RegIX + 1 ), ( UBYTE )( ad >> 8 ) );
		WriteVGA( OldVgaRom );
	}
	return( 2 );
}


/********************************************************* !NAME! **************
* Nom : ED_08
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\\/FLE!, ligne : !./LN!
*
* Description : Emulation spéciale code op. ED 08 (PackWinDiff)
*
* Résultat    : 4
*
* Variables globales modifiées : 
*
********************************************************** !0! ****************/
int ED_08( void )
{
	int ad;

	if ( RomPack )
	{
		Log( MODULENAME, "ED_08 (PackWinDiff)", LOG_DEBUG );
		OldVgaRom = Cpc.VGARom;
		WriteVGA( Cpc.VGARom | ROMSUP_OFF );
		ad = PackWinDC( RegHL, TRUE, ConvLigne, ConvCoord );
		Poke8Ext( RegIX, ( UBYTE )ad );
		Poke8Ext( ( USHORT )( RegIX + 1 ), ( UBYTE )( ad >> 8 ) );
		Poke8Ext( ( USHORT )( RegIX + 2 ), ( UBYTE )( ad >> 16 ) );
		Poke8Ext( ( USHORT )( RegIX + 3 ), ( UBYTE )( ad >> 24 ) );
		WriteVGA( OldVgaRom );
	}
	return( 2 );
}


/********************************************************* !NAME! **************
* Nom : ED_09
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\\/FLE!, ligne : !./LN!
*
* Description : Emulation spéciale code op. ED 09 (InitBuffDiff)
*
* Résultat    : 4
*
* Variables globales modifiées : 
*
********************************************************** !0! ****************/
int ED_09( void )
{
	if ( RomPack )
	{
		Log( MODULENAME, "ED_09 (InitBuffDiff)", LOG_DEBUG );
		OldVgaRom = Cpc.VGARom;
		WriteVGA( Cpc.VGARom | ROMSUP_OFF );
		InitBuffDiff( RegHL );
		WriteVGA( OldVgaRom );
	}
	return( 2 );
}


/********************************************************* !NAME! **************
* Nom : ED_0A
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\\/FLE!, ligne : !./LN!
*
* Description : Emulation spéciale code op. ED 0A (ConvertBitmap)
*
* Résultat    : 4
*
* Variables globales modifiées : 
*
********************************************************** !0! ****************/
int ED_0A( void )
{
	// ###
	return( 2 );
}


/********************************************************* !NAME! **************
* Nom : ED_0B
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\\/FLE!, ligne : !./LN!
*
* Description : Emulation spéciale code op. ED 0B (ConvLigne)
*
* Résultat    : 4
*
* Variables globales modifiées : ConvLigne, ConvCoord
*
********************************************************** !0! ****************/
int ED_0B( void )
{
	if ( RomPack )
	{
		Log( MODULENAME, "ED_0B (ConvLigne)", LOG_DEBUG );
		ConvLigne = RegA;
		ConvCoord = RegB;
	}
	return( 2 );
}


/********************************************************* !NAME! **************
* Nom : ED_0C
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\\/FLE!, ligne : !./LN!
*
* Description : Emulation spéciale code op. ED 0C (SauveBitmap)
*
* Résultat    : 4
*
* Variables globales modifiées : 
*
********************************************************** !0! ****************/
int ED_0C( void )
{
	if ( RomPack )
	{
		Log( MODULENAME, "ED_0C (SauveBitmap)", LOG_DEBUG );
		SauveBitmap();
	}
	return( 2 );
}


/********************************************************* !NAME! **************
* Nom : ED_0D
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\\/FLE!, ligne : !./LN!
*
* Description : Emulation spéciale code op. ED 0D (Turbo On/Off)
*
* Résultat    : 4
*
* Variables globales modifiées : 
*
********************************************************** !0! ****************/
int ED_0D( void )
{
	if ( RomPack )
	{
		Log( MODULENAME, "ED_0D (Turbo On/Off)", LOG_DEBUG );
		Turbo = RegA;
	}
	return( 2 );
}


/********************************************************* !NAME! **************
* Nom : ED_0E
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\\/FLE!, ligne : !./LN!
*
* Description : Emulation spéciale code op. ED 0E (ReadSnap)
*
* Résultat    : 4
*
* Variables globales modifiées : 
*
********************************************************** !0! ****************/
int ED_0E( void )
{
	if ( RomPack )
	{
		Log( MODULENAME, "ED_0E (ReadSnap)", LOG_DEBUG );
		GetName( RegHL, RegB, Nom );
		LireSnap( Nom );
	}
	return( 2 );
}


/********************************************************* !NAME! **************
* Nom : ED_0F
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\\/FLE!, ligne : !./LN!
*
* Description : Emulation spéciale code op. ED 0F (SaveSnap)
*
* Résultat    : 4
*
* Variables globales modifiées : 
*
********************************************************** !0! ****************/
int ED_0F( void )
{
	if ( RomPack )
	{
		Log( MODULENAME, "ED_0F (SaveSnap)", LOG_DEBUG );
		GetName( RegHL, RegB, Nom );
		SauveSnap( Nom );
	}
	return( 2 );
}


/********************************************************* !NAME! **************
* Nom : ED_10
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\\/FLE!, ligne : !./LN!
*
* Description : Emulation spéciale code op. ED 10 (Debug On)
*
* Résultat    : 4
*
* Variables globales modifiées : 
*
********************************************************** !0! ****************/
int ED_10( void )
{
	if ( RomPack )
	{
		Log( MODULENAME, "ED_10 (DebugOn)", LOG_DEBUG );
		DebugMode = 1;
	}
	return( 2 );
}


/********************************************************* !NAME! **************
* Nom : ED_11
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\\/FLE!, ligne : !./LN!
*
* Description : Emulation spéciale code op. ED 11 (SetDskFile)
*
* Résultat    : 4
*
* Variables globales modifiées : 
*
********************************************************** !0! ****************/
int ED_11( void )
{
	int Drive = 0;

	if ( RomPack )
	{
		Log( MODULENAME, "ED_11 (SetDskFile)", LOG_DEBUG );
		GetName( RegHL, RegB, Nom );
		if ( * Nom )
			LoadDSK( Nom, Drive );
	}
	return( 2 );
}


/********************************************************* !NAME! **************
* Nom : ED_12
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\\/FLE!, ligne : !./LN!
*
* Description : Emulation spéciale code op. ED 12 (SaveAvi)
*
* Résultat    : 4
*
* Variables globales modifiées : 
*
********************************************************** !0! ****************/
int ED_12( void )
{
	if ( RomPack )
	{
		Log( MODULENAME, "ED_12 (SaveAvi)", LOG_DEBUG );
		GetName( RegHL, RegB, Nom );
		if ( ! WriteAviStarted() && * Nom )
			WriteAviInit( hWnd, Nom, DISPLAY_X, DISPLAY_Y );
	}
	return( 2 );
}


/********************************************************* !NAME! **************
* Nom : ED_13
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\\/FLE!, ligne : !./LN!
*
* Description : Emulation spéciale code op. ED 13 (EndAvi)
*
* Résultat    : 4
*
* Variables globales modifiées : 
*
********************************************************** !0! ****************/
int ED_13( void )
{
	if ( RomPack )
	{
		Log( MODULENAME, "ED_13 (EndAvi)", LOG_DEBUG );
		if ( WriteAviStarted() )
			WriteAviEnd( hWnd );
	}
	return( 2 );
}


/********************************************************* !NAME! **************
* Nom : ED_14
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\\/FLE!, ligne : !./LN!
*
* Description : Emulation spéciale code op. ED 14 (SaveYM)
*
* Résultat    : 4
*
* Variables globales modifiées : 
*
********************************************************** !0! ****************/
int ED_14( void )
{
	if ( RomPack )
	{
		Log( MODULENAME, "ED_14 (SaveYM)", LOG_DEBUG );
		GetName( RegHL, RegB, Nom );
		if ( ! WriteYMStarted() && * Nom )
			WriteYMInit( hWnd, Nom, RegDE );
	}
	return( 2 );
}


/********************************************************* !NAME! **************
* Nom : ED_15
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\\/FLE!, ligne : !./LN!
*
* Description : Emulation spéciale code op. ED 15 (EndYM)
*
* Résultat    : 4
*
* Variables globales modifiées : 
*
********************************************************** !0! ****************/
int ED_15( void )
{
	if ( RomPack )
	{
		Log( MODULENAME, "ED_15 (EndYM)", LOG_DEBUG );
		if ( WriteYMStarted() )
			WriteYMEnd( hWnd );
	}
	return( 2 );
}


/********************************************************* !NAME! **************
* Nom : ED_16
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\\/FLE!, ligne : !./LN!
*
* Description : Emulation spéciale code op. ED 16 (SetDirectDir)
*
* Résultat    : 4
*
* Variables globales modifiées : 
*
********************************************************** !0! ****************/
int ED_16( void )
{
	if ( RomPack )
	{
		Log( MODULENAME, "ED_16 (SetDirectDir)", LOG_DEBUG );
		GetName( RegHL, RegB, Nom );
		if ( * Nom )
		{
			strcpy( DirEmuDisc, Nom );
			FlushCloseIn();
			FlushCloseOut();
			NewDiscOn = TRUE;
			AfficheInfos();
		}
	}
	return( 2 );
}


/********************************************************* !NAME! **************
* Nom : ED_17
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\\/FLE!, ligne : !./LN!
*
* Description : Emulation spéciale code op. ED 17 (SymbVidOpen)
*
* Résultat    : 4
*
* Variables globales modifiées : 
*
********************************************************** !0! ****************/
int ED_17( void )
{
	DWORD BytesRead;

	if ( RomPack )
	{
		FLAGS |= FLAG_Z;
		if ( SymbVidFile != INVALID_HANDLE_VALUE )
			ED_19();

		Log( MODULENAME, "ED_17 (SymbVidOpen)", LOG_DEBUG );
		GetName( RegHL, RegB, Nom );
		if ( * Nom )
		{
			SymbVidFile = CreateFile( Nom
				, GENERIC_READ
				, 0
				, NULL
				, OPEN_EXISTING
				, 0
				, NULL
				);
			if ( SymbVidFile != INVALID_HANDLE_VALUE )
			{
				FLAGS &= ~FLAG_Z;
				ReadFile( SymbVidFile, SymbVidBuffer, 0x200, &BytesRead, NULL );
				SymbVidSizeX = ( SymbVidBuffer[ 0x0E ] + ( SymbVidBuffer[ 0x0F ] << 8 ) ) >> 2;
				SymbVidSizeY = SymbVidBuffer[ 0x10 ] + ( SymbVidBuffer[ 0x11 ] << 8 );
				SymbVidBufSize = SymbVidBuffer[ 0x33 ] + ( SymbVidBuffer[ 0x34 ] << 8 );
			}
		}
	}
	return( 2 );
}


/********************************************************* !NAME! **************
* Nom : ED_18
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\\/FLE!, ligne : !./LN!
*
* Description : Emulation spéciale code op. ED 18 (SymbVidRead)
*
* Résultat    : 4
*
* Variables globales modifiées : 
*
********************************************************** !0! ****************/
int ED_18( void )
{
	DWORD BytesRead;

	if ( RomPack )
	{
		FLAGS |= FLAG_Z;
		if ( SymbVidFile != INVALID_HANDLE_VALUE )
		{
			Log( MODULENAME, "ED_18 (SymbVidRead)", LOG_DEBUG );
			int Size = min( sizeof( SymbVidBuffer ), SymbVidBufSize );
			ReadFile( SymbVidFile, SymbVidBuffer, Size, &BytesRead, NULL );
			RegA = ( UBYTE )BytesRead;
			MemHostToCpc( SymbVidBuffer, RegHL, Size );
			FLAGS &= ~FLAG_Z;
		}
	}
	return( 2 );
}


/********************************************************* !NAME! **************
* Nom : ED_19
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\\/FLE!, ligne : !./LN!
*
* Description : Emulation spéciale code op. ED 19 (SymbVidPlay)
*
* Résultat    : 4
*
* Variables globales modifiées : 
*
********************************************************** !0! ****************/
int ED_19( void )
{
	DWORD BytesRead;

	if ( RomPack )
	{
		FLAGS |= FLAG_Z;
		if ( SymbVidFile != INVALID_HANDLE_VALUE )
		{
			Log( MODULENAME, "ED_19 (SymbVidPlay)", LOG_DEBUG );
			int Size = min( sizeof( SymbVidBuffer ), SymbVidBufSize );
			ReadFile( SymbVidFile, SymbVidBuffer, Size, &BytesRead, NULL );
			RegA = ( UBYTE )BytesRead;
			for ( int l = 0; l < SymbVidSizeY; l++ )
			{
				int AdrCPC = ( l >> 3 ) * 80 + ( l & 7 ) * 0x800;
				MemHostToCpc( &SymbVidBuffer[ l * SymbVidSizeX ]
				, ( USHORT )( 0xC000 + AdrCPC )
					, SymbVidSizeX
					);
			}

			FLAGS &= ~FLAG_Z;
		}
	}
	return( 2 );
}


/********************************************************* !NAME! **************
* Nom : ED_1A
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\\/FLE!, ligne : !./LN!
*
* Description : Emulation spéciale code op. ED 1A (SymbVidClose)
*
* Résultat    : 4
*
* Variables globales modifiées : 
*
********************************************************** !0! ****************/
int ED_1A( void )
{
	if ( RomPack )
	{
		FLAGS |= FLAG_Z;
		if ( SymbVidFile != INVALID_HANDLE_VALUE )
		{
			Log( MODULENAME, "ED_1A (SymbVidClose)", LOG_DEBUG );
			FLAGS &= ~FLAG_Z;
			CloseHandle( SymbVidFile );
			SymbVidFile = INVALID_HANDLE_VALUE;
		}
	}
	return( 2 );
}


/********************************************************* !NAME! **************
* Nom : ED_1B
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\\/FLE!, ligne : !./LN!
*
* Description : Emulation spéciale code op. ED 1B (ProfileStart)
*
* Résultat    : 2
*
* Variables globales modifiées : 
*
********************************************************** !0! ****************/
int ED_1B( void )
{
	// ####
	return( 2 );
}


/********************************************************* !NAME! **************
* Nom : ED_1C
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\\/FLE!, ligne : !./LN!
*
* Description : Emulation spéciale code op. ED 1C (ProfileStop)
*
* Résultat    : 2
*
* Variables globales modifiées : 
*
********************************************************** !0! ****************/
int ED_1C( void )
{
	// ####
	return( 2 );
}


/********************************************************* !NAME! **************
* Nom : ED_1D
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\\/FLE!, ligne : !./LN!
*
* Description : Emulation spéciale code op. ED 1D (SetNewDisc)
*
* Résultat    : 2
*
* Variables globales modifiées : 
*
********************************************************** !0! ****************/
int ED_1D( void )
{
	if ( RomPack )
	{
		Log( MODULENAME, "ED_1D (SetNewDisc)", LOG_DEBUG );
		FlushCloseIn();
		FlushCloseOut();
		NewDiscOn = RegA;
		AfficheInfos();
	}
	return( 2 );
}


/********************************************************* !NAME! **************
* Nom : ED_1E
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\\/FLE!, ligne : !./LN!
*
* Description : Emulation spéciale code op. ED 1E (DrawCadreWin)
*
* Résultat    : 2
*
* Variables globales modifiées : 
*
********************************************************** !0! ****************/
int ED_1E( void )
{
	if ( RomPack )
	{
		Log( MODULENAME, "ED_1E (DrawCadreWin)", LOG_DEBUG );

		OldVgaRom = Cpc.VGARom;
		WriteVGA( Cpc.VGARom | ROMSUP_OFF );
		MemCpcToHost( 0xC000, BufMem, 0x4000 );

		// Lignes horizontales
		for ( int c = RegB; c <= RegD; c++ )
		{
			BufMem[ c + ( RegC >> 3 ) * 80 + ( RegC & 7 ) * 0x800 ] ^= 0xFF;
			if ( RegC != RegE )
				BufMem[ c + ( RegE >> 3 ) * 80 + ( RegE & 7 ) * 0x800 ] ^= 0xFF;
		}

		// Lignes verticales
		for ( int l = RegC + 1; l < RegE; l++ )
		{
			int adr = ( l >> 3 ) * 80 + ( l & 7 ) * 0x800;
			BufMem[ RegB + adr] ^= 0xFF;
			if ( RegB != RegD )
				BufMem[ RegD + adr ] ^= 0xFF;
		}

		MemHostToCpc( BufMem, 0xC000, 0x4000 );
		WriteVGA( OldVgaRom );
	}
	return( 2 );
}


/********************************************************* !NAME! **************
* Nom : ED_1F
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\\/FLE!, ligne : !./LN!
*
* Description : Emulation spéciale code op. ED 1F (Assemble)
*
* Résultat    : 2
*
* Variables globales modifiées : 
*
********************************************************** !0! ****************/
int ED_1F( void )
{
	if ( RomPack )
	{
		FLAGS |= FLAG_Z;
		Log( MODULENAME, "ED_1F (Assemble)", LOG_DEBUG );
		GetName( RegHL, RegB, Nom );
		if ( * Nom )
		{
			AsmOutput p;
			p.Assemble( Nom );
		}
	}
	return( 2 );
}


/********************************************************* !NAME! **************
* Nom : ED_20
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\\/FLE!, ligne : !./LN!
*
* Description : Emulation spéciale code op. ED 20 (SetCoordDiff)
*
* Résultat    : 2
*
* Variables globales modifiées : 
*
********************************************************** !0! ****************/
int ED_20( void )
{
	if ( RomPack )
	{
		Log( MODULENAME, "ED_20 (SetCoordDiff)", LOG_DEBUG );
		OldVgaRom = Cpc.VGARom;
		WriteVGA( Cpc.VGARom | ROMSUP_OFF );
		SetCoordDiff( RegB, RegC, RegD, RegE );
		WriteVGA( OldVgaRom );
	}
	return( 2 );
}


/********************************************************* !NAME! **************
* Nom : ED_21
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\\/FLE!, ligne : !./LN!
*
* Description : Emulation spéciale code op. ED 21 (FillTriangle)
*
* Résultat    : 2
*
* Variables globales modifiées : 
*
********************************************************** !0! ****************/
int ED_21( void )
{
	if ( RomPack )
	{
		Log( MODULENAME, "ED_21 (FillTriangle)", LOG_DEBUG );

		OldVgaRom = Cpc.VGARom;
		WriteVGA( Cpc.VGARom | ROMSUP_OFF );
		FillTriangle( RegB, RegC, RegD, RegE, RegH, RegL, RegA );
		WriteVGA( OldVgaRom );
	}
	return( 2 );
}

// Réserves...
int ED_22( void ) { return( 2 ); }
int ED_23( void ) { return( 2 ); }
int ED_24( void ) { return( 2 ); }
int ED_25( void ) { return( 2 ); }
int ED_26( void ) { return( 2 ); }
int ED_27( void ) { return( 2 ); }
int ED_28( void ) { return( 2 ); }
int ED_29( void ) { return( 2 ); }
int ED_2A( void ) { return( 2 ); }
int ED_2B( void ) { return( 2 ); }
int ED_2C( void ) { return( 2 ); }
int ED_2D( void ) { return( 2 ); }
int ED_2E( void ) { return( 2 ); }
int ED_2F( void ) { return( 2 ); }
int ED_30( void ) { return( 2 ); }
int ED_31( void ) { return( 2 ); }
int ED_32( void ) { return( 2 ); }
int ED_33( void ) { return( 2 ); }
int ED_34( void ) { return( 2 ); }
int ED_35( void ) { return( 2 ); }
int ED_36( void ) { return( 2 ); }
int ED_37( void ) { return( 2 ); }
int ED_38( void ) { return( 2 ); }
int ED_39( void ) { return( 2 ); }
int ED_3A( void ) { return( 2 ); }
int ED_3B( void ) { return( 2 ); }
int ED_3C( void ) { return( 2 ); }
int ED_3D( void ) { return( 2 ); }
int ED_3E( void ) { return( 2 ); }
int ED_3F( void ) { return( 2 ); }

