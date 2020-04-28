/******************************************************************************/
/* Configuration pour l'archivage des diff�rents �l�ments du fichier source   */
/******************************************************************************/
// !CONFIG!=/L/* /R/* /W"* Nom : "
// D�finition du syst�me       !CONFIG!=/V1!EMULATEUR CPC!
// D�finition du sous syst�me  !CONFIG!=/V2!PC-CPC!
// D�finition du sous ensemble !CONFIG!=/V3!Noyau!
// D�finition du module        !CONFIG!=/V4!Types!
/******************************************************************************/

/********************************************************* !NAME! **************
* !./FLE!
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\Fichiers
********************************************************** !0! *****************
* ------------------------------------------------------------------------------
*  Fichier     : TYPES.H               | Version : 0.1p
* ------------------------------------------------------------------------------
*  Date        : 05/11/2002            | Auteur  : L.DEPLANQUE
* ------------------------------------------------------------------------------
*  Description : D�finitions de types pr�d�finis et de constantes de compilation
*
* ------------------------------------------------------------------------------
*  Historique  :
*           Date           |         Auteur          |       Description
* ------------------------------------------------------------------------------
*  05/11/2002              | L.DEPLANQUE             | creation
* ------------------------------------------------------------------------------
********************************************************** !END! **************/


#ifndef TYPES_H
#define TYPES_H


#pragma warning( disable : 4711 )
#pragma warning( disable : 4996 )


typedef unsigned short          USHORT;

typedef short                   SHORT;

typedef unsigned char           UBYTE;

typedef unsigned long           ULONG;

typedef int                     BOOL;


/********************************************************* !NAME! **************
* Nom : stToucheClav
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Structures
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Structure clavier CPC
*
********************************************************** !0! ****************/
typedef struct
    {
    UBYTE Port;
    UBYTE Ligne;
    } stToucheClav;


/********************************************************* !NAME! **************
* Nom : StAmsdos
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Structures
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Structure de l'en-t�te Amsdos avant les donn�es des fichiers
*
********************************************************** !0! ****************/
typedef struct
    {
    UBYTE   UserNumber;             // User
    UBYTE   FileName[ 15 ];         // Nom + extension
    UBYTE   BlockNum;               // Num�ro du premier bloc (disquette)
    UBYTE   LastBlock;              // Num�ro du dernier bloc (disquette)
    UBYTE   FileType;               // Type de fichier
    USHORT  Length;                 // Longueur
    USHORT  Adress;                 // Adresse
    UBYTE   FirstBlock;             // Premier bloc de fichier (disquette)
    USHORT  LogicalLength;          // Longueur logique
    USHORT  EntryAdress;            // Point d'entr�e
    UBYTE   Unused[ 0x24 ];
    USHORT  RealLength;             // Longueur r�elle
    UBYTE   BigLength;              // Longueur r�elle (3 octets)
    USHORT  CheckSum;               // Checksum Amsdos
    UBYTE   Unused2[ 0x3B ];
    } StAmsdos;


#ifndef FALSE
enum { FALSE = 0, TRUE };
#endif

#if _MSC_VER > 1200
#define     USE_CIMAGE
#endif

//#define     USE_AYC

//#define USE_SOUND_HBL

#define USE_PHENIX

#endif
