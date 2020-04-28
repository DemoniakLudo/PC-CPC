/******************************************************************************/
/* Configuration pour l'archivage des diff�rents �l�ments du fichier source   */
/******************************************************************************/
// !CONFIG!=/L/* /R/* /W"* Nom : "
// D�finition du syst�me       !CONFIG!=/V1!EMULATEUR CPC!
// D�finition du sous syst�me  !CONFIG!=/V2!PC-CPC!
// D�finition du sous ensemble !CONFIG!=/V3!Chips!
// D�finition du module        !CONFIG!=/V4!Imprimante!
/******************************************************************************/

/********************************************************* !NAME! **************
* !./FLE!
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\Fichiers
********************************************************** !0! *****************
* ------------------------------------------------------------------------------
*          SYSTEME         |      SOUS SYSTEME       |      SOUS ENSEMBLE
* ------------------------------------------------------------------------------
*  EMULATEUR CPC           | PC-CPC                  | Chips
* ------------------------------------------------------------------------------
*  Fichier     : Printer.c             | Version : 0.1v
* ------------------------------------------------------------------------------
*  Date        : 09/01/2003            | Auteur  : L.DEPLANQUE
* ------------------------------------------------------------------------------
*  Description : Emulation du port imprimante
*
* ------------------------------------------------------------------------------
*  Historique  :
*           Date           |         Auteur          |       Description
* ------------------------------------------------------------------------------
*  09/01/2003              | L.DEPLANQUE             | creation
* ------------------------------------------------------------------------------
********************************************************** !END! **************/


#include  <stdio.h>

#include  "Types.h"
#include  "Printer.h"


/********************************************************* !NAME! **************
* Nom : fPrint
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Handle du fichier de redirection de l'impression
*
********************************************************** !0! ****************/
static FILE * fPrint = NULL;


/********************************************************* !NAME! **************
* Nom : StartPrinter
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Initialise l'�mulation de l'impression
*
* R�sultat    : TRUE si initialisation OK, FALSE sinon
*
* Variables globales modifi�es : fPrint
*
********************************************************** !0! ****************/
BOOL StartPrinter( char * LogName )
{
    EndPrinter();
    fPrint = fopen( LogName, "a+" );
    return( fPrint != NULL );
}


/********************************************************* !NAME! **************
* Nom : EndPrinter
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Termine l'�mulation de l'impression
*
* R�sultat    : 
*
* Variables globales modifi�es : fPrint
*
********************************************************** !0! ****************/
void EndPrinter( void )
{
    if ( fPrint )
        {
        fclose( fPrint );
        fPrint = NULL;
        }
}


/********************************************************* !NAME! **************
* Nom : PrintVal
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Redirige le caract�re imprim� dans le fichier d'�mulation
*
* R�sultat    : 
*
* Variables globales modifi�es : 
*
********************************************************** !0! ****************/
void PrintVal( int val )
{
    static BOOL MemoEscape = 0;

    if ( val & 0x80 )
        {
        val &= 0x7F;
        if ( val == 0x1B )
            {
            MemoEscape = 1;
            }
        else
            {
            if ( ! MemoEscape )
                {
                fprintf( fPrint, "%c", val );
                fflush( fPrint );
                }
            else
                {
                MemoEscape = 0;
                }
            }
        }
}
