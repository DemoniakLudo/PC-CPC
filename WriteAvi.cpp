/******************************************************************************/
/* Configuration pour l'archivage des différents éléments du fichier source   */
/******************************************************************************/
// !CONFIG!=/L/* /R/* /W"* Nom : "
// Définition du système       !CONFIG!=/V1!EMULATEUR CPC!
// Définition du sous système  !CONFIG!=/V2!PC-CPC!
// Définition du sous ensemble !CONFIG!=/V3!Accessoires!
// Définition du module        !CONFIG!=/V4!WriteAvi!
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
*  Fichier     : WRITEAVI.C            | Version : 0.1aj
* ------------------------------------------------------------------------------
*  Date        : 30/11/2005            | Auteur  : L.DEPLANQUE
* ------------------------------------------------------------------------------
*  Description : Ecriture d'un fichier avi contenant les images de l'émulation
*
* ------------------------------------------------------------------------------
*  Historique  :
*           Date           |         Auteur          |       Description
* ------------------------------------------------------------------------------
*  30/11/2005              | L.DEPLANQUE             | creation
* ------------------------------------------------------------------------------
*  09/01/2006              | L.DEPLANQUE             | Version 0.1aj : 
*                          |                         | Ajout de l'écriture des
*                          |                         | sons dans le fichier AVI
* ------------------------------------------------------------------------------
********************************************************** !END! **************/


#include  "Types.h"

#include  <windows.h>
#include  <vfw.h>

#include  "Log.h"
#include  "AY8912.h"
#include  "Config.h"
#include  "WriteAvi.h"
#include  "Plateform.h"


#define     MODULENAME      "WRITEAVI"


#define FPS         25                  // Nbre d'images par secondes

#define AVIIF_KEYFRAME    0x00000010L   // frame "clé"


/********************************************************* !NAME! **************
* Nom : pfile
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Pointeur vers l'interface de manipulation de fichiers avi
*
********************************************************** !0! ****************/
static PAVIFILE pfile = NULL;


/********************************************************* !NAME! **************
* Nom : ps
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Pointeur vers un flux de données avi
*
********************************************************** !0! ****************/
static PAVISTREAM ps = NULL;


/********************************************************* !NAME! **************
* Nom : as
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Pointeur vers un flux de données avi (audio)
*
********************************************************** !0! ****************/
static PAVISTREAM as = NULL;


/********************************************************* !NAME! **************
* Nom : psCmp
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Pointeur vers le flux de données avi compressées
*
********************************************************** !0! ****************/
static PAVISTREAM psCmp = NULL;


/********************************************************* !NAME! **************
* Nom : BmInfoHeader
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Structure bitmap à sauvegarder (contient des informations sur
*               la taille et les couleurs de l'image)
*
********************************************************** !0! ****************/
static BITMAPINFOHEADER BmInfoHeader;


/********************************************************* !NAME! **************
* Nom : NumFrame
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Numéro d'image en cours d'écriture
*
********************************************************** !0! ****************/
static int NumFrame = 0;


/********************************************************* !NAME! **************
* Nom : NumSample
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Numéro de samples en cours d'écriture
*
********************************************************** !0! ****************/
static int NumSample = 0;


/********************************************************* !NAME! **************
* Nom : AviFileStarted
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Indique si l'écriture est démarrée ou non
*
********************************************************** !0! ****************/
static BOOL AviFileStarted = FALSE;


/********************************************************* !NAME! **************
* Nom : WriteAviInit
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\\/FLE!, ligne : !./LN!
*
* Description : Initialisation (création) du fichier AVI
*
* Résultat    : TRUE si ok, FALSE sinon
*
* Variables globales modifiées : pfile, ps, psCmp, BmInfoHeader
*
********************************************************** !0! ****************/
BOOL WriteAviInit( HWND hWnd, char * Nom, int TailleX, int TailleY )
{
    AVISTREAMINFO strhdr;
    AVISTREAMINFO ahdr;
    AVICOMPRESSOPTIONS opts;
    AVICOMPRESSOPTIONS * aopts[ 1 ];
    WAVEFORMATEX wfx;
    HRESULT hr;

    wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nChannels = 1;
    wfx.nSamplesPerSec =
    wfx.nAvgBytesPerSec = SndFreqReplay;
    wfx.nBlockAlign = 1;
    wfx.wBitsPerSample = 8;
    wfx.cbSize = 0;

    memset( &ahdr, 0, sizeof( ahdr ) );
    ahdr.fccType = streamtypeAUDIO;
    ahdr.dwScale = wfx.nBlockAlign;
    ahdr.dwRate = wfx.nSamplesPerSec * wfx.nBlockAlign;
    ahdr.dwSampleSize = wfx.nBlockAlign;
    ahdr.dwQuality = ( DWORD )-1;

    memset( &BmInfoHeader, 0, sizeof( BmInfoHeader ) );
    memset( &opts, 0, sizeof( opts ) );
    memset( &strhdr, 0, sizeof( strhdr ) );

//    opts.
    aopts[ 0 ] = &opts;
    BmInfoHeader.biSize = sizeof( BmInfoHeader );
    BmInfoHeader.biWidth = TailleX;
    BmInfoHeader.biHeight = TailleY;
    BmInfoHeader.biPlanes = 1;
    BmInfoHeader.biSizeImage = TailleX * TailleY * 4;
    BmInfoHeader.biBitCount = 32;
    BmInfoHeader.biCompression = BI_RGB;
    BmInfoHeader.biClrUsed = 0;
    BmInfoHeader.biClrImportant = 0;

    NumFrame = 0;
    NumSample = 0;
    AVIFileInit();
    if ( ! AVIFileOpen( &pfile, Nom, OF_WRITE | OF_CREATE, NULL ) )
        {
        strhdr.fccType = streamtypeVIDEO;
        strhdr.dwScale = 1;
        strhdr.dwRate = FPS;
        SetRect( &strhdr.rcFrame, 0, 0, TailleX, TailleY );
        if ( ! AVIFileCreateStream( pfile, &ps, &strhdr ) )
            {
            if ( AVISaveOptions( hWnd
                               , ICMF_CHOOSE_KEYFRAME
                               , 1
                               , &ps
                               , ( LPAVICOMPRESSOPTIONS * )&aopts
                               )
               )
                {
                if ( ! AVIMakeCompressedStream( &psCmp, ps, &opts, NULL ) )
                    {
                    if ( ! AVIStreamSetFormat( psCmp
                                             , 0
                                             , &BmInfoHeader
                                             , BmInfoHeader.biSize
                                             ) 
                       )
                        {
                        hr = AVIFileCreateStream( pfile, &as, &ahdr );
                        if ( hr == AVIERR_OK )
                            {
                            hr = AVIStreamSetFormat( as
                                                   , 0
                                                   , &wfx
                                                   , sizeof( WAVEFORMATEX )
                                                   );
                            if ( hr == AVIERR_OK )
                                {
                                AviFileStarted = TRUE;
                                return( TRUE );
                                }
                            else
                                Erreur( "Erreur 1 " );
                            }
                        else
                            Erreur( "Erreur 2 " );
                        }
                    else
                        Erreur( "Format d'image incompatible avec ce codec." );
                    }
                else
                    Erreur( "Erreur WriteAvi #3" );
                }
            else
                {
                // Annulation type de compression par l'utilisateur
                }
            }
        else
            Erreur( "Erreur WriteAvi #1" );
        }
    else
        Log( MODULENAME, "Impossible de créer le fichier %s", Nom, LOG_ERROR );

    WriteAviEnd( hWnd );
    return( FALSE );
}


/********************************************************* !NAME! **************
* Nom : WriteAviFrame
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\\/FLE!, ligne : !./LN!
*
* Description : Ecrit une Frame (image) dans le fichier AVI
*
* Résultat    : TRUE si ok, FALSE sinon
*
* Variables globales modifiées : NumFrame
*
********************************************************** !0! ****************/
BOOL WriteAviFrame( void )
{
    if ( AVIStreamWrite( psCmp
                       , NumFrame++
                       , 1
                       , MemBitmap
                       , BmInfoHeader.biSizeImage
                       , ( NumFrame & 15 ) ? 0 : AVIIF_KEYFRAME
                       , NULL
                       , NULL
                       ) == AVIERR_OK
       )
        return( TRUE );

    return( FALSE );
}


/********************************************************* !NAME! **************
* Nom : WriteAviSound
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\\/FLE!, ligne : !./LN!
*
* Description : Ecrit d'un sample dans le fichier AVI
*
* Résultat    : TRUE si ok, FALSE sinon
*
* Variables globales modifiées : NumSample
*
********************************************************** !0! ****************/
BOOL WriteAviSound( UBYTE * Buff, int Longueur )
{
    if ( AVIStreamWrite( as
                       , NumSample
                       , Longueur
                       , Buff
                       , Longueur
                       , 0
                       , NULL
                       , NULL
                       ) == AVIERR_OK
       )
        {
        NumSample += Longueur;
        return( TRUE );
        }
    return( FALSE );
}


/********************************************************* !NAME! **************
* Nom : WriteAviEnd
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\\/FLE!, ligne : !./LN!
*
* Description : Termine le fichier AVI
*
* Résultat    : /
*
* Variables globales modifiées : pfile, ps, psCmp
*
********************************************************** !0! ****************/
void WriteAviEnd( HWND hWnd )
{
    static char Tmp[ 128 ];

    AviFileStarted = FALSE;
    if ( NumFrame )
        {
        wsprintf( Tmp
                , "Sauvegarde Avi terminée. Nombre d'images écrites : %d"
                , NumFrame
                );
        MessageBox( hWnd, Tmp, "Sauvegarde Avi", MB_OK );
        }
    if ( ps )
        {
        AVIStreamClose( ps );
        ps = NULL;
        }
    if ( psCmp )
        {
        AVIStreamClose( psCmp );
        psCmp = NULL;
        }
    if ( as )
        {
        AVIStreamClose( as );
        as = NULL;
        }
    if ( pfile )
        {
        AVIFileClose( pfile );
        pfile = NULL;
        }
    AVIFileExit();
}


/********************************************************* !NAME! **************
* Nom : WriteAviStarted
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\\/FLE!, ligne : !./LN!
*
* Description : Indique si un fichier AVI est en cours de traitement
*
* Résultat    : TRUE si en cours, FALSE sinon
*
* Variables globales modifiées : 
*
********************************************************** !0! ****************/
BOOL WriteAviStarted( void )
{
    return( AviFileStarted );
}
