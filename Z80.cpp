/******************************************************************************/
/* Configuration pour l'archivage des différents éléments du fichier source   */
/******************************************************************************/
// !CONFIG!=/L/* /R/* /W"* Nom : "
// Définition du système       !CONFIG!=/V1!EMULATEUR CPC!
// Définition du sous système  !CONFIG!=/V2!PC-CPC!
// Définition du sous ensemble !CONFIG!=/V3!Chips!
// Définition du module        !CONFIG!=/V4!CPU Z80!
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
*  Fichier     : Z80.C                 | Version : 0.1as
* ------------------------------------------------------------------------------
*  Date        : 05/11/2002            | Auteur  : L.DEPLANQUE
* ------------------------------------------------------------------------------
*  Description : Emulation des instructions du Z80
*
* ------------------------------------------------------------------------------
*  Historique  :
*           Date           |         Auteur          |       Description
* ------------------------------------------------------------------------------
*  05/11/2002              | L.DEPLANQUE             | creation
* ------------------------------------------------------------------------------
*  13/11/2002              | L.DEPLANQUE             | Optimisation PEEK16() et
*                          |                         | POKE16(), ajout #defines
*                          |                         | USE_16_BIT, FAST_16_BIT.
*                          |                         | Optimisation LDIR et LDDR
* ------------------------------------------------------------------------------
*  14/11/2002              | L.DEPLANQUE             | Optimisation CPIR et CPDR
* ------------------------------------------------------------------------------
*  20/02/2003              | L.DEPLANQUE             | Version 0.1w :
*                          |                         | Optimisation PEEK et POKE
*                          |                         | Dépendant de VC++
* ------------------------------------------------------------------------------
*  21/01/2004              | L.DEPLANQUE             | Version 0.1x :
*                          |                         | Gestion nouvelles
*                          |                         | routines disc
* ------------------------------------------------------------------------------
*  30/03/2004              | L.DEPLANQUE             | Version 0.1z :
*                          |                         | Optimisations fonctions
*                          |                         | émulation Z80 : retour
*                          |                         | du nombre de cycles
*                          |                         | plutôt que incrément
*                          |                         | variable globale
* ------------------------------------------------------------------------------
*  21/04/2004              | L.DEPLANQUE             | Version 0.1aa :
*                          |                         | Correction des temps de
*                          |                         | cycle des instructions
*                          |                         | après préfixe DD et FD
*                          |                         | (sur registres IX et IY)
*                          |                         | Suppression du #define
*                          |                         | USE_16_BIT, car toujours
*                          |                         | utilisé maintenant.
* ------------------------------------------------------------------------------
*  06/08/2004              | L.DEPLANQUE             | Version 0.1ab :
*                          |                         | Ajout des instructions
*                          |                         | NEG aux OPCODES ED_4C,
*                          |                         | ED_54, ED_5C, ED_64,
*                          |                         | ED_6C, ED_74, ED_7C
* ------------------------------------------------------------------------------
*  11/12/2004              | L.DEPLANQUE             | Version 0.1ae :
*                          |                         | Ajout des instructions
*                          |                         | ED_01 à ED_07 pour
*                          |                         | gestion rom extension
*                          |                         | compactage/décompactage
* ------------------------------------------------------------------------------
*  27/11/2005              | L.DEPLANQUE             | Version 0.1ah :
*                          |                         | Ajout instruction ED_08
*                          |                         | (pour rompack)
* ------------------------------------------------------------------------------
*  18/12/2005              | L.DEPLANQUE             | Version 0.1aj :
*                          |                         | Ajout instruction ED_09
*                          |                         | (pour rompack)
* ------------------------------------------------------------------------------
*  16/02/2006              | L.DEPLANQUE             | Version 0.1ak :
*                          |                         | Ajout instruction ED_0A
*                          |                         | (pour rompack)
* ------------------------------------------------------------------------------
*  20/07/2006              | L.DEPLANQUE             | Version 0.1al :
*                          |                         | Ajout instruction ED_0B
*                          |                         | à ED_16 (pour rompack),
*                          |                         | ajout instructions DD_FD
*                          |                         | et FD_DD
* ------------------------------------------------------------------------------
*  05/09/2006              | L.DEPLANQUE             | Version 0.1am :
*                          |                         | Ajout instruction ED_17
*                          |                         | à ED_1A (pour rompack),
*                          |                         | Correction timings
*                          |                         | instructions.
* ------------------------------------------------------------------------------
*  09/03/2007              | L.DEPLANQUE             | Version 0.1an :
*                          |                         | Correction bugs dans les
*                          |                         | instructions LD A,I et
*                          |                         | LD A,R : positionnement
*                          |                         | des flags.
*                          |                         | Autorisation des 
*                          |                         | interruptions après
*                          |                         | l'instruction qui suit
*                          |                         | un EI
*                          |                         | BUG dans SLL A : il 
*                          |                         | manquait la mise à 1
*                          |                         | BUG dans NMI : le flag
*                          |                         | IFF2 n'est pas affecté
*                          |                         | par la prise en compte
*                          |                         | d'une NMI.
* ------------------------------------------------------------------------------
*  30/05/2007              | L.DEPLANQUE             | Version 0.1ao :
*                          |                         | Optimisations mineures
*                          |                         | (macros PUSH et POP)
* ------------------------------------------------------------------------------
*  08/10/2009              | L.DEPLANQUE             | Version 0.1ap :
*                          |                         | Correction temps cycle de
*                          |                         | certaines instructions
* ------------------------------------------------------------------------------
*  15/12/2010              | L.DEPLANQUE             | Version 0.1as :
*                          |                         | Simplification
*                          |                         | instruction DAA
* ------------------------------------------------------------------------------
*  15/02/2011              | L.DEPLANQUE             | Version 0.1at :
*                          |                         | Correction temps cycle de
*                          |                         | certaines instructions
*                          |                         | Gestion des flags F5 et
*                          |                         | F3 (flags non documentés)
********************************************************** !END! **************/

#include  <string.h>

#include  "types.h"
#include  "z80.h"
#include  "vga.h"
#include  "Crtc.h"
#include  "gestport.h"
#include  "config.h"
#include  "SnapShot.h"
#include  "log.h"
#include  "BreakPoints.h"
#include  "Desasm.h"
#include  "NewDisc.h"
#include  "RomPack.h"

#define     MODULENAME      "Z80"

#define MASK_14BIT      0x7FFF3FFF

#define FLAG_C          0x01
#define FLAG_N          0x02
#define FLAG_V          0x04
#define FLAG_3          0x08
#define FLAG_H          0x10
#define FLAG_5          0x20
#define FLAG_Z          0x40
#define FLAG_S          0x80

#define FLAGS_SZC       ( FLAG_S | FLAG_Z | FLAG_C )
#define FLAGS_SZV       ( FLAG_S | FLAG_Z | FLAG_V )
#define FLAGS_S53       ( FLAG_S | FLAG_5 | FLAG_3 )
#define FLAGS_SZ        ( FLAG_S | FLAG_Z )
#define FLAGS_53        ( FLAG_5 | FLAG_3 )
#define FLAGS_HC        ( FLAG_H | FLAG_C )

#define RegXY           ( char )Fetch() + *( USHORT * )PtrRegs[ 6 ]

#define IND_HL          0x06
#define IND_HL3         0x30


extern int DebugMode;

typedef int ( * pfct )( void );

extern pfct TabInstr[ 256 ], TabInstrCB[ 256 ], TabInstrED[ 256 ];

static UBYTE * Regs[ 16 ], * RegsDD[ 16 ], * RegsFD[ 16 ], ** PtrRegs;
static int LastInstr, SupIrqWaitState, memPtr, Halt = 0;

static int TabSZYX[ 256 ], TabSZYXP[ 256 ], TabV[ 512 ];

static int TabCC[ 16 ] = { FLAG_Z, FLAG_Z, 0, FLAG_Z, FLAG_C, FLAG_C, 0, FLAG_C, FLAG_V, FLAG_V, 0, FLAG_V, FLAG_S, FLAG_S, 0, FLAG_S };

/********************************************************* !NAME! **************
* Nom : Peek8Ext
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Lecture d'un octet (8 bits) depuis la mémoire du CPC (utilisée
*               depuis l'extérieur du module)
*
* Résultat    : La valeur de l'octet à l'adresse désirée
*
* Variables globales modifiées : /
*
********************************************************** !0! ****************/
UBYTE Peek8Ext( USHORT adr ) {
	UBYTE val = TabPEEK[ adr >> 14 ][ adr & MASK_14BIT ];
	Log( MODULENAME, "Peek8Ext( 0x%04X ) = 0x%02X ", adr, val, LOG_DEBUG_FULL );
	return( val );
}

/********************************************************* !NAME! **************
* Nom : Poke8Ext
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Ecriture d'un octet (8 bits) dans la mémoire du CPC (utilisée
*               depuis l'extérieur du module)
*
* Résultat    : /
*
* Variables globales modifiées : /
*
********************************************************** !0! ****************/
void Poke8Ext( USHORT adr, UBYTE val ) {
	Log( MODULENAME, "Poke8Ext( 0x%04X, 0x%02X )", adr, val, LOG_DEBUG_FULL );
	TabPOKE[ adr >> 14 ][ adr & MASK_14BIT ] = val;
}

static int Fetch( void ) {
	return TabPEEK[ RegPC >> 14 ][ RegPC++ & 0x3FFF ];
}

/********************************************************* !NAME! **************
* Nom : PEEK8
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Lecture d'un octet (8 bits) depuis la mémoire du CPC
*
* Résultat    : La valeur de l'octet à l'adresse désirée
*
* Variables globales modifiées : /
*
********************************************************** !0! ****************/
static int PEEK8( USHORT adr ) {
	SetPeekAdr( adr );
	return TabPEEK[ adr >> 14 ][ adr & 0x3FFF ];
}

/********************************************************* !NAME! **************
* Nom : POKE8
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Ecriture d'un octet (8 bits) dans la mémoire du CPC
*
* Résultat    : /
*
* Variables globales modifiées : /
*
********************************************************** !0! ****************/
static void POKE8( int adr, UBYTE val ) {
	SetPokeAdr( adr );
	TabPOKE[ adr >> 14 ][ adr & MASK_14BIT ] = val;
}

/********************************************************* !NAME! **************
* Nom : PEEK16
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Lecture d'un mot (16 bits) depuis la mémoire du CPC
*
* Résultat    : La valeur du mot à l'adresse désirée
*
* Variables globales modifiées : /
*
********************************************************** !0! ****************/
static USHORT PEEK16( int adr ) {
	int q = adr >> 14;
	adr &= MASK_14BIT;
	return( ( USHORT )( TabPEEK[ q ][ adr ] | ( TabPEEK[ q ][ adr + 1 ] << 8 ) ) );
}

/********************************************************* !NAME! **************
* Nom : POKE16
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Ecriture d'un registre 16 bits dans la mémoire du CPC
*
* Résultat    : /
*
* Variables globales modifiées : /
*
********************************************************** !0! ****************/
static void POKE16( int adr, int val ) {
	int q = adr >> 14;
	TabPOKE[ q ][ adr &= 0x3FFF ] = val;
	TabPOKE[ q ][ ++adr ] = val >> 8;
}


/********************************************************* !NAME! **************
* Nom : PUSH
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Sauvegarde un registre sur la pile
*
* Résultat    : /
*
* Variables globales modifiées : Cpc
*
********************************************************** !0! ****************/
void PUSH( USHORT Reg )
{
    RegSP -= 2;
    int q = RegSP >> 14;
    int adr = RegSP & 0x3FFF;
    TabPOKE[ q ][ adr++ ] = ( UBYTE )Reg;
    TabPOKE[ q ][ adr ] = ( UBYTE )( Reg >> 8 );
}


/* 8 Bits Load Group */
static int LD_R8( void ) {
	* PtrRegs[ ( LastInstr >> 3 ) & 0x07 ] = LastInstr >> 6 ? * PtrRegs[ LastInstr & 0x07 ] : Fetch();
	return 2 - ( LastInstr >> 6 );
}

static int LD_R_HL( void ) {
	* Regs[ ( LastInstr >> 3 ) & 0x07 ] = PEEK8( PtrRegs == Regs ? RegHL : RegXY );
	return PtrRegs == Regs ? 2 : 4;
}

static int LD_HL_R( void ) {
	POKE8( PtrRegs == Regs ? RegHL : RegXY, * Regs[ LastInstr & 0x07 ] );
	return PtrRegs == Regs ? 2 : 4;
}

static int LD_HL_N( void ) {
	POKE8( PtrRegs == Regs ? RegHL : RegXY, Fetch() );
	return PtrRegs == Regs ? 3 : 5;
}

static int LD_A_BCDE( void ) {
	RegA = PEEK8( memPtr = LastInstr & 0x10 ? RegDE : RegBC );
	memPtr++;
	return 2;
}

static int LD_A_MM( void ) {
	memPtr = PEEK16( RegPC );
	RegA = TabPEEK[ memPtr >> 14 ][ memPtr++ & 0x3FFF ];
	RegPC += 2;
	return 4;
}

static int LD_BCDE_A( void ) {
	POKE8( memPtr = LastInstr & 0x10 ? RegDE : RegBC, RegA );
	memPtr = ( ++memPtr & 0xFF ) + ( RegA << 8 );
	return 2;
}

static int LD_MM_A( void ) {
	POKE8( memPtr = PEEK16( RegPC ), RegA );
	memPtr = ( ++memPtr & 0xFF ) | ( RegA << 8 );
	RegPC += 2;
	return 4;
}

static int LD_I_A( void ) {
	RegI = RegA;
	return SupIrqWaitState = 2;
}

static int LD_R_A( void ) {
	RegR = RegA;
	return SupIrqWaitState = 2;
}

static int LD_A_I( void ) {
	RegAF = ( RegI << 8 ) | TabSZYX[ RegI ] | Cpc.IFF2 | ( FLAGS & FLAG_C );
	return SupIrqWaitState = 2;
}

static int LD_A_R( void ) {
	RegAF = ( RegR << 8 ) | TabSZYX[ RegR ] | Cpc.IFF2 | ( FLAGS & FLAG_C );
	return SupIrqWaitState = 2;
}

/* 16 Bits Load Group */
static int LD_RR_NN( void ) {
	*( USHORT * )PtrRegs[ ( LastInstr >> 4 ) + 8 ] = PEEK16( RegPC );
	RegPC += 2;
	return 3;
}

static int LD_RR_MM( void ) {
	memPtr = PEEK16( RegPC );
	*( USHORT * )PtrRegs[ ( LastInstr >> 4 ) + 4 ] = PEEK16( memPtr++ );
	RegPC += 2;
	return 5;
}

static int LD_MM_RR( void ) {
	memPtr = PEEK16( RegPC );
	POKE16( memPtr++, *( USHORT * )PtrRegs[ ( LastInstr >> 4 ) + 4 ] );
	RegPC += 2;
	return 5;
}

static int LD_SP_HL( void ) {
	RegSP = *( USHORT * )PtrRegs[ 6 ];
	return 2;
}

static int PUSH_SS( void ) {
	RegSP -= 2;
	POKE16( RegSP, *( USHORT * )PtrRegs[ LastInstr >> 4 ] );
	return 4;
}

static int POP_SS( void ) {
	*( USHORT * )PtrRegs[ LastInstr >> 4 ] = PEEK16( RegSP );
	RegSP += 2;
	SupIrqWaitState = 1;
	return 3;
}

/* Exchange, Block Transfer and Search Groups */
static int EX_DE_HL( void ) {
	int x = RegDE;
	RegDE = RegHL;
	RegHL = x;
	return 1;
}

static int EX_AF_AF_PRIME( void ) {
	int x = RegAF;
	RegAF = Reg_AF;
	Reg_AF = x;
	return 1;
}

static int EXX( void ) {
	static int tmp[ 4 ];

	memcpy( tmp, &RegBC, 6 );
	memcpy( &RegBC, &Cpc._BC, 6 );
	memcpy( &Cpc._BC, &tmp, 6 );
	return 1;
}

static int EX_SP_HL( void ) {
	memPtr = PEEK16( RegSP );
	POKE16( RegSP, *( USHORT * )PtrRegs[ 6 ] );
	*( USHORT * )PtrRegs[ 6 ] = memPtr;
	SupIrqWaitState = 2;
	return 6;
}

static int LDIx( void ) {
	int x = ( TabPOKE[ RegDE >> 14 ][ RegDE++ & 0x3FFF ] = TabPEEK[ RegHL >> 14 ][ RegHL++ & 0x3FFF ] ) + RegA;
	FLAGS = ( FLAGS & FLAGS_SZC ) | ( --RegBC ? FLAG_V : 0 ) | ( x & FLAG_3 ) | ( ( x << 4 ) & FLAG_5 );
	SupIrqWaitState = 2;
	if ( RegBC && ( LastInstr & 0x10 ) ) {
		memPtr = --RegPC;
		RegPC--;
		return 5;
	}
	return 4;
}

static int LDDx( void ) {
	int x = ( TabPOKE[ RegDE >> 14 ][ RegDE-- & 0x3FFF ] = TabPEEK[ RegHL >> 14 ][ RegHL-- & 0x3FFF ] ) + RegA;
	FLAGS = ( FLAGS & FLAGS_SZC ) | ( --RegBC ? FLAG_V : 0 ) | ( x & FLAG_3 ) | ( ( x << 4 ) & FLAG_5 );
	SupIrqWaitState = 2;
	if ( RegBC && ( LastInstr & 0x10 ) ) {
		memPtr = --RegPC;
		RegPC--;
		return 5;
	}
	return 4;
}

static int CPIx_CPDx( void ) {
	int x = TabPEEK[ RegHL >> 14 ][ RegHL & 0x3FFF ], z = RegA - x, y = ( RegA ^ x ^ z ) & FLAG_H;
	x = z - ( y >> 4 );
	FLAGS = y | ( ( x << 4 ) & FLAG_5 ) | ( x & FLAG_3 ) | ( TabSZYX[ z & 0xFF ] & FLAGS_SZ ) | ( --RegBC ? FLAG_V : 0 ) | FLAG_N | ( FLAGS & FLAG_C );
	RegHL += LastInstr & 0x08 ? -1 : 1;
	memPtr += LastInstr & 0x08 ? -1 : 1;
	SupIrqWaitState = 1;
	if ( z && RegBC && ( LastInstr & 0x10 ) ) {
		memPtr = --RegPC;
		RegPC--;
		return 5;
	}
	return 3;
}

/* 8 Bit Arithmetic and Logical Group */
static int ADD_R8( void ) {
	int x = LastInstr & 0x40 ? Fetch() : ( LastInstr & 0x07 ) == IND_HL ? PEEK8( PtrRegs == Regs ? RegHL : RegXY ) : * PtrRegs[ LastInstr & 0x07 ], z = RegA + x + ( FLAGS & ( LastInstr & 0x08 ) >> 3 ), y = RegA ^ x ^ z;
	FLAGS = TabSZYX[ RegA = z ] | TabV[ y ];
	return LastInstr & 0x40 ? 2 : ( LastInstr & 0x07 ) == IND_HL ? PtrRegs == Regs ? 2 : 4 : 1;
}

static int SUB_R8( void ) {
	int x = LastInstr & 0x40 ? Fetch() : ( LastInstr & 0x07 ) == IND_HL ? PEEK8( PtrRegs == Regs ? RegHL : RegXY ) : * PtrRegs[ LastInstr & 0x07 ], z = RegA - x - ( FLAGS & ( LastInstr & 0x08 ) >> 3 ), y = RegA ^ x ^ z;
	FLAGS = FLAG_N | TabSZYX[ RegA = z ] | TabV[ y & 0x1FF ];
	return LastInstr & 0x40 ? 2 : ( LastInstr & 0x07 ) == IND_HL ? PtrRegs == Regs ? 2 : 4 : 1;
}

static int CP_R8( void ) {
	int x = LastInstr & 0x40 ? Fetch() : ( LastInstr & 0x07 ) == IND_HL ? PEEK8( PtrRegs == Regs ? RegHL : RegXY ) : * PtrRegs[ LastInstr & 0x07 ], z = RegA - x;
	FLAGS = FLAG_N | ( TabSZYX[ z & 0xFF ] & FLAGS_SZ ) | ( x & FLAGS_53 ) | TabV[ ( RegA ^ x ^ z ) & 0x1FF ];
	return LastInstr & 0x40 ? 2 : ( LastInstr & 0x07 ) == IND_HL ? PtrRegs == Regs ? 2 : 4 : 1;
}

static int LOG_R8( void ) {
	int x = LastInstr & 0x40 ? Fetch() : ( LastInstr & 0x07 ) == IND_HL ? PEEK8( PtrRegs == Regs ? RegHL : RegXY ) : * PtrRegs[ LastInstr & 0x07 ], y = LastInstr & 0x18;
	FLAGS = y == 0 ? TabSZYXP[ RegA &= x ] | FLAG_H : y == 0x08 ? TabSZYXP[ RegA ^= x ] : TabSZYXP[ RegA |= x ];
	return LastInstr & 0x40 ? 2 : ( LastInstr & 0x07 ) == IND_HL ? PtrRegs == Regs ? 2 : 4 : 1;
}

static int INC_R( void ) {
	UBYTE z = ++* PtrRegs[ LastInstr >> 3 ];
	FLAGS = ( FLAGS & FLAG_C ) | ( z & 0x0F ? 0 : FLAG_H ) | TabSZYX[ z ] | ( z == 0x80 ? FLAG_V : 0 );
	return 1;
}

static int INC_HL( void ) {
	int y = PtrRegs == Regs ? RegHL : RegXY, z = TabPOKE[ y >> 14 ][ y & 0x3FFF ] = TabPEEK[ y >> 14 ][ y & 0x3FFF ] + 1;
	FLAGS = ( FLAGS & FLAG_C ) | ( z & 0x0F ? 0 : FLAG_H ) | TabSZYX[ z & 0xFF ] | ( z == 0x80 ? FLAG_V : 0 );
	return PtrRegs == Regs ? 3 : 5;
}

static int DEC_R( void ) {
	UBYTE z = --* PtrRegs[ LastInstr >> 3 ];
	FLAGS = FLAG_N | ( FLAGS & FLAG_C ) | ( ( z & 0x0F ) == 0x0F ? FLAG_H : 0 ) | TabSZYX[ z ] | ( z == 0x7F ? FLAG_V : 0 );
	return 1;
}

static int DEC_HL( void ) {
	int y = PtrRegs == Regs ? RegHL : RegXY, z = TabPOKE[ y >> 14 ][ y & 0x3FFF ] = TabPEEK[ y >> 14 ][ y & 0x3FFF ] - 1;
	FLAGS = FLAG_N | ( FLAGS & FLAG_C ) | ( ( z & 0x0F ) == 0x0F ? FLAG_H : 0 ) | TabSZYX[ z & 0xFF ] | ( z == 0x7F ? FLAG_V : 0 );
	return PtrRegs == Regs ? 3 : 5;
}

/* 16 Bit Arithmetic Group */
static int ADD_HL_RR( void ) {
	int y = *( USHORT * )PtrRegs[ ( LastInstr >> 4 ) + 8 ], z = ( memPtr = *( USHORT * )PtrRegs[ 6 ] ) + y, x = ( memPtr++ ^ y ^ z ) >> 8;
	FLAGS = ( FLAGS & FLAGS_SZV ) | ( x & FLAG_H ) | ( x >> 8 ) | ( ( z >> 8 ) & FLAGS_53 );
	*( USHORT * )PtrRegs[ 6 ] = z;
	return 3;
}

static int ADC_HL_RR( void ) {
	int y = *( USHORT * )Regs[ ( LastInstr >> 4 ) + 4 ], z = ( memPtr = RegHL ) + y + ( FLAGS & FLAG_C );
	FLAGS = ( z & 0xffff ? ( z >> 8 ) & FLAGS_S53 : FLAG_Z ) | TabV[ ( memPtr++ ^ y ^ z ) >> 8 ];
	RegHL = z;
	return 3;
}

static int SBC_HL_RR( void ) {
	int y = *( USHORT * )Regs[ ( LastInstr >> 4 ) + 4 ], z = ( memPtr = RegHL ) - y - ( FLAGS & FLAG_C );
	FLAGS = FLAG_N | ( z & 0xffff ? ( z >> 8 ) & FLAGS_S53 : FLAG_Z ) | TabV[ ( ( memPtr++ ^ y ^ z ) >> 8 ) & 0x1FF ];
	RegHL = z;
	return 3;
}

static int INC_RR( void ) {
	( *( USHORT * )PtrRegs[ ( LastInstr >> 4 ) + 8 ] )++;
	return SupIrqWaitState = 2;
}

static int DEC_RR( void ) {
	( *( USHORT * )PtrRegs[ ( LastInstr >> 4 ) + 8 ] )--;
	return SupIrqWaitState = 2;
}

/* General Purpose Arithmetic and CPU Control Groups */
static int DAA( void ) {
	int x = RegA, y = ( x & 0x0F ) > 0x09 | ( FLAGS & FLAG_H ) ? 6 : 0;
	if ( x > 0x99 | ( FLAGS & FLAG_C ) )
		y |= 0x60;

	RegA += FLAGS & FLAG_N ? -y : y;
	FLAGS = TabSZYXP[ RegA ] | ( ( RegA ^ x ) & FLAG_H ) | ( FLAGS & FLAG_N ) | ( y >> 6 );
	return 1;
}

static int CPL( void ) {
	FLAGS = ( FLAGS & ( FLAGS_SZV | FLAG_C ) ) | ( ( RegA ^= 0xFF ) & FLAGS_53 ) | FLAG_H | FLAG_N;
	return 1;
}

static int NEG( void ) {
	int x = RegA ^ -RegA;
	FLAGS = FLAG_N | TabSZYX[ RegA = -RegA ] | TabV[ x & 0x1FF ];
	return 1;
}

static int CCF( void ) {
	int x = FLAGS & FLAG_C;
	FLAGS = ( FLAGS & FLAGS_SZV ) | ( x << 4 ) | ( RegA & FLAGS_53 ) | ( x ^ FLAG_C );
	return 1;
}

static int SCF( void ) {
	FLAGS = ( FLAGS & FLAGS_SZV ) | ( RegA & FLAGS_53 ) | FLAG_C;
	return 1;
}

static int NOP( void ) {
	return 1;
}

static int HALT( void ) {
	RegPC--;
	return Halt = 1;
}

static int DI_EI( void ) {
	Cpc.IFF1 = Cpc.IFF2 = ( LastInstr & 0x08 ) >> 1;
	return 1;
}

static int IM_N( void ) {
	Cpc.InterruptMode = ( LastInstr & 0x18 ) == 0x18 ? 2 : ( LastInstr & 0x18 ) == 0x10;
	return SupIrqWaitState = 1;
}

/* Rotate and Shift Group */
static int RLCA( void ) {
	FLAGS = ( FLAGS & FLAGS_SZV ) | ( ( RegA = ( RegA << 1 ) | ( RegA >> 7 ) ) & ( FLAGS_53 | FLAG_C ) );
	return 1;
}

static int RLA( void ) {
	int x = RegA << 1;
	FLAGS = ( ( RegA = x | ( FLAGS & FLAG_C ) ) & FLAGS_53 ) | ( x >> 8 ) | ( FLAGS & FLAGS_SZV );
	return 1;
}

static int RRCA( void ) {
	FLAGS = ( FLAGS & FLAGS_SZV ) | ( RegA & FLAG_C );
	FLAGS |= ( ( RegA = ( RegA >> 1 ) | ( FLAGS << 7 ) ) & FLAGS_53 );
	return 1;
}

static int RRA( void ) {
	int x = RegA & FLAG_C;
	FLAGS = ( FLAGS & FLAGS_SZV ) | ( ( RegA = ( RegA >> 1 ) | ( FLAGS << 7 ) ) & FLAGS_53 ) | x;
	return 1;
}

static int RLC_R( void ) {
	if ( PtrRegs == Regs && ( LastInstr & 0x07 ) != IND_HL ) {
		int x = * Regs[ LastInstr & 0x07 ] << 1, z = x >> 8;
		FLAGS = TabSZYXP[ * Regs[ LastInstr & 0x07 ] = x | z ] | z;
		return 2;
	}
	else {
		int y = PtrRegs == Regs ? RegHL : RegXY, x = TabPEEK[ y >> 14 ][ y & 0x3FFF ] << 1, z = x >> 8;
		FLAGS = TabSZYXP[ ( x |= z ) & 0xFF ] | z;
		TabPOKE[ y >> 14 ][ y & 0x3FFF ] = x;
		if ( PtrRegs == Regs )
			return 4;

		RegPC++;
		if ( ( LastInstr & 0x07 ) != IND_HL )
			* Regs[ LastInstr & 0x07 ] = x;

		return 6;
	}
}

static int RL_R( void ) {
	if ( PtrRegs == Regs && ( LastInstr & 0x07 ) != IND_HL ) {
		int x = ( FLAGS & FLAG_C ) | * Regs[ LastInstr & 0x07 ] << 1;
		FLAGS = TabSZYXP[ * Regs[ LastInstr & 0x07 ] = x ] | ( x >> 8 );
		return 2;
	}
	else {
		int y = PtrRegs == Regs ? RegHL : RegXY, x = ( FLAGS & FLAG_C ) | TabPEEK[ y >> 14 ][ y & 0x3FFF ] << 1;
		FLAGS = TabSZYXP[ x & 0xFF ] | ( x >> 8 );
		TabPOKE[ y >> 14 ][ y & 0x3FFF ] = x;
		if ( PtrRegs == Regs )
			return 4;

		RegPC++;
		if ( ( LastInstr & 0x07 ) != IND_HL )
			* Regs[ LastInstr & 0x07 ] = x;

		return 6;
	}
}

static int RRC_R( void ) {
	if ( PtrRegs == Regs && ( LastInstr & 0x07 ) != IND_HL ) {
		int x = * Regs[ LastInstr & 0x07 ], z = x & 0x01;
		FLAGS = TabSZYXP[ * Regs[ LastInstr & 0x07 ] = ( x >> 1 ) | ( z << 7 ) ] | z;
		return 2;
	}
	else {
		int y = PtrRegs == Regs ? RegHL : RegXY, x = TabPEEK[ y >> 14 ][ y & 0x3FFF ], z = x & 0x01;
		FLAGS = TabSZYXP[ x = ( x >> 1 ) | ( z << 7 ) ] | z;
		TabPOKE[ y >> 14 ][ y & 0x3FFF ] = x;
		if ( PtrRegs == Regs )
			return 4;

		RegPC++;
		if ( ( LastInstr & 0x07 ) != IND_HL )
			* Regs[ LastInstr & 0x07 ] = x;

		return 6;
	}
}

static int RR_R( void ) {
	if ( PtrRegs == Regs && ( LastInstr & 0x07 ) != IND_HL ) {
		int x = * Regs[ LastInstr & 0x07 ];
		FLAGS = TabSZYXP[ * Regs[ LastInstr & 0x07 ] = ( x >> 1 ) | ( ( FLAGS & FLAG_C ) << 7 ) ] | ( x & 0x01 );
		return 2;
	}
	else {
		int y = PtrRegs == Regs ? RegHL : RegXY, x = TabPEEK[ y >> 14 ][ y & 0x3FFF ], z = x & 0x01;
		FLAGS = TabSZYXP[ x = ( x >> 1 ) | ( ( FLAGS & FLAG_C ) << 7 ) ] | z;
		TabPOKE[ y >> 14 ][ y & 0x3FFF ] = x;
		if ( PtrRegs == Regs )
			return 4;

		RegPC++;
		if ( ( LastInstr & 0x07 ) != IND_HL )
			* Regs[ LastInstr & 0x07 ] = x;

		return 6;
	}
}

static int SLA_R( void ) {
	if ( PtrRegs == Regs && ( LastInstr & 0x07 ) != IND_HL ) {
		int x = * Regs[ LastInstr & 0x07 ] << 1;
		FLAGS = TabSZYXP[ * Regs[ LastInstr & 0x07 ] = x ] | ( x >> 8 );
		return 2;
	}
	else {
		int y = PtrRegs == Regs ? RegHL : RegXY, x = TabPEEK[ y >> 14 ][ y & 0x3FFF ] << 1;
		FLAGS = TabSZYXP[ x & 0xFF ] | ( x >> 8 );
		TabPOKE[ y >> 14 ][ y & 0x3FFF ] = x;
		if ( PtrRegs == Regs )
			return 4;

		RegPC++;
		if ( ( LastInstr & 0x07 ) != IND_HL )
			* Regs[ LastInstr & 0x07 ] = x;

		return 6;
	}
}

static int SLL_R( void ) {
	if ( PtrRegs == Regs && ( LastInstr & 0x07 ) != IND_HL ) {
		int x = ( * Regs[ LastInstr & 0x07 ] << 1 ) | 0x01;
		FLAGS = TabSZYXP[ * Regs[ LastInstr & 0x07 ] = x ] | ( x >> 8 );
		return 2;
	}
	else {
		int y = PtrRegs == Regs ? RegHL : RegXY, x = ( TabPEEK[ y >> 14 ][ y & 0x3FFF ] << 1 ) | 0x01;
		FLAGS = TabSZYXP[ x & 0xFF ] | ( x >> 8 );
		TabPOKE[ y >> 14 ][ y & 0x3FFF ] = x;
		if ( PtrRegs == Regs )
			return 4;

		RegPC++;
		if ( ( LastInstr & 0x07 ) != IND_HL )
			* Regs[ LastInstr & 0x07 ] = x;

		return 6;
	}
}

static int SRA_R( void ) {
	if ( PtrRegs == Regs && ( LastInstr & 0x07 ) != IND_HL ) {
		int x = * Regs[ LastInstr & 0x07 ];
		FLAGS = ( x & 1 ) | TabSZYXP[ * Regs[ LastInstr & 0x07 ]  = ( ( char )x ) >> 1 ];
		return 2;
	}
	else {
		int y = PtrRegs == Regs ? RegHL : RegXY, x = TabPEEK[ y >> 14 ][ y & 0x3FFF ], z = x & 1;
		FLAGS = z | TabSZYXP[ x = ( ( ( char )x ) >> 1 ) & 0xFF ];
		TabPOKE[ y >> 14 ][ y & 0x3FFF ] = x;
		if ( PtrRegs == Regs )
			return 4;

		RegPC++;
		if ( ( LastInstr & 0x07 ) != IND_HL )
			* Regs[ LastInstr & 0x07 ] = x;

		return 6;
	}
}

static int SRL_R( void ) {
	if ( PtrRegs == Regs && ( LastInstr & 0x07 ) != IND_HL ) {
		int x = * Regs[ LastInstr & 0x07 ];
		FLAGS = TabSZYXP[ * Regs[ LastInstr & 0x07 ] >>= 1 ] | ( x & 1 );
		return 2;
	}
	else {
		int y = PtrRegs == Regs ? RegHL : RegXY, x = TabPEEK[ y >> 14 ][ y & 0x3FFF ], z = x & 1;
		FLAGS = TabSZYXP[ x >>= 1 ] | z;
		TabPOKE[ y >> 14 ][ y & 0x3FFF ] = x;
		if ( PtrRegs == Regs )
			return 4;

		RegPC++;
		if ( ( LastInstr & 0x07 ) != IND_HL )
			* Regs[ LastInstr & 0x07 ] = x;

		return 6;
	}
}

static int RLD_RRD( void ) {
	int x = TabPEEK[ ( memPtr = RegHL ) >> 14 ][ RegHL & 0x3FFF ], y;
	TabPOKE[ memPtr >> 14 ][ memPtr++ & 0x3FFF ] = y = ( ( RegA & 0xF0 ) << 8 ) | ( LastInstr & 0x08 ? ( x << 4 ) | ( RegA & 0x0F ) : ( ( x & 0x0F ) << 8 ) | ( ( RegA & 0x0F ) << 4 ) | ( x >> 4 ) );
	FLAGS = TabSZYXP[ RegA = y >> 8 ] | ( FLAGS & FLAG_C );
	return 4;
}

/* Bit Manipulation Group */
static int BIT_B_HL( void ) {
	int d = PtrRegs == Regs ? RegHL : RegXY, y = PtrRegs == Regs ? memPtr >> 8 : d, x = TabPEEK[ d >> 14 ][ d & 0x3FFF ] & ( 1 << ( ( LastInstr >> 3 ) & 0x07 ) );
	FLAGS = FLAG_H | ( FLAGS & FLAG_C ) | ( x ? x & FLAG_S : FLAG_Z | FLAG_V ) | ( y & FLAGS_53 );
	RegPC += PtrRegs != Regs;
	return PtrRegs == Regs ? 3 : 5;
}

static int BIT_B_R( void ) {
	int x = ( PtrRegs == Regs ? * Regs[ LastInstr & 0x07 ] : PEEK8( RegXY ) ) & ( 1 << ( ( LastInstr >> 3 ) & 0x07 ) );
	FLAGS = FLAG_H | ( FLAGS & FLAG_C ) | ( x ? x & FLAG_S : FLAG_Z | FLAG_V ) | ( * PtrRegs[ LastInstr & 0x07 ] & FLAGS_53 );
	RegPC += PtrRegs != Regs;
	return PtrRegs == Regs ? 2 : 5;
}

static int SET_B_R( void ) {
	if ( PtrRegs != Regs ) {
		int y = RegXY, x = TabPEEK[ y >> 14 ][ y & 0x3FFF ] | 1 << ( ( LastInstr >> 3 ) & 0x07 );
		TabPOKE[ y >> 14 ][ y & 0x3FFF ] = * Regs[ LastInstr & 0x07 ] = x;
		RegPC++;
		return 6;
	}
	* Regs[ LastInstr & 0x07 ] |= 1 << ( ( LastInstr >> 3 ) & 0x07 );
	return 2;
}

static int SET_B_HL( void ) {
	if ( PtrRegs != Regs ) {
		int y = RegXY, x = TabPEEK[ y >> 14 ][ y & 0x3FFF ] | 1 << ( ( LastInstr >> 3 ) & 0x07 );
		TabPOKE[ y >> 14 ][ y & 0x3FFF ] = x;
		RegPC++;
		return 6;
	}
	TabPOKE[ RegHL >> 14 ][ RegHL & 0x3FFF ] = TabPEEK[ RegHL >> 14 ][ RegHL & 0x3FFF ] | 1 << ( ( LastInstr >> 3 ) & 0x07 );
	return 4;
}

static int RES_B_R( void ) {
	if ( PtrRegs != Regs ) {
		int y = RegXY, x = TabPEEK[ y >> 14 ][ y & 0x3FFF ] & ~( 1 << ( ( LastInstr >> 3 ) & 0x07 ) );
		TabPOKE[ y >> 14 ][ y & 0x3FFF ] = * Regs[ LastInstr & 0x07 ] = x;
		RegPC++;
		return 6;
	}
	* Regs[ LastInstr & 0x07 ] &= ~( 1 << ( ( LastInstr >> 3 ) & 0x07 ) );
	return 2;
}

static int RES_B_HL( void ) {
	if ( PtrRegs != Regs ) {
		int y = RegXY, x = TabPEEK[ y >> 14 ][ y & 0x3FFF ] & ~( 1 << ( ( LastInstr >> 3 ) & 0x07 ) );
		TabPOKE[ y >> 14 ][ y & 0x3FFF ] = x;
		RegPC++;
		return 6;
	}
	TabPOKE[ RegHL >> 14 ][ RegHL & 0x3FFF ] = TabPEEK[ RegHL >> 14 ][ RegHL & 0x3FFF ] & ~( 1 << ( ( LastInstr >> 3 ) & 0x07 ) );
	return 4;
}

/* Input and Output Group*/
static int IN_A_N( void ) {
	memPtr = ( RegA << 8 ) | Fetch();
	RegA = ReadPort( memPtr++ );
	return 3;
}

static int IN_R_C( void ) {
	int x = ReadPort( memPtr = RegBC );
	if ( ( LastInstr & 0x38 ) != IND_HL3 )
		* Regs[ ( LastInstr >> 3 ) & 0x07 ] = x;

	memPtr++;
	FLAGS = TabSZYXP[ x ] | ( FLAGS & FLAG_C );
	return 3;
}

static int INIx_INDx( void ) {
	int x = TabPOKE[ RegHL >> 14 ][ RegHL & 0x3FFF ] = ReadPort( memPtr = RegBC );
	memPtr += LastInstr & 0x08 ? - 1 : 1;
	RegHL += LastInstr &0x08 ? -1 : 1;
	FLAGS = TabSZYX[ --RegB & 0xFF ] | ( x >> 6 );
	x += memPtr & 0xFF;
	FLAGS |= ( x & 0x0100 ? FLAGS_HC : 0 ) | ( TabSZYXP[ ( x & 0x07 ) ^ RegB ] & FLAG_V );
	if ( RegB && ( LastInstr & 0x10 ) ) {
		RegPC -= 2;
		return 5;
	}
	return 4;
}

static int OUT_N_A( void ) {
	memPtr = ( RegA << 8 ) | Fetch();
	WritePort( memPtr++, RegA );
	return 3;
}

static int OUT_C_R( void ) {
	memPtr = RegBC;
	WritePort( memPtr++, ( LastInstr & 0x38 ) == IND_HL3 ? 0 : * Regs[ ( LastInstr >> 3 ) & 0x07 ] );
	return 3;
}

static int OxTI_OxTD( void ) {
	FLAGS = FLAGS & ~FLAG_H & ~FLAG_S | ( --RegB ? 0 : FLAG_Z );
	WritePort( memPtr = RegBC, TabPEEK[ RegHL >>14 ][ RegHL & 0x3FFF ] );
	RegHL += LastInstr & 0x08 ? -1 : 1;
	memPtr += LastInstr & 0x08 ? - 1 : 1;
	if ( RegB && ( LastInstr & 0x10 ) ) {
		RegPC -= 2;
		return 5;
	}
	return 4;
}

/* Jump Group */
static int JR_E( void ) {
	memPtr = RegPC = RegPC + ( char )TabPEEK[ RegPC >> 14 ][ RegPC & 0x3FFF ] + 1;
	return 3;
}

static int JR_DD_E( void ) {
	if ( ( FLAGS ^ TabCC[ ( LastInstr >> 2 ) & 0x06 ] ) & TabCC[ 1 + ( ( LastInstr >> 2 ) & 0x06 ) ] )
		return JR_E();

	RegPC++;
	return 2;
}

static int JP_NN( void ) {
	memPtr = RegPC = PEEK16( RegPC );
	return 3;
}

static int JP_CC_NN( void ) {
	memPtr = RegPC = ( FLAGS ^ TabCC[ ( LastInstr >> 2 ) & 0x0E ] ) & TabCC[ 1 + ( ( LastInstr >> 2 ) & 0x0E ) ] ? PEEK16( RegPC ) : RegPC + 2;
	return 3;
}

static int JP_HL( void ) {
	RegPC = *( USHORT * )PtrRegs[ 6 ];
	return 1;
}

static int DJNZ_E( void ) {
	if ( --RegB )
		return 1 + JR_E();

	RegPC++;
	return 3;
}

/* Call and Return Group */
static int CALL_NN( void ) {
	memPtr = PEEK16( RegPC );
	RegSP -= 2;
	POKE16( RegSP, RegPC + 2 );
	RegPC = memPtr;
	return 5;
}

static int CALL_CC_NN( void ) {
	if ( ( FLAGS ^ TabCC[ ( LastInstr >> 2 ) & 0x0E ] ) & TabCC[ 1 + ( ( LastInstr >> 2 ) & 0x0E ) ] )
		return CALL_NN();

	RegPC += 2;
	return 3;
}

int RET( void ) {
	memPtr = RegPC = PEEK16( RegSP );
	RegSP += 2;
	return 3;
}

static int RET_CC( void ) {
	return ( FLAGS ^ TabCC[ ( LastInstr >> 2 ) & 0x0E ] ) & TabCC[ 1 + ( ( LastInstr >> 2 ) & 0x0E ) ] ? 1 + RET() : ( SupIrqWaitState = 2 );
}

static int RETI_RETN( void ) {
	Cpc.IFF1 = Cpc.IFF2;
	return RET();
}

static int RST_P( void ) {
	RegSP -= 2;
	POKE16( RegSP, RegPC );
	memPtr = RegPC = LastInstr & 0x38;
	return 4;
}

/* Prefixed Group */
static int CB_PREFIX( void ) {
	RegR ^= ( ( RegR + 1 ) ^ RegR ) & 0x7F;
	return TabInstrCB[ LastInstr = PEEK8( PtrRegs == Regs ? RegPC++ : RegPC + 1 ) ]();
}

static int DD_FD_PREFIX( void ) {
	PtrRegs = LastInstr == 0xDD ? RegsDD : RegsFD;
	RegR ^= ( ( RegR + 1 ) ^ RegR ) & 0x7F;
	return 1 + TabInstr[ LastInstr = Fetch() ]();
}

static int ED_PREFIX( void ) {
	PtrRegs = Regs;
	RegR ^= ( ( RegR + 1 ) ^ RegR ) & 0x7F;
	return 1 + TabInstrED[ LastInstr = Fetch() ]();
}

pfct TabInstr[ 256 ] = {
	NOP, LD_RR_NN, LD_BCDE_A, INC_RR, INC_R, DEC_R, LD_R8, RLCA,				// 00
	EX_AF_AF_PRIME, ADD_HL_RR, LD_A_BCDE, DEC_RR, INC_R, DEC_R, LD_R8, RRCA,	// 08
	DJNZ_E, LD_RR_NN, LD_BCDE_A, INC_RR, INC_R, DEC_R, LD_R8, RLA,				// 10
	JR_E, ADD_HL_RR, LD_A_BCDE, DEC_RR, INC_R, DEC_R, LD_R8, RRA,				// 18
	JR_DD_E, LD_RR_NN, LD_MM_RR, INC_RR, INC_R, DEC_R, LD_R8, DAA,				// 20
	JR_DD_E, ADD_HL_RR, LD_RR_MM, DEC_RR, INC_R, DEC_R, LD_R8, CPL,				// 28
	JR_DD_E, LD_RR_NN, LD_MM_A, INC_RR, INC_HL, DEC_HL, LD_HL_N, SCF,			// 30
	JR_DD_E, ADD_HL_RR, LD_A_MM, DEC_RR, INC_R, DEC_R, LD_R8, CCF,				// 38
	NOP, LD_R8, LD_R8, LD_R8, LD_R8, LD_R8, LD_R_HL, LD_R8,						// 40
	LD_R8, NOP, LD_R8, LD_R8, LD_R8, LD_R8, LD_R_HL, LD_R8,						// 48
	LD_R8, LD_R8, NOP, LD_R8, LD_R8, LD_R8, LD_R_HL, LD_R8,						// 50
	LD_R8, LD_R8, LD_R8, NOP, LD_R8, LD_R8, LD_R_HL, LD_R8,						// 58
	LD_R8, LD_R8, LD_R8, LD_R8, NOP, LD_R8, LD_R_HL, LD_R8,						// 60
	LD_R8, LD_R8, LD_R8, LD_R8, LD_R8, NOP, LD_R_HL, LD_R8,						// 68
	LD_HL_R, LD_HL_R, LD_HL_R, LD_HL_R, LD_HL_R, LD_HL_R, HALT, LD_HL_R,		// 70
	LD_R8, LD_R8, LD_R8, LD_R8, LD_R8, LD_R8, LD_R_HL, NOP,						// 78
	ADD_R8, ADD_R8, ADD_R8, ADD_R8, ADD_R8, ADD_R8, ADD_R8, ADD_R8,				// 80
	ADD_R8, ADD_R8, ADD_R8, ADD_R8, ADD_R8, ADD_R8, ADD_R8, ADD_R8,				// 88
	SUB_R8, SUB_R8, SUB_R8, SUB_R8, SUB_R8, SUB_R8, SUB_R8, SUB_R8,				// 90
	SUB_R8, SUB_R8, SUB_R8, SUB_R8, SUB_R8, SUB_R8, SUB_R8, SUB_R8,				// 98
	LOG_R8, LOG_R8, LOG_R8, LOG_R8, LOG_R8, LOG_R8, LOG_R8, LOG_R8,				// A0
	LOG_R8, LOG_R8, LOG_R8, LOG_R8, LOG_R8, LOG_R8, LOG_R8, LOG_R8,				// A8
	LOG_R8, LOG_R8, LOG_R8, LOG_R8, LOG_R8, LOG_R8, LOG_R8, LOG_R8,				// B0
	CP_R8, CP_R8, CP_R8, CP_R8, CP_R8, CP_R8, CP_R8, CP_R8,						// B8
	RET_CC, POP_SS, JP_CC_NN, JP_NN, CALL_CC_NN, PUSH_SS, ADD_R8, RST_P,		// C0
	RET_CC, RET, JP_CC_NN, CB_PREFIX, CALL_CC_NN, CALL_NN, ADD_R8, RST_P,		// C8
	RET_CC, POP_SS, JP_CC_NN, OUT_N_A, CALL_CC_NN, PUSH_SS, SUB_R8, RST_P,		// D0
	RET_CC, EXX, JP_CC_NN, IN_A_N, CALL_CC_NN, DD_FD_PREFIX, SUB_R8, RST_P,		// D8
	RET_CC, POP_SS, JP_CC_NN, EX_SP_HL, CALL_CC_NN, PUSH_SS, LOG_R8, RST_P,		// E0
	RET_CC, JP_HL, JP_CC_NN, EX_DE_HL, CALL_CC_NN, ED_PREFIX, LOG_R8, RST_P,	// E8
	RET_CC, POP_SS, JP_CC_NN, DI_EI, CALL_CC_NN, PUSH_SS, LOG_R8, RST_P,		// F0
	RET_CC, LD_SP_HL, JP_CC_NN, DI_EI, CALL_CC_NN, DD_FD_PREFIX, CP_R8, RST_P	// F8
};

pfct TabInstrCB[ 256 ] = {
	RLC_R, RLC_R, RLC_R, RLC_R, RLC_R, RLC_R, RLC_R, RLC_R,						// 00
	RRC_R, RRC_R, RRC_R, RRC_R, RRC_R, RRC_R, RRC_R, RRC_R,						// 08
	RL_R, RL_R, RL_R, RL_R, RL_R, RL_R, RL_R, RL_R,								// 10
	RR_R, RR_R, RR_R, RR_R, RR_R, RR_R, RR_R, RR_R,								// 18
	SLA_R, SLA_R, SLA_R, SLA_R, SLA_R, SLA_R, SLA_R, SLA_R,						// 20
	SRA_R, SRA_R, SRA_R, SRA_R, SRA_R, SRA_R, SRA_R, SRA_R,						// 28
	SLL_R, SLL_R, SLL_R, SLL_R, SLL_R, SLL_R, SLL_R, SLL_R,						// 30
	SRL_R, SRL_R, SRL_R, SRL_R, SRL_R, SRL_R, SRL_R, SRL_R,						// 38
	BIT_B_R, BIT_B_R, BIT_B_R, BIT_B_R, BIT_B_R, BIT_B_R, BIT_B_HL, BIT_B_R,	// 40
	BIT_B_R, BIT_B_R, BIT_B_R, BIT_B_R, BIT_B_R, BIT_B_R, BIT_B_HL, BIT_B_R,	// 48
	BIT_B_R, BIT_B_R, BIT_B_R, BIT_B_R, BIT_B_R, BIT_B_R, BIT_B_HL, BIT_B_R,	// 50
	BIT_B_R, BIT_B_R, BIT_B_R, BIT_B_R, BIT_B_R, BIT_B_R, BIT_B_HL, BIT_B_R,	// 58
	BIT_B_R, BIT_B_R, BIT_B_R, BIT_B_R, BIT_B_R, BIT_B_R, BIT_B_HL, BIT_B_R,	// 60
	BIT_B_R, BIT_B_R, BIT_B_R, BIT_B_R, BIT_B_R, BIT_B_R, BIT_B_HL, BIT_B_R,	// 68
	BIT_B_R, BIT_B_R, BIT_B_R, BIT_B_R, BIT_B_R, BIT_B_R, BIT_B_HL, BIT_B_R,	// 70
	BIT_B_R, BIT_B_R, BIT_B_R, BIT_B_R, BIT_B_R, BIT_B_R, BIT_B_HL, BIT_B_R,	// 78
	RES_B_R, RES_B_R, RES_B_R, RES_B_R, RES_B_R, RES_B_R, RES_B_HL, RES_B_R,	// 80
	RES_B_R, RES_B_R, RES_B_R, RES_B_R, RES_B_R, RES_B_R, RES_B_HL, RES_B_R,	// 88
	RES_B_R, RES_B_R, RES_B_R, RES_B_R, RES_B_R, RES_B_R, RES_B_HL, RES_B_R,	// 90
	RES_B_R, RES_B_R, RES_B_R, RES_B_R, RES_B_R, RES_B_R, RES_B_HL, RES_B_R,	// 98
	RES_B_R, RES_B_R, RES_B_R, RES_B_R, RES_B_R, RES_B_R, RES_B_HL, RES_B_R,	// A0
	RES_B_R, RES_B_R, RES_B_R, RES_B_R, RES_B_R, RES_B_R, RES_B_HL, RES_B_R,	// A8
	RES_B_R, RES_B_R, RES_B_R, RES_B_R, RES_B_R, RES_B_R, RES_B_HL, RES_B_R,	// B0
	RES_B_R, RES_B_R, RES_B_R, RES_B_R, RES_B_R, RES_B_R, RES_B_HL, RES_B_R,	// B8
	SET_B_R, SET_B_R, SET_B_R, SET_B_R, SET_B_R, SET_B_R, SET_B_HL, SET_B_R,	// C0
	SET_B_R, SET_B_R, SET_B_R, SET_B_R, SET_B_R, SET_B_R, SET_B_HL, SET_B_R,	// C8
	SET_B_R, SET_B_R, SET_B_R, SET_B_R, SET_B_R, SET_B_R, SET_B_HL, SET_B_R,	// D0
	SET_B_R, SET_B_R, SET_B_R, SET_B_R, SET_B_R, SET_B_R, SET_B_HL, SET_B_R,	// D8
	SET_B_R, SET_B_R, SET_B_R, SET_B_R, SET_B_R, SET_B_R, SET_B_HL, SET_B_R,	// E0
	SET_B_R, SET_B_R, SET_B_R, SET_B_R, SET_B_R, SET_B_R, SET_B_HL, SET_B_R,	// E8
	SET_B_R, SET_B_R, SET_B_R, SET_B_R, SET_B_R, SET_B_R, SET_B_HL, SET_B_R,	// F0
	SET_B_R, SET_B_R, SET_B_R, SET_B_R, SET_B_R, SET_B_R, SET_B_HL, SET_B_R		// F8
};

pfct TabInstrED[ 256 ] = {
	ED_00, ED_01, ED_02, ED_03, ED_04, ED_05, ED_06, ED_07,						// 00
	ED_08, ED_09, ED_0A, ED_0B, ED_0C, ED_0D, ED_0E, ED_0F,						// 08
	ED_10, ED_11, ED_12, ED_13, ED_14, ED_15, ED_16, ED_17,						// 00
	ED_18, ED_19, ED_1A, ED_1B, ED_1C, ED_1D, ED_1E, ED_1F,						// 08
	ED_20, ED_21, ED_22, ED_23, ED_24, ED_25, ED_26, ED_27,						// 00
	ED_28, ED_29, ED_2A, ED_2B, ED_2C, ED_2D, ED_2E, ED_2F,						// 08
	ED_30, ED_31, ED_32, ED_33, ED_34, ED_35, ED_36, ED_37,						// 00
	ED_38, ED_39, ED_3A, ED_3B, ED_3C, ED_3D, ED_3E, ED_3F,						// 08
	IN_R_C, OUT_C_R, SBC_HL_RR, LD_MM_RR, NEG, RETI_RETN, IM_N, LD_I_A,			// 40
	IN_R_C, OUT_C_R, ADC_HL_RR, LD_RR_MM, NEG, RETI_RETN, IM_N, LD_R_A,			// 48
	IN_R_C, OUT_C_R, SBC_HL_RR, LD_MM_RR, NEG, RETI_RETN, IM_N, LD_A_I,			// 50
	IN_R_C, OUT_C_R, ADC_HL_RR, LD_RR_MM, NEG, RETI_RETN, IM_N, LD_A_R,			// 58
	IN_R_C, OUT_C_R, SBC_HL_RR, LD_MM_RR, NEG, RETI_RETN, IM_N, RLD_RRD,		// 60
	IN_R_C, OUT_C_R, ADC_HL_RR, LD_RR_MM, NEG, RETI_RETN, IM_N, RLD_RRD,		// 68
	IN_R_C, OUT_C_R, SBC_HL_RR, LD_MM_RR, NEG, RETI_RETN, IM_N, NOP,			// 70
	IN_R_C, OUT_C_R, ADC_HL_RR, LD_RR_MM, NEG, RETI_RETN, IM_N, NOP,			// 78
	NOP, NOP, NOP, NOP, NOP, NOP, NOP, NOP,										// 80
	NOP, NOP, NOP, NOP, NOP, NOP, NOP, NOP,										// 88
	NOP, NOP, NOP, NOP, NOP, NOP, NOP, NOP,										// 90
	NOP, NOP, NOP, NOP, NOP, NOP, NOP, NOP,										// 98
	LDIx, CPIx_CPDx, INIx_INDx, OxTI_OxTD, NOP, NOP, NOP, NOP,					// A0
	LDDx, CPIx_CPDx, INIx_INDx, OxTI_OxTD, NOP, NOP, NOP, NOP,					// A8
	LDIx, CPIx_CPDx, INIx_INDx, OxTI_OxTD, NOP, NOP, NOP, NOP,					// B0
	LDDx, CPIx_CPDx, INIx_INDx, OxTI_OxTD, NOP, NOP, NOP, NOP,					// B8
	NOP, NOP, NOP, NOP, NOP, NOP, NOP, NOP,										// C0
	NOP, NOP, NOP, NOP, NOP, NOP, NOP, NOP,										// C8
	NOP, NOP, NOP, NOP, NOP, NOP, NOP, NOP,										// D0
	NOP, NOP, NOP, NOP, NOP, NOP, NOP, NOP,										// D8
	NOP, NOP, NOP, NOP, NOP, NOP, NOP, NOP,										// E0
	NOP, NOP, NOP, NOP, NOP, NOP, NOP, NOP,										// E8
	NOP, NOP, NOP, NOP, NOP, NOP, NOP, NOP,										// F0
	NOP, NOP, NOP, NOP, NOP, NOP, NOP, NOP										// F8
};

/********************************************************* !NAME! **************
* Nom : Z80_NMI
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Emulation d'une NMI du Cpc
*
* Résultat    : /
*
* Variables globales modifiées : Cpc
*
********************************************************** !0! ****************/
int Z80_NMI( void )
{
	Cpc.IFF1 = 0;
	PUSH( RegPC );
	RegPC = 0x66;
	return( 4 /* 4 NOPs */ );
}


/********************************************************* !NAME! **************
* Nom : ExecInstZ80
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Execution de l'instruction Z80 à l'adresse du PC
*
* Résultat    : /
*
* Variables globales modifiées : Cpc, IRQ, CntHSync
*
********************************************************** !0! ****************/
int ExecInstZ80( void ) {

	PtrRegs = Regs;
	RegR ^= ( ( RegR + 1 ) ^ RegR ) & 0x7F;
	if ( NewDiscOn )
		NewDiscAdrPC();

	int t = TabInstr[ LastInstr = Fetch() ]();
	if ( Cpc.IRQ && Cpc.IFF1 && LastInstr != 0xFB ) { // Pas d'irq juste après un EI
		RegSP -= 2;
		POKE16( RegSP, RegPC + Halt );
		RegR ^= ( ( RegR + 1 ) ^ RegR ) & 0x7F;
		memPtr = RegPC = Cpc.InterruptMode < 2 ? 0x38 : PEEK16( Cpc.IR.Word | 0xFF );
		t += 6 + ( Cpc.InterruptMode >> 1 ) - SupIrqWaitState;
		Halt = SupIrqWaitState = Cpc.IRQ = Cpc.IFF1 = Cpc.IFF2 = 0;
		Cpc.CntHSync &= 0x1F;
	}
	return t;
}


/********************************************************* !NAME! **************
* Nom : ResetZ80
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Reset du Z80, initialisation tableau Parite
*
* Résultat    : /
*
* Variables globales modifiées : Cpc, Parite
*
********************************************************** !0! ****************/
void ResetZ80( void ) {
	int i;

	Regs[ 0 ] = &Cpc.BC.Byte.High;
	Regs[ 1 ] = Regs[ 8 ] = Regs[ 12 ] = &Cpc.BC.Byte.Low;
	Regs[ 2 ] = &Cpc.DE.Byte.High;
	Regs[ 3 ] = Regs[ 9 ] = Regs[ 13 ] = &Cpc.DE.Byte.Low;
	Regs[ 4 ] = &Cpc.HL.Byte.High;
	Regs[ 5 ] = Regs[ 6 ] = Regs[ 10 ] = Regs[ 14 ] = &Cpc.HL.Byte.Low;
	Regs[ 7 ] = &Cpc.AF.Byte.High;
	Regs[ 11 ] = &Cpc.SP.Byte.Low;
	Regs[ 15 ] = &Cpc.AF.Byte.Low;
	memcpy( RegsDD, Regs, sizeof( RegsDD ) );
	memcpy( RegsFD, Regs, sizeof( RegsFD ) );
	RegsDD[ 4 ] = &Cpc.IX.Byte.High;
	RegsDD[ 5 ] = RegsDD[ 6 ] = RegsDD[ 10 ] = RegsDD[ 14 ] = &Cpc.IX.Byte.Low;
	RegsFD[ 4 ] = &Cpc.IY.Byte.High;
	RegsFD[ 5 ] = RegsFD[ 6 ] = RegsFD[ 10 ] = RegsFD[ 14 ] = &Cpc.IY.Byte.Low;
	for ( i = 0; i < 256; i++ )  {
		TabSZYX[ i ] = i == 0 ? FLAG_Z : i & 0xA8;
		TabSZYXP[ i ] = TabSZYX[ i ] + ( ( ( i >> 7 ) ^ ( i >> 6 ) ^ ( i >> 5 ) ^ ( i >> 4 ) ^ ( i >> 3 ) ^ ( i >> 2 ) ^ ( i >> 1 ) ^ i ) & 1 ? 0 : FLAG_V );
		TabV[ i << 1 ] = TabV[ ( i << 1 ) + 1 ] = ( i >> 7 ) | ( ( i << 1 ) & FLAG_H ) | ( i < 64 || i > 191 ? 0 : FLAG_V );
	}
	RegPC = RegSP = 0;
}
