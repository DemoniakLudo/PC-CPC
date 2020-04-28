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
*  Fichier     : ROMPACK.H             | Version : 0.1am
* ------------------------------------------------------------------------------
*  Date        : 30/11/2005            | Auteur  : L.DEPLANQUE
* ------------------------------------------------------------------------------
*  Description : Définitions pour le module ROMPACK.C 
*
* ------------------------------------------------------------------------------
*  Historique  :
*           Date           |         Auteur          |       Description
* ------------------------------------------------------------------------------
*  30/11/2005              | L.DEPLANQUE             | creation
* ------------------------------------------------------------------------------
*  18/12/2005              | L.DEPLANQUE             | Version 0.1aj :
*                          |                         | Ajout instruction ED_09
*                          |                         | (pour rompack)
* ------------------------------------------------------------------------------
*  16/02/2006              | L.DEPLANQUE             | Version 0.1ak :
*                          |                         | Ajout instruction ED_0A
*                          |                         | (pour rompack)
* ------------------------------------------------------------------------------
*  27/06/2006              | L.DEPLANQUE             | Version 0.1al :
*                          |                         | Ajout instruction ED_0B
*                          |                         | à ED_16 (pour rompack)
* ------------------------------------------------------------------------------
*  20/09/2006              | L.DEPLANQUE             | Version 0.1am :
*                          |                         | Ajout instruction ED_17
*                          |                         | à ED_1C
* ------------------------------------------------------------------------------
*  22/03/2007              | L.DEPLANQUE             | Version 0.1an :
*                          |                         | Ajout instruction ED_1D
*                          |                         | à ED_1F
* ------------------------------------------------------------------------------
********************************************************** !END! **************/


#ifndef ROMPACK_H
#define ROMPACK_H


extern int RomPack;


//
// Fonctions de la "rom pack"
//
int ED_00( void );

int ED_01( void );

int ED_02( void );

int ED_03( void );

int ED_04( void );

int ED_05( void );

int ED_06( void );

int ED_07( void );

int ED_08( void );

int ED_09( void );

int ED_0A( void );

int ED_0B( void );

int ED_0C( void );

int ED_0D( void );

int ED_0E( void );

int ED_0F( void );

int ED_10( void );

int ED_11( void );

int ED_12( void );

int ED_13( void );

int ED_14( void );

int ED_15( void );

int ED_16( void );

int ED_17( void );

int ED_18( void );

int ED_19( void );

int ED_1A( void );

int ED_1B( void );

int ED_1C( void );

int ED_1D( void );

int ED_1E( void );

int ED_1F( void );

int ED_20( void );

int ED_21( void );

int ED_22( void );

int ED_23( void );

int ED_24( void );

int ED_25( void );

int ED_26( void );

int ED_27( void );

int ED_28( void );

int ED_29( void );

int ED_2A( void );

int ED_2B( void );

int ED_2C( void );

int ED_2D( void );

int ED_2E( void );

int ED_2F( void );

int ED_30( void );

int ED_31( void );

int ED_32( void );

int ED_33( void );

int ED_34( void );

int ED_35( void );

int ED_36( void );

int ED_37( void );

int ED_38( void );

int ED_39( void );

int ED_3A( void );

int ED_3B( void );

int ED_3C( void );

int ED_3D( void );

int ED_3E( void );

int ED_3F( void );

#endif
