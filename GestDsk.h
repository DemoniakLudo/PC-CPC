/******************************************************************************/
/* Configuration pour l'archivage des différents éléments du fichier source   */
/******************************************************************************/
// !CONFIG!=/L/* /R/* /W"* Nom : "
// Définition du système       !CONFIG!=/V1!EMULATEUR CPC!
// Définition du sous système  !CONFIG!=/V2!PC-CPC!
// Définition du sous ensemble !CONFIG!=/V3!Accessoires!
// Définition du module        !CONFIG!=/V4!Gestion fichiers DSK!
/******************************************************************************/

/********************************************************* !NAME! **************
* !./FLE!
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\Fichiers
********************************************************** !0! *****************
* ------------------------------------------------------------------------------
*  Fichier     : GESTDSK.H             | Version : 0.1at
* ------------------------------------------------------------------------------
*  Date        : 24/01/2011            | Auteur  : L.DEPLANQUE
* ------------------------------------------------------------------------------
*  Description : Définitions pour le module GestDsk.cpp
*
* ------------------------------------------------------------------------------
*  Historique  :
*           Date           |         Auteur          |       Description
* ------------------------------------------------------------------------------
*  24/01/2011              | L.DEPLANQUE             | creation
* ------------------------------------------------------------------------------
********************************************************** !END! **************/


#ifndef GESTDSK_H
#define GESTDSK_H

//#include  <windows.h>


#define     MAX_DSK         2       // Nbre de fichiers DSK gérés

#define     MAX_TRACKS      99      // Nbre maxi de pistes/DSK

#define     MAX_SECTS       29      // Nbre maxi de secteurs/pistes


/********************************************************* !NAME! **************
* Nom : CPCEMUEnt
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Structures
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : En-tête d'un fichier DSK
*
********************************************************** !0! ****************/
typedef struct
    {
    char  debut[ 0x30 ]; // "MV - CPCEMU Disk-File\r\nDisk-Info\r\n"
    UBYTE NbTracks;
    UBYTE NbHeads;
    SHORT TrackSize; // 0x1300 = 256 + ( 512 * nbsecteurs )
    UBYTE TrackSizeTable[ 0xCC ]; // Si "EXTENDED CPC DSK File"
    } CPCEMUEnt;


/********************************************************* !NAME! **************
* Nom : CPCEMUSect
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Structures
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Description d'un secteur
*
********************************************************** !0! ****************/
typedef struct
    {
    UBYTE C;                // track
    UBYTE H;                // head
    UBYTE R;                // sect
    UBYTE N;                // size
    UBYTE ST1;              // Valeur ST1
    UBYTE ST2;              // Valeur ST2
    SHORT SectSize;         // Taille du secteur en octets
    } CPCEMUSect;


/********************************************************* !NAME! **************
* Nom : CPCEMUTrack
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Structures
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Description d'une piste
*
********************************************************** !0! ****************/
typedef struct
    {
    char        ID[ 0x10 ];   // "Track-Info\r\n"
    UBYTE       Track;
    UBYTE       Head;
    SHORT       Unused;
    UBYTE       SectSize; // 2
    UBYTE       NbSect;   // 9
    UBYTE       Gap3;     // 0x4E
    UBYTE       OctRemp;  // 0xE5
    CPCEMUSect  Sect[ MAX_SECTS ];
    } CPCEMUTrack;


typedef struct
    {
    char        NomFic[ 256 ];
    DWORD       LongFic;
    UBYTE       Image;
    CPCEMUEnt   Infos;
    CPCEMUTrack * Tracks[ MAX_TRACKS ][ 2 ];
    UBYTE * Data[ MAX_TRACKS ][ 2 ];
    UBYTE       FlagWrite;
    } ImgDSK;


extern ImgDSK Dsk[ MAX_DSK ];


void ResetDSK( void );

void EjectDSK( HWND hWnd, int DrvNum );

void SetDSK( HWND hWnd, char * n, int DrvNum );

#endif
