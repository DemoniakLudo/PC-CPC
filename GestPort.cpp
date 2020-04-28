/******************************************************************************/
/* Configuration pour l'archivage des diff�rents �l�ments du fichier source   */
/******************************************************************************/
// !CONFIG!=/L/* /R/* /W"* Nom : "
// D�finition du syst�me       !CONFIG!=/V1!EMULATEUR CPC!
// D�finition du sous syst�me  !CONFIG!=/V2!PC-CPC!
// D�finition du sous ensemble !CONFIG!=/V3!Chips!
// D�finition du module        !CONFIG!=/V4!Ports!
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
*  Fichier     : GESTPORT.C            | Version : 0.1at
* ------------------------------------------------------------------------------
*  Date        : 05/11/2002            | Auteur  : L.DEPLANQUE
* ------------------------------------------------------------------------------
*  Description : Gestion des ports d'entr�e/sortie
*
* ------------------------------------------------------------------------------
*  Historique  :
*           Date           |         Auteur          |       Description
* ------------------------------------------------------------------------------
*  05/11/2002              | L.DEPLANQUE             | creation
* ------------------------------------------------------------------------------
*  09/01/2003              | L.DEPLANQUE             | Version 0.1u : Test
*                          |                         | nbre de ports �crits
*                          |                         | simultan�ments (proto)
* ------------------------------------------------------------------------------
*  22/01/2004              | L.DEPLANQUE             | Version 0.1x : Ajout
*                          |                         | adresse du PC dans les
*                          |                         | traces du fichier .LOG
* ------------------------------------------------------------------------------
*  07/03/2011              | L.DEPLANQUE             | Version 0.1t : Ajout du
*                          |                         | port #FEFE,
*                          |                         | retourne l'ID de PC-CPC
*                          |                         | (#AA)
* ------------------------------------------------------------------------------
********************************************************** !END! **************/


#include  "types.h"
#include  "Config.h"
#include  "Ppi.h"
#include  "Crtc.h"
#include  "Upd.h"
#include  "Vga.h"
#include  "Z80.h"
#include  "Log.h"
#include  "SnapShot.h"
#include  "Multiface.h"
#include  "Printer.h"

#define     MODULENAME      "GestPort"


/********************************************************* !NAME! **************
* Nom : ReadPort
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Lecture d'un port d'entr�e
*
* R�sultat    : La valeur du port selectionn�
*
* Variables globales modifi�es : /
*
********************************************************** !0! ****************/
int ReadPort( int port )
{
    if ( ! ( port & 0x0480 ) )
        return( ReadUPD( port ) );

    if ( ! ( port & 0x0800 ) )
        return( ReadPPI( port ) );

    if ( ! ( port & 0x4000 ) )
        return( ReadCRTC( port) );

    if ( port == 0xFEFE )
        return( 0xAA ); // Code �mulateur - utilis� par la "ROM PACK"

    Log( MODULENAME, "* Acc�s lecture port %04X (PC=%04X)", port, RegPC, LOG_INFO );
    return( 0xFF );
}


/********************************************************* !NAME! **************
* Nom : WritePort
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Ecriture d'un port de sortie
*
* R�sultat    : /
*
* Variables globales modifi�es : /
*
********************************************************** !0! ****************/
void WritePort( int port, int val )
{
    int bit = 0;

    if ( ( port & 0xC000 ) == 0x04000 )
        {
        WriteVGA( val );
        bit++;
        }

    if ( ! ( port & 0x4000 ) )
        {
        WriteCRTC( port, val );
        bit++;
        }

    if ( ! ( port & 0x2000 ) )
        {
        WriteROM( val );
        bit++;
        }

    if ( ! ( port & 0x1000 ) )
        {
        if ( UsePrinter )
            PrintVal( val );
//        bit++;
        }

    if ( ! ( port & 0x0800 ) )
        {
        WritePPI( port, val );
        bit++;
        }

    if ( ! ( port & 0x0480 ) )
        {
        WriteUPD( port, val );
        bit++;
        }

    if ( bit != 1 )
        Log( MODULENAME, "* Acc�s �criture port %04X (PC=%04X)", port, RegPC, LOG_INFO );

    if ( Multiface )
        MultifaceWriteIO( port, val );
}