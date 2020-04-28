/******************************************************************************/
/* Configuration pour l'archivage des différents éléments du fichier source   */
/******************************************************************************/
// !CONFIG!=/L/* /R/* /W"* Nom : "
// Définition du système       !CONFIG!=/V1!EMULATEUR CPC!
// Définition du sous système  !CONFIG!=/V2!PC-CPC!
// Définition du sous ensemble !CONFIG!=/V3!Accessoires!
// Définition du module        !CONFIG!=/V4!WriteAvi!
/******************************************************************************/

/********************************************************* !NAME! **************
* !./FLE!
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\Fichiers
********************************************************** !0! *****************
* ------------------------------------------------------------------------------
*  Fichier     : WRITEAVI.H            | Version : 0.1ah
* ------------------------------------------------------------------------------
*  Date        : 30/11/2005            | Auteur  : L.DEPLANQUE
* ------------------------------------------------------------------------------
*  Description : Définitions pour le module WRITEAVI.C
*
* ------------------------------------------------------------------------------
*  Historique  :
*           Date           |         Auteur          |       Description
* ------------------------------------------------------------------------------
*  30/11/2005              | L.DEPLANQUE             | creation
* ------------------------------------------------------------------------------
********************************************************** !END! **************/


#ifndef WRITEAVI_H
#define WRITEAVI_H


BOOL WriteAviInit( HWND hWnd, char * Nom, int TailleX, int TailleY );

BOOL WriteAviFrame( void );

BOOL WriteAviSound( UBYTE * Buff, int Longueur );

void WriteAviEnd( HWND hWnd );

BOOL WriteAviStarted( void );


#endif
