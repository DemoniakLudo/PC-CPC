/******************************************************************************/
/* Configuration pour l'archivage des différents éléments du fichier source   */
/******************************************************************************/
// !CONFIG!=/L/* /R/* /W"* Nom : "
// Définition du système       !CONFIG!=/V1!EMULATEUR CPC!
// Définition du sous système  !CONFIG!=/V2!PC-CPC!
// Définition du sous ensemble !CONFIG!=/V3!Chips!
// Définition du module        !CONFIG!=/V4!CRTC 6845!
/******************************************************************************/

/********************************************************* !NAME! **************
* !./FLE!
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\Fichiers
********************************************************** !0! *****************
* ------------------------------------------------------------------------------
*  Fichier     : CRTC.H                | Version : 0.1an
* ------------------------------------------------------------------------------
*  Date        : 05/11/2002            | Auteur  : L.DEPLANQUE
* ------------------------------------------------------------------------------
*  Description : Définitions pour le module CRTC.C
*
* ------------------------------------------------------------------------------
*  Historique  :
*           Date           |         Auteur          |       Description
* ------------------------------------------------------------------------------
*  05/11/2002              | L.DEPLANQUE             | creation
* ------------------------------------------------------------------------------
*  13/11/2002              | L.DEPLANQUE             | Changement du type de
*                          |                         | la variable TabAdrCrtc : 
*                          |                         | passage en USHORT, pour
*                          |                         | optimiser le memset
*                          |                         | dans CalcAdrEcrCPC()
* ------------------------------------------------------------------------------
*  08/02/2006              | L.DEPLANQUE             | Version 0.1ak :
*                          |                         | Ajout variables XStart,
*                          |                         | XEnd, et fonction      
*                          |                         | CalcCRTCLine
* ------------------------------------------------------------------------------
*  06/12/2006              | L.DEPLANQUE             | Version 0.1am :
*                          |                         | Variable "VBL" remplacée
*                          |                         | par "VSync" et supprimée
*                          |                         | du module PPI
* ------------------------------------------------------------------------------
*  01/03/2007              | L.DEPLANQUE             | Version 0.1an :
*                          |                         | Réécriture complète de
*                          |                         | la fonction principale
*                          |                         | du CRTC, prise en compte
*                          |                         | d'un cycle (1 mot vidéo)
*                          |                         | au lieu d'une ligne HBL
* ------------------------------------------------------------------------------
********************************************************** !END! **************/


#ifndef CRTC_H
#define CRTC_H


extern int VSync;

extern int LastMode;


int ReadCRTC( int adr );

void WriteCRTC( int adr, int val );

int CycleCRTC( int NbCycles );

void ResetCRTC( void );


#endif
