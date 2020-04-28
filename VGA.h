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
*  Fichier     : VGA.H                 | Version : 0.1an
* ------------------------------------------------------------------------------
*  Date        : 05/11/2002            | Auteur  : L.DEPLANQUE
* ------------------------------------------------------------------------------
*  Description : Définitions pour le module VGA.C
*
* ------------------------------------------------------------------------------
*  Historique  :
*           Date           |         Auteur          |       Description
* ------------------------------------------------------------------------------
*  05/11/2002              | L.DEPLANQUE             | creation
* ------------------------------------------------------------------------------
*  06/10/2006              | L.DEPLANQUE             | Version 0.1am:
*                          |                         | Gestion 512K de ram.
* ------------------------------------------------------------------------------
*  22/03/2007              | L.DEPLANQUE             | Version 0.1an:
*                          |                         | Mémorisation nouveau
*                          |                         | mode demandé avant
*                          |                         | affectation à la
*                          |                         | prochaine HBL
********************************************************** !END! **************/


#ifndef VGA_H
#define VGA_H


#define   ROMINF_OFF        0x04
#define   ROMSUP_OFF        0x08

extern UBYTE MemCPC[ 0x90000 ];

extern int TabCoul[ 32 ];

extern UBYTE * TabPOKE[ 4 ];
extern UBYTE * TabPEEK[ 4 ];

extern int DelayGa;


void VGASyncColor( void );

void WriteVGA( int val );

void WriteROM( int val );

void SetMemCPC( void );

BOOL ReadRom( char * NomFic, int numRom );

BOOL InitMemCPC( void );


#endif
