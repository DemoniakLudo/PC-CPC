/******************************************************************************/
/* Configuration pour l'archivage des différents éléments du fichier source   */
/******************************************************************************/
// !CONFIG!=/L/* /R/* /W"* Nom : "
// Définition du système       !CONFIG!=/V1!EMULATEUR CPC!
// Définition du sous système  !CONFIG!=/V2!PC-CPC!
// Définition du sous ensemble !CONFIG!=/V3!Accessoires!
// Définition du module        !CONFIG!=/V4!SauveBmp!
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
*  Fichier     : SAUVEBMP.C            | Version : 0.1at
* ------------------------------------------------------------------------------
*  Date        : 30/11/2005            | Auteur  : L.DEPLANQUE
* ------------------------------------------------------------------------------
*  Description : Ecriture d'un fichier bmp contenant l'écran de l'émulateur
*
* ------------------------------------------------------------------------------
*  Historique  :
*           Date           |         Auteur          |       Description
* ------------------------------------------------------------------------------
*  30/11/2005              | L.DEPLANQUE             | creation
* ------------------------------------------------------------------------------
*  08/12/2005              | L.DEPLANQUE             | Version 0.1ai:
*                          |                         | prise en compte
*                          |                         | paramètres bitmap pour
*                          |                         | sauvegarde BMP sous
*                          |                         | plusieurs formats
* ------------------------------------------------------------------------------
*  21/05/2007              | L.DEPLANQUE             | Version 0.1ao:
*                          |                         | Correction problème lors
*                          |                         | de la sauvegarde en mode
*                          |                         | "petit écran"
* ------------------------------------------------------------------------------
*  15/02/2011              | L.DEPLANQUE             | Version 0.1at:
*                          |                         | Optimisations mineures
* ------------------------------------------------------------------------------
********************************************************** !END! **************/



#include  <windows.h>
#include  <stdio.h>

#include  "Types.h"

#include  "Plateform.h"
#include  "Config.h"
#include  "Log.h"


#define     MODULENAME      "SauveBmp"


/********************************************************* !NAME! **************
* Nom : SauveBitmap
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\\/FLE!, ligne : !./LN!
*
* Description : Ecrit une image au format BMP
*
* Résultat    : 
*
* Variables globales modifiées : 
*
********************************************************** !0! ****************/
void SauveBitmap( void )
{
    static char NomFic[ LEN_PATH ];
    static int NumBitmap = 0;
    BITMAPFILEHEADER bmHeader;
    BITMAPINFO pbmiBmp;
    DWORD BytesWritten;
    HANDLE f;
    int NbLig = pbmi.bmiHeader.biHeight;
    int NbCols = pbmi.bmiHeader.biWidth;
    int Depth = pbmi.bmiHeader.biBitCount;
    //int Size = pbmi.bmiHeader.biWidth * pbmi.bmiHeader.biHeight * ( Depth >> 3 );
    int OfsY = 0;

    do
        {
        char * p = "\\";
        if ( ! DirBMPFile || DirBMPFile[ strlen( DirBMPFile ) - 1 ] == * p )
            p = "";

        sprintf( NomFic, "%s%sCPC_%04d.BMP", DirBMPFile, p, NumBitmap++ );
        HANDLE h = CreateFile( NomFic
                             , GENERIC_READ
                             , 0
                             , NULL
                             , OPEN_EXISTING
                             , 0
                             , NULL
                             );
        if ( h == INVALID_HANDLE_VALUE )
            break;

        CloseHandle( h );
        }
    while( NumBitmap < 9999 );

    memcpy( &pbmiBmp, &pbmi, sizeof( pbmi ) );
	pbmiBmp.bmiHeader.biHeight = NbLig; // 400;
	pbmiBmp.bmiHeader.biWidth = NbCols; // 640;

    memset( &bmHeader, 0, sizeof( bmHeader ) );
    bmHeader.bfType = * ( WORD * )"BM";
    bmHeader.bfOffBits = sizeof( bmHeader ) + sizeof( BITMAPINFOHEADER );
    if ( Depth == 8 )
        bmHeader.bfOffBits += sizeof( RgbCPCColor );

    bmHeader.bfSize = pbmi.bmiHeader.biSizeImage + bmHeader.bfOffBits;

    f = CreateFile( NomFic
                  , GENERIC_READ | GENERIC_WRITE
                  , 0
                  , NULL
                  , CREATE_ALWAYS
                  , 0
                  , NULL
                  );
    if ( f != INVALID_HANDLE_VALUE )
        {
        WriteFile( f, &bmHeader, sizeof( bmHeader ), &BytesWritten, NULL );
        WriteFile( f
                 , &pbmiBmp.bmiHeader
                 , sizeof( pbmiBmp.bmiHeader )
                 , &BytesWritten
                 , NULL
                 );
        if ( Depth == 8 )
            WriteFile( f
                     , Monochrome ? &RgbCPCMono : &RgbCPCColor
                     , sizeof( RgbCPCColor )
                     , &BytesWritten
                     , NULL
                     );

        for ( int y = 0 /*64*/; y < NbLig /*464*/; y++ )
            WriteFile( f
                     , &MemBitmap[ ( ( ( y + OfsY ) * DISPLAY_X ) * ( Depth >> 3 ) / 4 ) + 64 ]
                     , ( Depth >> 3 ) * 640
                     , &BytesWritten
                     , NULL
                     );

        CloseHandle( f );
        }
    else
        Log( MODULENAME, "Impossible de créer le fichier %s.", NomFic, LOG_ERROR );
}
