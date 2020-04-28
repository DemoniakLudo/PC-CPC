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
*          SYSTEME         |      SOUS SYSTEME       |      SOUS ENSEMBLE
* ------------------------------------------------------------------------------
*  EMULATEUR CPC           | PC-CPC                  | Accessoires
* ------------------------------------------------------------------------------
*  Fichier     : CONFIG.C              | Version : 0.1aq
* ------------------------------------------------------------------------------
*  Date        : 05/11/2002            | Auteur  : L.DEPLANQUE
* ------------------------------------------------------------------------------
*  Description : Décodage du fichier de configuration
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
*  06/05/2003              | L.DEPLANQUE             | Version 0.1w :
*                          |                         | Initialisation des
*                          |                         | valeurs par défaut pour
*                          |                         | les variables contenant
*                          |                         | les scan-codes des
*                          |                         | touches émulant le
*                          |                         | joystick
* ------------------------------------------------------------------------------
*  22/01/2004              | L.DEPLANQUE             | Version 0.1x : Gestion
*                          |                         | nouvelle variable :
*                          |                         | localisation répertoire
*                          |                         | emulation disque
* ------------------------------------------------------------------------------
*  03/02/2004              | L.DEPLANQUE             | Version 0.1y : 
*                          |                         | Suppression des
*                          |                         | fonctions de lecture
*                          |                         | des variables de type
*                          |                         | paramètres, et passage
*                          |                         | de ces variables en
*                          |                         | variables globales.
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
*                          |                         | utilisation de la rom pack
*                          |                         | avec la commande
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
*                          |                         | Supperssion Flag
*                          |                         | #NEW_DISC
* ------------------------------------------------------------------------------
*  14/10/2008              | L.DEPLANQUE             | Version 0.1ao :
*                          |                         | Ajout Flags
*                          |                         | #RGB_COLORXX
*                          |                         | #MONO_COLORXX
*                          |                         | #SCANCODEXXX
* ------------------------------------------------------------------------------
*  18/04/2010              | L.DEPLANQUE             | Version 0.1aq :
*                          |                         | Séparation variables
*                          |                         | RgbCPCColor et
*                          |                         | RgbCPCMono
* ------------------------------------------------------------------------------
*  02/08/2011              | L.DEPLANQUE             | Version 0.1at :
*                          |                         | Suppression du FrameSkip
* ------------------------------------------------------------------------------
********************************************************** !END! **************/


#include  <stdlib.h>
#include  <string.h>
#include  <ctype.h>
#include  <stdio.h>

#include  "Types.h"
#include  "PlateForm.h"
#include  "Config.h"
#include  "Log.h"


#define     MODULENAME      "Config"

#define     TAB_ALIGN       "\t\t\t\t\t\t"


enum FctJoy { JOY_UP = 0
            , JOY_DOWN
            , JOY_LEFT
            , JOY_RIGHT
            , JOY_FIRE1
            , JOY_FIRE2
            , JOY_FIRE3
            , JOY_FIRE4
            , JOY_FIRE5
            , JOY_FIRE6
            , JOY_FIRE7
            , JOY_FIRE8
            , JOY_FIRE9
            , JOY_FIREA
            };


/********************************************************* !NAME! **************
* Nom : DoubleLines
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Indique s'il faut doubler les lignes en mode grand écran ou non
*
********************************************************** !0! ****************/
int DoubleLines = 0;


/********************************************************* !NAME! **************
* Nom : PalEmul
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Indique s'il faut activer l'émulation "PAL"
*
********************************************************** !0! ****************/
int PalEmul = 0;


/********************************************************* !NAME! **************
* Nom : BorderLess
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Indique si la fenêtre doit être sans bordure ou avec bordure
*
********************************************************** !0! ****************/
int BorderLess = 0;


/********************************************************* !NAME! **************
* Nom : Monochrome
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Mode Monochrome activé/désactivé
*
********************************************************** !0! ****************/
int Monochrome = 0;


/********************************************************* !NAME! **************
* Nom : TailleScrX
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Indique la taille de l'écran en X
*
********************************************************** !0! ****************/
int TailleScrX = 768;


/********************************************************* !NAME! **************
* Nom : TailleScrY
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Indique la taille de l'écran en Y
*
********************************************************** !0! ****************/
int TailleScrY = 544;


/********************************************************* !NAME! **************
* Nom : Multiface
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Multiface active ou non
*
********************************************************** !0! ****************/
int Multiface = 0;


/********************************************************* !NAME! **************
* Nom : ComputerName
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Nom de l'ordinateur (ponts LK1 à LK3)
*
********************************************************** !0! ****************/
int ComputerName = 7;


/********************************************************* !NAME! **************
* Nom : LogLevel
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Niveau de logging
*
********************************************************** !0! ****************/
int LogLevel = 3;


/********************************************************* !NAME! **************
* Nom : LocRomInf
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Localisation du fichier contenant la rom inférieure du CPC
*
********************************************************** !0! ****************/
char LocRomInf[ LEN_PATH ];


/********************************************************* !NAME! **************
* Nom : LocRomExt
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Localisation des fichier contenant les roms d'extensions
*
********************************************************** !0! ****************/
char LocRomExt[ MAX_ROM_EXT ][ LEN_PATH ];


/********************************************************* !NAME! **************
* Nom : LocRomMulti
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Localisation du fichier contenant la rom de l'interface
*               Multiface II
*
********************************************************** !0! ****************/
char LocRomMulti[ LEN_PATH ];


/********************************************************* !NAME! **************
* Nom : DirSnapRead
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Localisation du répertoire contenant les fichiers snapshots
*
********************************************************** !0! ****************/
char DirSnapRead[ LEN_PATH ];


/********************************************************* !NAME! **************
* Nom : DirSnapWrite
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Localisation du répertoire contenant les fichiers snapshots
*
********************************************************** !0! ****************/
char DirSnapWrite[ LEN_PATH ];


/********************************************************* !NAME! **************
* Nom : DirTape
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Localisation du répertoire contenant les fichiers cassettes
*
********************************************************** !0! ****************/
char DirTape[ LEN_PATH ];


/********************************************************* !NAME! **************
* Nom : DirDisc
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Localisation du répertoire contenant les fichiers disquettes
*
********************************************************** !0! ****************/
char DirDsk[ LEN_PATH ];


/********************************************************* !NAME! **************
* Nom : DirEmuDisc
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Localisation du répertoire servant à l'émulation disque
*
********************************************************** !0! ****************/
char DirEmuDisc[ LEN_PATH ];


/********************************************************* !NAME! **************
* Nom : DirAviFile
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Localisation du répertoire servant à sauver les fichiers AVI
*
********************************************************** !0! ****************/
char DirAviFile[ LEN_PATH ];


/********************************************************* !NAME! **************
* Nom : DirYMFile
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Localisation du répertoire servant à sauver les fichiers YM
*
********************************************************** !0! ****************/
char DirYMFile[ LEN_PATH ];


/********************************************************* !NAME! **************
* Nom : DirBMPFile
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Localisation du répertoire servant à sauver les fichiers BMP
*
********************************************************** !0! ****************/
char DirBMPFile[ LEN_PATH ];


/********************************************************* !NAME! **************
* Nom : NomDsk
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Mémorise le nom de la disquette en cours 
*
********************************************************** !0! ****************/
char NomDsk[ 2 ][ LEN_PATH ];


/********************************************************* !NAME! **************
* Nom : NomSnapRead
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Mémorise le dernier nom du snapshot lu
*
********************************************************** !0! ****************/
char NomSnapRead[ LEN_PATH ];


/********************************************************* !NAME! **************
* Nom : NomSnapWrite
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Mémorise le dernier nom du snapshot écris
*
********************************************************** !0! ****************/
char NomSnapWrite[ LEN_PATH ];


/********************************************************* !NAME! **************
* Nom : NomYMFile
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Mémorise le nom du fichier YM en cours
*
********************************************************** !0! ****************/
char NomYMFile[ LEN_PATH ];


/********************************************************* !NAME! **************
* Nom : NomAviFile
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Mémorise le nom du fichier AVI en cours
*
********************************************************** !0! ****************/
char NomAviFile[ LEN_PATH ];


/********************************************************* !NAME! **************
* Nom : NomTape
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Mémorise le nom du fichier WAVE en cours
*
********************************************************** !0! ****************/
char NomTape[ LEN_PATH ];


/********************************************************* !NAME! **************
* Nom : NomAsm
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Mémorise le nom du fichier ASM en cours
*
********************************************************** !0! ****************/
char NomAsm[ LEN_PATH ];


/********************************************************* !NAME! **************
* Nom : NewDiscOn
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Activation/désactivation du module émulation disque
*
********************************************************** !0! ****************/
int NewDiscOn = 0;


/********************************************************* !NAME! **************
* Nom : UseSoundCas
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Activation/désactivation des sons sur cassette
*
********************************************************** !0! ****************/
int UseSoundCas = 0;


/********************************************************* !NAME! **************
* Nom : UsePrinter
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Activation/désactivation de l'émulation de l'imprimante
*
********************************************************** !0! ****************/
int UsePrinter = 0;


/********************************************************* !NAME! **************
* Nom : FreqWavOut
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Fréquence d'échantillonage des fichiers .WAV en sortie
*
********************************************************** !0! ****************/
int FreqWavOut = 11025;


/********************************************************* !NAME! **************
* Nom : Use512Ko
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Utilisation de 512Ko de mémoire supplémentaire
*
********************************************************** !0! ****************/
int Use512Ko = 0;


/********************************************************* !NAME! **************
* Nom : Use128Ko
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Utilisation de 128Ko de mémoire totale
*
********************************************************** !0! ****************/
int Use128Ko = 0;


/********************************************************* !NAME! **************
* Nom : UseCpcPlus
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Emulation du CPC+
*
********************************************************** !0! ****************/
int UseCpcPlus = 0;


/********************************************************* !NAME! **************
* Nom : ShowMenu
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Indique si le menu est visible ou non
*
********************************************************** !0! ****************/
int ShowMenu = 0;


/********************************************************* !NAME! **************
* Nom : SndFreqReplay
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Fréquence de "replay" des sons
*
********************************************************** !0! ****************/
int SndFreqReplay = 44100;


/********************************************************* !NAME! **************
* Nom : SndEnabled
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Indique si les l'émulation des sons est activé
*
********************************************************** !0! ****************/
int SndEnabled = 1;


/********************************************************* !NAME! **************
* Nom : StTabRedirectJoy
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Tableau de redirection des fonctions du joystick.
*               (Valeurs par défaut)
*
********************************************************** !0! ****************/
stToucheClav StTabRedirectJoy[ 16 ] = 
    {
        { 0xFE, 0x09 },           // UP
        { 0xFD, 0x09 },           // DOWN
        { 0xFB, 0x09 },           // LEFT
        { 0xF7, 0x09 },           // RIGHT
        { 0xEF, 0x09 },           // FIRE 1
        { 0xDF, 0x09 },           // FIRE 2
        { 0xBF, 0x09 },           // FIRE 3
        { 0xFF, 0x0F },           // FIRE 4
        { 0xFF, 0x0F },           // FIRE 5
        { 0xFF, 0x0F },           // FIRE 6
        { 0xFF, 0x0F },           // FIRE 7
        { 0xFF, 0x0F },           // FIRE 8
        { 0xFF, 0x0F },           // FIRE 9
        { 0xFF, 0x0F },           // FIRE 10
    };


/********************************************************* !NAME! **************
* Nom : RgbCPCColor
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Tableau <R>,<V>,<B> des couleurs CPC sur écran couleur 
*
********************************************************** !0! ****************/
unsigned int RgbCPCColor[ 256 ] =
    {
    //RRVVBB
    0x6E7D6B,                   // Blanc            (13) -> #40
    0x6E7B6D,                   // Blanc            (13) -> #41
    0x00F36B,                   // Vert Marin       (19) -> #42
    0xF3F36D,                   // Jaune Pastel     (25) -> #43
    0x00026B,                   // Bleu              (1) -> #44
    0xF00268,                   // Pourpre           (7) -> #45
    0x007868,                   // Turquoise        (10) -> #46
    0xF37D6B,                   // Rose             (16) -> #47
    0xF30268,                   // Pourpre           (7) -> #48
    0xF3F36B,                   // Jaune pastel     (25) -> #49
    0xF3F30D,                   // Jaune vif        (24) -> #4A
    0xFFF3F9,                   // Blanc Brillant   (26) -> #4B
    0xF30506,                   // Rouge vif         (6) -> #4C
    0xF302F4,                   // Magenta vif       (8) -> #4D
    0xF37D0D,                   // Orange           (15) -> #4E
    0xFA80F9,                   // Magenta pastel   (17) -> #4F
    0x000268,                   // Bleu              (1) -> #50
    0x02F36B,                   // Vert Marin       (19) -> #51
    0x02F001,                   // Vert vif         (18) -> #52
    0x0FF3F2,                   // Turquoise vif    (20) -> #53
    0x000201,                   // Noir              (0) -> #54
    0x0C02F4,                   // Bleu vif          (2) -> #55
    0x027801,                   // Vert              (9) -> #56
    0x0C7BF4,                   // Bleu ciel        (11) -> #57
    0x690268,                   // Magenta           (4) -> #58
    0x71F36B,                   // Vert pastel      (22) -> #59
    0x71F504,                   // Vert citron      (21) -> #5A
    0x71F3F4,                   // Turquoise pastel (23) -> #5B
    0x6C0201,                   // Rouge             (3) -> #5C
    0x6C02F2,                   // Mauve             (5) -> #5D
    0x6E7B01,                   // Jaune            (12) -> #5E
    0x6E7BF6                    // Bleu pastel      (14) -> #5F
    };


/********************************************************* !NAME! **************
* Nom : RgbCPCMono
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Tableau <R>,<V>,<B> des couleurs CPC sur écran monochrome
*
********************************************************** !0! ****************/
unsigned int RgbCPCMono[ 256 ] =
    {
    //RRVVBB
    0x459836,                   // Blanc            (13) -> #40
    0x459836,                   // Blanc            (13) -> #41
    0x5DB84B,                   // Vert Marin       (19) -> #42
    0x6FD05B,                   // Jaune Pastel     (25) -> #43
    0x1C6110,                   // Bleu              (1) -> #44
    0x307B22,                   // Pourpre           (7) -> #45
    0x3C8C2D,                   // Turquoise        (10) -> #46
    0x50A740,                   // Rose             (16) -> #47
    0x307B22,                   // Pourpre           (7) -> #48
    0x6FD05B,                   // Jaune pastel     (25) -> #49
    0x6DCE5A,                   // Jaune vif        (24) -> #4A
    0x6ECF5B,                   // Blanc Brillant   (26) -> #4B
    0x2D7820,                   // Rouge vif         (6) -> #4C
    0x338025,                   // Magenta vif       (8) -> #4D
    0x4EA43D,                   // Orange           (15) -> #4E
    0x54AC43,                   // Magenta pastel   (17) -> #4F
    0x1C6110,                   // Bleu              (1) -> #50
    0x5DB84B,                   // Vert Marin       (19) -> #51
    0x5AB348,                   // Vert vif         (18) -> #52
    0x60BC4E,                   // Turquoise vif    (20) -> #53
    0x1A5E0E,                   // Noir              (0) -> #54
    0x206714,                   // Bleu vif          (2) -> #55
    0x39872A,                   // Vert              (9) -> #56
    0x409130,                   // Bleu ciel        (11) -> #57
    0x256D18,                   // Magenta           (4) -> #58
    0x66C453,                   // Vert pastel      (22) -> #59
    0x62BF50,                   // Vert citron      (21) -> #5A
    0x69C856,                   // Turquoise pastel (23) -> #5B
    0x226915,                   // Rouge             (3) -> #5C
    0x28711B,                   // Mauve             (5) -> #5D
    0x429433,                   // Jaune            (12) -> #5E
    0x499D38                    // Bleu pastel      (14) -> #5F
    };


/********************************************************* !NAME! **************
* Nom : SetFullDir
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Affecte un nom complet de fichier à une variable
*
* Résultat    : /
*
* Variables globales modifiées : /
*
********************************************************** !0! ****************/
static void SetFullDir( char * Dir, char * Dest )
{
    sprintf( Dir, "%s\\%s", ExecutableDir, Dest );
    HANDLE h = CreateFile( Dir
                         , GENERIC_READ
                         , 0
                         , NULL
                         , OPEN_EXISTING
                         , 0
                         , NULL
                         );
    if ( h == INVALID_HANDLE_VALUE )
        strcpy( Dir, Dest );

    CloseHandle( h );
}


/********************************************************* !NAME! **************
* Nom : AffecteToucheJoy
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Affecte une touche clavier à une fonction du joystick
*
* Résultat    : /
*
* Variables globales modifiées : StTabRedirectJoy
*
********************************************************** !0! ****************/
static void AffecteToucheJoy( int NumFctJoy, char * ToucheJoy )
{
    static char * TabDefKey[ 80 ] =
        {
        "KEY_UP",
        "KEY_RIGHT",
        "KEY_DOWN",
        "KEY_F9",
        "KEY_F6",
        "KEY_F3",
        "KEY_ENTER",
        "KEY_UNDEF",
        "KEY_LEFT",
        "KEY_COPY",
        "KEY_F7",
        "KEY_F8",
        "KEY_F5",
        "KEY_F1",
        "KEY_F2",
        "KEY_F0",
        "KEY_CLR",
        "KEY_[",
        "KEY_RETURN",
        "KEY_]",
        "KEY_F4",
        "KEY_SHIFT",
        "KEY_\\",
        "KEY_CTRL",
        "KEY_£",
        "KEY_=",
        "KEY_@",
        "KEY_P",
        "KEY_+",
        "KEY_*",
        "KEY_?",
        "KEY_>",
        "KEY_0",
        "KEY_9",
        "KEY_O",
        "KEY_I",
        "KEY_L",
        "KEY_K",
        "KEY_M",
        "KEY_<",
        "KEY_8",
        "KEY_7",
        "KEY_U",
        "KEY_Y",
        "KEY_H",
        "KEY_J",
        "KEY_N",
        "KEY_SPC",
        "KEY_6",
        "KEY_5",
        "KEY_R",
        "KEY_T",
        "KEY_G",
        "KEY_F",
        "KEY_B",
        "KEY_V",
        "KEY_4",
        "KEY_3",
        "KEY_E",
        "KEY_Z",
        "KEY_S",
        "KEY_D",
        "KEY_C",
        "KEY_X",
        "KEY_2",
        "KEY_1",
        "KEY_ESC",
        "KEY_Q",
        "KEY_TAB",
        "KEY_A",
        "KEY_CAPS",
        "KEY_W",
        "JOY_UP",
        "JOY_DOWN",
        "JOY_LEFT",
        "JOY_RIGHT",
        "JOY_FIRE1",
        "JOY_FIRE2",
        "JOY_FIRE3",
        "KEY_DEL",
        };

    BOOL Trouve = FALSE;
    // Recherche la touche clavier correspondante
    for ( int i = 0; i < 80; i++ )
        if ( ! strcmp( TabDefKey[ i ], ToucheJoy ) )
            {
            StTabRedirectJoy[ NumFctJoy ].Port = ( UBYTE )( ~ ( 1 << ( i & 7 ) ) );
            StTabRedirectJoy[ NumFctJoy ].Ligne = ( UBYTE )( i >> 3 );
            Trouve = TRUE;
            break;
            }

    if ( ! Trouve )
        Log( MODULENAME, "Touche %s inconnue", ToucheJoy, LOG_WARNING );
}


/********************************************************* !NAME! **************
* Nom : GetVal
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Retourne une valeur numérique contenue dans une chaine
*
* Résultat    : La valeur contenue dans la chaine
*
* Variables globales modifiées : /
*
********************************************************** !0! ****************/
static int GetVal( char * Chaine )
{
    int val = 0;

    while ( * Chaine == ' ' || * Chaine == '\t' )
        Chaine++;

    if ( Chaine[ 0 ] == 'O' && Chaine[ 1 ] == 'N' ) 
        val = 1;
    else
        if ( Chaine[ 0 ] == '$' )
            {
            // Conversion hexa->décimal
            while( isxdigit( *++Chaine ) )
                {
                val <<= 4;
                int c = toupper( * Chaine );
                if ( c > '9' )
                    val += ( c - 55 );
                else
                    val += ( c - '0' );
                } 
            }
        else
            val = atoi( Chaine );

    return( val );
}


/********************************************************* !NAME! **************
* Nom : GetChaine
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Retourne une valeur alphanumérique contenue dans une chaine
*
* Résultat    : La valeur contenue dans la chaine
*
* Variables globales modifiées : /
*
********************************************************** !0! ****************/
static char * GetChaine( char * Chaine )
{
    char * p;

    while ( * Chaine == ' ' || * Chaine == '\t' )
        Chaine++;

    p = Chaine;
    while( * p && * p >= ' ' )
        p++;

    * p = 0;
    return( Chaine );
}


/********************************************************* !NAME! **************
* Nom : GetRomExt
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Initialise le chemin d'une rom externe
*
* Résultat    : /
*
* Variables globales modifiées : LocRomExt
*
********************************************************** !0! ****************/
static void GetRomExt( char * Chaine )
{
    int NumRom = atoi( Chaine );
    if ( NumRom < MAX_ROM_EXT && NumRom >= 0 )
        SetFullDir( LocRomExt[ NumRom ], GetChaine( &Chaine[ 2 ] ) );
}


/********************************************************* !NAME! **************
* Nom : GetRGBColor
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Initialise une couleur CPC pour écran couleur en fonction des
*               paramètres R,V,B
*
* Résultat    : /
*
* Variables globales modifiées : RgbCPC32
*
********************************************************** !0! ****************/
static void GetRGBColor( char * Chaine )
{
    int NumCoul = atoi( Chaine );
    if ( NumCoul < 32 && NumCoul >= 0 )
        RgbCPCColor[ NumCoul ] = GetVal( &Chaine[ 2 ] );
}


/********************************************************* !NAME! **************
* Nom : GetMonoColor
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Initialise une couleur CPC pour écran monochrome en fonction des
*               paramètres R,V,B
*
* Résultat    : /
*
* Variables globales modifiées : RgbCPC32
*
********************************************************** !0! ****************/
static void GetMonoColor( char * Chaine )
{
    int NumCoul = atoi( Chaine );
    if ( NumCoul < 32 && NumCoul >= 0 )
        RgbCPCMono[ NumCoul ] = GetVal( &Chaine[ 2 ] );
}


/********************************************************* !NAME! **************
* Nom : GetScanCode
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Affecte un scancode à une touche particulière
*
* Résultat    : /
*
* Variables globales modifiées : TrKey
*
********************************************************** !0! ****************/
static void GetScanCode( char * Chaine )
{
    int ScanCode = atoi( Chaine );
    if ( ScanCode < 256 && ScanCode >= 0 )
        {
        char * p = strchr( Chaine, ',' );
        if ( p )
            {
            TrKey[ ScanCode ].Port = ( UBYTE )GetVal( &Chaine[ 4 ] );
            TrKey[ ScanCode ].Ligne = ( UBYTE )GetVal( &p[ 1 ] );
            }
        }
}


/********************************************************* !NAME! **************
* Nom : GetBool
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Retourne "ON" ou "OFF" en fonction d'une valeur entière
*
* Résultat    : "ON" ou "OFF"
*
* Variables globales modifiées : /
*
********************************************************** !0! ****************/
static char * GetBool( int var )
{
    return( var ? "ON" : "OFF" );
}


/********************************************************* !NAME! **************
* Nom : GetName
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Retourne le nom d'un fichier à partir d'un chemin complet
*
* Résultat    : Le nom du fichier
*
* Variables globales modifiées : /
*
********************************************************** !0! ****************/
char * GetName( char * path )
{
    static char Nom[ LEN_PATH ];

    strcpy( Nom, path );
    char * p = &Nom[ strlen( Nom ) - 1 ];
    while ( p > Nom && * p != '\\' )
        p--;

    if ( * p == '\\' )
        p++;

    return( p );
}


/********************************************************* !NAME! **************
* Nom : GetPath
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Retourne le nom d'un chemin à partir d'un chemin+nom fichier
*
* Résultat    : Le nom du chemin
*
* Variables globales modifiées : /
*
********************************************************** !0! ****************/
char * GetPath( char * path )
{
    static char Nom[ LEN_PATH ];

    strcpy( Nom, path );
    char * p = &Nom[ strlen( Nom ) - 1 ];
    while ( p > Nom && * p != '\\' )
        p--;

    if ( * p == '\\' )
        p++;

    * p = 0;
    return( Nom );
}


/********************************************************* !NAME! **************
* Nom : ReadConfig
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Lecture du fichier config. et affectation des variables
*               concernées
*
* Résultat    : TRUE si lecture fichier config. OK, FALSE sinon
*
* Variables globales modifiées : Multiface, LogLevel, DoubleLines?
*                                LocRomInf, LocRomExt, LocRomMulti,
*                                JoyLeft, JoyRight, JoyUp, JoyDown, JoyFire1,
*                                JoyFire2, DirSnap, DirTape, DirDisc
*
********************************************************** !0! ****************/
BOOL ReadConfig( char * Nom, BOOL InitFiles )
{
    static char Buffer[ LEN_PATH ];

    if ( ! Nom )
        sprintf( Buffer , "%s\\%s", ExecutableDir, CONFIG_NAME );
    else
        strcpy( Buffer, Nom );

    FILE * fp = fopen( Buffer, "rt" );
    if ( fp )
        {
        memset( LocRomExt, 0, sizeof( LocRomExt ) );
        while( fgets( Buffer, sizeof( Buffer ), fp ) )
            {
            if ( Buffer[ 0 ] == '#' )
                {
                if ( ! strncmp( Buffer, "#LOG_LEVEL", 10 ) )
                    LogLevel = GetVal( &Buffer[ 11 ] );

                if ( ! strncmp( Buffer, "#COMPUTERNAME", 13 ) )
                    ComputerName = GetVal( &Buffer[ 14 ] ) & 7;

                if ( ! strncmp( Buffer, "#DOUBLE_LINES", 13 ) )
                    DoubleLines = GetVal( &Buffer[ 14 ] );

                if ( ! strncmp( Buffer, "#PAL_EMUL", 9 ) )
                    PalEmul = GetVal( &Buffer[ 10 ] );

                if ( ! strncmp( Buffer, "#MONOCHROME", 11 ) )
                    Monochrome = GetVal( &Buffer[ 12 ] );

                if ( ! strncmp( Buffer, "#BORDERLESS", 11 ) )
                    BorderLess = GetVal( &Buffer[ 12 ] );

                if ( ! strncmp( Buffer, "#RESO_X", 7 ) )
                    TailleScrX = GetVal( &Buffer[ 8 ] );

                if ( ! strncmp( Buffer, "#RESO_Y", 7 ) )
                    TailleScrY = GetVal( &Buffer[ 9 ] );
                if ( ! strncmp( Buffer, "#ROM_INF", 8 ) )
                    SetFullDir( LocRomInf, GetChaine( &Buffer[ 9 ] ) );

                if ( ! strncmp( Buffer, "#ROM_EXT", 8 ) )
                    GetRomExt( &Buffer[ 8 ] );

                if ( ! strncmp( Buffer, "#MULTIFACE", 10 ) )
                    Multiface = GetVal( &Buffer[ 11 ] );

                if ( ! strncmp( Buffer, "#ROM_MULTI", 10 ) )
                    SetFullDir( LocRomMulti, GetChaine( &Buffer[ 11 ] ) );

                if ( InitFiles )
                    {
                    if ( ! strncmp( Buffer, "#DIR_DISC", 9 ) )
                        strcpy( DirDsk, GetChaine( &Buffer[ 10 ] ) );

                    if ( ! strncmp( Buffer, "#LAST_DSK_A", 11 ) )
                        strcpy( NomDsk[ 0 ], GetChaine( &Buffer[ 12 ] ) );

                    if ( ! strncmp( Buffer, "#LAST_DSK_B", 11 ) )
                        strcpy( NomDsk[ 1 ], GetChaine( &Buffer[ 12 ] ) );

                    if ( ! strncmp( Buffer, "#DIR_SNAP_RD", 12 ) )
                        strcpy( DirSnapRead, GetChaine( &Buffer[ 13 ] ) );

                    if ( ! strncmp( Buffer, "#DIR_SNAP_WR", 12 ) )
                        strcpy( DirSnapWrite, GetChaine( &Buffer[ 13 ] ) );

                    if ( ! strncmp( Buffer, "#LAST_SNA_RD", 12 ) )
                        strcpy( NomSnapRead, GetChaine( &Buffer[ 13 ] ) );

                    if ( ! strncmp( Buffer, "#LAST_SNA_WR", 12 ) )
                        strcpy( NomSnapWrite, GetChaine( &Buffer[ 13 ] ) );

                    if ( ! strncmp( Buffer, "#DIR_TAPE", 9 ) )
                        strcpy( DirTape, GetChaine( &Buffer[ 10 ] ) );

                    if ( ! strncmp( Buffer, "#LAST_WAV", 9 ) )
                        strcpy( NomTape, GetChaine( &Buffer[ 10 ] ) );

                    if ( ! strncmp( Buffer, "#DIR_EMU_DISC", 13 ) )
                        strcpy( DirEmuDisc, GetChaine( &Buffer[ 14 ] ) );

                    if ( ! strncmp( Buffer, "#DIR_AVI_FILE", 13 ) )
                        strcpy( DirAviFile, GetChaine( &Buffer[ 14 ] ) );

                    if ( ! strncmp( Buffer, "#LAST_AVI", 9 ) )
                        strcpy( NomAviFile, GetChaine( &Buffer[ 10 ] ) );

                    if ( ! strncmp( Buffer, "#DIR_YM_FILE", 12 ) )
                        strcpy( DirYMFile, GetChaine( &Buffer[ 13 ] ) );

                    if ( ! strncmp( Buffer, "#LAST_YM", 8 ) )
                        strcpy( NomYMFile, GetChaine( &Buffer[ 9 ] ) );

                    if ( ! strncmp( Buffer, "#DIR_BMP_FILE", 13 ) )
                        strcpy( DirBMPFile, GetChaine( &Buffer[ 14 ] ) );

                    if ( ! strncmp( Buffer, "#LAST_ASM", 9 ) )
                        strcpy( NomAsm, GetChaine( &Buffer[ 10 ] ) );
                    }
                if ( ! strncmp( Buffer, "#USE_PRINTER", 12 ) )
                    UsePrinter = GetVal( &Buffer[ 13 ] );

                if ( ! strncmp( Buffer, "#USE_SOUND_CAS", 14 ) )
                    UseSoundCas = GetVal( &Buffer[ 15 ] );

                if ( ! strncmp( Buffer, "#FREQ_WAV_OUT", 13 ) )
                    FreqWavOut = GetVal( &Buffer[ 14 ] );

                if ( ! strncmp( Buffer, "#USE_128KO", 10 ) )
                    Use128Ko = GetVal( &Buffer[ 11 ] );

                if ( ! strncmp( Buffer, "#USE_512KO", 10 ) )
                    Use512Ko = GetVal( &Buffer[ 11 ] );

                if ( ! strncmp( Buffer, "#USE_CPC_PLUS", 13 ) )
                    UseCpcPlus = GetVal( &Buffer[ 14 ] );

                if ( ! strncmp( Buffer, "#SHOW_MENU", 10 ) )
                    ShowMenu = GetVal( &Buffer[ 11 ] );

                if ( ! strncmp( Buffer, "#SND_ENABLED", 12 ) )
                    SndEnabled = GetVal( &Buffer[ 13 ] );

                if  ( ! strncmp( Buffer, "#FREQ_REPLAY", 13 ) )
                    SndFreqReplay = GetVal( &Buffer[ 14 ] );

                if ( ! strncmp( Buffer, "#RGB_COLOR", 10 ) )
                    GetRGBColor( &Buffer[ 10 ] );

                if ( ! strncmp( Buffer, "#MONO_COLOR", 11 ) )
                    GetMonoColor( &Buffer[ 11 ] );

                if ( ! strncmp( Buffer, "#JOY_UP_KEY", 11 ) )
                    AffecteToucheJoy( JOY_UP, GetChaine( &Buffer[ 12 ] ) );

                if ( ! strncmp( Buffer, "#JOY_DOWN_KEY", 13 ) )
                    AffecteToucheJoy( JOY_DOWN, GetChaine( &Buffer[ 14 ] ) );

                if ( ! strncmp( Buffer, "#JOY_LEFT_KEY", 13 ) )
                    AffecteToucheJoy( JOY_LEFT, GetChaine( &Buffer[ 14 ] ) );

                if ( ! strncmp( Buffer, "#JOY_RIGHT_KEY", 14 ) )
                    AffecteToucheJoy( JOY_RIGHT, GetChaine( &Buffer[ 15 ] ) );

                if ( ! strncmp( Buffer, "#JOY_FIRE1_KEY", 14 ) )
                    AffecteToucheJoy( JOY_FIRE1, GetChaine( &Buffer[ 15 ] ) );

                if ( ! strncmp( Buffer, "#JOY_FIRE2_KEY", 14 ) )
                    AffecteToucheJoy( JOY_FIRE2, GetChaine( &Buffer[ 15 ] ) );

                if ( ! strncmp( Buffer, "#JOY_FIRE3_KEY", 14 ) )
                    AffecteToucheJoy( JOY_FIRE3, GetChaine( &Buffer[ 15 ] ) );

                if ( ! strncmp( Buffer, "#JOY_FIRE4_KEY", 14 ) )
                    AffecteToucheJoy( JOY_FIRE4, GetChaine( &Buffer[ 15 ] ) );

                if ( ! strncmp( Buffer, "#JOY_FIRE5_KEY", 14 ) )
                    AffecteToucheJoy( JOY_FIRE5, GetChaine( &Buffer[ 15 ] ) );

                if ( ! strncmp( Buffer, "#JOY_FIRE6_KEY", 14 ) )
                    AffecteToucheJoy( JOY_FIRE6, GetChaine( &Buffer[ 15 ] ) );

                if ( ! strncmp( Buffer, "#JOY_FIRE7_KEY", 14 ) )
                    AffecteToucheJoy( JOY_FIRE7, GetChaine( &Buffer[ 15 ] ) );

                if ( ! strncmp( Buffer, "#JOY_FIRE8_KEY", 14 ) )
                    AffecteToucheJoy( JOY_FIRE8, GetChaine( &Buffer[ 15 ] ) );

                if ( ! strncmp( Buffer, "#JOY_FIRE9_KEY", 14 ) )
                    AffecteToucheJoy( JOY_FIRE9, GetChaine( &Buffer[ 15 ] ) );

                if ( ! strncmp( Buffer, "#JOY_FIREA_KEY", 14 ) )
                    AffecteToucheJoy( JOY_FIREA, GetChaine( &Buffer[ 15 ] ) );

                if ( ! strncmp( Buffer, "#SCANCODE", 9 ) )
                    GetScanCode( &Buffer[ 9 ] );
                }
            }
        fclose( fp );
        return( TRUE );
        }
    return( FALSE );
}


/********************************************************* !NAME! **************
* Nom : SaveConfig
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Sauvegarde fichier config.
*
* Résultat    : TRUE si sauvegarde OK, FALSE sinon
*
* Variables globales modifiées : /
*
********************************************************** !0! ****************/
BOOL SaveConfig( char * Nom )
{
    static char Buffer[ LEN_PATH ];
    int i;

    if ( ! Nom )
        sprintf( Buffer , "%s\\%s", ExecutableDir, CONFIG_NAME );
    else
        strcpy( Buffer, Nom );

    FILE * fp = fopen( Buffer, "wt" );
    if ( fp )
        {
        fprintf( fp, "'#############################################################################\n" );
        fprintf( fp, "'#                                                                           #\n" );
        fprintf( fp, "'# Fichier de configuration de l'émulateur CPC.                              #\n" );
        fprintf( fp, "'# Les variables à configurer commencent par un '#'.                         #\n" );
        fprintf( fp, "'# Elles doivent être écrites en majuscules.                                 #\n" );
        fprintf( fp, "'# Elles prennent des valeurs entières (0 à 4294967295),                     #\n" );
        fprintf( fp, "'# hexadécimales ($0 à $FFFFFFFF),                                           #\n" );
        fprintf( fp, "'# chaines de caractères ou booléennes (OFF ou ON)                           #\n" );
        fprintf( fp, "'#                                                                           #\n" );
        fprintf( fp, "'# Les commentaires commencent par ' et se terminent en fin de ligne.        #\n" );
        fprintf( fp, "'#                                                                           #\n" );
        fprintf( fp, "'#############################################################################\n\n" );

        fprintf( fp, "#LOG_LEVEL\t\t\t%d\t' Niveau de précision du fichier trace.\n", LogLevel );
        fprintf( fp, "%s' 0 = Aucune trace dans le fichier\n", TAB_ALIGN );
        fprintf( fp, "%s' 1 = warnings\n", TAB_ALIGN );
        fprintf( fp, "%s' 2 = warnings + informations\n", TAB_ALIGN );
        fprintf( fp, "%s' 3 = warnings + informations + infos de debug.\n\n", TAB_ALIGN );

        fprintf( fp, "#DOUBLE_LINES\t\t%s\t' Indique s'il faut doubler les lignes en mode\n", GetBool( DoubleLines ) );
        fprintf( fp, "%s' \"grand écran\" (inactif si #SMALL_SCREEN=ON)\n", TAB_ALIGN );
        fprintf( fp, "#PAL_EMUL\t\t\t%s\n", GetBool( PalEmul ) );
        fprintf( fp, "#MONOCHROME\t\t\t%s\n", GetBool( Monochrome ) );

        fprintf( fp, "#BORDERLESS\t\t\t%s\t' Indique pas de bords ni de titre à la fenêtre\n", GetBool( BorderLess ) );
        fprintf( fp, "#RESO_X\t\t\t\t%3d\t' Résolution en X (prise en compte si borderless=on)\n", TailleScrX );
        fprintf( fp, "#RESO_Y\t\t\t\t%3d\t' Résolution en Y (prise en compte si borderless=on)\n", TailleScrY );

        fprintf( fp, "\n#COMPUTERNAME\t\t%d\t' Valeurs possibles :\n", ComputerName );
        fprintf( fp, "%s' 0 = Isp\n", TAB_ALIGN );
        fprintf( fp, "%s' 1 = Triumph\n", TAB_ALIGN );
        fprintf( fp, "%s' 2 = Saisho\n", TAB_ALIGN );
        fprintf( fp, "%s' 3 = Solavox\n", TAB_ALIGN );
        fprintf( fp, "%s' 4 = Awa\n", TAB_ALIGN );
        fprintf( fp, "%s' 5 = Schneider\n", TAB_ALIGN );
        fprintf( fp, "%s' 6 = Orion\n", TAB_ALIGN );
        fprintf( fp, "%s' 7 = Amstrad\n", TAB_ALIGN );

        fprintf( fp, "\n'#############################################################################\n" );
        fprintf( fp, "'#                                                                           #\n" );
        fprintf( fp, "'# Localisation des ROMs du cpc.                                             #\n" );
        fprintf( fp, "'#                                                                           #\n" );
        fprintf( fp, "'#############################################################################\n\n" );

        fprintf( fp, "#ROM_INF\t\t\t%s\n", LocRomInf );
        for ( i = 0; i < MAX_ROM_EXT; i++ )
            if ( LocRomExt[ i ][ 0 ] )
                fprintf( fp, "#ROM_EXT%02d\t\t\t%s\n", i, LocRomExt[ i ] );
        fprintf( fp, "\n#MULTIFACE\t\t\t%s\t' Si ON, émule la cartouche MULTIFACE II,\n", GetBool( Multiface ) );
        fprintf( fp, "%s' utilisable par l'appui sur la touche F3.\n", TAB_ALIGN );
        fprintf( fp, "%s' Si OFF, pas de multiface. Utile car certains\n", TAB_ALIGN );
        fprintf( fp, "%s' logiciels CPC détectent la multiface et\n", TAB_ALIGN );
        fprintf( fp, "%s' refusent de fonctionner si celle-ci est\n", TAB_ALIGN );
        fprintf( fp, "%s' présente.\n", TAB_ALIGN );
        if ( LocRomMulti[ 0 ] )
            fprintf( fp, "\n#ROM_MULTI\t\t\t%s\n", LocRomMulti );

        fprintf( fp, "\n'#############################################################################\n" );
        fprintf( fp, "'#                                                                           #\n" );
        fprintf( fp, "'# Localisation des répertoires utilisés                                     #\n" );
        fprintf( fp, "'#                                                                           #\n" );
        fprintf( fp, "'#############################################################################\n\n" );

        fprintf( fp, "#DIR_DISC\t\t\t%s\n", DirDsk );
        fprintf( fp, "#LAST_DSK_A\t\t\t%s\n", NomDsk[ 0 ] );
        fprintf( fp, "#LAST_DSK_B\t\t\t%s\n", NomDsk[ 1 ] );
        fprintf( fp, "#DIR_SNAP_RD\t\t%s\n", DirSnapRead );
        fprintf( fp, "#LAST_SNA_RD\t\t%s\n", NomSnapRead );
        fprintf( fp, "#DIR_SNAP_WR\t\t%s\n", DirSnapWrite );
        fprintf( fp, "#LAST_SNA_WR\t\t%s\n", NomSnapWrite );
        fprintf( fp, "#DIR_TAPE\t\t\t%s\n", DirTape );
        fprintf( fp, "#LAST_WAV\t\t\t%s\n", NomTape );

        fprintf( fp, "#DIR_AVI_FILE\t\t%s\n", DirAviFile );
        fprintf( fp, "#LAST_AVI\t\t\t%s\n", NomAviFile );
        fprintf( fp, "#DIR_YM_FILE\t\t%s\n", DirYMFile );
        fprintf( fp, "#LAST_YM\t\t\t%s\n", NomYMFile );
        fprintf( fp, "#DIR_BMP_FILE\t\t%s\n", DirBMPFile );

        fprintf( fp, "#LAST_ASM\t\t\t%s\n", NomAsm );

        fprintf( fp, "#DIR_EMU_DISC\t\t%s\n", DirEmuDisc );
        fprintf( fp, "\n" );

        fprintf( fp, "#USE_PRINTER\t\t%s\n\n", GetBool( UsePrinter ) );

        fprintf( fp, "#USE_SOUND_CAS\t\t%s\n\n", GetBool( UseSoundCas ) );

        fprintf( fp, "#FREQ_WAV_OUT\t\t%d\n\n", FreqWavOut );
        fprintf( fp, "#USE_128KO\t\t\t%s\n\n", GetBool( Use128Ko | Use512Ko ) );

        fprintf( fp, "#USE_512KO\t\t\t%s\n\n", GetBool( Use512Ko ) );

        fprintf( fp, "#USE_CPC_PLUS\t\t%s\n\n", GetBool( UseCpcPlus ) );

        fprintf( fp, "#SHOW_MENU\t\t\t%s\n\n", GetBool( ShowMenu ) );

        fprintf( fp, "#SND_ENABLED\t\t%s\n", GetBool( SndEnabled ) );
        fprintf( fp, "#FREQ_REPLAY\t\t%d\n\n", SndFreqReplay );

        fprintf( fp, "\n'#############################################################################\n" );
        fprintf( fp, "'#                                                                           #\n" );
        fprintf( fp, "'# Définition des couleurs                                                   #\n" );
        fprintf( fp, "'#                                                                           #\n" );
        fprintf( fp, "'#############################################################################\n\n" );

        for ( i = 0; i < 32; i++ )
            fprintf( fp, "#RGB_COLOR%02d\t\t$%06X\n", i, RgbCPCColor[ i ] );

        fprintf( fp, "\n" );
        for ( i = 0; i < 32; i++ )
            fprintf( fp, "#MONO_COLOR%02d\t\t$%06X\n", i, RgbCPCMono[ i ] );

        fprintf( fp, "\n" );

        /*
        if ( ! strncmp( Buffer, "#JOY_UP_KEY", 11 ) )
            AffecteToucheJoy( JOY_UP, GetChaine( &Buffer[ 13 ] ) );

        if ( ! strncmp( Buffer, "#JOY_DOWN_KEY", 13 ) )
            AffecteToucheJoy( JOY_DOWN, GetChaine( &Buffer[ 15 ] ) );

        if ( ! strncmp( Buffer, "#JOY_LEFT_KEY", 13 ) )
            AffecteToucheJoy( JOY_LEFT, GetChaine( &Buffer[ 15 ] ) );

        if ( ! strncmp( Buffer, "#JOY_RIGHT_KEY", 14 ) )
            AffecteToucheJoy( JOY_RIGHT, GetChaine( &Buffer[ 16 ] ) );

        if ( ! strncmp( Buffer, "#JOY_FIRE1_KEY", 14 ) )
            AffecteToucheJoy( JOY_FIRE1, GetChaine( &Buffer[ 16 ] ) );

        if ( ! strncmp( Buffer, "#JOY_FIRE2_KEY", 14 ) )
            AffecteToucheJoy( JOY_FIRE2, GetChaine( &Buffer[ 16 ] ) );

        if ( ! strncmp( Buffer, "#JOY_FIRE3_KEY", 14 ) )
            AffecteToucheJoy( JOY_FIRE3, GetChaine( &Buffer[ 16 ] ) );

        if ( ! strncmp( Buffer, "#JOY_FIRE4_KEY", 14 ) )
            AffecteToucheJoy( JOY_FIRE4, GetChaine( &Buffer[ 16 ] ) );

        if ( ! strncmp( Buffer, "#JOY_FIRE5_KEY", 14 ) )
            AffecteToucheJoy( JOY_FIRE5, GetChaine( &Buffer[ 16 ] ) );

        if ( ! strncmp( Buffer, "#JOY_FIRE6_KEY", 14 ) )
            AffecteToucheJoy( JOY_FIRE6, GetChaine( &Buffer[ 16 ] ) );

        if ( ! strncmp( Buffer, "#JOY_FIRE7_KEY", 14 ) )
            AffecteToucheJoy( JOY_FIRE7, GetChaine( &Buffer[ 16 ] ) );

        if ( ! strncmp( Buffer, "#JOY_FIRE8_KEY", 14 ) )
            AffecteToucheJoy( JOY_FIRE8, GetChaine( &Buffer[ 16 ] ) );

        if ( ! strncmp( Buffer, "#JOY_FIRE9_KEY", 14 ) )
            AffecteToucheJoy( JOY_FIRE9, GetChaine( &Buffer[ 16 ] ) );

        if ( ! strncmp( Buffer, "#JOY_FIREA_KEY", 14 ) )
            AffecteToucheJoy( JOY_FIREA, GetChaine( &Buffer[ 16 ] ) );
        */
 
        /*
        if ( ! strncmp( Buffer, "#SCANCODE", 9 ) )
            GetScanCode( &Buffer[ 9 ] );
        */

        fclose( fp );
        return( TRUE );
        }
    return( FALSE );
}
