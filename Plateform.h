/******************************************************************************/
/* Configuration pour l'archivage des différents éléments du fichier source   */
/******************************************************************************/
// !CONFIG!=/L/* /R/* /W"* Nom : "
// Définition du système       !CONFIG!=/V1!EMULATEUR CPC!
// Définition du sous système  !CONFIG!=/V2!PC-CPC!
// Définition du sous ensemble !CONFIG!=/V3!Noyau!
// Définition du module        !CONFIG!=/V4!Ecran Clavier!
/******************************************************************************/

/********************************************************* !NAME! **************
* !./FLE!
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\Fichiers
********************************************************** !0! *****************
* ------------------------------------------------------------------------------
*  Fichier     : PLATEFORM.H           | Version : 0.1al
* ------------------------------------------------------------------------------
*  Date        : 05/11/2002            | Auteur  : L.DEPLANQUE
* ------------------------------------------------------------------------------
*  Description : Définitions pour le module PLATEFORM.C
*
* ------------------------------------------------------------------------------
*  Historique  :
*           Date           |         Auteur          |       Description
* ------------------------------------------------------------------------------
*  05/11/2002              | L.DEPLANQUE             | creation
* ------------------------------------------------------------------------------
*  07/02/2006              | L.DEPLANQUE             | Version 0.1ak:
*                          |                         | Modification prototypes
*                          |                         | des fonctions de tracé
*                          |                         | de lignes
* ------------------------------------------------------------------------------
*  21/07/2006              | L.DEPLANQUE             | Version 0.1al:
*                          |                         | Modification chaine de
*                          |                         | version
* ------------------------------------------------------------------------------
********************************************************** !END! **************/


#ifndef PLATEFORM_H
#define PLATEFORM_H


#include  <windows.h>


//
// Version
//
#define     VERSION         "0.1at beta32"

#define     TITRE           "PC-CPC - Version " VERSION " (Build " __DATE__ ")"


//
// Taille affichage écran
//
#define     DISPLAY_X   768
#define     DISPLAY_Y   544


//
// Couleurs textes et fond
//
#define     TEXT_COLOR      RGB( 0xFF, 0xFF, 0xFF )
#define     BK_COLOR        RGB( 0x00, 0x00, 0x00 )


typedef void ( * pfctDraw )( int, int, int, BOOL );


extern int DebugMode, Pause;

extern HWND hWnd;

extern HDC DeviceContext;

extern int Turbo;

extern int * MemBitmap;

extern BITMAPINFO pbmi;

extern char ExecutableDir[ MAX_PATH ];

extern BOOL ClavFree;

extern stToucheClav TrKey[ 256 ];

void TraceMot32B640( int x, int y, int Adr, BOOL VBL );

char * OpenNewFile( char * Filtre, char * Ext, char * Dir, char * OldName, BOOL Save );

void Erreur( char * Msg );

void Info( char * Msg );

void AfficheInfos( void );

void AffichePiste( void );

void LoadDSK( char * Nom, int Drive );

BOOL Register( HINSTANCE i, WNDCLASSEX * wcex );

BOOL InitScreen( HINSTANCE hInstance, int nCmdShow );

void EndScreen( void );

void ResetCPC( BOOL InitMem );

void SetEmulCmd( UBYTE * cmd );

void Unregister( HINSTANCE hInstance, WNDCLASSEX * wcex );

int GetTpsRefresh( void );

void WaitPcVBL( void );

void InitPlateforme( void );

void UpdateScreen( void );

void TestJoy( void );

void SetLedFDC( int Etat );

void SetKeybEn( void );

int OpenZipFile( char * Nom, int Drive, int ForceType );

#endif
