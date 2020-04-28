/******************************************************************************/
/* Configuration pour l'archivage des différents éléments du fichier source   */
/******************************************************************************/
// !CONFIG!=/L/* /R/* /W"* Nom : "
// Définition du système       !CONFIG!=/V1!EMULATEUR CPC!
// Définition du sous système  !CONFIG!=/V2!PC-CPC!
// Définition du sous ensemble !CONFIG!=/V3!Accessoires!
// Définition du module        !CONFIG!=/V4!Snapshots!
/******************************************************************************/

/********************************************************* !NAME! **************
* !./FLE!
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\Fichiers
********************************************************** !0! *****************
* ------------------------------------------------------------------------------
*  Fichier     : SNAPSHOT.H            | Version : 0.1at
* ------------------------------------------------------------------------------
*  Date        : 05/11/2002            | Auteur  : L.DEPLANQUE
* ------------------------------------------------------------------------------
*  Description : Définitions pour le module SNAPSHOT.C
*
* ------------------------------------------------------------------------------
*  Historique  :
*           Date           |         Auteur          |       Description
* ------------------------------------------------------------------------------
*  05/11/2002              | L.DEPLANQUE             | creation
* ------------------------------------------------------------------------------
*  13/11/2002              | L.DEPLANQUE             | Passage du nom de fichier
*                          |                         | snapshot en paramètre aux
*                          |                         | fonctions LireSnap() et
*                          |                         | SauveSnap().  
* ------------------------------------------------------------------------------
*  03/03/2011              | L.DEPLANQUE             | Version 0.1at :
*                          |                         | Passage "partiel" en
*                          |                         | version 3 de .SNA
* ------------------------------------------------------------------------------
********************************************************** !END! **************/

#ifndef SNAPSHOT_H
#define SNAPSHOT_H


#include  "Z80.h"


/********************************************************* !NAME! **************
* Nom : StSnapShot
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Structures
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Structure en-tête fichier snapshot
*
********************************************************** !0! ****************/
typedef struct {
	char Id[ 0x10 ];
	UBYTE Version;
	Registre AF, BC, DE, HL, IR;
	UBYTE IFF1, IFF2;
	Registre IX, IY, SP, PC;
	UBYTE InterruptMode;
	Registre _AF, _BC, _DE, _HL;
	UBYTE InkReg, InkData[ 17 ];
	UBYTE VGARom, VGARam;
	UBYTE CRTCIndex, CRTCReg[ 18 ];
	UBYTE NumRom;
	UBYTE PPI[ 4 ];
	UBYTE PsgIndex, PsgData[ 16 ];
	USHORT MemSize;
	UBYTE CpcType;
	UBYTE last_interrupt;
	UBYTE ScrModes[ 6 ];
	UBYTE DrvAFilename[ 13 ], DrvBFilename[ 13 ], CartFilename[ 13 ];
	UBYTE FddMotor, FddCurrTrack[ 4 ];
	UBYTE PrinterData;
	UBYTE EnvStep, EnvDir;
	UBYTE CrtcType;
	ULONG Unused3;
	UBYTE HCC, Unused4, VCC, VLC, VtAdj, TailleHBL, TailleVBL;
	USHORT CrtcFlag;
	UBYTE SyncCount, CntHSync;
	UBYTE IRQ;
	UBYTE Unused5[ 0x4B ];
} StCpc;

extern StCpc Cpc;

BOOL SetSnap( StCpc * EnTete );

void SauveSnap( char * Nom );

void LireSnap( char * Nom );


#endif
