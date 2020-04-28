/******************************************************************************/
/* Configuration pour l'archivage des diff�rents �l�ments du fichier source   */
/******************************************************************************/
// !CONFIG!=/L/* /R/* /W"* Nom : "
// D�finition du syst�me       !CONFIG!=/V1!EMULATEUR CPC!
// D�finition du sous syst�me  !CONFIG!=/V2!PC-CPC!
// D�finition du sous ensemble !CONFIG!=/V3!Accessoires!
// D�finition du module        !CONFIG!=/V4!Compactage!
/******************************************************************************/

/********************************************************* !NAME! **************
* !./FLE!
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\Fichiers
********************************************************** !0! *****************
* ------------------------------------------------------------------------------
*  Fichier     : PACK.H                | Version : 0.1as
* ------------------------------------------------------------------------------
*  Date        : 18/11/2004            | Auteur  : L.DEPLANQUE
* ------------------------------------------------------------------------------
*  Description : D�finitions pour le module PACK.C
*
* ------------------------------------------------------------------------------
*  Historique  :
*           Date           |         Auteur          |       Description
* ------------------------------------------------------------------------------
*  18/11/2004              | L.DEPLANQUE             | creation
* ------------------------------------------------------------------------------
*  27/11/2005              | L.DEPLANQUE             | Version 0.1ah :
*                          |                         | Prise en compte de
*                          |                         | l'instruction PackWinDiff
*                          |                         | (Compactage diff�rences
*                          |                         | entre m�moire vid�o et
*                          |                         | buffer vid�o)
* ------------------------------------------------------------------------------
*  18/12/2005              | L.DEPLANQUE             | Version 0.1aj :
*                          |                         | Prise en compte de
*                          |                         | l'instruction
*                          |                         | InitBuffDiff
*                          |                         | (Initialisation buffer
*                          |                         | diff�rences inter-images)
* ------------------------------------------------------------------------------
*  22/12/2010              | L.DEPLANQUE             | Version 0.1as :
*                          |                         | Prise en compte de
*                          |                         | l'instruction
*                          |                         | SetCoordDiff
*                          |                         | (coordonn�es pour le 
*                          |                         | calcul de la diff�rence
*                          |                         | inter-images)
* ------------------------------------------------------------------------------
********************************************************** !END! **************/


#ifndef PACK_H
#define PACK_H


int Pack( int Longueur );

int Depack( UBYTE * BufIn, UBYTE * BufOut );

void MemCpcToHost( USHORT AdrCpc, UBYTE * Buf, int Lg );

void MemHostToCpc( UBYTE * Buf, USHORT AdrCpc, int Lg );

int PackScreen( USHORT Adr, BOOL ConvLigne );

int PackWindow( UBYTE xDeb
              , UBYTE yDeb
              , UBYTE xFin
              , UBYTE yFin
              , USHORT Adr
              , BOOL ConvLigne
              , BOOL ConvCoord
              );

void DepackScreen( USHORT Adr, BOOL ConvLigne );

void DepackWindow( USHORT Adr, BOOL ConvLigne );

int PackWinDC( USHORT Adr, BOOL ImageDiff, BOOL ConvLigne, BOOL ConvCoord );

int PackBuffer( USHORT AdrSrc, USHORT Longueur, USHORT AdrDest );

int DepackBuffer( USHORT AdrSrc, USHORT AdrDest );

void InitBuffDiff( USHORT Adr );

void SetCoordDiff( int xd, int yd, int xa, int ya );


#endif

