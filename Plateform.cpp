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
*          SYSTEME         |      SOUS SYSTEME       |      SOUS ENSEMBLE
* ------------------------------------------------------------------------------
*  EMULATEUR CPC           | PC-CPC                  | Noyau
* ------------------------------------------------------------------------------
*  Fichier     : PLATEFORM.C           | Version : 0.1at
* ------------------------------------------------------------------------------
*  Date        : 05/11/2002            | Auteur  : L.DEPLANQUE
* ------------------------------------------------------------------------------
*  Description : Gestion écran, clavier
*
* ------------------------------------------------------------------------------
*  Historique  :
*           Date           |         Auteur          |       Description
* ------------------------------------------------------------------------------
*  05/11/2002              | L.DEPLANQUE             | creation
* ------------------------------------------------------------------------------
*  07/11/2002              | L.DEPLANQUE             | Ajout gestion lecture et
*                          |                         | écriture cassette :
*                          |                         | touches F9 (lecture) et
*                          |                         | F11 (écriture)
* ------------------------------------------------------------------------------
*  13/11/2002              | L.DEPLANQUE             | Passage du nom de fichier
*                          |                         | snapshot en paramètre aux
*                          |                         | fonctions LireSnap() et
*                          |                         | SauveSnap().  
* ------------------------------------------------------------------------------
*  06/01/2003              | L.DEPLANQUE             | Version 0.1t : Happy new
*                          |                         | Year
* ------------------------------------------------------------------------------
*  09/01/2003              | L.DEPLANQUE             | Version 0.1u : Ajout d'un
*                          |                         | tableau pour permettre la
*                          |                         | redéfinition dynamique
*                          |                         | des touches émulant le
*                          |                         | joystick
* ------------------------------------------------------------------------------
*  03/02/2003              | L.DEPLANQUE             | Version 0.1v : Gestion
*                          |                         | localisation fichiers
*                          |                         | DSK, SNA, TAP
* ------------------------------------------------------------------------------
*  06/05/2003              | L.DEPLANQUE             | Version 0.1w : Correction
*                          |                         | d'un bug dans le mapping
*                          |                         | des touches joystick :
*                          |                         | Le membre .Ligne ne doit
*                          |                         | pas dépasser 0x0F (15)
*                          |                         | car clav[ nb=16 ]        
* ------------------------------------------------------------------------------
*  25/02/2004              | L.DEPLANQUE             | Version 0.1y : Changement
*                          |                         | du nom de l'application
*                          |                         | dans le titre de la
*                          |                         | fenêtre (PC-CPC) et
*                          |                         | mise à disposition de la
*                          |                         | variable de titre        
* ------------------------------------------------------------------------------
*  30/03/2004              | L.DEPLANQUE             | Version 0.1z
* ------------------------------------------------------------------------------
*  21/04/2004              | L.DEPLANQUE             | Version 0.1aa :
*                          |                         | Correction d'un petit
*                          |                         | bug dans les fonctions
*                          |                         | de dessin des lignes
*                          |                         | écran : couleur du bord
* ------------------------------------------------------------------------------
*  04/11/2004              | L.DEPLANQUE             | Version 0.1ad :
*                          |                         | Possibilité de choisir
*                          |                         | le répertoire dans le
*                          |                         | mode émulation fichier
*                          |                         | et affichage du
*                          |                         | répertoire ou de la
*                          |                         | disquette en cours
* ------------------------------------------------------------------------------
*  16/05/2005              | L.DEPLANQUE             | Version 0.1af :
*                          |                         | Modification gestion des
*                          |                         | touches F7 et F12 :
*                          |                         | F7 passe en mode "dsk" et
*                          |                         | F12 pass en mode "direct"
* ------------------------------------------------------------------------------
*  02/06/2005              | L.DEPLANQUE             | Version 0.1ag :
*                          |                         | Utilisation de fichiers
*                          |                         | .WAV pour l'émulation de
*                          |                         | lecture et écriture sur
*                          |                         | cassette
* ------------------------------------------------------------------------------
*  30/11/2005              | L.DEPLANQUE             | Version 0.1ah:
*                          |                         | Gestion touche 'fin'
*                          |                         | pour sauvegarde .AVI
* ------------------------------------------------------------------------------
*  08/12/2005              | L.DEPLANQUE             | Version 0.1ai:
*                          |                         | Mémorise le nom du
*                          |                         | dernier fichier DSK
*                          |                         | chargé.
*                          |                         | Modification paramètres
*                          |                         | bitmap pour sauvegarde
*                          |                         | BMP
* ------------------------------------------------------------------------------
*  07/02/2006              | L.DEPLANQUE             | Version 0.1ak:
*                          |                         | Modification routines
*                          |                         | de tracé de lignes
* ------------------------------------------------------------------------------
*  21/07/2006              | L.DEPLANQUE             | Version 0.1al:
*                          |                         | Possibilité d'utiliser
*                          |                         | une fenêtre sans bords
*                          |                         | ni barre de titre
*                          |                         | Modification affectation
*                          |                         | touches fonctions (avec
*                          |                         | shift)
*                          |                         | Prise en compte du
*                          |                         | joystick
* ------------------------------------------------------------------------------
*  21/08/2006              | L.DEPLANQUE             | Version 0.1am:
*                          |                         | Suppression du mode
*                          |                         | "Ligne directe".
*                          |                         | Possibilité de changer
*                          |                         | de résolution en cours
*                          |                         | de fonctionnement.
* ------------------------------------------------------------------------------
*  05/03/2007              | L.DEPLANQUE             | Version 0.1an:
*                          |                         | Remplacement des fcts de
*                          |                         | tracé de lignes par des
*                          |                         | fcts de tracé de mots
*                          |                         | (CRTC précis au mot)
*                          |                         | Amélioration du rendu
*                          |                         | des couleurs en mode
*                          |                         | 32 bits et petit écran.
*                          |                         | Gestion des 10 boutons
*                          |                         | du joystick 1 avec la
*                          |                         | possibilité d'affecter
*                          |                         | une touche du clavier
*                          |                         | à chaque fonction du
*                          |                         | joystick.
*                          |                         | Ajout de la touche
*                          |                         | SHIFT+F6 pour minimiser
*                          |                         | la fenêtre.
* ------------------------------------------------------------------------------
*  08/09/2008              | L.DEPLANQUE             | Version 0.1ao :
*                          |                         | Ajout de la touche
*                          |                         | SHIFT + F7
*                          |                         | Correction pb joystick 2
* ------------------------------------------------------------------------------
*  16/04/2010              | L.DEPLANQUE             | Version 0.1ap :
*                          |                         | Ajout de la touche
*                          |                         | SHIFT + 12 (Assembleur)
*                          |                         | Modification nom fcts
* ------------------------------------------------------------------------------
*  16/08/2010              | L.DEPLANQUE             | Version 0.1ar :
*                          |                         | Correction pb joystick 2
* ------------------------------------------------------------------------------
*  15/12/2010              | L.DEPLANQUE             | Version 0.1as :
*                          |                         | Utilisation SoundOff()
* ------------------------------------------------------------------------------
*  03/02/2011              | L.DEPLANQUE             | Version 0.1at :
*                          |                         | Gestion des DSK pour
*                          |                         | les lecteurs A: et B:
*                          |                         | Prise en compte du drag
*                          |                         | & drop de fichiers.
*                          |                         | Correction du mode 3
*                          |                         | (mode 0/4 couleurs)
* ------------------------------------------------------------------------------
*  07/05/2012              | StephBB75               | Drag'n'Drop répertoire
*                          |                         | Mémorisation du dernier
*                          |                         | répertoire choisi (pour
*                          |                         | lecture directe depuis
*                          |                         | PC)
* ------------------------------------------------------------------------------
*  10/05/2012              | StephBB75               | Lecture des fichier ZIP
*                          |                         | Ouverture de fichier ZIP
*                          |                         | Contenant soit un DSK, 
*                          |                         | SNA, WAV ou ASM et
*                          |                         | ouverture du fichier.
********************************************************** !END! **************/


#include  <afxwin.h>
#include  <afxdlgs.h>
#include  <shlobj.h>
#include  <mmsystem.h>
#include  <ddraw.h>

#include  "types.h"
#include  "plateform.h"
#include  "Resource.h"
#include  "AsmOutput.h"
#include  "config.h"
#include  "AY8912.h"
#include  "z80.h"
#include  "ppi.h"
#include  "vga.h"
#include  "upd.h"
#include  "crtc.h"
#include  "GestDsk.h"
#include  "snapshot.h"
#include  "EditConfig.h"
#include  "log.h"
#include  "PosFileTape.h"
#include  "multiface.h"
#include  "DebugDlg.h"
#include  "SauveBmp.h"
#include  "newdisc.h"
#include  "RomPack.h"
#include  "WriteAvi.h"
#include  "WriteYM.h"
#include  "Aide.h"
#include  "Asic.h"
#include  "AsmOutput.h"
#include  "ActionReplay.h"
#ifdef USE_PHENIX
#include  "PhenixAccess.h"
#endif

#include  "Zip/zip.h"
#include  "Zip/unzip.h"
#include  "DialZip.h"

#define     MODULENAME      "Plateform"

extern int finMain;

#define HALF 0xFEFEFE           // Masque pour "emulation pal"

//
// Nombre max. de modes plein écran
//
#define     D3DAPP_MAXMODES     1024

/********************************************************* !NAME! **************
* Nom : StMode
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Structures
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Structure modes plein écran
*
********************************************************** !0! ****************/
typedef struct {
	int TailleX;
	int TailleY;
	int Depth;
} StMode;

/********************************************************* !NAME! **************
* Nom : lpDD
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Pointeur sur objet DirectDraw
*
********************************************************** !0! ****************/
static LPDIRECTDRAW lpDD = NULL;

/********************************************************* !NAME! **************
* Nom : lpDDSPrimary
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Pointeur sur objet DirectDrawSurface
*
********************************************************** !0! ****************/
static LPDIRECTDRAWSURFACE lpDDSPrimary = NULL;

/********************************************************* !NAME! **************
* Nom : TabModes
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Tableau contenant la liste des modes plein écran
*
********************************************************** !0! ****************/
static StMode TabModes[ D3DAPP_MAXMODES ];

/********************************************************* !NAME! **************
* Nom : NbModes
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Nombre de modes plein écran
*
********************************************************** !0! ****************/
static int NbModes;

/********************************************************* !NAME! **************
* Nom : Tmp
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Chaine de caractères temporaire
*
********************************************************** !0! ****************/
static char Tmp[ 256 ];

/********************************************************* !NAME! **************
* Nom : menuItem
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Elément du menu
*
********************************************************** !0! ****************/
static MENUITEMINFO menuItem;

/********************************************************* !NAME! **************
* Nom : TailleWinX
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Taille de la fenêtre en X
*
********************************************************** !0! ****************/
static int TailleWinX = 320;

/********************************************************* !NAME! **************
* Nom : TailleWinY
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Taille de la fenêtre en Y
*
********************************************************** !0! ****************/
static int TailleWinY = 200;

/********************************************************* !NAME! **************
* Nom : f
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Police utilisée pour affichage écran
*
********************************************************** !0! ****************/
static HFONT f;

/********************************************************* !NAME! **************
* Nom : hWnd
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Handle de la fenêtre principale
*
********************************************************** !0! ****************/
HWND hWnd;

/********************************************************* !NAME! **************
* Nom : menu
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Menu de la fenêtre principale
*
********************************************************** !0! ****************/
static HMENU menu;

/********************************************************* !NAME! **************
* Nom : DeviceContext
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : structure HDC de la fenêtre principale
*
********************************************************** !0! ****************/
HDC DeviceContext = NULL;

/********************************************************* !NAME! **************
* Nom : DcBitmap
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Structure HDC du bitmap représentant la mémoire écran CPC
*
********************************************************** !0! ****************/
static HDC DcBitmap = NULL;

/********************************************************* !NAME! **************
* Nom : bm
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Handle vers le bitmap contenant l'image de l'écran du CPC
*
********************************************************** !0! ****************/
static HBITMAP bm = NULL;

/********************************************************* !NAME! **************
* Nom : hInstance
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : HANDLE de l'instance de l'application
*
********************************************************** !0! ****************/
static HINSTANCE hInstance;

/********************************************************* !NAME! **************
* Nom : MemBitmap
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Buffer contenant l'image de l'écran du CPC
*
********************************************************** !0! ****************/
int * MemBitmap;

/********************************************************* !NAME! **************
* Nom : pbmi
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Information sur la structure du bitmap représentant l'écran du
*               CPC
*
********************************************************** !0! ****************/
BITMAPINFO pbmi;

/********************************************************* !NAME! **************
* Nom : Turbo
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Mode turbo activé/désactivé
*
********************************************************** !0! ****************/
int Turbo = 0;

/********************************************************* !NAME! **************
* Nom : Pause
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Emulateur en pause
*
********************************************************** !0! ****************/
int Pause = 0;

/********************************************************* !NAME! **************
* Nom : DebugMode
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Mode debug activé/désactivé
*
********************************************************** !0! ****************/
int DebugMode = 0;

/********************************************************* !NAME! **************
* Nom : ClavFree
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Indique si la gestion du clavier est possible ou non
*
********************************************************** !0! ****************/
BOOL ClavFree = TRUE;

/********************************************************* !NAME! **************
* Nom : TrKey
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Transformations touches PC -> CPC
*
********************************************************** !0! ****************/
stToucheClav TrKey[ 256 ] = {
	{ 0xFF, 0x0F },           // 0x00
	{ 0xFF, 0x0F },           // 0x01
	{ 0xFF, 0x0F },           // 0x02
	{ 0xFF, 0x0F },           // 0x03
	{ 0xFF, 0x0F },           // 0x04
	{ 0xFF, 0x0F },           // 0x05
	{ 0xFF, 0x0F },           // 0x06
	{ 0xFF, 0x0F },           // 0x07
	{ 0x7F, 0x09 },           // DEL
	{ 0xEF, 0x08 },           // TAB
	{ 0xFF, 0x0F },           // 0x0A
	{ 0xFF, 0x0F },           // 0x0B
	{ 0xFF, 0x0F },           // 0x0C
	{ 0xFB, 0x02 },           // ENTER
	{ 0xFF, 0x0F },           // 0x0E
	{ 0xFF, 0x0F },           // 0x0F
	{ 0xDF, 0x02 },           // SHIFT
	{ 0x7F, 0x02 },           // CTRL
	{ 0xFD, 0x01 },           // ALTGR -> COPY
	{ 0xFF, 0x0F },           // 0x13
	{ 0xBF, 0x08 },           // CAPS LOCK
	{ 0xFF, 0x0F },           // 0x15
	{ 0xFF, 0x0F },           // 0x16
	{ 0xFF, 0x0F },           // 0x17
	{ 0xFF, 0x0F },           // 0x18
	{ 0xFF, 0x0F },           // 0x19
	{ 0xFF, 0x0F },           // 0x1A
	{ 0xFB, 0x08 },           // ESC
	{ 0xFF, 0x0F },           // 0x1C
	{ 0xFF, 0x0F },           // 0x1D
	{ 0xFF, 0x0F },           // 0x1E
	{ 0xFF, 0x0F },           // 0x1F
	{ 0x7F, 0x05 },           // ' '
	{ 0xFF, 0x0F },           // 0x21
	{ 0xFF, 0x0F },           // 0x22
	{ 0xFF, 0x0F },           // 0x23
	{ 0xFF, 0x0F },           // 0x24
	{ 0xFE, 0x01 },           // Fleche Gauche
	{ 0xFE, 0x00 },           // Fleche Haut
	{ 0xFD, 0x00 },           // Fleche Droite
	{ 0xFB, 0x00 },           // Fleche Bas
	{ 0xFF, 0x0F },           // 0x29
	{ 0xFF, 0x0F },           // 0x2A
	{ 0xFF, 0x0F },           // 0x2B
	{ 0xFF, 0x0F },           // 0x2C
	{ 0xFF, 0x0F },           // 0x2D
	{ 0xFE, 0x02 },           // SUPPR
	{ 0xFF, 0x0F },           // 0x2F
	{ 0xFE, 0x04 },           // '0'
	{ 0xFE, 0x08 },           // '1'
	{ 0xFD, 0x08 },           // '2'
	{ 0xFD, 0x07 },           // '3'
	{ 0xFE, 0x07 },           // '4'
	{ 0xFD, 0x06 },           // '5'
	{ 0xFE, 0x06 },           // '6'
	{ 0xFD, 0x05 },           // '7'
	{ 0xFE, 0x05 },           // '8'
	{ 0xFD, 0x04 },           // '9'
	{ 0xFF, 0x0F },           // 0x3A
	{ 0xFF, 0x0F },           // 0x3B
	{ 0xFF, 0x0F },           // 0x3C
	{ 0xFF, 0x0F },           // 0x3D
	{ 0xFF, 0x0F },           // 0x3E
	{ 0xFF, 0x0F },           // 0x3F
	{ 0xFF, 0x0F },           // 0x40
	{ 0xF7, 0x08 },           // 'A'
	{ 0xBF, 0x06 },           // 'B'
	{ 0xBF, 0x07 },           // 'C'
	{ 0xDF, 0x07 },           // 'D'
	{ 0xFB, 0x07 },           // 'E'
	{ 0xDF, 0x06 },           // 'F'
	{ 0xEF, 0x06 },           // 'G'
	{ 0xEF, 0x05 },           // 'H'
	{ 0xF7, 0x04 },           // 'I'
	{ 0xDF, 0x05 },           // 'J'
	{ 0xDF, 0x04 },           // 'K'
	{ 0xEF, 0x04 },           // 'L'
	{ 0xDF, 0x03 },           // 'M'
	{ 0xBF, 0x05 },           // 'N'
	{ 0xFB, 0x04 },           // 'O'
	{ 0xF7, 0x03 },           // 'P'
	{ 0xDF, 0x08 },           // 'Q'
	{ 0xFB, 0x06 },           // 'R'
	{ 0xEF, 0x07 },           // 'S'
	{ 0xF7, 0x06 },           // 'T'
	{ 0xFB, 0x05 },           // 'U'
	{ 0x7F, 0x06 },           // 'V'
	{ 0x7F, 0x08 },           // 'W'
	{ 0x7F, 0x07 },           // 'X'
	{ 0xF7, 0x05 },           // 'Y'
	{ 0xF7, 0x07 },           // 'Z'
	{ 0xFF, 0x0F },           // 0x5B
	{ 0xFF, 0x0F },           // 0x5C
	{ 0xFF, 0x0F },           // menu contextuel
	{ 0xFF, 0x0F },           // 0x5E
	{ 0xFF, 0x0F },           // 0x5F
	{ 0x7F, 0x01 },           // P'0'
	{ 0xDF, 0x01 },           // P'1'
	{ 0xBF, 0x01 },           // P'2'
	{ 0xDF, 0x00 },           // P'3'
	{ 0xEF, 0x02 },           // P'4'
	{ 0xEF, 0x01 },           // P'5'
	{ 0xEF, 0x00 },           // P'6'
	{ 0xFB, 0x01 },           // P'7'
	{ 0xF7, 0x01 },           // P'8'
	{ 0xF7, 0x00 },           // P'9'
	{ 0xFF, 0x0F },           // 0x6A
	{ 0xFF, 0x0F },           // 0x6B
	{ 0xFF, 0x0F },           // 0x6C
	{ 0xFF, 0x0F },           // 0x6D
	{ 0x7F, 0x00 },           // P'.'
	{ 0xFF, 0x0F },           // 0x6F
	{ 0xFF, 0x0F },           // 0x70
	{ 0xFF, 0x0F },           // 0x71
	{ 0xFF, 0x0F },           // 0x72
	{ 0xFF, 0x0F },           // 0x73
	{ 0xFF, 0x0F },           // 0x74
	{ 0xFF, 0x0F },           // 0x75
	{ 0xFF, 0x0F },           // 0x76
	{ 0xFF, 0x0F },           // 0x77
	{ 0xFF, 0x0F },           // 0x78
	{ 0xFF, 0x0F },           // 0x79
	{ 0xFF, 0x0F },           // 0x7A
	{ 0xFF, 0x0F },           // 0x7B
	{ 0xFF, 0x0F },           // 0x7C
	{ 0xFF, 0x0F },           // 0x7D
	{ 0xFF, 0x0F },           // 0x7E
	{ 0xFF, 0x0F },           // 0x7F
	{ 0xFF, 0x0F },           // 0x80
	{ 0xFF, 0x0F },           // 0x81
	{ 0xFF, 0x0F },           // 0x82
	{ 0xFF, 0x0F },           // 0x83
	{ 0xFF, 0x0F },           // 0x84
	{ 0xFF, 0x0F },           // 0x85
	{ 0xFF, 0x0F },           // 0x86
	{ 0xFF, 0x0F },           // 0x87
	{ 0xFF, 0x0F },           // 0x88
	{ 0xFF, 0x0F },           // 0x89
	{ 0xFF, 0x0F },           // 0x8A
	{ 0xFF, 0x0F },           // 0x8B
	{ 0xFF, 0x0F },           // 0x8C
	{ 0xBF, 0x00 },           // 0x8D   -> Emulation touche "return"
	{ 0xFF, 0x0F },           // 0x8E
	{ 0xFF, 0x0F },           // 0x8F
	{ 0xFF, 0x0F },           // 0x90
	{ 0xFF, 0x0F },           // 0x91
	{ 0xFF, 0x0F },           // 0x92
	{ 0xFF, 0x0F },           // 0x93
	{ 0xFF, 0x0F },           // 0x94
	{ 0xFF, 0x0F },           // 0x95
	{ 0xFF, 0x0F },           // 0x96
	{ 0xFF, 0x0F },           // 0x97
	{ 0xFF, 0x0F },           // 0x98
	{ 0xFF, 0x0F },           // 0x99
	{ 0xFF, 0x0F },           // 0x9A
	{ 0xFF, 0x0F },           // 0x9B
	{ 0xFF, 0x0F },           // 0x9C
	{ 0xFF, 0x0F },           // 0x9D
	{ 0xFF, 0x0F },           // 0x9E
	{ 0xFF, 0x0F },           // 0x9F
	{ 0xFF, 0x0F },           // 0xA0
	{ 0xFF, 0x0F },           // 0xA1
	{ 0xFF, 0x0F },           // 0xA2
	{ 0xFF, 0x0F },           // 0xA3
	{ 0xFF, 0x0F },           // 0xA4
	{ 0xFF, 0x0F },           // 0xA5
	{ 0xFF, 0x0F },           // 0xA6
	{ 0xFF, 0x0F },           // 0xA7
	{ 0xFF, 0x0F },           // 0xA8
	{ 0xFF, 0x0F },           // 0xA9
	{ 0xFF, 0x0F },           // 0xAA
	{ 0xFF, 0x0F },           // 0xAB
	{ 0xFF, 0x0F },           // 0xAC
	{ 0xFF, 0x0F },           // 0xAD
	{ 0xFF, 0x0F },           // 0xAE
	{ 0xFF, 0x0F },           // 0xAF
	{ 0xFF, 0x0F },           // 0xB0
	{ 0xFF, 0x0F },           // 0xB1
	{ 0xFF, 0x0F },           // 0xB2
	{ 0xFF, 0x0F },           // 0xB3
	{ 0xFF, 0x0F },           // 0xB4
	{ 0xFF, 0x0F },           // 0xB5
	{ 0xFF, 0x0F },           // 0xB6
	{ 0xFF, 0x0F },           // 0xB7
	{ 0xFF, 0x0F },           // 0xB8
	{ 0xFF, 0x0F },           // 0xB9
	{ 0xBF, 0x02 },           // '$'        En fct du type de clavier
	{ 0xFE, 0x03 },           // '='
	{ 0xBF, 0x04 },           // ','
	{ 0xFF, 0x0F },           // 0xBD
	{ 0x7F, 0x04 },           // ';'
	{ 0x7F, 0x03 },           // ':'
	{ 0xEF, 0x03 },           // 'œ'
	{ 0xFF, 0x0F },           // 0xC1
	{ 0xFF, 0x0F },           // 0xC2
	{ 0xFF, 0x0F },           // 0xC3
	{ 0xFF, 0x0F },           // 0xC4
	{ 0xFF, 0x0F },           // 0xC5
	{ 0xFF, 0x0F },           // 0xC6
	{ 0xFF, 0x0F },           // 0xC7
	{ 0xFF, 0x0F },           // 0xC8
	{ 0xFF, 0x0F },           // 0xC9
	{ 0xFF, 0x0F },           // 0xCA
	{ 0xFF, 0x0F },           // 0xCB
	{ 0xFF, 0x0F },           // 0xCC
	{ 0xFF, 0x0F },           // 0xCD
	{ 0xFF, 0x0F },           // 0xCE
	{ 0xFF, 0x0F },           // 0xCF
	{ 0xFF, 0x0F },           // 0xD0
	{ 0xFF, 0x0F },           // 0xD1
	{ 0xFF, 0x0F },           // 0xD2
	{ 0xFF, 0x0F },           // 0xD3
	{ 0xFF, 0x0F },           // 0xD4
	{ 0xFF, 0x0F },           // 0xD5
	{ 0xFF, 0x0F },           // 0xD6
	{ 0xFF, 0x0F },           // 0xD7
	{ 0xFF, 0x0F },           // 0xD8
	{ 0xFF, 0x0F },           // 0xD9
	{ 0xFF, 0x0F },           // 0xDA
	{ 0xFD, 0x03 },           // ')'
	{ 0xFD, 0x02 },           // '#'        En fct du type de clavier
	{ 0xFB, 0x03 },           // '^'        En fct du type de clavier
	{ 0xF7, 0x02 },           // '*'        En fct du type de clavier
	{ 0xBF, 0x03 },           // '!'
	{ 0xFF, 0x0F },           // 0xE0
	{ 0xFF, 0x0F },           // 0xE1
	{ 0xF7, 0x02 },           // '*'        En fct du type de clavier
	{ 0xFF, 0x0F },           // 0xE3
	{ 0xFF, 0x0F },           // 0xE4
	{ 0xFF, 0x0F },           // 0xE5
	{ 0xFF, 0x0F },           // 0xE6
	{ 0xFF, 0x0F },           // 0xE7
	{ 0xFF, 0x0F },           // 0xE8
	{ 0xFF, 0x0F },           // 0xE9
	{ 0xFF, 0x0F },           // 0xEA
	{ 0xFF, 0x0F },           // 0xEB
	{ 0xFF, 0x0F },           // 0xEC
	{ 0xFF, 0x0F },           // 0xED
	{ 0xFF, 0x0F },           // 0xEE
	{ 0xFF, 0x0F },           // 0xEF
	{ 0xFF, 0x0F },           // 0xF0
	{ 0xFF, 0x0F },           // 0xF1
	{ 0xFF, 0x0F },           // 0xF2
	{ 0xFF, 0x0F },           // 0xF3
	{ 0xFF, 0x0F },           // 0xF4
	{ 0xFF, 0x0F },           // 0xF5
	{ 0xFF, 0x0F },           // 0xF6
	{ 0xFF, 0x0F },           // 0xF7
	{ 0xFF, 0x0F },           // 0xF8
	{ 0xFF, 0x0F },           // 0xF9
	{ 0xFF, 0x0F },           // 0xFA
	{ 0xFF, 0x0F },           // 0xFB
	{ 0xFF, 0x0F },           // 0xFC
	{ 0xFF, 0x0F },           // 0xFD
	{ 0xFF, 0x0F },           // 0xFE
	{ 0xFF, 0x0F },           // 0xFF
};


/********************************************************* !NAME! **************
* Nom : TabPoints
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Conversions pixels CPC -> écran PC
*
********************************************************** !0! ****************/
static int TabPoints[ 4 ][ 256 ][ 8 ];

/********************************************************* !NAME! **************
* Nom : ZipTypeFile
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Structures
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Structure type de fichier contenue dans un ZIP
*
********************************************************** !0! ****************/
/*
typedef struct
{
char* Name;
int Type;
int index;
} ZipTypeFile;
*/

/********************************************************* !NAME! **************
* Nom : TabZipTypeFile
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Structures
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : tableau de structure type de fichier contenue dans un ZIP
*
********************************************************** !0! ****************/
//static ZipTypeFile TabZipTypeFile[ 2048 ];

/********************************************************* !NAME! **************
* Nom : CalcPoints
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Calcule l'image des points à afficher à l'écran en fonction du
*               mode d'affichage du CPC (mode 0, 1 ou 2)
*
* Résultat    : /
*
* Variables globales modifiées : TabPoints
*
********************************************************** !0! ****************/
static void CalcPoints( void ) {
	int i;

	for ( i = 0; i < 256; i++ ) {
		int b0 = i & 1;
		int b1 = i & 2;
		int b2 = ( i & 4 ) >> 1;
		int b3 = ( i & 8 ) >> 2;
		int b4 = ( i & 0x10 ) >> 4;
		int b5 = ( i & 0x20 ) >> 5;
		int b6 = ( i & 0x40 ) >> 6;
		int b7 = i >> 7;

		TabPoints[ 0 ][ i ][ 0 ] = TabPoints[ 0 ][ i ][ 1 ] = TabPoints[ 0 ][ i ][ 2 ] = TabPoints[ 0 ][ i ][ 3 ] = b7 + ( b5 << 2 ) + b3 + ( b1 << 2 );
		TabPoints[ 0 ][ i ][ 4 ] = TabPoints[ 0 ][ i ][ 5 ] = TabPoints[ 0 ][ i ][ 6 ] = TabPoints[ 0 ][ i ][ 7 ] = b6 + ( b4 << 2 ) + b2 + ( b0 << 3 );

		TabPoints[ 1 ][ i ][ 0 ] = TabPoints[ 1 ][ i ][ 1 ] = b7 + b3;
		TabPoints[ 1 ][ i ][ 2 ] = TabPoints[ 1 ][ i ][ 3 ] = b6 + b2;
		TabPoints[ 1 ][ i ][ 4 ] = TabPoints[ 1 ][ i ][ 5 ] = b5 + b1;
		TabPoints[ 1 ][ i ][ 6 ] = TabPoints[ 1 ][ i ][ 7 ] = b4 + ( b0 << 1 );

		TabPoints[ 2 ][ i ][ 0 ] = b7;
		TabPoints[ 2 ][ i ][ 1 ] = b6;
		TabPoints[ 2 ][ i ][ 2 ] = b5;
		TabPoints[ 2 ][ i ][ 3 ] = b4;
		TabPoints[ 2 ][ i ][ 4 ] = ( b3 >> 1 );
		TabPoints[ 2 ][ i ][ 5 ] = ( b2 >> 1 );
		TabPoints[ 2 ][ i ][ 6 ] = ( b1 >> 1 );
		TabPoints[ 2 ][ i ][ 7 ] = b0;

		TabPoints[ 3 ][ i ][ 0 ] = TabPoints[ 3 ][ i ][ 1 ] = TabPoints[ 3 ][ i ][ 2 ] = TabPoints[ 3 ][ i ][ 3 ] = b7 + b3;
		TabPoints[ 3 ][ i ][ 4 ] = TabPoints[ 3 ][ i ][ 5 ] = TabPoints[ 3 ][ i ][ 6 ] = TabPoints[ 3 ][ i ][ 7 ] = b6 + b2;
	}
}

void SetKeybFr( void ) {
	TrKey[ 0x41 ].Port = 0xF7;        // 'A'
	TrKey[ 0x41 ].Ligne = 0x08;       // 'A'
	TrKey[ 0x5A ].Port = 0xF7;        // 'Z'
	TrKey[ 0x5A ].Ligne = 0x07;       // 'Z'
	TrKey[ 0x51 ].Port = 0xDF;        // 'Q'
	TrKey[ 0x51 ].Ligne = 0x08;       // 'Q'
	TrKey[ 0x4D ].Port = 0xDF;        // 'M'
	TrKey[ 0x4D ].Ligne = 0x03;       // 'M'
	TrKey[ 0x57 ].Port = 0x7F;        // 'W'
	TrKey[ 0x57 ].Ligne = 0x08;       // 'W'
	TrKey[ 0xBC ].Port = 0xBF;        // ','
	TrKey[ 0xBC ].Ligne = 0x04;       // ','


	/*
	TrKey[ 186 ].Port = 0xF7;   // ']'
	TrKey[ 220 ].Port = 0xFB;   // '@'
	TrKey[ 220 ].Ligne = 0x03;
	TrKey[ 221 ].Port = 0xFD;   // '['
	TrKey[ 221 ].Ligne = 0x02;
	TrKey[ 222 ].Port = TrKey[ 226 ].Port = 0xBF;   // '\'
	*/
}

void SetKeybEn( void ) {
	TrKey[ 0x41 ].Port = 0xDF;        // 'Q'
	TrKey[ 0x41 ].Ligne = 0x08;       // 'Q'
	TrKey[ 0x5A ].Port = 0x7F;        // 'W'
	TrKey[ 0x5A ].Ligne = 0x08;       // 'W'
	TrKey[ 0x51 ].Port = 0xF7;        // 'A'
	TrKey[ 0x51 ].Ligne = 0x08;       // 'A'
	TrKey[ 0x4D ].Port = 0xBF;        // ';'
	TrKey[ 0x4D ].Ligne = 0x04;       // ';'
	TrKey[ 0x57 ].Port = 0xF7;        // 'Z'
	TrKey[ 0x57 ].Ligne = 0x07;       // 'Z'
	TrKey[ 0xBC ].Port = 0xDF;        // 'M'
	TrKey[ 0xBC ].Ligne = 0x03;       // 'M'
}

/********************************************************* !NAME! **************
* Nom : SetLedFDC
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Affiche l'état de la LED du contrôleur disc
*
* Résultat    : /
*
* Variables globales modifiées : /
*
********************************************************** !0! ****************/
void SetLedFDC( int Etat ) {
	static RECT Rect = { 660, 550, 674, 556 };

	HBRUSH br = CreateSolidBrush( RGB( Etat ? 0xFF : 0x40, 0x00, 0x00 ) );
	FillRect( DeviceContext, &Rect, br );
	DeleteObject( br );
}

/********************************************************* !NAME! **************
* Nom : AffichePiste
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\\/FLE!, ligne : !./LN!
*
* Description : Affichage du numéro de piste en cours (disquette)
*
* Résultat    : /
*
* Variables globales modifiées : /
*
********************************************************** !0! ****************/
void AffichePiste( void ) {
	static char Msg[ 16 ];

	sprintf( Msg, "Piste:%02d", GetCurrTrack() );
	TextOut( DeviceContext, 710, 544, Msg, ( int )strlen( Msg ) );
}

/********************************************************* !NAME! **************
* Nom : AfficheInfos
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\\/FLE!, ligne : !./LN!
*
* Description : Affichage informations générales en bas de l'écran
*
* Résultat    : /
*
* Variables globales modifiées : /
*
********************************************************** !0! ****************/
void AfficheInfos( void )
{
	static char Cop1[] = "(C) 1996-2011 DEMONIAK (ldeplanque@nordnet.fr)";
	static char Buffer[ MAX_PATH ];

	TextOut( DeviceContext, 20, 544, Cop1, ( int )strlen( Cop1 ) );

	TextOut( DeviceContext, 500, 544, Turbo ? "T" : " ", 1 );
	TextOut( DeviceContext, 560, 544, RomPack ? "RP" : "  ", 2 );

	if ( NewDiscOn )
		sprintf( Buffer, "Repertoire=%-120s", DirEmuDisc );
	else
		sprintf( Buffer, "Disquette=%-120s", Dsk[ 0 ].Image ? NomDsk[ 0 ] : "<Aucun>" );

	TextOut( DeviceContext, 20, 556, Buffer, ( int )strlen( Buffer ) );
	SetLedFDC( 0 );
	AffichePiste();
}

/********************************************************* !NAME! **************
* Nom : BrowseCallbackProc
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : The browse dialog box calls this function to notify it about events.
*
* Résultat    : 0
*
* Variables globales modifiées : /
*
********************************************************** !0! ****************/
int CALLBACK BrowseCallbackProc( HWND hwnd, UINT uMsg, LPARAM lp, LPARAM pData ) {
	if ( uMsg == BFFM_INITIALIZED )
		// WParam is TRUE since you are passing a path
			// it would be FALSE if you were passing a pidl
				SendMessage( hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)(LPCSTR)DirEmuDisc );

	return 0;
}

/********************************************************* !NAME! **************
* Nom : ChoixRepertoire
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Ouvre une fenêtre de sélection de répertoire
*
* Résultat    : TRUE si choix validé, FALSE sinon
*
* Variables globales modifiées : /
*
********************************************************** !0! ****************/
static BOOL ChoixRepertoire( char * Rep ) {
	static char Buffer[ MAX_PATH ];
	BROWSEINFO bi;
	ITEMIDLIST * pidl;

	memset( &bi, 0, sizeof( bi ) );
	bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT | BIF_NEWDIALOGSTYLE;
	bi.hwndOwner = hWnd;
	bi.lpfn = BrowseCallbackProc;
	pidl = SHBrowseForFolder( &bi );
	if ( pidl ) {
		SHGetPathFromIDList( pidl, Buffer );
		strcpy( Rep, Buffer );
		return( TRUE );
	}
	return( FALSE );
}

/********************************************************* !NAME! **************
* Nom : TraceMot32B640
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Trace une ligne écran de 16 points en couleurs 32 bits
*
* Résultat    : /
*
* Variables globales modifiées : /
*
********************************************************** !0! ****************/
void TraceMot32B640( int x, int y, int Adr, BOOL VBL ) {
	static int lastPix;

	int * p = MemBitmap + x + ( DISPLAY_Y - y ) * DISPLAY_X;
	if ( VBL )
		memset( p, 0, 16 * sizeof( int ) );
	else {
		if ( Adr < 0 )
			for ( x = 0; x < 16; x++ ) {
				p[ x ] = TabCoul[ 16 ];
				if ( x == 7 )
					VGASyncColor();
			}
		else {
			int * ad = TabPoints[ LastMode ][ MemCPC[ Adr++ ] ];
			if ( PalEmul ) {
				lastPix = * p++ = ( ( lastPix & HALF ) + ( TabCoul[ ad[ 0 ] ] & HALF ) ) >> 1;
				lastPix = * p++ = ( ( lastPix & HALF ) + ( TabCoul[ ad[ 1 ] ] & HALF ) ) >> 1;
				lastPix = * p++ = ( ( lastPix & HALF ) + ( TabCoul[ ad[ 2 ] ] & HALF ) ) >> 1;
				lastPix = * p++ = ( ( lastPix & HALF ) + ( TabCoul[ ad[ 3 ] ] & HALF ) ) >> 1;
				lastPix = * p++ = ( ( lastPix & HALF ) + ( TabCoul[ ad[ 4 ] ] & HALF ) ) >> 1;
				lastPix = * p++ = ( ( lastPix & HALF ) + ( TabCoul[ ad[ 5 ] ] & HALF ) ) >> 1;
				lastPix = * p++ = ( ( lastPix & HALF ) + ( TabCoul[ ad[ 6 ] ] & HALF ) ) >> 1;
				lastPix = * p++ = ( ( lastPix & HALF ) + ( TabCoul[ ad[ 7 ] ] & HALF ) ) >> 1;
			}
			else {
				* p++ = TabCoul[ ad[ 0 ] ];
				* p++ = TabCoul[ ad[ 1 ] ];
				* p++ = TabCoul[ ad[ 2 ] ];
				* p++ = TabCoul[ ad[ 3 ] ];
				* p++ = TabCoul[ ad[ 4 ] ];
				* p++ = TabCoul[ ad[ 5 ] ];
				* p++ = TabCoul[ ad[ 6 ] ];
				* p++ = TabCoul[ ad[ 7 ] ];
			}
			VGASyncColor();
			ad = TabPoints[ LastMode ][ MemCPC[ Adr ] ];
			if ( PalEmul ) {
				lastPix = * p++ = ( ( lastPix & HALF ) + ( TabCoul[ ad[ 0 ] ] & HALF ) ) >> 1;
				lastPix = * p++ = ( ( lastPix & HALF ) + ( TabCoul[ ad[ 1 ] ] & HALF ) ) >> 1;
				lastPix = * p++ = ( ( lastPix & HALF ) + ( TabCoul[ ad[ 2 ] ] & HALF ) ) >> 1;
				lastPix = * p++ = ( ( lastPix & HALF ) + ( TabCoul[ ad[ 3 ] ] & HALF ) ) >> 1;
				lastPix = * p++ = ( ( lastPix & HALF ) + ( TabCoul[ ad[ 4 ] ] & HALF ) ) >> 1;
				lastPix = * p++ = ( ( lastPix & HALF ) + ( TabCoul[ ad[ 5 ] ] & HALF ) ) >> 1;
				lastPix = * p++ = ( ( lastPix & HALF ) + ( TabCoul[ ad[ 6 ] ] & HALF ) ) >> 1;
				lastPix = * p++ = ( ( lastPix & HALF ) + ( TabCoul[ ad[ 7 ] ] & HALF ) ) >> 1;
			}
			else {
				* p++ = TabCoul[ ad[ 0 ] ];
				* p++ = TabCoul[ ad[ 1 ] ];
				* p++ = TabCoul[ ad[ 2 ] ];
				* p++ = TabCoul[ ad[ 3 ] ];
				* p++ = TabCoul[ ad[ 4 ] ];
				* p++ = TabCoul[ ad[ 5 ] ];
				* p++ = TabCoul[ ad[ 6 ] ];
				* p++ = TabCoul[ ad[ 7 ] ];
			}
			p -= 16;
			if ( AsicOn ) {
				for ( int NumSpt = 0; NumSpt < 16; NumSpt++ ) {
					int CtrlSpt = 0x2000 + ( NumSpt << 3 );
					int xSpt = MemASIC[ CtrlSpt ] + 256 * MemASIC[ CtrlSpt + 1 ];
					xSpt = xSpt + ( ( Cpc.CRTCReg[ 2 ] - Cpc.CRTCReg[ 1 ] ) << 3 );
					int ySpt = ( MemASIC[ CtrlSpt + 2 ] + 256 * MemASIC[ CtrlSpt + 3 ] ) << 1;
					ySpt = ySpt + ( ( 35 - Cpc.CRTCReg[ 7 ] ) << 3 );
					int ZoomX = ( MemASIC[ CtrlSpt + 4 ] >> 2 ) & 3;
					int ZoomY = MemASIC[ CtrlSpt + 4 ] & 3;
					if ( ZoomX == 3 )
						ZoomX = 4;

					if ( ZoomY == 3 )
						ZoomY = 4;

					if (  y >= ySpt && y < ySpt + ( 16 * ZoomY ) && x >= xSpt && x < xSpt + ( 8 * ZoomX ) )
					{
						int LigY = ( ( y - ySpt ) / ZoomY ) >> 1;
						int ColX = ( ( x - xSpt ) / ZoomX );
						int NbX = 16 - ( x - xSpt ) / ZoomX;
						int AdrSpt = ( NumSpt << 8 ) + ( LigY << 4 );
						for ( ; NbX--; ) {
							UBYTE c = ( UBYTE )( MemASIC[ AdrSpt + ( ColX / ZoomX ) ] & 0x0F );
							if ( c )
								p[ ColX ] = TabCoul[ c + 17 ];

							ColX++;
						}
					}
				}
			}
		}
	}
	if ( DoubleLines ) {
		if ( PalEmul )
			for ( int i = 0; i < 16; i++ )
				p[ i + DISPLAY_X ] = ( p[ i ] & HALF ) >> 1;
		else
			memcpy( p + DISPLAY_X, p, 64 );
	}
}

/********************************************************* !NAME! **************
* Nom : FctListeModes
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Liste les modes plein écran disponnibles
*
* Résultat    : DDENUMRET_OK si ok, DDENUMRET_CANCEL sinon
*
* Variables globales modifiées : TabModes, NbModes
*
********************************************************** !0! ****************/
static HRESULT CALLBACK FctListeModes( LPDDSURFACEDESC pddsd, LPVOID lpContext ) {
	TabModes[ NbModes ].TailleX = pddsd->dwWidth;
	TabModes[ NbModes ].TailleY = pddsd->dwHeight;
	TabModes[ NbModes ].Depth = pddsd->ddpfPixelFormat.dwRGBBitCount;
	NbModes++;
	if ( NbModes == D3DAPP_MAXMODES )
		return( DDENUMRET_CANCEL );

	return( DDENUMRET_OK );
}

/********************************************************* !NAME! **************
* Nom : CompareModes
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Fonction de comparaison de deux modes plein écran
*
* Résultat    : Résultat de la comparaison
*
* Variables globales modifiées : /
*
********************************************************** !0! ****************/
static int _cdecl CompareModes( const void * element1, const void * element2 ) {
	StMode * Mode1 = ( StMode * )element1;
	StMode * Mode2 = ( StMode * )element2;

	int a = Mode1->TailleY - Mode2->TailleY;
	if ( a )
		return( a );

	a = Mode1->TailleX - Mode2->TailleX;
	if ( a )
		return( a );

	return( Mode1->Depth - Mode2->Depth );
}

/********************************************************* !NAME! **************
* Nom : SetDirectXDisplayMode
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Positionne le mode plein écran DirectX le plus proche de
*               la résolution souhaitée.
*
* Résultat    : TRUE si initialisation OK, FALSE sinon
*
* Variables globales modifiées : /
*
********************************************************** !0! ****************/
static BOOL SetDirectXDisplayMode( int TailleX, int TailleY ) {
	int i;
	//
	// Récupérer la liste des modes plein écran disponnibles et tri par
	// ordre croissant (réso X, réso Y puis profondeur)
	//
	NbModes = 0;
	lpDD->EnumDisplayModes( 0, NULL, NULL, FctListeModes );
	qsort( TabModes, NbModes, sizeof( StMode ), CompareModes );

	//
	// Rechercher le mode le plus approprié
	//
	for ( i = 0; i < NbModes; i++ ) 
		if (  TabModes[ i ].TailleX >= TailleX && TabModes[ i ].TailleY >= TailleY && TabModes[ i ].Depth == 32 )
			break;

	sprintf( Tmp, "Mode écran choisi : %dx%d %d bits", TabModes[ i ].TailleX, TabModes[ i ].TailleY, TabModes[ i ].Depth );
	Log( MODULENAME, Tmp, LOG_DEBUG );

	return( lpDD->SetDisplayMode( TabModes[ i ].TailleX, TabModes[ i ].TailleY, TabModes[ i ].Depth ) == DD_OK );
}

/********************************************************* !NAME! **************
* Nom : InitDirectX
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Initialisation écran DirectX
*
* Résultat    : TRUE si initialisation OK, FALSE sinon
*
* Variables globales modifiées : lpDD, lpDDSPrimary, NbModes
*
********************************************************** !0! ****************/
static BOOL InitDirectX( HWND hwnd, int TailleX, int TailleY ) {
	DDSURFACEDESC ddsd;

	if ( lpDD->SetCooperativeLevel( hwnd, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN) == DD_OK ) {
		ddsd.dwSize = sizeof( ddsd );
		ddsd.dwFlags = DDSD_CAPS;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
		ddsd.dwBackBufferCount = 1;
		if ( lpDD->CreateSurface( &ddsd, &lpDDSPrimary, NULL ) == DD_OK ) {
			if ( SetDirectXDisplayMode( TailleX, TailleY ) )
				return( lpDDSPrimary->GetDC( &DeviceContext ) == DD_OK );
		}
	}
	return( FALSE );
}

/********************************************************* !NAME! **************
* Nom : ReleaseDirectX
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Fermeture écran DirectX
*
* Résultat    : /
*
* Variables globales modifiées : lpDD, lpDDSPrimary
*
********************************************************** !0! ****************/
static void ReleaseDirectX( void ) {
	if( lpDDSPrimary != NULL ) {
		lpDDSPrimary->ReleaseDC( DeviceContext );
		lpDDSPrimary->Release();
		lpDDSPrimary = NULL;
	}
}

/********************************************************* !NAME! **************
* Nom : SetMenuItems
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Active/désactive les options du menu en fonction de la 
*               configuration choisie
*
* Résultat    : /
*
* Variables globales modifiées : menuItem
*
********************************************************** !0! ****************/
static void SetMenuItems( void ) {
	menuItem.fState = MFS_UNCHECKED;
	SetMenuItemInfo( menu, ID_WINSIZE, FALSE, &menuItem );
	menuItem.fState = Monochrome ? MFS_CHECKED : MFS_UNCHECKED;
	SetMenuItemInfo( menu, ID_COLOR_MONO, FALSE, &menuItem );
	menuItem.fState = DoubleLines ? MFS_CHECKED : MFS_UNCHECKED;
	SetMenuItemInfo( menu, ID_DOUBLE_LINES, FALSE, &menuItem );
	menuItem.fState = PalEmul ? MFS_CHECKED : MFS_UNCHECKED;
	SetMenuItemInfo( menu, ID_PAL_EMUL, FALSE, &menuItem );
	menuItem.fState = SndEnabled? MFS_CHECKED : MFS_UNCHECKED;
	SetMenuItemInfo( menu, ID_SND_ACTIF, FALSE, &menuItem );

#ifndef USE_PHENIX
	EnableMenuItem( menu, ID_PHENIXINFORMATIQUE, MF_GRAYED );
#endif
	EnableMenuItem( menu, ID_DOUBLE_LINES, MF_ENABLED );
	EnableMenuItem( menu, ID_PAL_EMUL, MF_ENABLED );
}

/********************************************************* !NAME! **************
* Nom : SetScreenSize
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Positionne la taille de l'écran
*
* Résultat    : TRUE
*
* Variables globales modifiées : 
*
********************************************************** !0! ****************/
static void SetScreenSize( void ) {
	if ( BorderLess ) {
		TailleWinX = TailleScrX;
		TailleWinY = TailleScrY;
	}
	else {
		int minY = GetSystemMetrics( SM_CYMIN ) + ShowMenu * GetSystemMetrics( SM_CYMENU );
		TailleScrX = DISPLAY_X;
		TailleScrY = DISPLAY_Y;
		TailleWinX = 774;
		TailleWinY = minY + 570;
	}
	memset( MemBitmap, 0, DISPLAY_Y * DISPLAY_X * 4 );
	SetWindowPos( hWnd, HWND_TOP, CW_USEDEFAULT, CW_USEDEFAULT, TailleWinX, TailleWinY, SWP_NOMOVE );
	SetMenuItems();
}

/********************************************************* !NAME! **************
* Nom : ChangeCurDir
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Change le répertoire courant
*
* Résultat    : /
*
* Variables globales modifiées : /
*
********************************************************** !0! ****************/
static void ChangeCurDir( char * FullName ) {
	static char Path[ 1024 ];

	strcpy( Path, FullName );
	char * p = &Path[ strlen( Path ) - 1 ];
	while( p > Path && * p != '\\' )
		p--;

	* p = 0;
	SetCurrentDirectory( Path );
}

/********************************************************* !NAME! **************
* Nom : OpenNewFile
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Ouverture d'une boite de dialogue de sélection de nom de fichier
*
* Résultat    : Le nom du fichier choisi
*
* Variables globales modifiées : /
*
********************************************************** !0! ****************/
char * OpenNewFile( char * Filtre, char * Ext, char * Dir, char * OldName, BOOL Save )
{
	static char NomFic[ LEN_PATH ];

	* NomFic = 0;
	CFileDialog d( ! Save, Ext, OldName && * OldName ? OldName : NULL, OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT, Filtre );
	SoundEnable( FALSE );
	if ( d.DoModal() == IDOK ) {
		strcpy( NomFic, d.GetPathName() );
		strcpy( Dir, GetPath( NomFic ) );
		ChangeCurDir( Dir );
		if ( OldName )
			strcpy( OldName, GetName( NomFic ) );
	}
	SoundEnable( SndEnabled );
	return( NomFic );
}

/********************************************************* !NAME! **************
* Nom : LoadDSK
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Initialise un nouveau DSK dans un lecteur
*
* Résultat    : /
*
* Variables globales modifiées : /
*
********************************************************** !0! ****************/
void LoadDSK( char * Nom, int Drive ) {
	NewDiscOn = FALSE;
	Log( "Plateform", "Chargement disquette : %s", Nom, LOG_INFO );
	SetDSK( hWnd, Nom, Drive );
	AfficheInfos();
}

/********************************************************* !NAME! **************
* Nom : SetNewDsk
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Demande le chargement d'un fichier DSK dans un lecteur
*
* Résultat    : /
*
* Variables globales modifiées : /
*
********************************************************** !0! ****************/
static void SetNewDsk( int Drive ) {
	char NomZ[MAX_PATH];

	char * Nom = OpenNewFile("DSK files (*.DSK)|*.DSK|ZIP files (*.ZIP)|*.ZIP||", "*.DSK,*.ZIP", DirDsk, NomDsk[ Drive ], FALSE );
	if ( * Nom ) {
		strcpy( NomZ, Nom );
		//_strupr( NomZ, MAX_PATH );

		char * pch = strrchr( NomZ , '.' );
		if ( pch != NULL ) {
			if ( ! strcmp( &NomZ[ pch-NomZ ], ".ZIP" ) ) {
				// c'est un fichier Zip
				OpenZipFile( NomZ, Drive, Z_TYPE_DSK );
			}
		}
		else
			LoadDSK( Nom, Drive );
	}
}

/********************************************************* !NAME! **************
* Nom : DragAndDrop
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Gestion du drag & drop de fichiers
*
* Résultat    : /
*
* Variables globales modifiées : /
*
********************************************************** !0! ****************/
static void DragAndDrop( HDROP dropHandle ) {
	static char NomFic[ 512 ];

	for ( unsigned int i = 0; i < DragQueryFile( dropHandle, 0xFFFFFFFF, NULL, 0 ); i++ ) {
		DragQueryFile( dropHandle, i, NomFic, sizeof( NomFic ) );

		// On teste si ce qui est D&D n'est pas un répertoire...
		// dans ce cas la on le prend comme source
		// sinon on teste le type de fichier...
		if ( GetFileAttributes( NomFic ) == FILE_ATTRIBUTE_DIRECTORY ) {
			strcpy( DirEmuDisc, NomFic );
			NewDiscOn = 1;
			AfficheInfos();
		}
		else {
			int LastPt = -1;
			for ( unsigned int j = 0; j < strlen( NomFic ); j++ ) {
				NomFic[ j ] = ( char )toupper( NomFic[ j ] );
				if ( NomFic[ j ] == '.' )
					LastPt = j;
			}
			if ( LastPt > -1 ) {
				if ( ! strcmp( &NomFic[ LastPt ], ".DSK" ) ) {
					strcpy( NomDsk[ 0 ], GetName( NomFic ) );
					strcpy( DirDsk, GetPath( NomFic ) );
					LoadDSK( NomFic, 0 );
				}
				if ( ! strcmp( &NomFic[ LastPt ], ".SNA" ) ) {
					strcpy( NomSnapRead, GetName( NomFic ) );
					strcpy( DirSnapRead, GetPath( NomFic ) );
					LireSnap( NomFic );
				}
				if ( ! strcmp( &NomFic[ LastPt ], ".WAV" ) )
					OpenTapRead( NomFic );

				if ( ! strcmp( &NomFic[ LastPt ], ".ASM" ) ) {
					AsmOutput p;
					p.Assemble( NomFic );
				}
				if ( ! strcmp( &NomFic[ LastPt ], ".ZIP" ) ) {
					OpenZipFile( NomFic, 0, Z_TYPE_ALL );
				}
			}
		}
	}
	DragFinish( dropHandle );
	SetActiveWindow( hWnd );
}

static void FicReadA( void ) {
	SetNewDsk( 0 );
}

static void FicEjectA( void ) {
	EjectDSK( hWnd, 0 );
	AfficheInfos();
}

static void FicEmptyA( void ) {
	// ####
}

static void FicReadB( void ) {
	SetNewDsk( 1 );
}

static void FicEjectB( void ) {
	EjectDSK( hWnd, 1 );
	AfficheInfos();
}

static void FicEmptyB( void ) {
	// ####
}

static void TapeRead( void ) {
	char NomZ[MAX_PATH];

	char * Nom = OpenNewFile( "WAV files (*.WAV)|*.WAV||", "*.WAV", DirTape, NomTape, FALSE );
	if ( * Nom ) {
		strcpy( NomZ, Nom );
		//_strupr_s( NomZ, MAX_PATH );

		char * pch = strrchr( NomZ , '.' );
		if ( pch != NULL ) {
			if ( ! strcmp( &NomZ[ pch-NomZ ], ".ZIP" ) ) {
				// c'est un fichier Zip
				OpenZipFile( NomZ, -1, Z_TYPE_WAV );
			}
		}
		else
			OpenTapRead( Nom );
	}
}

static void TapeEject( void ){
	// ####
}

static void TapeWrite( void ){
	char * Nom = OpenNewFile( "WAV files (*.WAV)|*.WAV||", "*.WAV", DirTape, NomTape, TRUE );
	if ( * Nom )
		OpenTapWrite( Nom );
}

static void TapeBrowse( void ){
	SoundEnable( FALSE );
	int Pos = GetPosTapeIn();
	SoundEnable( SndEnabled );
	if ( Pos != -1 ) {
		Pos = SetPosFileTape( Pos, hWnd );
		if ( Pos > -1 )
			SetPosTapeIn( Pos );
	}
}

static void SnaRead( void ){
	char NomZ[MAX_PATH];

	char * Nom = OpenNewFile( "Snapshot files (*.SNA)|*.SNA||", "*.SNA", DirSnapRead, NomSnapRead, FALSE );
	if ( * Nom ) {
		strcpy( NomZ, Nom );
		//_strupr_s( NomZ, MAX_PATH );

		char * pch = strrchr( NomZ , '.' );
		if ( pch != NULL ) 	{
			if ( ! strcmp( &NomZ[ pch-NomZ ], ".ZIP" ) ) {
				// c'est un fichier Zip
				OpenZipFile( NomZ, -1, Z_TYPE_SNA );
			}
		}
		else
			LireSnap( Nom );
	}
}

static void SnaReRead( void ) {
	if ( * NomSnapRead ) {
		strcpy( Tmp, DirSnapRead );
		strcat( Tmp, NomSnapRead );
		LireSnap( Tmp );
	}
}

static void SnaWrite( void ) {
	char * Nom = OpenNewFile( "Snapshot files (*.SNA)|*.SNA||", "*.SNA", DirSnapWrite, NomSnapWrite, TRUE );
	if ( * Nom )
		SauveSnap( Nom );
}

static void SnaReWrite( void ) {
	if ( * NomSnapWrite ) {
		strcpy( Tmp, DirSnapWrite );
		strcat( Tmp, NomSnapWrite );
		SauveSnap( Tmp );
	}
}

static void ReadWritePC( void ) {
	FlushCloseIn();
	FlushCloseOut();
	NewDiscOn = ChoixRepertoire( DirEmuDisc );
	AfficheInfos();
}

static void Assemble( void )
{
	static char DirAsm[ LEN_PATH ] = { 0 };
	char NomZ[MAX_PATH];

	char * Nom = OpenNewFile( "Assembler files (*.ASM)|*.ASM||", "*.ASM", DirAsm, NomAsm, FALSE );
	if ( * Nom ) {
		strcpy( NomZ, Nom );
		//_strupr_s( NomZ, MAX_PATH );

		char * pch = strrchr( NomZ , '.' );
		if ( pch != NULL ) {
			if ( ! strcmp( &NomZ[ pch-NomZ ], ".ZIP" ) ) {
				// c'est un fichier Zip
				OpenZipFile( NomZ, -1, Z_TYPE_ASM );
			}
		}
		else {
			AsmOutput p;
			p.Assemble( Nom );
		}
	}
}

static void OptPause( void ) {
	Pause ^= 1;
	menuItem.fState = Pause ? MFS_CHECKED : MFS_UNCHECKED;
	SetMenuItemInfo( menu, ID_PAUSE_CPC, FALSE, &menuItem );
}

static void OptReset( void ) {
	ResetCPC( FALSE );
}

static void EmulPhaser( void ) {
	// ####
}

static void EmulTrojan( void ) {
	// ####
}

static void EmulMagnum( void ) {
	// ####
}

static void BDDPhenix( void ) {
#ifdef USE_PHENIX
	SoundEnable( FALSE );
	InitPhenixAccess();
	SoundEnable( SndEnabled );
#endif
}

static void Configure( int FirstTab ) {
	SoundEnable( FALSE );
	CEditConfig x( FirstTab );
	x.DoModal();
	if ( x.IsValid() ) {
		SetMenuItems();
		SaveConfig( NULL );
		if ( x.ResetScr() ) {
			EndScreen();
			InitScreen( hInstance, SW_SHOW );
		}
		if ( x.ResetSnd() ) {
			EndSound();
			InitSound();
		}
		if ( x.ResetMem() )
			ResetCPC( TRUE );

		if ( ! DoubleLines )
			SetScreenSize();
	}
	else
		ReadConfig( NULL, FALSE );

	SoundEnable( SndEnabled );
	finMain = 0;
}

static void SetMultiface( void ) {
	if ( ! MultifaceEnable && Multiface ) {
		Log( MODULENAME, "Appui BP Multiface", LOG_INFO );
		MultifaceEnable = 1;
		Z80_NMI();
		WriteVGA( 0x89 );
	}
}

static void SetRomPack( void ) {
	RomPack ^= 1;
	Log( MODULENAME, "Utilisation Rompack = %s", RomPack ? "ON" : "OFF", LOG_INFO );
	AfficheInfos();
	menuItem.fState = RomPack ? MFS_CHECKED : MFS_UNCHECKED;
	SetMenuItemInfo( menu, ID_ROMPACK, FALSE, &menuItem );
}

static void ActionReplay( void ) {
	DWORD ThreadID;
	CreateThread( 0, 0, ( LPTHREAD_START_ROUTINE )StartActionReplay, 0, 0, &ThreadID );
}

static void Pokes( void ) {
	// ####
}

static void AutoType( void ) {
	static char DirTxt[ LEN_PATH ] = { 0 };
	static char Text[ 1024 * 1024 ];
	static char Line[ 1024 ];

	char * Nom = OpenNewFile( "Fichiers textes (*.TXT)|*.TXT||", "*.TXT", DirTxt, NULL, FALSE );
	if ( * Nom ) {
		FILE * fp = fopen( Nom, "rt" );
		* Text = 0;
		while( fgets( Line, sizeof( Line ), fp ) ) {
			if ( Line[ strlen( Line ) - 1 ] == '\n' )
				Line[ strlen( Line ) - 1 ] = '\r';

			strcat( Text, Line );
		}
		fclose( fp );
		SetEmulCmd( ( UBYTE * )Text );
	}
}

static void Debugger( void ) {
	ShowDebug( hWnd );
}

static void SetTurbo( void ) {
	Turbo ^= 1;
	Log( MODULENAME, "Mode turbo = %s", Turbo ? "ON" : "OFF", LOG_INFO );
	AfficheInfos();
	menuItem.fState = Turbo ? MFS_CHECKED : MFS_UNCHECKED;
	SetMenuItemInfo( menu, ID_TURBO, FALSE, &menuItem );
}

static void SetMonoColor( void ) {
	Monochrome ^= 1;
	menuItem.fState = Monochrome ? MFS_CHECKED : MFS_UNCHECKED;
	SetMenuItemInfo( menu, ID_COLOR_MONO, FALSE, &menuItem );
	SaveConfig( NULL );
}

static void SetDblLines( void ) {
	DoubleLines ^= 1;
	menuItem.fState = DoubleLines ? MFS_CHECKED : MFS_UNCHECKED;
	SetMenuItemInfo( menu, ID_DOUBLE_LINES, FALSE, &menuItem );
	SaveConfig( NULL );
	if ( ! DoubleLines )
		SetScreenSize();
}

static void SetPalEmul( void ) {
	PalEmul ^= 1;
	menuItem.fState = PalEmul ? MFS_CHECKED : MFS_UNCHECKED;
	SetMenuItemInfo( menu, ID_PAL_EMUL, FALSE, &menuItem );
	SaveConfig( NULL );
	if ( ! DoubleLines )
		SetScreenSize();
}

static void SaveBMP( void ) {
	SauveBitmap();
}

static void SaveAVI( void ) {
	if ( WriteAviStarted() )
		WriteAviEnd( hWnd );
	else {
		char * Nom = OpenNewFile( "AVI files (*.AVI)|*.AVI||", "*.AVI", DirAviFile, NomAviFile, TRUE );
		if ( * Nom )
			WriteAviInit( hWnd, Nom, DISPLAY_X, DISPLAY_Y );
	}
}

static void SetSndActif( void ) {
	SndEnabled ^= 1;
	SoundEnable( SndEnabled );
	menuItem.fState = SndEnabled ? MFS_CHECKED : MFS_UNCHECKED;
	SetMenuItemInfo( menu, ID_SND_ACTIF, FALSE, &menuItem );
	SaveConfig( NULL );
}

static void SetSSA1Actif( void ) {
	// ####
}

static void SetDkTronicActif( void ) {
	// ####
}

static void SetSndDsk( void ) {
	// ####
}

static void SetSndTape( void ) {
	// ####
}

static void SaveWave( void ) {
	// ####
}

static void SaveYM( void ) {
	if ( WriteYMStarted() )
		WriteYMEnd( hWnd );
	else {
		char * Nom = OpenNewFile( "YM files (*.YM)|*.YM||", "*.YM", DirYMFile, NomYMFile, TRUE );
		if ( * Nom )
			WriteYMInit( hWnd, Nom, 0 );
	}
}

/********************************************************* !NAME! **************
* Nom : TouchesFonctions
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Gestion des touches fonctions pendant l'émulation
*
* Résultat    : /
*
* Variables globales modifiées : DebugMode, Turbo, MultifaceEnable
*
********************************************************** !0! ****************/
static int TouchesFonctions( int Shift, WPARAM wParam ) {
	switch( wParam ) {
		/* 
		Touche "Menu contextuel"
		*/
	case 0x5D :
		ShowMenu ^= 1;
		SetMenu( hWnd, ShowMenu ? menu : NULL );
		SetScreenSize();
		break;

	case 0x70 :
		if ( Shift )
			BDDPhenix();        // SHIFT+F1
		else {
			DWORD ThreadID;
			CreateThread( 0, 0, ( LPTHREAD_START_ROUTINE )Aide, 0, 0, &ThreadID );
		}
		break;

	case 0x71 :
		if ( Shift )
			ReadWritePC();          // SHIFT+F2
		else
			SetTurbo();             // F2
		break;

	case 0x72 :
		if ( ! Shift )
			SetMultiface();         // F3
		break;

	case 0x73 :
		if ( Shift )
			SetMonoColor();         // SHIFT+F4
		else
			SnaRead();              // F4
		break;

	case 0x74 :
		if ( Shift )
			SaveYM();               // SHIFT+F5
		else
			SnaWrite();             // F5
		break;

	case 0x75 :
		if ( Shift ) {
			ShowWindow( hWnd, SW_MINIMIZE );
			return( 1 );
		}
		else
			Debugger();             // F6
		break;

	case 0x76 :
		SetNewDsk( Shift );         // F7 ou SHIFT+F7
		break;

	case 0x77 :
		if ( ! Shift )
			OptReset();             // F8
		else
			SetRomPack();           // SHIFT+F8
		break;

	case 0x78 :
		if ( Shift )
			TapeBrowse();           // SHIFT+F9
		else
			TapeRead();             // F9
		break;

	case 0x79 :
		finMain = 1;
		break;

	case 0x7A :
		if ( Shift )
			SaveAVI();              // SHIFT+F11
		else
			TapeWrite();            // F11
		break;

	case 0x7B :
		if ( Shift )
			Assemble();             // SHIFT + F12
		else
			SaveBMP();              // F12
		return( 1 );
		break;
	}
	return( 0 );
}

/********************************************************* !NAME! **************
* Nom : WndProc
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Gestion des messages de Windows
*
* Résultat    : /
*
* Variables globales modifiées : finMain, clav
*
********************************************************** !0! ****************/
LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	static int Shift = 0;

	switch( message ) {
	case WM_PAINT :
		AfficheInfos();
		break;

	case WM_ACTIVATEAPP :
		if ( BorderLess ) {
			if ( wParam )
				InitDirectX( hWnd, TailleWinX, TailleWinY );
			else
				ReleaseDirectX();
		}
		break;

	case WM_DESTROY :
	case WM_CLOSE :
		finMain = 1;
		break;

	case WM_COMMAND :
		switch( LOWORD( wParam ) ) {
		case ID_DSK_READ_A :
			FicReadA();
			break;

		case ID_DSK_EJECT_A :
			FicEjectA();
			break;

		case ID_DSK_EMPTY_A :
			FicEmptyA();
			break;

		case ID_DSK_READ_B :
			FicReadB();
			break;

		case ID_DSK_EJECT_B :
			FicEjectB();
			break;

		case ID_DSK_EMPTY_B :
			FicEmptyB();
			break;

		case ID_TAPE_LOAD :
			TapeRead();
			break;

		case ID_TAPE_EJECT :
			TapeEject();
			break;

		case ID_TAPE_SAVE :
			TapeWrite();
			break;

		case ID_TAPE_BROWSE :
			TapeBrowse();
			break;

		case ID_SNA_READ :
			SnaRead();
			break;

		case ID_SNA_REREAD :
			SnaReRead();
			break;

		case ID_SNA_SAVE :
			SnaWrite();
			break;

		case ID_SNA_RESAVE :
			SnaReWrite();
			break;

		case ID_NEW_DISC :
			ReadWritePC();
			break;

		case ID_ASSEMBLE :
			Assemble();
			break;

		case IDM_EXIT :
			finMain = 1;
			break;

		case ID_PAUSE_CPC :
			OptPause();
			break;

		case ID_RESET :
			OptReset();
			break;

		case ID_CONF_KBD :
			Configure( CONF_TAB_KBD );
			break;

		case ID_EMULE_PHASER :
			EmulPhaser();
			break;

		case ID_EMULE_TROJAN :
			EmulTrojan();
			break;

		case ID_EMULE_LIGHTGUN :
			EmulMagnum();
			break;

		case ID_PHENIXINFORMATIQUE :
			BDDPhenix();
			break;

		case ID_CONFIG :
			Configure( CONF_TAB_MEM );
			break;

		case ID_MULTIFACE :
			SetMultiface();
			break;

		case ID_ROMPACK :
			SetRomPack();
			break;

		case ID_ACTIONREPLAY :
			ActionReplay();
			break;

		case ID_POKES :
			Pokes();
			break;

		case ID_AUTOTYPE :
			AutoType();
			break;

		case ID_DEBUGGER :
			Debugger();
			break;

		case ID_TURBO :
			SetTurbo();
			break;

		case ID_COLOR_MONO :
			SetMonoColor();
			break;

		case ID_DOUBLE_LINES :
			SetDblLines();
			break;

		case ID_PAL_EMUL :
			SetPalEmul();
			break;

		case ID_BMP_SAVE :
			SaveBMP();
			break;

		case ID_AVI_SAVE :
			SaveAVI();
			break;

		case ID_CONF_VIDEO :
			Configure( CONF_TAB_SCR );
			break;

		case ID_SND_ACTIF :
			SetSndActif();
			break;

		case ID_SSA1_ACTIF :
			SetSSA1Actif();
			break;

		case ID_DKTRONIC_ACTIF :
			SetDkTronicActif();
			break;

		case ID_SND_DSK :
			SetSndDsk();
			break;

		case ID_SND_TAPE :
			SetSndTape();
			break;

		case ID_WAV_SAVE :
			SaveWave();
			break;

		case ID_YM_SAVE :
			SaveYM();
			break;

		case ID_CONF_SND :
			Configure( CONF_TAB_SND );
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;

	case WM_KEYUP :
	case WM_SYSKEYUP :
		if ( ClavFree ) {
			if ( wParam == 0x10 )
				Shift = 0;

			if ( wParam == 0x0D && ( lParam & 0x01000000 ) )
				wParam = 0x8D;

			clav[ TrKey[ wParam ].Ligne ] |= ~TrKey[ wParam ].Port;
			if ( TouchesFonctions( Shift, wParam ) ) {
				clav[ TrKey[ 0x10 ].Ligne ] |= ~TrKey[ 0x10 ].Port;
				Shift = 0;
			}
		}
		break;

	case WM_KEYDOWN :
	case WM_SYSKEYDOWN :
		if ( ClavFree ) {
			if ( wParam == 0x10 )
				Shift = 1;

			if ( wParam == 0x0D && ( lParam & 0x01000000 ) )
				wParam = 0x8D;

			clav[ TrKey[ wParam ].Ligne ] &= TrKey[ wParam ].Port;
		}
		break;

	case WM_DROPFILES :
		DragAndDrop( ( HDROP )wParam );
		break;
	}

	return( DefWindowProc( hWnd, message, wParam, lParam ) );
}

/********************************************************* !NAME! **************
* Nom : InitPlateforme
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Initialisation diverses
*
* Résultat    : /
*
* Variables globales modifiées : JoyKey, clav, TrKey
*
********************************************************** !0! ****************/
void InitPlateforme( void ) {
	CalcPoints();
	memset( clav, 0xFF, sizeof( clav ) );
	/*

	TrKey[ 186 ].Port = 0xF7;   // ']'
	TrKey[ 220 ].Port = 0xFB;   // '@'
	TrKey[ 220 ].Ligne = 0x03;
	TrKey[ 221 ].Port = 0xFD;   // '['
	TrKey[ 221 ].Ligne = 0x02;
	TrKey[ 222 ].Port = TrKey[ 226 ].Port = 0xBF;   // '\'

	*/
}

/********************************************************* !NAME! **************
* Nom : Erreur
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Affichage d'un message d'erreur
*
* Résultat    : /
*
* Variables globales modifiées : /
*
********************************************************** !0! ****************/
void Erreur( char * Msg ) {
	MessageBox( hWnd, Msg, "Emulateur CPC - Erreur", MB_ICONWARNING );
}

/********************************************************* !NAME! **************
* Nom : Info
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Affichage d'un message d'information
*
* Résultat    : /
*
* Variables globales modifiées : /
*
********************************************************** !0! ****************/
void Info( char * Msg ) {
	MessageBox( hWnd, Msg, "Emulateur CPC - Information", MB_ICONINFORMATION );
}

/********************************************************* !NAME! **************
* Nom : UpdateScreen
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Affiche l'écran du CPC
*
* Résultat    : /
*
* Variables globales modifiées : /
*
********************************************************** !0! ****************/
void UpdateScreen( void ) {
	int xScr = DISPLAY_X;
	int yScr = DISPLAY_Y;

	if ( xScr == TailleScrX && yScr == TailleScrY )
		BitBlt( DeviceContext, 0, 0, xScr, yScr, DcBitmap, 0, 0, SRCCOPY) ;
	else
		StretchBlt( DeviceContext, 0, 0, TailleScrX, TailleScrY, DcBitmap, 0, 0, xScr, yScr, SRCCOPY );
}

/********************************************************* !NAME! **************
* Nom : CreateBitmapCPC
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Création du bitmap contenant l'image écran du CPC
*
* Résultat    : TRUE si ok, FALSE sinon
*
* Variables globales modifiées : MemBitmap
*
********************************************************** !0! ****************/
static BOOL CreateBitmapCPC( HDC dc ) {
	pbmi.bmiHeader.biSize = sizeof( BITMAPINFOHEADER );
	pbmi.bmiHeader.biWidth = DISPLAY_X;
	pbmi.bmiHeader.biHeight = DISPLAY_Y;
	pbmi.bmiHeader.biPlanes = 1;
	pbmi.bmiHeader.biBitCount = 32;
	pbmi.bmiHeader.biCompression = BI_RGB;
	pbmi.bmiHeader.biSizeImage = 0;
	pbmi.bmiHeader.biXPelsPerMeter = 0;
	pbmi.bmiHeader.biYPelsPerMeter = 0;
	pbmi.bmiHeader.biClrUsed = 0;
	pbmi.bmiHeader.biClrImportant = 0;
	bm = CreateDIBSection( dc, &pbmi, DIB_RGB_COLORS, ( void ** )&MemBitmap, NULL, 0 );
	if ( bm ) {
		DcBitmap = CreateCompatibleDC( dc );
		if ( DcBitmap ) {
			SelectObject( DcBitmap, bm );
			return( TRUE );
		}
	}
	return( FALSE );
}

/********************************************************* !NAME! **************
* Nom : ReleaseBitmap
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Libération des ressources allouées par la création du du bitmap 
*               contenant l'image écran du CPC
*
* Résultat    : /
*
* Variables globales modifiées : DcBitmap, bm
*
********************************************************** !0! ****************/
static void ReleaseBitmap( void ) {
	if ( bm ) {
		DeleteObject( bm );
		bm = NULL;
	}
	if ( DcBitmap ) {
		DeleteDC( DcBitmap );
		DcBitmap = NULL;
	}
}

/********************************************************* !NAME! **************
* Nom : Register
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Enregistrement de la classe de la fenêtrre
*
* Résultat    : TRUE si ok, FALSE sinon
*
* Variables globales modifiées : hInstance
*
********************************************************** !0! ****************/
BOOL Register( HINSTANCE i, WNDCLASSEX * wcex ) {
	hInstance = i;
	memset( wcex, 0, sizeof( WNDCLASSEX ) );
	wcex->cbSize = sizeof( WNDCLASSEX ); 
	wcex->style = CS_HREDRAW | CS_VREDRAW;
	wcex->lpfnWndProc = ( WNDPROC )WndProc;
	wcex->hInstance = hInstance;
	wcex->hIcon = LoadIcon( hInstance, MAKEINTRESOURCE( IDI_PCCPC ) );
	wcex->hCursor = LoadCursor( NULL, IDC_ARROW );
	wcex->hbrBackground = CreateSolidBrush( BK_COLOR );
	wcex->lpszMenuName = MAKEINTRESOURCE( IDC_PCCPC );
	wcex->lpszClassName = TITRE;
	return( RegisterClassEx( wcex ) );
}

/********************************************************* !NAME! **************
* Nom : Unregister
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Désenregistrement de la classe de la fenêtrre
*
* Résultat    : /
*
* Variables globales modifiées : /
*
********************************************************** !0! ****************/
void Unregister( HINSTANCE hInstance, WNDCLASSEX * wcex ) {
	if ( wcex != NULL )
		UnregisterClass( wcex->lpszClassName, hInstance );
}

/********************************************************* !NAME! **************
* Nom : InitScreen
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Initialisation fenêtre principale
*
* Résultat    : TRUE si ok, FALSE sinon
*
* Variables globales modifiées : hWnd, DeviceContext
*
********************************************************** !0! ****************/
BOOL InitScreen( HINSTANCE hInstance, int nCmdShow ) {
	hWnd = CreateWindowEx( WS_EX_ACCEPTFILES, TITRE, TITRE, BorderLess ? WS_POPUP : WS_SYSMENU | WS_MINIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL );
	if ( hWnd ) {
		if ( DirectDrawCreate( NULL, &lpDD, NULL ) == DD_OK ) {
			DeviceContext = GetDC( hWnd );
			if ( CreateBitmapCPC( DeviceContext ) ) {
				f = CreateFont( 14, 0, 0, 0, FW_BOLD, 0, 0, 0, OEM_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH | FF_DONTCARE,NULL );
				if ( f ) {
					SetScreenSize();
					SetTextColor( DeviceContext, TEXT_COLOR );
					SetBkColor( DeviceContext, BK_COLOR );
					SelectObject( DeviceContext, f );
					ShowWindow( hWnd, nCmdShow );
					menu = GetMenu( hWnd );
					SetMenu( hWnd, ShowMenu ? menu : NULL );
					memset( &menuItem, 0, sizeof( menuItem ) );
					menuItem.cbSize = sizeof( menuItem );
					menuItem.fMask = MIIM_STATE;
					SetMenuItems();
					return( TRUE );
				}
			}
		}
	}
	return( FALSE );
}

/********************************************************* !NAME! **************
* Nom : GetTpsRefresh
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Retourne la période de rafraichissement verticale
*
* Résultat    : la période de rafraichissement verticale en us
*
* Variables globales modifiées : /
*
********************************************************** !0! ****************/
int GetTpsRefresh( void ) {
	int f = GetDeviceCaps( DeviceContext, VREFRESH );
	return( f > 1 ? 1000000 / f : 0 );
}

/********************************************************* !NAME! **************
* Nom : EndScreen
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Fermeture de la fenêtre et libération des ressources allouées
*
* Résultat    : /
*
* Variables globales modifiées : /
*
********************************************************** !0! ****************/
void EndScreen( void ) {
	ReleaseBitmap();
	if ( DeviceContext )
		ReleaseDC( hWnd, DeviceContext );

	if ( f )
		DeleteObject( f );

	ReleaseDirectX();
	if( lpDD != NULL ) {
		lpDD->Release();
		lpDD = NULL;
	}
	DestroyWindow( hWnd );
}

/********************************************************* !NAME! **************
* Nom : TestJoy
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Lecture joystick et affectation si mouvement
*
* Résultat    : /
*
* Variables globales modifiées : clav
*
********************************************************** !0! ****************/
void TestJoy( void ) {
	static char OldDeltaXJoy1 = 0, OldDeltaYJoy1 = 0;
	static char OldDeltaXJoy2 = 0, OldDeltaYJoy2 = 0;
	static unsigned int OldButtonsJoy1 = 0;
	static unsigned int OldButtonsJoy2 = 0;

	JOYINFOEX joyInfoEx;

	joyInfoEx.dwSize = sizeof( joyInfoEx );
	joyInfoEx.dwFlags = JOY_RETURNALL;

	// Test Joystick 1
	if ( joyGetPosEx( JOYSTICKID1, &joyInfoEx ) == JOYERR_NOERROR ) {
		char DeltaX = ( char )( ( joyInfoEx.dwXpos - 0x7800 ) >> 13 );
		char DeltaY = ( char )( ( joyInfoEx.dwYpos - 0x7800 ) >> 13 );

		stToucheClav * Key = StTabRedirectJoy;
		if ( OldDeltaYJoy1 != DeltaY ) {
			// Up
			if ( DeltaY < 0 )
				clav[ Key->Ligne ] &= Key->Port;
			else
				clav[ Key->Ligne ] |= ~Key->Port;

			Key++;
			// Down
			if ( DeltaY > 0 )
				clav[ Key->Ligne ] &= Key->Port;
			else
				clav[ Key->Ligne ] |= ~Key->Port;
		}
		else
			Key++;

		Key++;
		if ( OldDeltaXJoy1 != DeltaX ) {
			// Left
			if ( DeltaX < 0 )
				clav[ Key->Ligne ] &= Key->Port;
			else
				clav[ Key->Ligne ] |= ~Key->Port;

			Key++;
			// Right
			if ( DeltaX > 0 )
				clav[ Key->Ligne ] &= Key->Port;
			else
				clav[ Key->Ligne ] |= ~Key->Port;
		}
		else
			Key++;

		if ( OldButtonsJoy1 != joyInfoEx.dwButtons ) {
			// Boutons de 1 à 10
			for ( int b = 0; b < 10; b++ ) {
				Key++;
				if ( joyInfoEx.dwButtons & ( 1 << b ) )
					clav[ Key->Ligne ] &= Key->Port;
				else
					clav[ Key->Ligne ] |= ~Key->Port;
			}
		}
		OldDeltaXJoy1 = DeltaX;
		OldDeltaYJoy1 = DeltaY;
		OldButtonsJoy1 = joyInfoEx.dwButtons; 
	}

	// Test Joystick 2
	if ( joyGetPosEx( JOYSTICKID2, &joyInfoEx ) == JOYERR_NOERROR ) {
		char DeltaX = ( char )( ( joyInfoEx.dwXpos - 0x7800 ) >> 13 );
		char DeltaY = ( char )( ( joyInfoEx.dwYpos - 0x7800 ) >> 13 );

		if ( OldDeltaYJoy2 != DeltaY ) {
			// Up
			if ( DeltaY < 0 )
				clav[ 6 ] &= 0xFE;
			else
				clav[ 6 ] |= 0x01;

			// Down
			if ( DeltaY > 0 )
				clav[ 6 ] &= 0xFD;
			else
				clav[ 6 ] |= 0x02;
		}

		if ( OldDeltaXJoy2 != DeltaX ) {
			// Left
			if ( DeltaX < 0 )
				clav[ 6 ] &= 0xFB;
			else
				clav[ 6 ] |= 0x04;

			// Right
			if ( DeltaX > 0 )
				clav[ 6 ] &= 0xF7;
			else
				clav[ 6 ] |= 0x08;
		}

		if ( OldButtonsJoy2 != joyInfoEx.dwButtons ) {
			if ( joyInfoEx.dwButtons & 1 )
				clav[ 6 ] &= 0xEF;
			else
				clav[ 6 ] |= 0x10;

			if ( joyInfoEx.dwButtons & 2 )
				clav[ 6 ] &= 0xDF;
			else
				clav[ 6 ] |= 0x20;
		}
		OldDeltaXJoy2 = DeltaX;
		OldDeltaYJoy2 = DeltaY;
		OldButtonsJoy2 = joyInfoEx.dwButtons; 
	}
}

/********************************************************* !NAME! **************
* Nom : OpenZipFile
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Lecture d'un fichier ZIP
*
* Résultat    : /
*
* Variables globales modifiées : /
*
********************************************************** !0! ****************/
int OpenZipFile( char * Nom, int Drive, int ForceType ) {
	ZipOpenFile ZipResult;
	ZipResult.Name[0] = '\0';

	if ( OpenZip( Nom, ForceType, &ZipResult) == 0 ) {
		// et on charge vraiment le fichier....
		if ( ZipResult.Type == Z_TYPE_DSK ) {
			strcpy( NomDsk[ Drive ], GetName( ZipResult.Name ) );
			strcpy( DirDsk, GetPath( ZipResult.Name ) );
			LoadDSK( ZipResult.Name, Drive );
		}
		else if ( ZipResult.Type == Z_TYPE_SNA ) {
			strcpy( NomSnapRead, GetName( ZipResult.Name ) );
			strcpy( DirSnapRead, GetPath( ZipResult.Name ) );
			LireSnap( ZipResult.Name );
		}
		else if ( ZipResult.Type == Z_TYPE_WAV ) {
			OpenTapRead( ZipResult.Name );
		}
		else if ( ZipResult.Type == Z_TYPE_ASM ) {
			AsmOutput p;
			p.Assemble( ZipResult.Name );
		}
		else {
			Log(MODULENAME, "Tentative d'ouverture d'un fichier de type inconnue depuis un ZIP ! ", LOG_WARNING);
			return -1;
		}
		MemZipIsOpen(&ZipResult, Drive);
	}
	return 0;
}
