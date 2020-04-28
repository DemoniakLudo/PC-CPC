/******************************************************************************/
/* Configuration pour l'archivage des différents éléments du fichier source   */
/******************************************************************************/
// !CONFIG!=/L/* /R/* /W"* Nom : "
// Définition du système       !CONFIG!=/V1!EMULATEUR CPC!
// Définition du sous système  !CONFIG!=/V2!PC-CPC!
// Définition du sous ensemble !CONFIG!=/V3!Accessoires!
// Définition du module        !CONFIG!=/V4!Gestion Configuration!
/******************************************************************************/

/********************************************************* !NAME! **************
* !./FLE!
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\Fichiers
********************************************************** !0! *****************
* ------------------------------------------------------------------------------
*  Fichier     : CONFIG.H              | Version : 0.1ao
* ------------------------------------------------------------------------------
*  Date        : 05/11/2002            | Auteur  : L.DEPLANQUE
* ------------------------------------------------------------------------------
*  Description : Définitions pour le module CONFIG.C
*
* ------------------------------------------------------------------------------
*  Historique  :
*           Date           |         Auteur          |       Description
* ------------------------------------------------------------------------------
*  05/11/2002              | L.DEPLANQUE             | creation
* ------------------------------------------------------------------------------
*  06/01/2003              | L.DEPLANQUE             | Version 0.1t : Gestion
*                          |                         | nouvelles variables :
*                          |                         | localisation des roms
* ------------------------------------------------------------------------------
*  09/01/2003              | L.DEPLANQUE             | Version 0.1u : Gestion
*                          |                         | nouvelles variables :
*                          |                         | touches joystick
* ------------------------------------------------------------------------------
*  03/02/2003              | L.DEPLANQUE             | Version 0.1v : Gestion
*                          |                         | nouvelles variables :
*                          |                         | localisation fichiers
*                          |                         | DSK, SNA, TAP
* ------------------------------------------------------------------------------
*  04/08/2004              | L.DEPLANQUE             | Version 0.1ab : 
*                          |                         | Prise en compte de
*                          |                         | 16 roms d'extensions.
* ------------------------------------------------------------------------------
*  05/10/2004              | L.DEPLANQUE             | Version 0.1ac : 
*                          |                         | Prise en compte de
*                          |                         | l'émulation de disque
*                          |                         | avec la commande
*                          |                         | avec la commande
*                          |                         | #NEW_DISC ON ou OFF
* ------------------------------------------------------------------------------
*  11/12/2004              | L.DEPLANQUE             | Version 0.1ae : 
*                          |                         | Ajout d'un flag pour
*                          |                         | utilisation de la rom
*                          |                         | pack avec la commande
*                          |                         | #USE_ROM_PACK ON ou OFF
* ------------------------------------------------------------------------------
*  02/06/2005              | L.DEPLANQUE             | Version 0.1ag : 
*                          |                         | Ajout de flags divers
*                          |                         | (sons cassette,
*                          |                         | émulation iprimante...)
* ------------------------------------------------------------------------------
*  30/11/2005              | L.DEPLANQUE             | Version 0.1ah : 
*                          |                         | Suppression flag
*                          |                         | utilisation rom pack
*                          |                         | (prise comme rom externe)
*                          |                         | Ajout variable répertoire
*                          |                         | pour sauvegarde .AVI
* ------------------------------------------------------------------------------
*  08/02/2006              | L.DEPLANQUE             | Version 0.1ak : 
*                          |                         | Suppression flag
*                          |                         | inutiles (maintenant
*                          |                         | calculés avec les
*                          |                         | registres du CRTC
* ------------------------------------------------------------------------------
*  20/07/2006              | L.DEPLANQUE             | Version 0.1al : 
*                          |                         | Suppression flag
*                          |                         | #SOUND, ajout flag
*                          |                         | #DOUBLE_LINES
*                          |                         | #BORDERLESS
*                          |                         | #RESO_X
*                          |                         | #RESO_Y
*                          |                         | #DIR_YM_FILE
* ------------------------------------------------------------------------------
*  06/09/2006              | L.DEPLANQUE             | Version 0.1am :
*                          |                         | Suppression flag
*                          |                         | #LIGNE_DIRECT
*                          |                         | ajout flag
*                          |                         | #USE_512KO
* ------------------------------------------------------------------------------
*  23/02/2007              | L.DEPLANQUE             | Version 0.1an :
*                          |                         | Ajout Flags
*                          |                         | #USE_PC_VBL,
*                          |                         | #JOY_XXX_KEY
* ------------------------------------------------------------------------------
*  30/05/2007              | L.DEPLANQUE             | Version 0.1ao :
*                          |                         | Ajout Flags
*                          |                         | #RGB_COLORXX
*                          |                         | #MONO_COLORXX
* ------------------------------------------------------------------------------
********************************************************** !END! **************/


#ifndef CONFIG_H
#define CONFIG_H


#define     MAX_ROM_EXT     256

#define     LEN_PATH        256

#define     CONFIG_NAME     "CONFIG.CPC"


extern int DoubleLines;

extern int PalEmul;

extern int BorderLess;

extern int Monochrome;

extern int TailleScrX;

extern int TailleScrY;

extern int Multiface;

extern int LogLevel;

extern int ComputerName;

extern char LocRomInf[ LEN_PATH ];

extern char LocRomExt[ MAX_ROM_EXT ][ LEN_PATH ];

extern char LocRomMulti[ LEN_PATH ];

extern char DirSnapRead[ LEN_PATH ];

extern char DirSnapWrite[ LEN_PATH ];

extern char DirTape[ LEN_PATH ];

extern char DirDsk[ LEN_PATH ];

extern char DirEmuDisc[ LEN_PATH ];

extern char DirAviFile[ LEN_PATH ];

extern char DirYMFile[ LEN_PATH ];

extern char DirBMPFile[ LEN_PATH ];

extern char NomDsk[ 2 ][ LEN_PATH ];

extern char NomSnapRead[ LEN_PATH ];

extern char NomSnapWrite[ LEN_PATH ];

extern char NomYMFile[ LEN_PATH ];

extern char NomAviFile[ LEN_PATH ];

extern char NomTape[ LEN_PATH ];

extern char NomAsm[ LEN_PATH ];

extern int NewDiscOn;

extern int UseSoundCas;

extern int UsePrinter;

extern int FreqWavOut;

extern int Use512Ko;

extern int Use128Ko;

extern int UseCpcPlus;

extern int ShowMenu;

extern int SndEnabled;

extern int SndFreqReplay;

extern stToucheClav StTabRedirectJoy[ 16 ];

extern unsigned int RgbCPCColor[ 256 ];

extern unsigned int RgbCPCMono[ 256 ];


char * GetName( char * path );

char * GetPath( char * path );

BOOL ReadConfig( char * Nom, BOOL InitFiles );

BOOL SaveConfig( char * Nom );


#endif
