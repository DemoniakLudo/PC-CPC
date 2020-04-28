/******************************************************************************/
/* Configuration pour l'archivage des différents éléments du fichier source   */
/******************************************************************************/
// !CONFIG!=/L/* /R/* /W"* Nom : "
// Définition du système       !CONFIG!=/V1!EMULATEUR CPC!
// Définition du sous système  !CONFIG!=/V2!PC-CPC!
// Définition du sous ensemble !CONFIG!=/V3!Accessoires!
// Définition du module        !CONFIG!=/V4!Emulation Disque!
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
*  Fichier     : NEWDISC.C             | Version : 0.1al
* ------------------------------------------------------------------------------
*  Date        : 21/01/2004            | Auteur  : L.DEPLANQUE
* ------------------------------------------------------------------------------
*  Description : Emulation acces direct au disque
*
* ------------------------------------------------------------------------------
*  Historique  :
*           Date           |         Auteur          |       Description
* ------------------------------------------------------------------------------
*  21/01/2004              | L.DEPLANQUE             | creation
* ------------------------------------------------------------------------------
*  29/04/2006              | L.DEPLANQUE             | Version 0.1al :
*                          |                         | Correction d'un bugg dans
*                          |                         | la fonction "cat" :
*                          |                         | les répertoires étaient
*                          |                         | aussi affichés
* ------------------------------------------------------------------------------
********************************************************** !END! **************/


#include  "types.h"

#include  <windows.h>
#include  <string.h>
#include  <stdio.h>

#include  "Z80.h"
#include  "Vga.h"
#include  "NewDisc.h"
#include  "Config.h"
#include  "SnapShot.h"
#include  "Log.h"


#define     MODULENAME      "NewDisc"


//
// Taille du buffer "tampon" de lecture de fichiers
//
#define SIZE_COPY_BUFF  0x100


/********************************************************* !NAME! **************
* Nom : Entete
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : En-tête au format AMSDOS
*
********************************************************** !0! ****************/
static StAmsdos Entete;


/********************************************************* !NAME! **************
* Nom : fOut
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Handle du fichier de sortie
*
********************************************************** !0! ****************/
static HANDLE fOut = NULL;


/********************************************************* !NAME! **************
* Nom : fIn
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Handle du fichier d'entrée
*
********************************************************** !0! ****************/
static HANDLE fIn = NULL;


/********************************************************* !NAME! **************
* Nom : NomFic
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Nom du fichier à ouvrir
*
********************************************************** !0! ****************/
static char NomFic[ 128 ];


/********************************************************* !NAME! **************
* Nom : MemoOut
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Mémorisation fichier de sortie ouvert
*
********************************************************** !0! ****************/
static int MemoOut = 0;


/********************************************************* !NAME! **************
* Nom : MemoIn
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Mémorisation fichier d'entrée ouvert
*
********************************************************** !0! ****************/
static int MemoIn = 0;


/********************************************************* !NAME! **************
* Nom : CopyMemDirect
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Buffer utilisé pour les fonctions de lecture/écritures directes
*
********************************************************** !0! ****************/
static UBYTE CopyMemDirect[ 0x10000 ];


/********************************************************* !NAME! **************
* Nom : CopyMem
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Buffer utilisé pour les fonctions de lecture/écriture d'octets
*
********************************************************** !0! ****************/
static UBYTE CopyMem[ SIZE_COPY_BUFF ];


/********************************************************* !NAME! **************
* Nom : PosCopyMem
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Position dans le buffer de lecture/écriture d'octets
*
********************************************************** !0! ****************/
static int PosCopyMem = 0;


/********************************************************* !NAME! **************
* Nom : MaxCopyMem
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Taille du buffer de lecture/écriture d'octets

*
********************************************************** !0! ****************/
static int MaxCopyMem = 0;


/********************************************************* !NAME! **************
* Nom : BytesWritten
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Nombre d'octets écris dans le fichier de sortie

*
********************************************************** !0! ****************/
static DWORD BytesWritten;


/********************************************************* !NAME! **************
* Nom : BytesRead
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Nombre d'octets lus depuis le fichier d'entrée

*
********************************************************** !0! ****************/
static DWORD BytesRead;


/********************************************************* !NAME! **************
* Nom : CasInOpen
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Emule la routine CAS_IN_OPEN (0xBC77)
*
* Résultat    : /
*
* Variables globales modifiées : Cpc, fIn, MemoIn, NomFic, PosCopyMem, 
*                                MaxCopyMem
*
********************************************************** !0! ****************/
static void CasInOpen( void )
{
    static char NomTmp[ 1024 ];
    int i, Checksum = 0;
    UBYTE * pEntete;

    FLAGS &= ! FLAG_Z & ! FLAG_C;
    if ( MemoIn )
        RegA = 1;   // Code Erreur
    else
        {
        memset( NomFic, 0, sizeof( NomFic ) );
        for ( i = 0; i < RegB; i++ )
            NomFic[ i ] = ( char )toupper( Peek8Ext( ( USHORT )( RegHL + i ) ) );

        sprintf( NomTmp, "%s\\%s", DirEmuDisc, NomFic );
        fIn = CreateFile( NomTmp
                        , GENERIC_READ
                        , 0
                        , NULL
                        , OPEN_EXISTING
                        , 0
                        , NULL
                        );
        if ( fIn == INVALID_HANDLE_VALUE )
            {
            if ( ! strchr( NomFic, '.' ) )
                {
                sprintf( NomTmp, "%s\\%s.bas", DirEmuDisc, NomFic );
                fIn = CreateFile( NomTmp
                                , GENERIC_READ
                                , 0
                                , NULL
                                , OPEN_EXISTING
                                , 0
                                , NULL
                                );
                if ( fIn == INVALID_HANDLE_VALUE )
                    {
                    sprintf( NomTmp, "%s\\%s.bin", DirEmuDisc, NomFic );
                    fIn = CreateFile( NomTmp
                                    , GENERIC_READ
                                    , 0
                                    , NULL
                                    , OPEN_EXISTING
                                    , 0
                                    , NULL
                                    );
                    }
                }
            }
        if ( fIn )
            {
            Log( MODULENAME, "Ouverture en lecture fichier %s réussie." , NomTmp, LOG_DEBUG );
            ReadFile( fIn, &Entete, sizeof( Entete ), &BytesRead, NULL );
            pEntete = ( UBYTE * )&Entete;
            for ( i = 0; i < 67; i++ )
                Checksum += * pEntete++;

            if ( ! Entete.Length )
                Entete.Length = Entete.LogicalLength;

            Log( MODULENAME, "Entete.FileType      = 0x%04X", Entete.FileType, LOG_DEBUG );
            Log( MODULENAME, "Entete.Length        = 0x%04X", Entete.Length, LOG_DEBUG );
            Log( MODULENAME, "Entete.Adress        = 0x%04X", Entete.Adress, LOG_DEBUG );
            Log( MODULENAME, "Entete.LogicalLength = 0x%04X", Entete.LogicalLength, LOG_DEBUG );
            Log( MODULENAME, "Entete.EntryAdress   = 0x%04X", Entete.EntryAdress, LOG_DEBUG );
            Log( MODULENAME, "Entete.RealLehgth    = 0x%04X", Entete.RealLength, LOG_DEBUG );

            RegHL = ( USHORT )( Peek8Ext( 0xBE7D ) + ( Peek8Ext( 0xBE7E ) << 8 ) );
            FLAGS |= FLAG_C;

            Poke8Ext( ( USHORT )( RegHL + 0x51 ), RegE );
            Poke8Ext( ( USHORT )( RegHL + 0x52 ), RegD );
            pEntete = ( UBYTE * )&Entete;
            for ( i = 0; i < 0x45; i++ )
                {
                Poke8Ext( ( USHORT )( RegHL + i + 0x55 ), pEntete[ i ] );
                Poke8Ext( ( USHORT )( RegHL + i + 0x9F ), pEntete[ i ] );
                Poke8Ext( ( USHORT )( RegHL + i + 0x2B0 ), pEntete[ i ] );
                }
            RegHL += 0x55;

            if ( Checksum == Entete.CheckSum )
                {
                RegA = Entete.FileType;
                RegBC = Entete.LogicalLength;
                RegDE = Entete.Adress;
                }
            else
                {
                RegA = 0x16;
                RegBC = 0;
                RegDE = 0;
                SetFilePointer( fIn, 0, NULL, FILE_BEGIN );
                }
            MemoIn = 1;
            PosCopyMem = MaxCopyMem = -1;
            }
        else
            {
            Log( MODULENAME, "Ouverture en lecture fichier %s échouée." , NomTmp, LOG_DEBUG );
            RegHL = 0;
            RegDE = 0;
            RegBC = 0;
            RegA = 0;
            }
        }

    // Effectuer un "RET"
    RET();
}


/********************************************************* !NAME! **************
* Nom : FlushCloseIn
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Ferme le fichier d'entrée
*
* Résultat    : /
*
* Variables globales modifiées : MemoIn, fIn
*
********************************************************** !0! ****************/
void FlushCloseIn( void )
{
    if ( MemoIn )
        {
        if ( fIn )
            CloseHandle( fIn );

        MemoIn = 0;
        fIn = INVALID_HANDLE_VALUE;
        }
}


/********************************************************* !NAME! **************
* Nom : CasInClose
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Emule la routine CAS_IN_CLOSE (0xBC7A)
*
* Résultat    : /
*
* Variables globales modifiées : Cpc
*
********************************************************** !0! ****************/
static void CasInClose( void )
{
    FLAGS &= ! FLAG_Z & ! FLAG_C;
    if ( MemoIn )
        {
        FlushCloseIn();
        FLAGS |= FLAG_C;
        RegA = 0;
        Log( MODULENAME, "Fermeture fichier d'entrée.", LOG_DEBUG );
        }

    // Effectuer un "RET"
    RET();
}


/********************************************************* !NAME! **************
* Nom : CasInAbandon
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Emule la routine CAS_IN_ABANDON (0xBC7D)
*
* Résultat    : /
*
* Variables globales modifiées : /
*
********************************************************** !0! ****************/
static void CasInAbandon( void )
{
    Log( MODULENAME, "Abandon fichier d'entrée.", LOG_DEBUG );
    CasInClose();
}


/********************************************************* !NAME! **************
* Nom : CasInChar
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Emule la routine CAS_IN_CHAR (0xBC80)
*
* Résultat    : /
*
* Variables globales modifiées : Cpc, MaxCopyMem, PosCopyMem
*
********************************************************** !0! ****************/
static void CasInChar( void )
{
    FLAGS &= ! FLAG_Z & ! FLAG_C;
    if ( MemoIn )
        {
        if ( PosCopyMem == MaxCopyMem )
            {
            ReadFile( fIn, CopyMem, SIZE_COPY_BUFF, ( DWORD * )&MaxCopyMem, NULL );
            PosCopyMem = 0;
            }
        if ( MaxCopyMem )
            {
            RegA = CopyMem[ PosCopyMem++ ];
            FLAGS |= FLAG_C;
            }
        }

    // Effectuer un "RET"
    RET();
}


/********************************************************* !NAME! **************
* Nom : CasInDirect
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Emule la routine CAS_IN_DIRECT (0xBC83)
*
* Résultat    : /
*
* Variables globales modifiées : Cpc
*
********************************************************** !0! ****************/
static void CasInDirect( void )
{
    int i, lg;

/*
    // Désélectionner les Roms pour ne pas écrire dedans...
    SveDecodeur = DecodeurAdresse;
    WriteVGA( DecodeurAdresse | ROMINF_OFF | ROMSUP_OFF );
*/
    FLAGS &= ! FLAG_Z & ! FLAG_C;
    if ( fIn )
        {
        ReadFile( fIn, CopyMemDirect, sizeof( CopyMemDirect ), ( DWORD * )&lg, NULL );
        for ( i = 0; i < Entete.Length; i++ )
            Poke8Ext( ( USHORT )( RegHL + i ), CopyMemDirect[ i ] );

        RegA = 0xFF;
        FLAGS |= FLAG_C;
        RegHL = Entete.EntryAdress;
        Log( MODULENAME, "Longueur lue         = 0x%04X", lg, LOG_DEBUG );
        Log( MODULENAME, "HL                   = 0x%04X", RegHL, LOG_DEBUG );
        }
    else
        RegA = 1;

    // Effectuer un "RET"
    RET();
}


/********************************************************* !NAME! **************
* Nom : CasReturn
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Emule la routine CAS_RETURN (0xBC86)
*
* Résultat    : /
*
* Variables globales modifiées : Cpc, PosCopyMem
*
********************************************************** !0! ****************/
static void CasReturn( void )
{
    if ( PosCopyMem > 0 )
        PosCopyMem--;

    // Effectuer un "RET"
    RET();
}


/********************************************************* !NAME! **************
* Nom : CasTestEof
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Emule la routine CAS_TEST_EOF (0xBC89)
*
* Résultat    : /
*
* Variables globales modifiées : Cpc
*
********************************************************** !0! ****************/
static void CasTestEof( void )
{
    if ( ! MaxCopyMem )
        FLAGS = ( UBYTE )( FLAGS & ! FLAG_Z & ! FLAG_C );
    else
        FLAGS = ( UBYTE )( FLAGS & ! FLAG_Z | FLAG_C );

    // Effectuer un "RET"
    RET();
}


/********************************************************* !NAME! **************
* Nom : CasOutOpen
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Emule la routine CAS_OUT_OPEN (0xBC8C)
*
* Résultat    : /
*
* Variables globales modifiées : Cpc, NomFic, PosCopyMem, MaxCopyMem
*
********************************************************** !0! ****************/
static void CasOutOpen( void )
{
    int i;

    FLAGS &= ! FLAG_Z & ! FLAG_C;
    if ( MemoOut )
        RegA = 1;   // Code Erreur
    else
        {
        memset( NomFic, 0, sizeof( NomFic ) );
        for ( i = 0; i < RegB; i++ )
            NomFic[ i ] = ( char )toupper( Peek8Ext( ( USHORT )( RegHL + i ) ) );

        memcpy( Entete.FileName, NomFic, RegB );

        FLAGS |= FLAG_C;
        RegHL = RegDE;
        RegA = 0;
        MemoOut = 1;
        PosCopyMem = MaxCopyMem = -1;
        }
    // Effectuer un "RET"
    RET();
}


/********************************************************* !NAME! **************
* Nom : FlushCloseOut
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Ferme et "flush" le fichier de sortie
*
* Résultat    : /
*
* Variables globales modifiées : fOut, MemoOut, PosCopyMem
*
********************************************************** !0! ****************/
void FlushCloseOut( void )
{
    if ( MemoOut )
        {
        if ( fOut )
            {
            if ( PosCopyMem > 0 )
                WriteFile( fOut, CopyMem, PosCopyMem, &BytesWritten, NULL );

            CloseHandle( fOut );
            }
        MemoOut = 0;
        fOut = INVALID_HANDLE_VALUE;
        }
}


/********************************************************* !NAME! **************
* Nom : CasOutClose
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Emule la routine CAS_OUT_CLOSE (0xBC8F)
*
* Résultat    : /
*
* Variables globales modifiées : Cpc
*
********************************************************** !0! ****************/
static void CasOutClose( void )
{
    FLAGS &= ! FLAG_Z & ! FLAG_C;
    if ( MemoOut )
        {
        FlushCloseOut();
        FLAGS |= FLAG_C;
        RegA = 0;
        Log( MODULENAME, "Fermeture fichier de sortie.", LOG_DEBUG );
        }
    else
        RegA = 2;   // Code Erreur

    // Effectuer un "RET"
    RET();
}


/********************************************************* !NAME! **************
* Nom : CasOutAbandon
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Emule la routine CAS_OUT_ABANDON  (0xBC92)
*
* Résultat    : /
*
* Variables globales modifiées : /
*
********************************************************** !0! ****************/
static void CasOutAbandon( void )
{
    Log( MODULENAME, "Abandon fichier de sortie.", LOG_DEBUG );
    CasOutClose();
}


/********************************************************* !NAME! **************
* Nom : OpenWriteFile
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Ouvre le fichier de sortie, en créant éventuellement un fichier
*               .BAK si il existe un fichier avec le même nom.
*
* Résultat    : /
*
* Variables globales modifiées : NomFic, fOut
*
********************************************************** !0! ****************/
static void OpenWriteFile( int FileType )
{
    static char NewName[ 1024 ];
    static char NomTmp[ 1024 ];
    char * p;

    if ( ! strchr( NomFic, '.' ) && FileType != -1 )
        {
        if ( FileType & 2 )
            strcat( NomFic, ".BIN" );
        else
            strcat( NomFic, ".BAS" );
        }
    sprintf( NewName, "%s\\%s", DirEmuDisc, NomFic );
    fOut = CreateFile( NewName
                     , GENERIC_READ
                     , 0
                     , NULL
                     , OPEN_EXISTING
                     , 0
                     , NULL
                     );
    if ( fOut == INVALID_HANDLE_VALUE )
        {
        fOut = CreateFile( NewName
                         , GENERIC_READ | GENERIC_WRITE
                         , 0
                         , NULL
                         , CREATE_ALWAYS
                         , 0
                         , NULL
                         );
        Log( MODULENAME
           , "Ouverture en écriture fichier %s %s."
           , NewName
           , fOut != INVALID_HANDLE_VALUE ? "réussie" : "échouée", LOG_DEBUG
           );
        }
    else
        {
        CloseHandle( fOut );
        sprintf( NewName, "%s\\%s", DirEmuDisc, NomFic );
        strcpy( NomTmp, NewName );
        p = strchr( NewName, '.' );
        if ( p )
            strcpy( p, ".BAK" );
        else
            strcat( NewName, ".BAK" );

        remove( NewName );
        rename( NomTmp, NewName );
        fOut = CreateFile( NomTmp
                         , GENERIC_READ | GENERIC_WRITE
                         , 0
                         , NULL
                         , CREATE_ALWAYS
                         , 0
                         , NULL
                         );
        Log( MODULENAME
           , "Ouverture en écriture fichier %s %s."
           , NomTmp
           , fOut != INVALID_HANDLE_VALUE ? "réussie" : "échouée", LOG_DEBUG
           );
        }
}


/********************************************************* !NAME! **************
* Nom : CasOutChar
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Emule la routine CAS_OUT_CHAR (0xBC95)
*
* Résultat    : /
*
* Variables globales modifiées : Cpc, PosCopyMem, MaCopyMem
*
********************************************************** !0! ****************/
static void CasOutChar( void )
{
    FLAGS &= ! FLAG_Z & ! FLAG_C;
    if ( fOut == INVALID_HANDLE_VALUE )
        {
        OpenWriteFile( -1 );
        MaxCopyMem = SIZE_COPY_BUFF;
        PosCopyMem = 0;
        }
    if ( fOut != INVALID_HANDLE_VALUE )
        {
        if ( PosCopyMem < MaxCopyMem )
            CopyMem[ PosCopyMem++ ] = RegA;
        else
            {
            WriteFile( fOut, CopyMem, PosCopyMem, &BytesWritten, NULL );
            PosCopyMem = 0;
            CopyMem[ PosCopyMem++ ] = RegA;
            }
        FLAGS |= FLAG_C;
        }
    // Effectuer un "RET"
    RET();
}


/********************************************************* !NAME! **************
* Nom : CasOutDirect
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Emule la routine CAS_OUT_DIRECT (0xBC98)
*
* Résultat    : /
*
* Variables globales modifiées : Cpc
*
********************************************************** !0! ****************/
static void CasOutDirect( void )
{
    int i, SveDecodeur, Checksum = 0;
    UBYTE * pEntete;
    
    pEntete = ( UBYTE * )&Entete;
    Entete.Adress = RegHL;
    Entete.Length = RegDE;
    Entete.RealLength = RegDE;
    Entete.LogicalLength = RegDE;
    Entete.FileType = RegA;
    Entete.EntryAdress = RegBC;

    for ( i = 0; i < 67; i++ )
        Checksum += * pEntete++;

    Entete.CheckSum = ( USHORT )Checksum;


    // Désélectionner la Rom supérieure pour ne pas lire dedans...
    SveDecodeur = Cpc.VGARom;
    WriteVGA( Cpc.VGARom | ROMSUP_OFF );

    FLAGS &= ! FLAG_Z & ! FLAG_C;
    OpenWriteFile( Entete.FileType );
    if ( fOut != INVALID_HANDLE_VALUE )
        {
        WriteFile( fOut, &Entete, sizeof( Entete ), &BytesWritten, NULL );
        for ( i = 0; i < RegDE; i++ )
            CopyMemDirect[ i ] = ( UBYTE )Peek8Ext( ( USHORT )( RegHL + i ) );

        WriteFile( fOut, CopyMemDirect, RegDE, &BytesWritten, NULL );
        RegA = 0;
        FLAGS |= FLAG_C;
        }
    else
        RegA = 0x0E;

    // Repositionner les roms
    WriteVGA( SveDecodeur );

    // Effectuer un "RET"
    RET();
}


/********************************************************* !NAME! **************
* Nom : CasCatalog
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Emule la routine CAS_CATALOG (0xBC9B)
*
* Résultat    : /
*
* Variables globales modifiées : Cpc
*
********************************************************** !0! ****************/
static void CasCatalog( void )
{
    static char Recherche[ 1024 ];
    static UBYTE AffDir[] = 
        {
        0x7E, 0xFE, 0xFF, 0x37, 0xC0, 0xE5, 0xE5, 0x54, 
        0x5D, 0x06, 0x08, 0x13, 0x1A, 0xE6, 0x7F, 0xCD, 
        0x5A, 0xBB, 0x10, 0xF7, 0x3E, 0x2E, 0xCD, 0x5A, 
        0xBB, 0x06, 0x03, 0x13, 0x1A, 0xE6, 0x7F, 0xCD, 
        0x5A, 0xBB, 0x10, 0xF7, 0x3E, 0x20, 0xCD, 0x5A, 
        0xBB, 0xDD, 0xE1, 0xDD, 0x66, 0x0D, 0xDD, 0x6E, 
        0x0C, 0x16, 0x20, 0x01, 0x9C, 0xFF, 0x3E, 0xFF, 
        0xE5, 0x3C, 0x09, 0x30, 0x04, 0xE3, 0xE1, 0x18, 
        0xF7, 0xE1, 0xB7, 0x28, 0x02, 0x16, 0x30, 0x82, 
        0xCD, 0x5A, 0xBB, 0x01, 0xF6, 0xFF, 0x3E, 0xFF, 
        0xE5, 0x3C, 0x09, 0x30, 0x04, 0xE3, 0xE1, 0x18, 
        0xF7, 0xE1, 0xB7, 0x28, 0x02, 0x16, 0x30, 0x82, 
        0xCD, 0x5A, 0xBB, 0x7D, 0xC6, 0x30, 0xCD, 0x5A, 
        0xBB, 0x3E, 0x4B, 0xCD, 0x5D, 0xBB, 0x3E, 0x20, 
        0xCD, 0x5A, 0xBB, 0xCD, 0x5A, 0xBB, 0xCD, 0x5A, 
        0xBB, 0xE1, 0x11, 0x0E, 0x00, 0x19, 0x18, 0x80
        };

    USHORT adr;
    int i, l, cnt = 0;
    char * Nom;
    char * Ext;
    BOOL Cont;
    WIN32_FIND_DATA FindData;
    HANDLE h;

    RegHL = RegDE;
    adr = ( USHORT )( RegDE + 0x780 );
    RegPC = adr;
    for ( i = 0; i < sizeof( AffDir ); i++ )
        Poke8Ext( adr++, AffDir[ i ] );

    adr = RegDE;
    Poke8Ext( adr, 0 );
    sprintf( Recherche, "%s\\*.*", DirEmuDisc );
    h = FindFirstFile( Recherche, &FindData );
    if ( h != INVALID_HANDLE_VALUE )
        {
        do
            {
            if ( ! ( FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) )
                {
                Nom = FindData.cFileName;
                if ( Nom )
                    {
                    char * p = strchr( Nom, '.' );
                    if ( p )
                        {
                        * p++ = 0;
                        Ext = p;
                        }
                    else
                        Ext = "   ";

                    l = ( int )strlen( Nom );
                    Poke8Ext( adr++, 0xFF );
                    for ( i = 0; i < 8; i++ )
                        Poke8Ext( adr++, ( UBYTE )( i < l ? Nom[ i ] : ' ' ) );

                    l = ( int )strlen( Ext );
                    for ( i = 0; i < 3; i++ )
                        Poke8Ext( adr++, ( UBYTE )( i < l ? Ext[ i ] : ' ' ) );

                    l = ( FindData.nFileSizeLow + 1023 ) >> 10;
                    Poke8Ext( adr++, ( UBYTE )l );
                    Poke8Ext( adr++, ( UBYTE )( l >> 8 ) );
                    }
                }
            Cont = FindNextFile( h, &FindData );
            if ( cnt++ > 128 )  // Ne pas dépasser les 2K de buffers...
                Cont = 0;
            }
        while( Cont );
        FindClose( h );
        Poke8Ext( adr, 0 );
        }
}


/********************************************************* !NAME! **************
* Nom : NewDiscAdrPC
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Détourne les appels aux routines CAS/Amsdos
*
* Résultat    : /
*
* Variables globales modifiées : /
*
********************************************************** !0! ****************/
void NewDiscAdrPC( void )
{
    switch( RegPC )
        {
        case 0xBC77 :
            CasInOpen();
            break;

        case 0xBC7A :
            CasInClose();
            break;

        case 0xBC7D :
            CasInAbandon();
            break;

        case 0xBC80 :
            CasInChar();
            break;

        case 0xBC83 :
            CasInDirect();
            break;

        case 0xBC86 :
            CasReturn();
            break;

        case 0xBC89 :
            CasTestEof();
            break;

        case 0xBC8C :
            CasOutOpen();
            break;

        case 0xBC8F :
            CasOutClose();
            break;

        case 0xBC92 :
            CasOutAbandon();
            break;

        case 0xBC95 :
            CasOutChar();
            break;

        case 0xBC98 :
            CasOutDirect();
            break;

        case 0xBC9B :
            CasCatalog();
            break;
        }
}
