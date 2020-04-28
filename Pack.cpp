/******************************************************************************/
/* Configuration pour l'archivage des différents éléments du fichier source   */
/******************************************************************************/
// !CONFIG!=/L/* /R/* /W"* Nom : "
// Définition du système       !CONFIG!=/V1!EMULATEUR CPC!
// Définition du sous système  !CONFIG!=/V2!PC-CPC!
// Définition du sous ensemble !CONFIG!=/V3!Accessoires!
// Définition du module        !CONFIG!=/V4!Compactage!
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
*  Fichier     : Pack.c                | Version : 0.1as
* ------------------------------------------------------------------------------
*  Date        : 18/11/2004            | Auteur  : L.DEPLANQUE
* ------------------------------------------------------------------------------
*  Description : Gestion compactage/décompactage LZW
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
*                          |                         | (Compactage différences
*                          |                         | entre mémoire vidéo et
*                          |                         | buffer vidéo)
* ------------------------------------------------------------------------------
*  18/12/2005              | L.DEPLANQUE             | Version 0.1aj :
*                          |                         | Prise en compte de
*                          |                         | l'instruction
*                          |                         | InitBuffDiff
*                          |                         | (Initialisation buffer
*                          |                         | différences inter-images)
*                          |                         | Pour les instruction
*                          |                         | PackWinDC et PackWinDiff,
*                          |                         | optimisation de la zone
*                          |                         | à compacter : découpe
*                          |                         | de la zone en 2 zones
*                          |                         | si possible
* ------------------------------------------------------------------------------
*  18/06/2007              | L.DEPLANQUE             | Version 0.1ao :
*                          |                         | Passage de MAX_BUF de 
*                          |                         | 0x4000 à 0x10000
*                          |                         | (Pb compactage buffer
*                          |                         | de taille > à 0x4000)
* ------------------------------------------------------------------------------
*  24/12/2010              | L.DEPLANQUE             | Version 0.1as :
*                          |                         | Prise en compte de
*                          |                         | coordonnées pour le 
*                          |                         | calcul de la différence
*                          |                         | inter-images.
*                          |                         | Correction problème
*                          |                         | longueur données
*                          |                         | lors d'un compactage
*                          |                         | (Bug trouvé grace à
*                          |                         | CMP ;-) )
* ------------------------------------------------------------------------------
********************************************************** !END! **************/


#include  "Types.h"

#include  <windows.h>

#include  "Types.h"
#include  "Z80.h"


#define     SEEKBACK    0x1000

#define     MAXSTRING   256

#define     MAX_BUF     0x10000


/********************************************************* !NAME! **************
* Nom : BufOut
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Buffer en sortie
*
********************************************************** !0! ****************/
static UBYTE BufOut[ MAX_BUF ];


/********************************************************* !NAME! **************
* Nom : BufIn
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Buffer en entrée
*
********************************************************** !0! ****************/
static UBYTE BufIn[ MAX_BUF ];


/********************************************************* !NAME! **************
* Nom : BufIn
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Buffer de calcul des différences inter-images
*
********************************************************** !0! ****************/
static UBYTE BufDiff[ MAX_BUF ];


/********************************************************* !NAME! **************
* Nom : xStartDiff 
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : position x de début de recherche des différences inter-image
*
********************************************************** !0! ****************/
static int xStartDiff = 0;


/********************************************************* !NAME! **************
* Nom : xEndDiff 
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : position x de fin de recherche des différences inter-images
*
********************************************************** !0! ****************/
static int xEndDiff = 80;


/********************************************************* !NAME! **************
* Nom : yStartDiff 
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : position y de début de recherche des différences inter-images
*
********************************************************** !0! ****************/
static int yStartDiff = 0;


/********************************************************* !NAME! **************
* Nom : yEndDiff 
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : position y de fin de recherche des différences inter-images
*
********************************************************** !0! ****************/
static int yEndDiff = 200;


/********************************************************* !NAME! **************
* Nom : Depack
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Décompacte un buffer
*
* Résultat    : Longueur du buffer décompacté
*
* Variables globales modifiées : /
*
********************************************************** !0! ****************/
int Depack( UBYTE * InBuf, UBYTE * OutBuf )
{
    UBYTE a, DepackBits = 0;
    int Bit, InBytes = 0, Longueur, Delta, OutBytes = 0;

    for ( ;; )
        {
        Bit = DepackBits & 1;
        DepackBits >>= 1;
        if ( ! DepackBits )
            {
            DepackBits = InBuf[ InBytes++ ];
            Bit = DepackBits & 1;
            DepackBits >>= 1;
            DepackBits |= 0x80;
            }
        if ( ! Bit )
            OutBuf[ OutBytes++ ] = InBuf[ InBytes++ ];
        else
            {
            if ( ! InBuf[ InBytes ] )
                break; /* EOF */

            a = InBuf[ InBytes ];
            if ( a & 0x80 )
                {
                Longueur = 3 + ( ( InBuf[ InBytes ] >> 4 ) & 7 );
                Delta = ( InBuf[ InBytes++ ] & 15 ) << 8;
                Delta |= InBuf[ InBytes++ ];
                Delta++;
                }
            else
                if ( a & 0x40 )
                    {
                    Longueur = 2;
                    Delta = InBuf[ InBytes++ ] & 0x3f;
                    Delta++;
                    }
                else
                    if ( a & 0x20 )
                        {
                        Longueur = 2 + ( InBuf[ InBytes++ ] & 31 );
                        Delta = InBuf[ InBytes++ ];
                        Delta++;
                        }
                    else
                        if ( a & 0x10 )
                            {
                            Delta = ( InBuf[ InBytes++ ] & 15 ) << 8;
                            Delta |= InBuf[ InBytes++ ];
                            Longueur = InBuf[ InBytes++ ] + 1;
                            Delta++;
                            }
                        else
                            {
                            if ( InBuf[ InBytes ] == 15 )
                                {
                                Longueur = Delta = InBuf[ InBytes+1 ] + 1;
                                InBytes += 2;
                                }
                            else
                                {
                                if ( InBuf[ InBytes ] > 1 )
                                    Longueur = Delta = InBuf[ InBytes ];
                                else
                                    Longueur = Delta = 256;

                                InBytes++;
                                }
                            }
            for ( ; Longueur--; )
                {
                OutBuf[ OutBytes ] = OutBuf[ OutBytes - Delta ];
                OutBytes++;
                }
            }
        }
    return( OutBytes );
}


/********************************************************* !NAME! **************
* Nom : Pack
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Compacte le buffer d'entrée (BufIn) dans le buffer de sortie
*               (BufOut)
*
* Résultat    : Longueur du buffer compacté
*
* Variables globales modifiées : BufOut
*
********************************************************** !0! ****************/
int Pack( int Longueur )
{
    static int Matches[ 256 ];
    static int MachTable[ 256 ][ SEEKBACK ];
    UBYTE CodeBuffer[ 24 ];
    UBYTE Bits = 0;
    int Count = 0, BitCount = 0, CodeCount = 0;
    int MachTableStart = 0, MatchTableEnd = 0, OldMatchTableStart;
    int c, Taille = 0;

    memset( Matches, 0, sizeof( Matches ) );
    for( ;; )
        {
        for ( c = MatchTableEnd; c < Count; c++ )
            {
            int b = BufIn[ c ];
            MachTable[ b ][ Matches[ b ] ] = c;
            Matches[ b ]++;
            }
        MatchTableEnd = Count;

        if ( Count >= 2 )
            {
            int StLen = 0;
            int StPos = 0;
            int StLen2 = 0;
            int bb = BufIn[ Count ];
            for ( c = Matches[ bb ] - 1; c >= 0; c-- )
                {
                int start = MachTable[ bb ][ c ];
                int end = start + MAXSTRING;
                if ( end > Count )
                    end = Count;

                if ( end - start >= StLen )
                    {
                    int maxt = end - start;
                    int d;

                    for ( d = 1; d < maxt; d++ )
                        if ( BufIn[ start+d ] != BufIn[ Count + d ] ) 
                            break;

                    if ( ( d >= 2 ) && ( d > StLen ) )
                        {
                        StLen = d;
                        StPos = Count - start;
                        }
                    if ( ( d == StLen ) && ( Count - start < StPos ) )
                        StPos = Count - start;
                    }

                if ( ( StLen == MAXSTRING ) && ( StPos == StLen ) )
                    break;

                if ( Count + StLen >= Longueur )
                    {
                    StLen = Longueur - Count;
                    break;
                    }
                }
            if ( Count + 1 < Longueur )
                {
                bb = BufIn[ Count + 1 ];
                for ( c = Matches[ bb ] - 1; c >= 0; c-- )
                    {
                    int start = MachTable[ bb ][ c ];
                    int end = start + MAXSTRING;
                    if ( end > Count + 1 )
                        end = Count + 1;

                    if ( end-start >= StLen2 )
                        {
                        int maxt = end - start;
                        int d;

                        for ( d = 1; d < maxt; d++ )
                            if ( BufIn[ start+d ] != BufIn[ Count + d + 1 ] )
                                break;

                        if ( ( d >= 2 ) && ( d >= StLen2 ) )
                            StLen2 = d;
                        }
                    if ( StLen2 == MAXSTRING )
                        break;
                    }
                if ( StLen2 - 1 > StLen )
                    StLen = 0;
                }

            if ( StLen > 1 )
                {
                if ( ( StLen == 2 ) && ( StPos >= 256 ) )
                    {
                    CodeBuffer[ CodeCount++ ] = BufIn[ Count++ ];
                    BitCount++;
                    }
                else
                    {
                    if ( StPos == StLen )
                        {
                        if ( StLen == MAXSTRING )
                            CodeBuffer[ CodeCount++ ] = 0x1;
                        else
                            {
                            if ( StLen <= 14 )
                                CodeBuffer[ CodeCount++ ] = ( UBYTE )StLen;
                            else
                                {
                                CodeBuffer[ CodeCount++ ] = 0xF;
                                CodeBuffer[ CodeCount++ ] = ( UBYTE )( StLen - 1 );
                                }
                            }
                        }
                    else
                        {
                        if ( ( StLen == 2 ) && ( StPos < 65 ) )
                            CodeBuffer[ CodeCount++ ] = ( UBYTE )( 0x40 + StPos - 1 );
                        else
                            {
                            if ( ( StLen <= 33 ) && ( StPos < 257 ) )
                                {
                                CodeBuffer[ CodeCount++ ] = ( UBYTE )( 0x20 + StLen - 2 );
                                CodeBuffer[ CodeCount++ ] = ( UBYTE )( StPos - 1 );
                                }
                            else
                                {
                                if ( ( StLen >= 3 ) && ( StLen <= 10 ) )
                                    {
                                    CodeBuffer[ CodeCount++ ] = ( UBYTE )( 0x80 + ( ( StLen - 3 ) << 4 ) + ( ( StPos - 1 ) >> 8 ) );
                                    CodeBuffer[ CodeCount++ ] = ( UBYTE )( StPos - 1 );
                                    }
                                else
                                    {
                                    CodeBuffer[ CodeCount++ ] = ( UBYTE )( 0x10 + ( ( StPos - 1 ) >> 8 ) );
                                    CodeBuffer[ CodeCount++ ] = ( UBYTE )( StPos - 1 );
                                    CodeBuffer[ CodeCount++ ] = ( UBYTE )( StLen - 1 );
                                    }
                                }
                            }
                        }
                    Bits |= 1 << BitCount;
                    BitCount++;
                    Count += StLen;
                    }
                }
            else
                {
                CodeBuffer[ CodeCount++ ] = BufIn[ Count++ ];
                BitCount++;
                }
            }
        else
            {
            CodeBuffer[ CodeCount++ ] = BufIn[ Count++ ];
            BitCount++;
            }
        if ( BitCount == 8 )
            {
            BufOut[ Taille++ ] = Bits;
            memcpy( &BufOut[ Taille ], CodeBuffer, CodeCount );
            Taille += CodeCount;
            BitCount = CodeCount = 0;
            Bits = 0;
            }
        if ( Count >= Longueur )
            break;

        OldMatchTableStart = MachTableStart;
        MachTableStart = max( 0, Count - SEEKBACK );
        for ( c = OldMatchTableStart; c < MachTableStart; c++ )
            {
            int b = BufIn[ c ];
            int d;
            for ( d = 0; d < Matches[ b ]; d++ )
                if ( MachTable[ b ][ d ] >= MachTableStart )
                    {
                    memmove( &MachTable[ b ][ 0 ]
                           , &MachTable[ b ][ d ]
                           , ( Matches[ b ] - d ) * sizeof( int )
                           );
                    break;
                    }

            Matches[ b ] -= d;
            }
        }
    CodeBuffer[ CodeCount++ ] = 0;
    Bits |= 1 << BitCount;
    BufOut[ Taille++ ] = Bits;
    memcpy( &BufOut[ Taille ], CodeBuffer, CodeCount );
    Taille += CodeCount;
    return( Taille );
}


/********************************************************* !NAME! **************
* Nom : ConvertBuffScreenLigne
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Conversion mémoire écran CPC -> mémoire écran linéaire
*               (Linéarisation par lignes)
*
* Résultat    : Longueur buffer converti
*
* Variables globales modifiées : /
*
********************************************************** !0! ****************/
static int ConvertBuffScreenLigne( UBYTE xDeb
                                 , UBYTE yDeb
                                 , UBYTE xFin
                                 , UBYTE yFin
                                 )
{
    static UBYTE BLigne[ 0x4000 ];
    int p = 0;
    int NbOctets = xFin - xDeb + 1;
    int Length = NbOctets * ( yFin + 1 - yDeb );

    for ( int l = yDeb; l <= yFin; l++ )
        {
        memcpy( &BLigne[ p ]
              , &BufIn[ ( l >> 3 ) * 80 + ( l & 7 ) * 0x800 + xDeb ]
              , NbOctets
              );
        p += NbOctets;
        }
    memcpy( BufIn, BLigne, Length );
    return( Length );
}


/********************************************************* !NAME! **************
* Nom : ConvertBuffScreenCol
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Conversion mémoire écran CPC -> mémoire écran linéaire
*               (Linéarisation par colonnes)
*
* Résultat    : Longueur buffer converti
*
* Variables globales modifiées : /
*
********************************************************** !0! ****************/
static int ConvertBuffScreenCol( UBYTE xDeb
                               , UBYTE yDeb
                               , UBYTE xFin
                               , UBYTE yFin
                               )
{
    static UBYTE BLigne[ 0x4000 ];
    int p = 0;
    int Length = ( xFin + 1 - xDeb ) * ( yFin + 1 - yDeb );

    for ( int c = xDeb; c <= xFin; c++ )
        for ( int l = yDeb; l <= yFin; l++ )
            BLigne[ p++ ] = BufIn[ ( l >> 3 ) * 80 + ( l & 7 ) * 0x800 + c ];

    memcpy( BufIn, BLigne, Length );
    return( Length );
}


/********************************************************* !NAME! **************
* Nom : FindBuffScreen
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Détermine les coordonnées d'un objet affiché à l'écran
*
* Résultat    : /
*
* Variables globales modifiées : /
*
********************************************************** !0! ****************/
static void FindBuffScreen( UBYTE * BufIn
                          , UBYTE * BufImg
                          , UBYTE * xDeb
                          , UBYTE * yDeb
                          , UBYTE * xFin
                          , UBYTE * yFin
                          )
{
    int adr = 0, l, oct;

    * xFin = 0;
    * xDeb = 79;
    * yDeb = 199;
    * yFin = 0;
    for ( l = yStartDiff; l < yEndDiff; l++ )
        {
        adr = ( l >> 3 ) * 80 + ( l & 7 ) * 0x800;
        for ( oct = xStartDiff; oct < xEndDiff; oct++ )
            {
            if ( BufIn[ adr + oct ] ^ BufImg[ adr + oct ] )
                {
                * xDeb = ( UBYTE )min( * xDeb, oct );
                * xFin = ( UBYTE )max( * xFin, oct );
                * yDeb = ( UBYTE )min( * yDeb, l );
                * yFin = ( UBYTE )max( * yFin, l );
                }
            }
        }
}


/********************************************************* !NAME! **************
* Nom : CompteObjets
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Détermine le nombre d'objets à l'écran
*
* Résultat    : Le nombre d'objets trouvés à l'écran
*
* Variables globales modifiées : /
*
********************************************************** !0! ****************/
static UBYTE * CompteObjets( UBYTE * BufIn, UBYTE * BufImg )
{
    static UBYTE TabObjets[ 2048 ];
    int adr = 0, l, oct;
    UBYTE xFin = 0, xDeb = 79, yDeb = 199, yFin = 0;
    int NumObj = 0, LigneVide;

    memset( TabObjets, -1, sizeof( TabObjets ) );
    for ( l = 0; l < 200; l++ )
        {
        LigneVide = 1;
        adr = ( l >> 3 ) * 80 + ( l & 7 ) * 0x800;
        for ( oct = 0; oct < 80; oct++ )
            {
            if ( BufIn[ adr + oct ] ^ BufImg[ adr + oct ] )
                {
                LigneVide = 0;
                if ( xDeb > oct )
                    xDeb = ( UBYTE )oct;

                if ( xFin < oct )
                    xFin = ( UBYTE )oct;

                if ( yDeb > l )
                    yDeb = ( UBYTE )l;

                if ( yFin < l )
                    yFin = ( UBYTE )l;
                }
            }
        if ( LigneVide && xFin >= xDeb && NumObj < 2040 )
            {
            TabObjets[ NumObj++ ] = xDeb;
            TabObjets[ NumObj++ ] = yDeb;
            TabObjets[ NumObj++ ] = xFin;
            TabObjets[ NumObj++ ] = yFin;
            xFin = 0;
            xDeb = 79;
            yDeb = 199;
            yFin = 0;
            }
        }
    if ( ! NumObj && xFin >= xDeb )
        {
        TabObjets[ NumObj++ ] = xDeb;
        TabObjets[ NumObj++ ] = yDeb;
        TabObjets[ NumObj++ ] = xFin;
        TabObjets[ NumObj++ ] = yFin;
        }
    return( TabObjets );
}


/********************************************************* !NAME! **************
* Nom : MemCpcToHost
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Copie une partie de la mémoire cpc vers un buffer
*
* Résultat    : /
*
* Variables globales modifiées : /
*
********************************************************** !0! ****************/
void MemCpcToHost( USHORT AdrCpc, UBYTE * Buf, int Lg )
{
    for ( ; Lg--; )
        * Buf++ = Peek8Ext( AdrCpc++ );
}


/********************************************************* !NAME! **************
* Nom : MemHostToCpc
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Copie un buffer vers une partie de la mémoire cpc
*
* Résultat    : /
*
* Variables globales modifiées : /
*
********************************************************** !0! ****************/
void MemHostToCpc( UBYTE * Buf, USHORT AdrCpc, int Lg )
{
    for ( ; Lg--; )
        Poke8Ext( AdrCpc++, * Buf++ );
}


/********************************************************* !NAME! **************
* Nom : PackWin
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Compacte une fenêtre et retourne sa longueur
*
* Résultat    : Longueur compactée
*
* Variables globales modifiées : BufOut
*
********************************************************** !0! ****************/
static int PackWin( UBYTE xDeb
                  , UBYTE yDeb
                  , UBYTE xFin
                  , UBYTE yFin
                  , BOOL ConvLigne
                  )
{
    int Longueur;

    MemCpcToHost( 0xC000, BufIn, 0x4000 );
    if ( ConvLigne )
        Longueur = ConvertBuffScreenLigne( xDeb, yDeb, xFin, yFin );
    else
        Longueur = ConvertBuffScreenCol( xDeb, yDeb, xFin, yFin );

    return( Pack( Longueur ) );
}


/********************************************************* !NAME! **************
* Nom : StockInfosTaillePos
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Stocke les informations relatives à la position et à la taille
*               d'une fenêtre compactée
*
* Résultat    : Nouvelle adresse de stockage des données
*
* Variables globales modifiées : /
*
********************************************************** !0! ****************/
static USHORT StockInfosTaillePos( USHORT Adr
                                 , UBYTE xDeb
                                 , UBYTE yDeb
                                 , UBYTE xFin
                                 , UBYTE yFin
                                 , BOOL ConvCoord
                                 )
{
    int AdrEcr;

    if ( ConvCoord )
        {
        AdrEcr = xDeb + ( yDeb >> 3 ) * 80 + ( yDeb & 7 ) * 0x800;
        Poke8Ext( Adr++, ( UBYTE )AdrEcr );
        Poke8Ext( Adr++, ( UBYTE )( AdrEcr >> 8 ) );
        Poke8Ext( Adr++, ( UBYTE )( xFin - xDeb + 1 ) );
        Poke8Ext( Adr++, ( UBYTE )( yFin - yDeb + 1 ) );
        }
    else
        {
        Poke8Ext( Adr++, xDeb );
        Poke8Ext( Adr++, yDeb );
        Poke8Ext( Adr++, xFin );
        Poke8Ext( Adr++, yFin );
        }
    return( Adr );
}


/********************************************************* !NAME! **************
* Nom : PackScreen
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Compacte un écran (depuis la mémoire vidéo CPC)
*
* Résultat    : Longueur compactée
*
* Variables globales modifiées : BufIn, BufOut
*
********************************************************** !0! ****************/
int PackScreen( USHORT Adr, BOOL ConvLigne )
{
    int lpack;

    lpack = PackWin( 0, 0, 79, 199, ConvLigne );
    MemHostToCpc( BufOut, Adr, lpack );
    return( lpack );
}


/********************************************************* !NAME! **************
* Nom : PackWindow
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Compacte une fenêtre (depuis la mémoire vidéo CPC)
*
* Résultat    : Longueur compactée
*
* Variables globales modifiées : BufIn, BufOut
*
********************************************************** !0! ****************/
int PackWindow( UBYTE xDeb
              , UBYTE yDeb
              , UBYTE xFin
              , UBYTE yFin
              , USHORT Adr
              , BOOL ConvLigne
              , BOOL ConvCoord
              )
{
    int lpack;

    lpack = PackWin( xDeb, yDeb, xFin, yFin, ConvLigne );
    Adr = StockInfosTaillePos( Adr, xDeb, yDeb, xFin, yFin, ConvCoord );
    MemHostToCpc( BufOut, Adr, lpack );
    return( lpack + 4 );
}


/********************************************************* !NAME! **************
* Nom : PackWinDC
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Compacte la fenêtre utile (depuis la mémoire vidéo CPC)
*
* Résultat    : Longueur compactée
*
* Variables globales modifiées : BufIn, BufOut
*
********************************************************** !0! ****************/
int PackWinDC( USHORT Adr, BOOL ImageDiff, BOOL ConvLigne, BOOL ConvCoord )
{
    int lpack, Methode = 1, LongFull, MemoLong;
    UBYTE * TabObj;
    USHORT MemoAdr;
    UBYTE xDeb, yDeb, xFin, yFin;

    if ( ! ImageDiff )
        memset( BufDiff, 0, sizeof( BufDiff ) );

    MemCpcToHost( 0xC000, BufIn, 0x4000 );
    FindBuffScreen( BufIn, BufDiff, &xDeb, &yDeb, &xFin, &yFin );
    TabObj = CompteObjets( BufIn, BufDiff );
    memcpy( BufDiff, BufIn, 0x4000 );
    LongFull = PackWin( xDeb, yDeb, xFin, yFin, ConvLigne );
/*
    // 2 objets trouvés ?
    if ( TabObj[ 8 ] == 0xFF && TabObj[ 4 ] != 0xFF )
        Methode = 2;
*/
    if ( Methode == 1 )
        {
        lpack = PackWin( xDeb, yDeb, xFin, yFin, ConvLigne );
        Adr = StockInfosTaillePos( Adr, xDeb, yDeb, xFin, yFin, ConvCoord );
        MemHostToCpc( BufOut, Adr, lpack );
        return( lpack + 4 );
        }
    else
        {
        //
        // Cas où l'on a 2 objets séparés à l'écran
        //
        lpack = PackWin( TabObj[ 0 ]
                       , TabObj[ 1 ]
                       , TabObj[ 2 ]
                       , TabObj[ 3 ]
                       , ConvLigne
                       );
        Adr = StockInfosTaillePos( Adr
                                 , TabObj[ 0 ]
                                 , TabObj[ 1 ]
                                 , TabObj[ 2 ]
                                 , TabObj[ 3 ]
                                 , ConvCoord
                                 );
        MemHostToCpc( BufOut, Adr, lpack );
        MemoAdr = ( USHORT )( Adr - 4 );
        Adr = ( USHORT )( Adr + lpack );
        MemoLong = lpack + 4;
        lpack += PackWin( TabObj[ 4 ]
                        , TabObj[ 5 ]
                        , TabObj[ 6 ]
                        , TabObj[ 7 ]
                        , ConvLigne
                        );
        //
        // Gain de place si compactage 2 objets séparés ?
        //
        if ( lpack + 8 < LongFull )
            {
            Adr = StockInfosTaillePos( Adr
                                     , TabObj[ 4 ]
                                     , TabObj[ 5 ]
                                     , TabObj[ 6 ]
                                     , TabObj[ 7 ]
                                     , ConvCoord
                                     );
            MemHostToCpc( BufOut, Adr, lpack );
            return( MemoLong + ( ( lpack + 4 ) << 16 ) );
            }
        else
            {
            //
            // Sinon, compactage comme un seul objet
            //
            Adr = MemoAdr;
            lpack = PackWin( xDeb, yDeb, xFin, yFin, ConvLigne );
            Adr = StockInfosTaillePos( Adr, xDeb, yDeb, xFin, yFin, ConvCoord );
            MemHostToCpc( BufOut, Adr, lpack );
            return( lpack + 4 );
            }
        }
}


/********************************************************* !NAME! **************
* Nom : DepackArea
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Décompacte une zone écran écran (vers la mémoire vidéo CPC)
*
* Résultat    : 0
*
* Variables globales modifiées : BufIn, BufOut
*
********************************************************** !0! ****************/
static void DepackArea( UBYTE xDeb
                      , UBYTE yDeb
                      , UBYTE xFin
                      , UBYTE yFin
                      , USHORT Adr
                      , BOOL ConvLigne
                      )
{
    int oct, Longueur, l, AdrHost = 0;

    UBYTE NbX = ( UBYTE )( xFin - xDeb + 1 );
    MemCpcToHost( Adr, BufIn, 0x4000 );
    Longueur = Depack( BufIn, BufOut );
    if ( ConvLigne )
        {
        for ( l = yDeb; l <= yFin; l++ )
            {
            MemHostToCpc( &BufOut[ AdrHost ]
                        , ( USHORT )( 0xC000 + ( l >> 3 ) * 80 + ( l & 7 ) * 0x800 + xDeb )
                        , NbX
                        );
            AdrHost += NbX;
            }
        }
    else
        for ( oct = xDeb; oct <= xFin; oct++ )
            for ( l = yDeb; l <= yFin; l++ )
                Poke8Ext( ( USHORT )( 0xC000 + ( l >> 3 ) * 80 + ( l & 7 ) * 0x800 + oct )
                        , BufOut[ AdrHost++ ]
                        );
}


/********************************************************* !NAME! **************
* Nom : DepackScreen
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Décompacte un écran (vers la mémoire vidéo CPC)
*
* Résultat    : 0
*
* Variables globales modifiées : BufIn, BufOut
*
********************************************************** !0! ****************/
void DepackScreen( USHORT Adr, BOOL ConvLigne )
{
    DepackArea( 0, 0, 79, 199, Adr, ConvLigne );
}


/********************************************************* !NAME! **************
* Nom : DepackWindow
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Décompacte une fenêtre (vers la mémoire vidéo CPC)
*
* Résultat    : 0
*
* Variables globales modifiées : BufIn, BufOut
*
********************************************************** !0! ****************/
void DepackWindow( USHORT Adr, BOOL ConvLigne )
{
    UBYTE xDeb = Peek8Ext( Adr++ );
    UBYTE yDeb = Peek8Ext( Adr++ );
    UBYTE xFin = Peek8Ext( Adr++ );
    UBYTE yFin = Peek8Ext( Adr++ );
    DepackArea( xDeb, yDeb, xFin, yFin, Adr, ConvLigne );
}


/********************************************************* !NAME! **************
* Nom : PackBuffer
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Compacte un buffer (de et vers la mémoire CPC)
*
* Résultat    : Longueur compactée
*
* Variables globales modifiées : BufIn, BufOut
*
********************************************************** !0! ****************/
int PackBuffer( USHORT AdrSrc, USHORT Longueur, USHORT AdrDest )
{
    int lpack;

    MemCpcToHost( AdrSrc, BufIn, Longueur );
    lpack = Pack( Longueur );
    MemHostToCpc( BufOut, AdrDest, lpack );
    return( lpack );
}


/********************************************************* !NAME! **************
* Nom : DepackBuffer
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Décompacte un buffer (de et vers la mémoire CPC)
*
* Résultat    : Longueur décompactée
*
* Variables globales modifiées : BufIn, BufOut
*
********************************************************** !0! ****************/
int DepackBuffer( USHORT AdrSrc, USHORT AdrDest )
{
    int Longueur;
    
    MemCpcToHost( AdrSrc, BufIn, 0xFFFF - AdrSrc );
    Longueur = Depack( BufIn, BufOut );
    MemHostToCpc( BufOut, AdrDest, Longueur );
    return( Longueur );
}


/********************************************************* !NAME! **************
* Nom : InitBuffDiff
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Initialise le buffer des différences inter-images
*
* Résultat    : /
*
* Variables globales modifiées : BufDiff
*
********************************************************** !0! ****************/
void InitBuffDiff( USHORT Adr )
{
    MemCpcToHost( Adr, BufDiff, 0x4000 );
}


/********************************************************* !NAME! **************
* Nom : SetCoordDiff
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Initialise le buffer des différences inter-images
*
* Résultat    : /
*
* Variables globales modifiées : xStartDiff, xEndDiff, yStartDiff, yEndDiff
*
********************************************************** !0! ****************/
void SetCoordDiff( int xd, int yd, int xa, int ya )
{
    xStartDiff = xd;
    yStartDiff = yd;
    xEndDiff = xa;
    yEndDiff = ya;
}
