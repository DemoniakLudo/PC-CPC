/******************************************************************************/
/* Configuration pour l'archivage des différents éléments du fichier source   */
/******************************************************************************/
// !CONFIG!=/L/* /R/* /W"* Nom : "
// Définition du système       !CONFIG!=/V1!EMULATEUR CPC!
// Définition du sous système  !CONFIG!=/V2!PC-CPC!
// Définition du sous ensemble !CONFIG!=/V3!Noyau!
// Définition du module        !CONFIG!=/V4!Main!
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
*  Fichier     : MAIN.C                | Version : 0.1at
* ------------------------------------------------------------------------------
*  Date        : 05/11/2002            | Auteur  : L.DEPLANQUE
* ------------------------------------------------------------------------------
*  Description : Programme principal
*
* ------------------------------------------------------------------------------
*  Historique  :
*           Date           |         Auteur          |       Description
* ------------------------------------------------------------------------------
*  05/11/2002              | L.DEPLANQUE             | Création
* ------------------------------------------------------------------------------
*  07/11/2002              | L.DEPLANQUE             | Ajout gestion lecture et
*                          |                         | écriture cassette :
*                          |                         | fonctions WriteCas() et
*                          |                         | ReadCas() appelées à
*                          |                         | chaque Hsync.
* ------------------------------------------------------------------------------
*  03/12/2002              | L.DEPLANQUE             | Prise en compte de la
*                          |                         | directive USE_DEBUG
*                          |                         | Optimisation de
*                          |                         | l'affichage et
*                          |                         | l'archivage des messages
*                          |                         | d'erreurs
* ------------------------------------------------------------------------------
*  06/01/2003              | L.DEPLANQUE             | Version 0.1t :
*                          |                         | Suppression d'un Warning
* ------------------------------------------------------------------------------
*  02/02/2004              | L.DEPLANQUE             | Version 0.1x :
*                          |                         | Modification de la
*                          |                         | fréquence d'appel des
*                          |                         | fonctions lecture et
*                          |                         | écritures cassettes,
*                          |                         | ajout d'une constante
*                          |                         | pour cette fréquence.
* ------------------------------------------------------------------------------
*  30/03/2004              | L.DEPLANQUE             | Version 0.1z :
*                          |                         | Utilisation de la
*                          |                         | variable CptInstr en
*                          |                         | locale (suppression de
*                          |                         | la variable globale),
*                          |                         | les fonctions Z80
*                          |                         | retournent le nbre de
*                          |                         | cycles exécutés.
* ------------------------------------------------------------------------------
*  30/09/2004              | L.DEPLANQUE             | Version 0.1ac :
*                          |                         | Prise en compte des
*                          |                         | arguments passés par
*                          |                         | la ligne de commande.
* ------------------------------------------------------------------------------
*  18/03/2005              | L.DEPLANQUE             | Version 0.1af :
*                          |                         | Optimisation temps cpu
*                          |                         | par fonction Sleep(1)
* ------------------------------------------------------------------------------
*  30/11/2005              | L.DEPLANQUE             | Version 0.1ah :
*                          |                         | Gestion sauvegarde des
*                          |                         | trames en fichier .AVI
* ------------------------------------------------------------------------------
*  08/12/2005              | L.DEPLANQUE             | Version 0.1ai :
*                          |                         | Correction d'un bugg
*                          |                         | (régression) lors de la
*                          |                         | fin du programme : plus
*                          |                         | de sauvegarde du fichier
*                          |                         | DSK si modifié.
* ------------------------------------------------------------------------------
*  09/01/2006              | L.DEPLANQUE             | Version 0.1aj :
*                          |                         | Inhibition du mode turbo
*                          |                         | lors de la sauvegarde
*                          |                         | d'un fichier AVI.
* ------------------------------------------------------------------------------
*  20/02/2006              | L.DEPLANQUE             | Version 0.1ak :
*                          |                         | Suppression compteurs
*                          |                         | Timing (utilisés dans
*                          |                         | le module CRTC)
*                          |                         | Correction d'un bugg
*                          |                         | dans l'interpréteur de
*                          |                         | ligne de commandes
* ------------------------------------------------------------------------------
*  20/07/2006              | L.DEPLANQUE             | Version 0.1al :
*                          |                         | Gestion sauvegarde des
*                          |                         | sons en fichier .YM
* ------------------------------------------------------------------------------
*  04/08/2006              | L.DEPLANQUE             | Version 0.1am :
*                          |                         | Gestion envoi commandes
*                          |                         | ou fichier au CPC émulé.
* ------------------------------------------------------------------------------
*  20/02/2007              | L.DEPLANQUE             | Version 0.1an :
*                          |                         | Correction du "frameskip"
*                          |                         | qui passait au moins 1
*                          |                         | image, même quand il
*                          |                         | était réglé à zéro.
*                          |                         | Nouvelle gestion du cycle
*                          |                         | du CRTC.
* ------------------------------------------------------------------------------
*  30/05/2007              | L.DEPLANQUE             | Version 0.1ao :
*                          |                         | Ajout compteur 
*                          |                         | d'initialisations de la
*                          |                         | ROM 7 pour démarrage
*                          |                         | de l'envoi des touches
*                          |                         | si demandé
* ------------------------------------------------------------------------------
*  19/04/2010              | L.DEPLANQUE             | Version 0.1ar :
*                          |                         | Correction compteur
*                          |                         | d'initialisations des
*                          |                         | ROMs
* ------------------------------------------------------------------------------
*  02/08/2011              | L.DEPLANQUE             | Version 0.1at :
*                          |                         | Suppression du FrameSkip
*                          |                         | Réorganisation du code
* ------------------------------------------------------------------------------
********************************************************** !END! **************/

#include  <afxwin.h>
#include  <mmsystem.h>
#include  <ddraw.h>
#include  <stdio.h>

#include  "types.h"
#include  "plateform.h"
#include  "z80.h"
#include  "vga.h"
#include  "ppi.h"
#include  "upd.h"
#include  "GestDsk.h"
#include  "multiface.h"
#include  "AY8912.h"
#include  "Config.h"
#include  "crtc.h"
#include  "Log.h"
#include  "Printer.h"
#include  "desasm.h"
#include  "BreakPoints.h"
#include  "DebugDlg.h"
#include  "snapshot.h"
#include  "WriteAvi.h"
#include  "WriteYM.h"
#include  "Asic.h"

#define     MODULENAME      "Main"

#define     MAX_RUN_CMD     0x4000

/********************************************************* !NAME! **************
* Nom : CptInstr
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\\/FLE!, ligne : !./LN!
*
* Description : Compteur temps de cycle instructions Z80
*
********************************************************** !0! ****************/
int CptInstr;

/********************************************************* !NAME! **************
* Nom : finMain
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\\/FLE!, ligne : !./LN!
*
* Description : Fin du programme
*
********************************************************** !0! ****************/
int finMain;

/********************************************************* !NAME! **************
* Nom : ExecutableDir
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\\/FLE!, ligne : !./LN!
*
* Description : Répertoire d'exécution du programme
*
********************************************************** !0! ****************/
char ExecutableDir[ MAX_PATH ];

/********************************************************* !NAME! **************
* Nom : StConvAscii
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Structures
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Structure conversion codes ASCII->scancodes CPC
*
********************************************************** !0! ****************/
typedef struct {
	UBYTE Ligne;
	UBYTE Port;
	UBYTE Shift;
} StConvAscii;

/********************************************************* !NAME! **************
* Nom : TabAscii
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Tableau de conversion codes ASCII->scancodes CPC
*
********************************************************** !0! ****************/
static StConvAscii TabAscii[ 256 ] = {
	0x0F, 0xFF, 0x00,                   //  00
	0x0F, 0xFF, 0x00,                   //  01
	0x0F, 0xFF, 0x00,                   //  02
	0x0F, 0xFF, 0x00,                   //  03
	0x0F, 0xFF, 0x00,                   //  04
	0x0F, 0xFF, 0x00,                   //  05
	0x0F, 0xFF, 0x00,                   //  06
	0x0F, 0xFF, 0x00,                   //  07
	0x0F, 0xFF, 0x00,                   //  08
	0x08, 0xEF, 0x00,                   //  09  TAB
	0x0F, 0xFF, 0x00,                   //  10
	0x0F, 0xFF, 0x00,                   //  11
	0x0F, 0xFF, 0x00,                   //  12
	0x02, 0xFB, 0x00,                   //  13  RETURN
	0x0F, 0xFF, 0x00,                   //  14
	0x0F, 0xFF, 0x00,                   //  15
	0x0F, 0xFF, 0x00,                   //  16
	0x0F, 0xFF, 0x00,                   //  17
	0x0F, 0xFF, 0x00,                   //  18
	0x0F, 0xFF, 0x00,                   //  19
	0x0F, 0xFF, 0x00,                   //  20
	0x0F, 0xFF, 0x00,                   //  21
	0x0F, 0xFF, 0x00,                   //  22
	0x0F, 0xFF, 0x00,                   //  23
	0x0F, 0xFF, 0x00,                   //  24
	0x0F, 0xFF, 0x00,                   //  25
	0x0F, 0xFF, 0x00,                   //  26
	0x0F, 0xFF, 0x00,                   //  27
	0x0F, 0xFF, 0x00,                   //  28
	0x0F, 0xFF, 0x00,                   //  29
	0x0F, 0xFF, 0x00,                   //  30
	0x0F, 0xFF, 0x00,                   //  31
	0x05, 0x7F, 0x00,                   //  32  ESPACE
	0x05, 0xFE, 0x00,                   //  33  !
	0x07, 0xFD, 0x00,                   //  34  "
	0x02, 0xF7, 0x00,                   //  35  #
	0x02, 0xBF, 0x00,                   //  36  $
	0x03, 0xEF, 0x01,                   //  37  %
	0x08, 0xFE, 0x00,                   //  38  &
	0x07, 0xFE, 0x00,                   //  39  '
	0x06, 0xFD, 0x00,                   //  40  (
	0x03, 0xFD, 0x00,                   //  41  )
	0x02, 0xFD, 0x00,                   //  42  *
	0x03, 0xBF, 0x01,                   //  43  +
	0x04, 0xBF, 0x00,                   //  44  ,
	0x03, 0xFE, 0x00,                   //  45  -
	0x04, 0x7F, 0x01,                   //  46  .
	0x03, 0x7F, 0x01,                   //  47  /
	0x04, 0xFE, 0x01,                   //  48  0
	0x08, 0xFE, 0x01,                   //  49  1
	0x08, 0xFD, 0x01,                   //  50  2
	0x07, 0xFD, 0x01,                   //  51  3
	0x07, 0xFE, 0x01,                   //  52  4
	0x06, 0xFD, 0x01,                   //  53  5
	0x06, 0xFE, 0x01,                   //  54  6
	0x05, 0xFD, 0x01,                   //  55  7
	0x05, 0xFE, 0x01,                   //  56  8
	0x04, 0xFD, 0x01,                   //  57  9
	0x03, 0x7F, 0x00,                   //  58  :
	0x04, 0x7F, 0x00,                   //  59  ;
	0x02, 0xFD, 0x01,                   //  60  <
	0x03, 0xBF, 0x00,                   //  61  =
	0x02, 0xF7, 0x01,                   //  62  >
	0x04, 0xBF, 0x01,                   //  63  ?
	0x04, 0xFE, 0x00,                   //  64  @
	0x08, 0xF7, 0x01,                   //  65  A
	0x06, 0xBF, 0x01,                   //  66  B
	0x07, 0xBF, 0x01,                   //  67  C
	0x07, 0xDF, 0x01,                   //  68  D
	0x07, 0xFB, 0x01,                   //  69  E
	0x06, 0xDF, 0x01,                   //  70  F
	0x06, 0xEF, 0x01,                   //  71  G
	0x05, 0xEF, 0x01,                   //  72  H
	0x04, 0xF7, 0x01,                   //  73  I
	0x05, 0xDF, 0x01,                   //  74  J
	0x04, 0xDF, 0x01,                   //  75  K
	0x04, 0xEF, 0x01,                   //  76  L
	0x03, 0xDF, 0x01,                   //  77  M
	0x05, 0xBF, 0x01,                   //  78  N
	0x04, 0xFB, 0x01,                   //  79  O
	0x03, 0xF7, 0x01,                   //  80  P
	0x08, 0xDF, 0x01,                   //  81  Q
	0x06, 0xFB, 0x01,                   //  82  R
	0x07, 0xEF, 0x01,                   //  83  S
	0x06, 0xF7, 0x01,                   //  84  T
	0x05, 0xFB, 0x01,                   //  85  U
	0x06, 0x7F, 0x01,                   //  86  V
	0x08, 0x7F, 0x01,                   //  87  W
	0x07, 0x7F, 0x01,                   //  88  X
	0x05, 0xF7, 0x01,                   //  89  Y
	0x07, 0xF7, 0x01,                   //  90  Z
	0x03, 0xFD, 0x01,                   //  93  [
	0x04, 0xFD, 0x00,                   //  92  \ 
	0x06, 0xFE, 0x00,                   //  91  ]
	0x03, 0xFB, 0x00,                   //  94  ^
	0x05, 0xFE, 0x00,                   //  95  _
	0x03, 0xFB, 0x00,                   //  96  `
	0x08, 0xF7, 0x00,                   //  97  a
	0x06, 0xBF, 0x00,                   //  98  b
	0x07, 0xBF, 0x00,                   //  99  c
	0x07, 0xDF, 0x00,                   // 100  d
	0x07, 0xFB, 0x00,                   // 101  e
	0x06, 0xDF, 0x00,                   // 102  f
	0x06, 0xEF, 0x00,                   // 103  g
	0x05, 0xEF, 0x00,                   // 104  h
	0x04, 0xF7, 0x00,                   // 105  i
	0x05, 0xDF, 0x00,                   // 106  j
	0x04, 0xDF, 0x00,                   // 107  k
	0x04, 0xEF, 0x00,                   // 108  l
	0x03, 0xDF, 0x00,                   // 109  m
	0x05, 0xBF, 0x00,                   // 110  n
	0x04, 0xFB, 0x00,                   // 111  o
	0x03, 0xF7, 0x00,                   // 112  p
	0x08, 0xDF, 0x00,                   // 113  q
	0x06, 0xFB, 0x00,                   // 114  r
	0x07, 0xEF, 0x00,                   // 115  s
	0x06, 0xF7, 0x00,                   // 116  t
	0x05, 0xFB, 0x00,                   // 117  u
	0x06, 0x7F, 0x00,                   // 118  v
	0x08, 0x7F, 0x00,                   // 119  w
	0x07, 0x7F, 0x00,                   // 120  x
	0x05, 0xF7, 0x00,                   // 121  y
	0x07, 0xF7, 0x00,                   // 122  z
	0x07, 0xFE, 0x00,                   // 123  {
	0x03, 0xEF, 0x00,                   // 124  |
	0x03, 0xFE, 0x00,                   // 125  }
	0x02, 0xBF, 0x00,                   // 126  ~
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,
	0x0F, 0xFF, 0x00,                   // 222
	0x0F, 0xFF, 0x00,                   // 223
	0x04, 0xFE, 0x00,                   // 224  à
	0x0F, 0xFF, 0x00,                   // 225
	0x0F, 0xFF, 0x00,                   // 226
	0x0F, 0xFF, 0x00,                   // 227
	0x0F, 0xFF, 0x00,                   // 228
	0x0F, 0xFF, 0x00,                   // 229
	0x0F, 0xFF, 0x00,                   // 230
	0x04, 0xFD, 0x00,                   // 231  ç
	0x05, 0xFD, 0x00,                   // 232  è
	0x08, 0xFD, 0x00,                   // 233  é
	0x0F, 0xFF, 0x00,                   // 234
	0x0F, 0xFF, 0x00,                   // 235
	0x0F, 0xFF, 0x00,                   // 236
	0x0F, 0xFF, 0x00,                   // 237
	0x0F, 0xFF, 0x00,                   // 238
	0x0F, 0xFF, 0x00,                   // 239
	0x0F, 0xFF, 0x00,                   // 240
	0x0F, 0xFF, 0x00,                   // 241
	0x0F, 0xFF, 0x00,                   // 242
	0x0F, 0xFF, 0x00,                   // 243
	0x0F, 0xFF, 0x00,                   // 244
	0x0F, 0xFF, 0x00,                   // 245
	0x0F, 0xFF, 0x00,                   // 246
	0x0F, 0xFF, 0x00,                   // 247
	0x0F, 0xFF, 0x00,                   // 248
	0x03, 0xEF, 0x00,                   // 249  ù
	0x0F, 0xFF, 0x00,                   // 250
	0x0F, 0xFF, 0x00,                   // 251
	0x0F, 0xFF, 0x00,                   // 252
	0x0F, 0xFF, 0x00,                   // 253
	0x0F, 0xFF, 0x00,                   // 254
	0x0F, 0xFF, 0x00,                   // 255
};

/********************************************************* !NAME! **************
* Nom : AjouteKeyCPC
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\\/FLE!, ligne : !./LN!
*
* Description : Envoie un caractère au CPC en fonction de la commande demandée
*
* Résultat    : La Commande, éventuellement incrémentée
*
* Variables globales modifiées : /
*
********************************************************** !0! ****************/
static UBYTE * AjouteKeyCPC( UBYTE * Commande ) {
	static int CntCmd = 0;
	static UBYTE MemoLigne = 0, MemoPort = 0, MemoShift = 0;

	// Exécuter seulement 1 cycle sur 4
	if ( CntCmd++ > 2 ) {
		CntCmd = 0;
		if ( MemoPort ) {
			clav[ MemoLigne ] |= ~MemoPort;
			if ( MemoShift )
				clav[ 0x02 ] |= ~0xDF;
		}
		if ( * Commande ) {
			if ( ! MemoShift ) {
				MemoPort = TabAscii[ * Commande ].Port;
				MemoLigne = TabAscii[ * Commande ].Ligne;
				MemoShift = TabAscii[ * Commande ].Shift;
			}
			else
				MemoShift = 0;

			if ( MemoShift )
				clav[ 0x02 ] &= 0xDF;
			else {
				clav[ MemoLigne ] &= MemoPort;
				strcpy( ( char * )Commande, ( char * )&Commande[ 1 ] );
			}
		}
	}
	return( Commande );
}

/********************************************************* !NAME! **************
* Nom : SupBlancsFin
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\\/FLE!, ligne : !./LN!
*
* Description : Efface les caractères trouvés à partir du caractère '-' dans
*               une chaîne.
*
* Résultat    : /
*
* Variables globales modifiées : /
*
********************************************************** !0! ****************/
static void SupBlancsFin( char * Msg ) {
	char * p = strchr( Msg, '-' );
	if ( p )
		* p = 0;
}

static int OldRom, InitRomCnt, CptFrame;

/********************************************************* !NAME! **************
* Nom : CycleZ80
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\\/FLE!, ligne : !./LN!
*
* Description : Fonction principale : Cycle d'émulation d'une instruction Z80
*
* Résultat    : Le temps de l'instruction en micro-secondes
*
* Variables globales modifiées : /
*
********************************************************** !0! ****************/
void CycleZ80( UBYTE * Commande, BOOL Debug ) {
	MSG msg;
	static unsigned long TimeOut = 0, Time, OldTime = 0, TimeLine = 0;
	static int FrameAvi = 0;
	static int cpt = 0, Oldcpt = 0, CntTapeIn = 0, CntTapeOut = 0;
	static int cptWave = 0, cpt50Hz = 0;

	int t = ExecInstZ80();
	int CycleLigne = Cpc.CRTCReg[ 0 ] + 1;
	cpt += t;


#ifdef USE_SOUND_HBL
	cptWave += t;
	if ( cptWave >= 64 )
	{
		cptWave -= 64;
		CreateSoundBuffer();
	}
#endif
	cpt50Hz += t;
	if ( cpt50Hz >= 40000 ) {
		cpt50Hz -= 40000;
		if ( WriteAviStarted() )
			// Ecriture une frame sur 2 (pour 25 Fps)
				if ( FrameAvi++ & 1 ) {
					WriteAviFrame();
					FrameAvi = 0;
				}
				if ( WriteYMStarted() )
					WriteYMFrame();

				if ( * Commande && InitRomCnt > 1 ) {
					if ( CptFrame++ > 50 ) {
						ClavFree = FALSE;
						Commande = AjouteKeyCPC( Commande );// Exécution commande
					}
				}
				else
					ClavFree = TRUE;
	}

	if ( TpsTapeIn )
		CntTapeIn += ( cpt - Oldcpt );

	if ( TpsTapeOut )
		CntTapeOut += ( cpt - Oldcpt );

	if ( cpt < Oldcpt ) {
		CntTapeIn += CycleLigne;
		CntTapeOut += CycleLigne;
	}
	Oldcpt = cpt;
	if ( TpsTapeIn && ( CntTapeIn >= TpsTapeIn ) ) {
		ReadCas();
		CntTapeIn -= TpsTapeIn;
	}
	if ( TpsTapeOut && ( CntTapeOut >= TpsTapeOut ) ) {
		WriteCas();
		CntTapeOut -= TpsTapeOut;
	}
	TimeOut += t;
	if ( UseCpcPlus )
		SyncAsic();

	if ( Cpc.NumRom != OldRom ) {
		OldRom = Cpc.NumRom;
		if ( OldRom == 1 )
			InitRomCnt++;           // Combien de fois ROM 1 a été appelée
		else
			if ( InitRomCnt )
				InitRomCnt++;
	}
	CycleUPD( t );
	if ( CycleCRTC( t ) || TimeOut > 100000 ) {
		if ( ( ! Turbo && ! Debug ) || WriteAviStarted() ) {
			do {
				Time = timeGetTime() * 1000;
				Sleep( Time - OldTime < TimeOut - 1000 ? 1 : 0 ); // Synchro de l'image en fct de TimeOut
			}
			while( Time - OldTime < TimeOut );
			OldTime = Time;
		}
		if ( ! Turbo || TimeOut > 5000000 ) {
			UpdateScreen();             // Rafraichissement de l'écran
			TimeOut = 0;
			TestJoy();
			if ( PeekMessage( &msg, NULL, 0,0, PM_REMOVE ) )
				DispatchMessage( &msg );
		}
	}
	CptInstr += t;
}

/********************************************************* !NAME! **************
* Nom : AideLigneCmd
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\\/FLE!, ligne : !./LN!
*
* Description : Affiche l'aide de la ligne de commande
*
* Résultat    : /
*
* Variables globales modifiées :
*
********************************************************** !0! ****************/
static void AideLigneCmd( void ) {
	MessageBox( NULL
		, "Options utilisables :\n\n"
		"-? : affiche cet aide\n"
		"-disk NomFichier.DSK : charge le fichier .DSK\n"
		"-snapshot NomFichier.SNA : charge le fichier  SNA et le démarre\n"
		"-run NomFichier : démarre sur le dsk le nom du fichier désiré\n"
		"-cmd commandes : envoie les commandes désirées au basic\n"
		"-file fichier : envoie les commandes contenue dans le fichier au basic\n"
		"-romx fichier_rom : charge le fichier_rom à l'adresse de la rom x\n"
		"-borderless : passe l'affichage en mode 'plein écran'\n"
		, NULL
		, MB_OK
		);
}

/********************************************************* !NAME! **************
* Nom : TraiteArgs
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\\/FLE!, ligne : !./LN!
*
* Description : Traitement des arguments passés à la ligne de commande
*
* Résultat    : /
*
* Variables globales modifiées :
*
********************************************************** !0! ****************/
static void TraiteArgs( char * Msg, UBYTE * RunCmd ) {
	static char MsgTmp[ MAX_RUN_CMD ];
	int i;

	do {
		char * p = Msg;
		if ( * p == '-' ) {
			p++;
			if ( * p == '?' ) {
				AideLigneCmd();
				finMain = 1;
			}
			if ( ! strncmp( p, "disk", 4 ) ) {
				strcpy( MsgTmp, &p[ 5 ] );
				SupBlancsFin( MsgTmp );
				NewDiscOn = FALSE;
				LoadDSK( MsgTmp, 0 );
			}
			if ( ! strncmp( p, "snapshot", 8 ) ) {
				strcpy( MsgTmp, &p[ 9 ] );
				SupBlancsFin( MsgTmp );
				LireSnap( MsgTmp );
			}
			if ( ! strncmp( p, "run", 3 ) ) {
				strcpy( MsgTmp, &p[ 4 ] );
				SupBlancsFin( MsgTmp );
				memcpy( RunCmd, "run\"", 4 );
				for ( i = 0; i < ( int )strlen( MsgTmp ); i++ )
					RunCmd[ i + 4 ] = ( UBYTE )toupper( MsgTmp[ i ] );

				RunCmd[ i + 4 ] = '\r';
				RunCmd[ i + 5 ] = 0;
			}
			if ( ! strncmp( p, "cmd", 3 ) ) {
				strcpy( ( char * )RunCmd, &p[ 4 ] );
				strcat( ( char * )RunCmd, "\r" );
			}
			if ( ! strncmp( p, "file", 4 ) ) {
				FILE * fp = fopen( &p[ 5 ], "rt" );
				if ( fp ) {
					* RunCmd = 0;
					i = 0;
					while( fgets( MsgTmp, sizeof( MsgTmp ), fp ) ) {
						i += 1 + ( int )strlen( MsgTmp );
						if ( i < MAX_RUN_CMD ) {
							strcat( ( char * )RunCmd, MsgTmp );
							strcat( ( char * )RunCmd, "\r" );
						}
						else
							break;
					}
					fclose( fp );
				}
			}
			if ( ! strncmp( p, "borderless", 10 ) )
				BorderLess = TRUE;

			if ( ! strncmp( p, "scanlines", 9 ) )
				DoubleLines = TRUE;

			if ( ! strncmp( p, "rom", 3 ) )
				ReadRom( &p[ 4 ], atoi( &p[ 3 ] ) );

			if ( ! strncmp( p, "reso_x", 6 ) )
				TailleScrX = atoi( &p[ 6 ] );

			if ( ! strncmp( p, "reso_y", 6 ) )
				TailleScrY = atoi( &p[ 6 ] );
		}
		p = strchr( Msg, ' ' );
		if ( p )
			Msg = ++p;
		else
			Msg++;
	}
	while( * Msg );
}

void ResetCPC( BOOL InitMem ) {
	MultifaceEnable = 0;
	if ( InitMem )
		InitMemCPC();

	WriteVGA( 0x88 );
	ResetZ80();
	Reset8912();
	ResetCRTC();
	ResetUPD();
	ResetAsic();
	OldRom = 0;
	InitRomCnt = 0;
	CptFrame = 0;
}

static BOOL InitEmul( HINSTANCE hInstance ) {
	if ( ReadConfig( NULL, TRUE ) ) {
		if ( InitScreen( hInstance, SW_SHOW ) ) {
			InitMemCPC();
			if ( ! Multiface || InitMultiface() ) {
				if ( UsePrinter )
					StartPrinter( "print.txt" );
				//
				// Initialisations diverses
				//
				finMain = 0;
				InitPlateforme();
				InitSound();
				InitDesasm();
				return( TRUE );
			}
			else
				Log( MODULENAME, "Rom multiface non trouvée", LOG_ERROR );
		}
		else
			Log( MODULENAME, "Erreur ouverture fenêtre", LOG_ERROR );
	}
	else
		MessageBox( NULL
		, "Fichier de configuration du CPC non trouvé."
		, NULL
		, MB_ICONWARNING
		);

	return( FALSE );
}

static void EndEmul( void ) {
	EndSound();
	CloseTapIn();
	CloseTapOut();

	if ( UsePrinter )
		EndPrinter();

	if ( WriteAviStarted() )
		WriteAviEnd( hWnd );

	if ( WriteYMStarted() )
		WriteYMEnd( hWnd );

	EjectDSK( NULL, 0 );
	EjectDSK( NULL, 1 );
	EndScreen();
}

static UBYTE * EmulRunCmd = ( UBYTE * )"";

void SetEmulCmd( UBYTE * cmd ) {
	EmulRunCmd = cmd;
}

static void Emul( void ) {
	while( ! finMain ) {
		if ( DebugMode || CheckBreakPoint( RegPC ) )
			ShowDebug( hWnd );

		if ( ! Pause || DebugMode )
			CycleZ80( EmulRunCmd, FALSE );
		else {
			MSG msg;
			if ( PeekMessage( &msg, NULL, 0,0, PM_REMOVE ) )
				DispatchMessage( &msg );
			Sleep( 1 );
		}
	}
}

/********************************************************* !NAME! **************
* Nom : DemarreEmul
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\\/FLE!, ligne : !./LN!
*
* Description : Point d'entrée du programme
*
* Résultat    : TRUE si ok, FALSE si erreur (pas ou erreur de fichier config)
*
* Variables globales modifiées :
*
********************************************************** !0! ****************/
static BOOL DemarreEmul( LPSTR lpCmdLine, HINSTANCE hInstance ) {
	char * Msg;
	static char LogMsg[ 80 ];
	static UBYTE RunCmd[ MAX_RUN_CMD ];
	WNDCLASSEX wcex;
	BOOL ret = FALSE;

	* RunCmd = 0;
	CptInstr = 0;
	Msg = lpCmdLine;
	if ( StartLog( "Z80.LOG" ) ) {
		if ( timeBeginPeriod( 1 ) != TIMERR_NOERROR )
			MessageBox( NULL, "Problème de résolution de timer, l'émulateur aura une vitesse incorrecte...", NULL, MB_ICONWARNING );

		if ( Register( hInstance, &wcex ) ) {
			//
			// Décode les arguments de la ligne de commande
			//
			if ( * Msg )
				TraiteArgs( Msg, RunCmd );

			if ( InitEmul( hInstance ) ) {
				ret = TRUE;
				ResetCPC( FALSE );
				SetEmulCmd( RunCmd );
				Emul();
			}
			EndEmul();
			Unregister( hInstance, &wcex );
		}
		else
			Log( MODULENAME, "Erreur enregistrement fenêtre", LOG_ERROR );

		timeEndPeriod( 1 );
		EndLog();
	}
	else
		Erreur( "Impossible d'ouvrir le fichier de trace" );

	return( ret );
}

class CPCCPCApp : public CWinApp {
public:
	virtual BOOL InitInstance() {
		GetCurrentDirectory( sizeof( ExecutableDir ), ExecutableDir );
		if ( DemarreEmul( this->m_lpCmdLine, m_hInstance ) )
			SaveConfig( NULL );

		return( FALSE );
	}
	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CPCCPCApp, CWinApp)
END_MESSAGE_MAP()

CPCCPCApp theApp;
