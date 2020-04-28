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
*  Fichier     : PPI.H                 | Version : 0.1am
* ------------------------------------------------------------------------------
*  Date        : 05/11/2002            | Auteur  : L.DEPLANQUE
* ------------------------------------------------------------------------------
*  Description : Définitions pour le module PPI.C
*
* ------------------------------------------------------------------------------
*  Historique  :
*           Date           |         Auteur          |       Description
* ------------------------------------------------------------------------------
*  05/11/2002              | L.DEPLANQUE             | creation
* ------------------------------------------------------------------------------
*  07/11/2002              | L.DEPLANQUE             | Ajout gestion lecture et
*                          |                         | écriture cassette.:
*                          |                         | fonctions WriteCas() et
*                          |                         | ReadCas().
* ------------------------------------------------------------------------------
*  06/12/2006              | L.DEPLANQUE             | Version 0.1am :
*                          |                         | Variable "VBL" remplacée
*                          |                         | par "VSync" et déplacée
*                          |                         | dans le module CRTC
* ------------------------------------------------------------------------------
********************************************************** !END! **************/


#ifndef PPI_H
#define PPI_H


extern int clav[ 16 ];

extern int TpsTapeIn;

extern int TpsTapeOut;

void WritePPI( int adr, int val );

int ReadPPI( int adr );

void WriteCas( void );

void ReadCas( void );

void OpenTapWrite( char * Nom );

void OpenTapRead( char * Nom );

void CloseTapIn( void );

void CloseTapOut( void );

void SetPosTapeIn( int Pos );

int GetPosTapeIn( void );


#endif


