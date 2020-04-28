/******************************************************************************/
/* Configuration pour l'archivage des différents éléments du fichier source   */
/******************************************************************************/
// !CONFIG!=/L/* /R/* /W"* Nom : "
// Définition du système       !CONFIG!=/V1!EMULATEUR CPC!
// Définition du sous système  !CONFIG!=/V2!PC-CPC!
// Définition du sous ensemble !CONFIG!=/V3!Accessoires!
// Définition du module        !CONFIG!=/V4!Log!
/******************************************************************************/

/********************************************************* !NAME! **************
* !./FLE!
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\Fichiers
********************************************************** !0! *****************
* ------------------------------------------------------------------------------
*  Fichier     : LOG.H                 | Version : 0.1an
* ------------------------------------------------------------------------------
*  Date        : 05/11/2002            | Auteur  : L.DEPLANQUE
* ------------------------------------------------------------------------------
*  Description : Définitions pour le module LOG.C
*
* ------------------------------------------------------------------------------
*  Historique  :
*           Date           |         Auteur          |       Description
* ------------------------------------------------------------------------------
*  05/11/2002              | L.DEPLANQUE             | creation
* ------------------------------------------------------------------------------
*  06/03/2007              | L.DEPLANQUE             | V0.1an : Ajout du nom
*                          |                         | du module dans le log
* ------------------------------------------------------------------------------
********************************************************** !END! **************/


#ifndef LOG_H
#define LOG_H


enum { LOG_START = 0, LOG_ERROR, LOG_WARNING, LOG_INFO, LOG_DEBUG, LOG_DEBUG_FULL };


#define     MAX_SIZE_MSG_LOG    128


BOOL StartLog( char * LogName );

void Log( char * Module, char * Message, int Niveau );

void Log( char * Module, char * Message, int Valeur, int Niveau );

void Log( char * Module, char * Message, int V1, int V2, int Niveau );

void Log( char * Module, char * Message, int V1, int V2, int V3, int Niveau );

void Log( char * Module, char * Message, int V1, char * V2, int V3, int V4, int Niveau );

void Log( char * Module, char * Message, int V1, char * V2, int V3, int V4, int V5, int Niveau );

void Log( char * Module
        , char * Message
        , int V1
        , char * V2
        , int V3
        , int V4
        , int V5
        , int V6
        , int V7
        , int V8
        , int V9
        , int V10
        , int Niveau
        );

void Log( char * Module
        , char * Message
        , int V1
        , char * V2
        , int V3
        , int V4
        , int V5
        , int V6
        , int V7
        , int V8
        , int V9
        , int V10
        , int V11
        , int Niveau
        );

void Log( char * Module, char * Message, char * Valeur, int Niveau );

void Log( char * Module, char * Message, char * V1, char * V2, int Niveau );

void Log( char * Module, char * Message, int V1, char * V2, int Niveau );

void Log( char * Module, char * Message, int V1, char * V2, int V3, int Niveau );

void EndLog( void );


#endif
