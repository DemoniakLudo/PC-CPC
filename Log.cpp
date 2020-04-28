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
*          SYSTEME         |      SOUS SYSTEME       |      SOUS ENSEMBLE
* ------------------------------------------------------------------------------
*  EMULATEUR CPC           | PC-CPC                  | Accessoires
* ------------------------------------------------------------------------------
*  Fichier     : LOG.C                 | Version : 0.1at
* ------------------------------------------------------------------------------
*  Date        : 05/11/2002            | Auteur  : L.DEPLANQUE
* ------------------------------------------------------------------------------
*  Description : Archivage d'information dans un fichier log
*
* ------------------------------------------------------------------------------
*  Historique  :
*           Date           |         Auteur          |       Description
* ------------------------------------------------------------------------------
*  05/11/2002              | L.DEPLANQUE             | creation
* ------------------------------------------------------------------------------
*  06/11/2002              | L.DEPLANQUE             | suppression de la fct
*                          |                         | LogMessage qui ne servait
*                          |                         | pas.
* ------------------------------------------------------------------------------
*  08/01/2003              | L.DEPLANQUE             | V0.1t : optimisation
*                          |                         | minime.
* ------------------------------------------------------------------------------
*  25/02/2004              | L.DEPLANQUE             | V0.1y : Ajout du titre
*                          |                         | et du numéro de version
*                          |                         | lors du log du démarrage
*                          |                         | et du log de l'arrêt.
* ------------------------------------------------------------------------------
*  06/03/2007              | L.DEPLANQUE             | V0.1an : Ajout du nom
*                          |                         | du module dans le log
* ------------------------------------------------------------------------------
*  07/02/2011              | L.DEPLANQUE             | V0.1at : Ajout fonctions
*                          |                         | de loggin avec paramètres
* ------------------------------------------------------------------------------
********************************************************** !END! **************/


#include  <stdio.h>

#include  "Types.h"
#include  "Log.h"
#include  "Plateform.h"
#include  "Config.h"


#define     MODULENAME      "Log"


/********************************************************* !NAME! **************
* Nom : MsgLog
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Chaine de caractères servant au formattage des messages à
*               archiver dans le fichier LOG
*
********************************************************** !0! ****************/
static char MsgLog[ MAX_SIZE_MSG_LOG ];


/********************************************************* !NAME! **************
* Nom : fLog
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Handle fichier LOG
*
********************************************************** !0! ****************/
static FILE * fLog = NULL;


/********************************************************* !NAME! **************
* Nom : StartLog
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Description : Ouvre le fichier .LOG
*
* Résultat    : 
*
* Variables globales modifiées : 
*
********************************************************** !0! ****************/
BOOL StartLog( char * LogName )
{
    EndLog();
    fLog = fopen( LogName, "a+" );
    sprintf( MsgLog, "Démarrage application " TITRE );
    Log( MODULENAME, MsgLog, LOG_START );
    return( fLog != NULL );
}


/********************************************************* !NAME! **************
* Nom : Log
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Description : Archive un message dans le fichier .LOG
*
* Résultat    : 
*
* Variables globales modifiées : 
*
********************************************************** !0! ****************/
void Log( char * Module, char * Message, int Niveau )
{
    static SYSTEMTIME sTime;

    if ( fLog && ( Niveau <= LogLevel ) )
        {
        if ( * Message )
            {
            GetLocalTime( &sTime );
            fprintf( fLog, "%02d/%02d/%04d %02d:%02d:%02d [%s]\t%s %s\n"
                   , sTime.wDay
                   , sTime.wMonth
                   , sTime.wYear
                   , sTime.wHour
                   , sTime.wMinute
                   , sTime.wSecond
                   , Module
                   , Niveau == LOG_WARNING ? "!!!" : ""
                   , Message
                   );
            }
        else
            fprintf( fLog, "\n" );

        fflush( fLog );
        }
    if ( Niveau == LOG_ERROR )
        Erreur( Message );
}


void Log( char * Module, char * Message, int Valeur, int Niveau )
{
    if ( Niveau <= LogLevel )
        {
        sprintf( MsgLog, Message, Valeur );
        Log( Module, MsgLog, Niveau );
        }
}


void Log( char * Module, char * Message, int V1, int V2, int Niveau )
{
    if ( Niveau <= LogLevel )
        {
        sprintf( MsgLog, Message, V1, V2 );
        Log( Module, MsgLog, Niveau );
        }
}


void Log( char * Module, char * Message, int V1, int V2, int V3, int Niveau )
{
    if ( Niveau <= LogLevel )
        {
        sprintf( MsgLog, Message, V1, V2, V3 );
        Log( Module, MsgLog, Niveau );
        }
}


void Log( char * Module, char * Message, int V1, char * V2, int V3, int V4, int Niveau )
{
    if ( Niveau <= LogLevel )
        {
        sprintf( MsgLog, Message, V1, V2, V3, V4 );
        Log( Module, MsgLog, Niveau );
        }
}


void Log( char * Module, char * Message, int V1, char * V2, int V3, int V4, int V5, int Niveau )
{
    if ( Niveau <= LogLevel )
        {
        sprintf( MsgLog, Message, V1, V2, V3, V4, V5 );
        Log( Module, MsgLog, Niveau );
        }
}


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
        )
{
    if ( Niveau <= LogLevel )
        {
        sprintf( MsgLog, Message, V1, V2, V3, V4, V5, V6, V7, V8, V9, V10 );
        Log( Module, MsgLog, Niveau );
        }
}


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
        )
{
    if ( Niveau <= LogLevel )
        {
        sprintf( MsgLog, Message, V1, V2, V3, V4, V5, V6, V7, V8, V9, V10, V11 );
        Log( Module, MsgLog, Niveau );
        }
}


void Log( char * Module, char * Message, char * Valeur, int Niveau )
{
    if ( Niveau <= LogLevel )
        {
        sprintf( MsgLog, Message, Valeur );
        Log( Module, MsgLog, Niveau );
        }
}


void Log( char * Module, char * Message, char * V1, char * V2, int Niveau )
{
    if ( Niveau <= LogLevel )
        {
        sprintf( MsgLog, Message, V1, V2 );
        Log( Module, MsgLog, Niveau );
        }
}


void Log( char * Module, char * Message, int V1, char * V2, int Niveau )
{
    if ( Niveau <= LogLevel )
        {
        sprintf( MsgLog, Message, V1, V2 );
        Log( Module, MsgLog, Niveau );
        }
}


void Log( char * Module, char * Message, int V1, char * V2, int V3, int Niveau )
{
    if ( Niveau <= LogLevel )
        {
        sprintf( MsgLog, Message, V1, V2, V3 );
        Log( Module, MsgLog, Niveau );
        }
}



/********************************************************* !NAME! **************
* Nom : EndLog
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Description : Ferme le fichier .LOG
*
* Résultat    : 
*
* Variables globales modifiées : 
*
********************************************************** !0! ****************/
void EndLog( void )
{
    if ( fLog )
        {
        sprintf( MsgLog, "Arrêt application " TITRE );
        Log( MODULENAME, MsgLog, LOG_START );
        Log( MODULENAME, "", LOG_START );
        fclose( fLog );
        fLog = NULL;
        }
}
