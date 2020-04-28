/******************************************************************************/
/* Configuration pour l'archivage des différents éléments du fichier source   */
/******************************************************************************/
// !CONFIG!=/L/* /R/* /W"* Nom : "
// Définition du système       !CONFIG!=/V1!EMULATEUR CPC!
// Définition du sous système  !CONFIG!=/V2!PC-CPC!
// Définition du sous ensemble !CONFIG!=/V3!Accessoires!
// Définition du module        !CONFIG!=/V4!WriteYM!
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
*  Fichier     : WRITEYM.C             | Version : 0.1al
* ------------------------------------------------------------------------------
*  Date        : 20/07/2006            | Auteur  : L.DEPLANQUE
* ------------------------------------------------------------------------------
*  Description : Ecriture d'un fichier son au format YM (5)
*
* ------------------------------------------------------------------------------
*  Historique  :
*           Date           |         Auteur          |       Description
* ------------------------------------------------------------------------------
*  20/07/2006              | L.DEPLANQUE             | creation
* ------------------------------------------------------------------------------
********************************************************** !END! **************/


#include  "Types.h"

#include  <windows.h>
#include  <stdio.h>

#include  "Log.h"
#include  "AY8912.h"
#include  "WriteYM.h"
#include  "Plateform.h"
#include  "SnapShot.h"


#define     MODULENAME      "WRITEYM"


#define     MAX_YM_TRAMES       0x10000     // 22 minutes max.


/********************************************************* !NAME! **************
* Nom : YMEntrelace
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Indique si les Trames YM doivent être entrelacées ou non
*
********************************************************** !0! ****************/
static UBYTE YMEntrelace = 1;


/********************************************************* !NAME! **************
* Nom : NumTrame
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Numéro de trame en cours d'écriture
*
********************************************************** !0! ****************/
static int NumTrame = 0;


/********************************************************* !NAME! **************
* Nom : NumTrame
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Nombre max. de trames YM à écrire
*
********************************************************** !0! ****************/
static int MaxTrames = MAX_YM_TRAMES;


/********************************************************* !NAME! **************
* Nom : TabTrameReg
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Tableau contenant les registres PSG pour chaque "trame" audio
*
********************************************************** !0! ****************/
static UBYTE TabTrameReg[ MAX_YM_TRAMES ][ 16 ];


/********************************************************* !NAME! **************
* Nom : YMFileStarted
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Indique si l'écriture est démarrée ou non
*
********************************************************** !0! ****************/
static BOOL YMFileStarted = FALSE;


/********************************************************* !NAME! **************
* Nom : FirstWrite
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Indique première écriture
*
********************************************************** !0! ****************/
static BOOL FirstWrite;


/********************************************************* !NAME! **************
* Nom : FileYM
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Handle du fichier YM
*
********************************************************** !0! ****************/
static FILE * FileYM = NULL;


/********************************************************* !NAME! **************
* Nom : WriteYMInit
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\\/FLE!, ligne : !./LN!
*
* Description : Initialisation (création) du fichier YM
*
* Résultat    : TRUE si ok, FALSE sinon
*
* Variables globales modifiées : 
*
********************************************************** !0! ****************/
BOOL WriteYMInit( HWND hWnd, char * Nom, int l )
{
    FileYM = fopen( Nom, "wb+ " );
    if ( FileYM )
        {
        YMFileStarted = TRUE;
        NumTrame = 0;
        MaxTrames = l > 0 ? l : MAX_YM_TRAMES;
        FirstWrite = TRUE;
        return( TRUE );
        }
    else
        Log( MODULENAME, "Impossible de créer le fichier %s", Nom, LOG_ERROR );

    WriteYMEnd( hWnd );
    return( FALSE );
}


/********************************************************* !NAME! **************
* Nom : WriteYMFrame
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\\/FLE!, ligne : !./LN!
*
* Description : Ecrit une Frame (Registres AY) dans le fichier YM
*
* Résultat    : TRUE si ok, FALSE sinon
*
* Variables globales modifiées : NumTrame
*
********************************************************** !0! ****************/
BOOL WriteYMFrame( void )
{
    static int LastRegsPSG[ 16 ];

    BOOL Diff = ( NumTrame > 0 );

    if ( FirstWrite )
        {
        memcpy( LastRegsPSG, Cpc.PsgData, sizeof( LastRegsPSG ) );
        FirstWrite = FALSE;
        }

    if ( WriteYMStarted() )
        {
        if ( NumTrame < MaxTrames )
            {
            if ( ! Diff )
                {
                for ( int i = 0; i < 14; i++ )
                    if ( LastRegsPSG[ i ] != Cpc.PsgData[ i ] )
                        {
                        LastRegsPSG[ i ] = Cpc.PsgData[ i ];
                        Diff = TRUE;
                        }
                }
            if ( Diff )
                {
                for ( int i = 0; i < 13; i++ )
                    TabTrameReg[ NumTrame ][ i ] = ( UBYTE )Cpc.PsgData[ i ];

                // Si le registre 13 n'a pas changé, on écrit 0xFF
                TabTrameReg[ NumTrame ][ 13 ] = ( UBYTE )( IsWritePSGReg13() ?
                                                            Cpc.PsgData[ 13 ] :
                                                            0xFF 
                                                        );
                NumTrame++;
                }
            }
        else
            WriteYMEnd( hWnd );

        return( TRUE );
        }
    return( FALSE );
}


#ifdef USE_AYC
/**
 *
 * Process AYC crunching :
 * @param intBuffSize int defining the Compression Buffer Size (either 0x100 or 0x400)
 * @return Crunched array of Bytes
 */
static UBYTE* doAYCCrunch( UBYTE r, USHORT intRawLength, USHORT intBuffSize, USHORT* intCrunLen)
{
    // AYC crunching is as follow :
    // - A sequence is
    //        + A character
    //        + A lenght + offset
    // - First Byte is a character
    // - Flags are inputed every 8 sequences and bits represents the following sequences, ie : b00000111 means:
    //        + 5 characters
    //        + 3 previous sequences

    static UBYTE arrCrunchedData[ 0x10000 ];
    USHORT intCrunchedLength = 0;

    /**
     * Minimum interresting size for a previous Pattern
     * For BuffSize = 0x100, it's interresting only if the previous matching pattern size is > 1
     * offset + length + 1 bit = 17 bits > 1 char + 1 bit = 9 bits
     * offset + length + 1 bit = 17 bits < 2 chars + 2 bits = 18 bits
     * For BuffSize = 0x400, it's interresting only if the previous matching pattern size is > 2
     * offset + length + 1 bit = 25 bits > 1 char + 1 bit = 9 bits
     * offset + length + 1 bit = 25 bits > 2 chars + 2 bits = 18 bits
     * offset + length + 1 bit = 25 bits < 3 chars + 3 bits = 27 bits
     */ 
    int intMinSeqSize = 1;
    if (intBuffSize > 0x100) intMinSeqSize++;

    // First Byte
    arrCrunchedData[intCrunchedLength++] = TabTrameReg[0][r];

    // Reserve flag place 
    int intOffsetFlag = intCrunchedLength++;

    // Main Crunching Loop
    int curr_index = 1;
    UBYTE bsFlag = 0x00;
    int intSeqCounter = 8;
    
    while(curr_index<intRawLength)
    {
        // Put flag if needed
        if (intSeqCounter <= 0)
        {
            // Reset number of sequences
            intSeqCounter = 8;

            // Insert Flag in flow
            arrCrunchedData[intOffsetFlag] = bsFlag;

            // Reserve flag place 
            intOffsetFlag = intCrunchedLength++;

            // Clear Flag
            bsFlag = 0x00;
        }

        /********************************************************
         * Fast Crunching method                                *
         * We test until a byte isn't matching the buffered one *
         ********************************************************/

        // get Min index in buffer
        int index_min = curr_index - intBuffSize;
        index_min = (index_min<0)?0:index_min;
        int index_buffer = curr_index - 1;

        // get Max index in buffer + 1
        int index_max = curr_index + 0x100;
        index_max = (index_max>intRawLength)?intRawLength:index_max;

        // Set previous matching sequence size and index
        int intSeqSize = 0;
        int intSeqIndex = 0;

        // Loop through all possible matches in the buffer
        while(index_buffer>=index_min)
        {
            // Current matching Sequence size
            int intCurrSeqSize = 0;

            // Floating index
            int indexFloat = curr_index;
            int indexFloatBuffer = index_buffer;

            // Increase Sequence size
            // When bytes are equal
            while ( indexFloat<index_max
                    && (TabTrameReg[indexFloatBuffer++][r] == TabTrameReg[indexFloat++][r]))
            {
                intCurrSeqSize++;
            }

            // If CurrCalculated Size is bigger than the previous one, swap them
            if (intCurrSeqSize>intSeqSize)
            {
                intSeqSize = intCurrSeqSize;
                intSeqIndex = index_buffer;
            }

            // Decrease index in buffer
            index_buffer--;
        }

        if (intSeqSize > intMinSeqSize)
        {
            // A previously uncrunched pattern has been found
            // We need to:
            // - neg the length
            // - calculate the offset of previous maching Pattern in the buffer
            // - set bit Flag
            // - decrement Sequence Counter
            // - increment curr_index

            // length
            byte bytNegLength = (byte)((-intSeqSize) & 0xFF);
            arrCrunchedData[intCrunchedLength++] = bytNegLength;

            // Offset
            if (intBuffSize <= 0x100)
            {
                // Previous Offset
                UBYTE bytePrevOffset = (UBYTE)intSeqIndex;

                // Write 0x100 Offset
                arrCrunchedData[intCrunchedLength++] = bytePrevOffset;
            }
            else
            {
                // Previous Offset
                short shortPrevOffset = (short)intSeqIndex;

                // Write 0x400 Offset
                arrCrunchedData[intCrunchedLength++] = (byte)(shortPrevOffset & 0xFF);
                arrCrunchedData[intCrunchedLength++] = (byte)(shortPrevOffset>>8 & 0x3);
            }

            // Sequence Counter
            intSeqCounter--;

            // set bit Flag
            bsFlag |= 1<<intSeqCounter;

            // Inc index
            curr_index += intSeqSize;
        }
        else
        {
            // No Pattern found (put a character)
            // - put character
            // - unset bit Flag (nothing to do)
            // - decrement Sequence Counter
            // - increment curr_index

            // New character
            arrCrunchedData[intCrunchedLength++] = TabTrameReg[curr_index][r];

            // Sequence Counter
            intSeqCounter--;

            // Inc index
            curr_index++;
        }
    }

    // Insert Final Flag in flow
    arrCrunchedData[intOffsetFlag] = bsFlag;

    // return Crunched Array
    *intCrunLen = intCrunchedLength;
    return arrCrunchedData;
}
#endif


/********************************************************* !NAME! **************
* Nom : WriteYMEnd
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\\/FLE!, ligne : !./LN!
*
* Description : Termine le fichier YM
*
* Résultat    : /
*
* Variables globales modifiées : pfile, ps, psCmp
*
********************************************************** !0! ****************/
void WriteYMEnd( HWND hWnd )
{
    static char Tmp[ 128 ];
    static UBYTE HeaderYM[ 34 ];
    int FreqAYCPC = 1000000;

/*
En-tête fichier YM5 : (format BIG ENDIAN)

Offset  Size    Type        Comment 
0       4       DWORD       ID of YM5 format. ('YM5!') 
4       8       string[8]   Check String ('LeOnArD!') 
12      4       DWORD       Nb of valid VBL of the file 
16      4       DWORD       Song attributes (see bellow) 
20      2       WORD        Nb of digi-drum sample (can be 0) 
22      4       DWORD       YM2149 External frequency in Hz (1000000)
26      2       WORD        Player frequency in Hz (50Hz for almost player) 
28      4       DWORD       VBL number to loop the song. (0 is default)
32      2       WORD        Must be 0 for the moment
?       ?       NT-String   Name of the song 
?       ?       NT-String   Name of the author 
?       ?       NT-String   Comments (Name of the YM converter !) 
?       ?                   All YM2149 registers 
?       4       DWORD       End-File check ('End!') 

*/


    memset( HeaderYM, 0, sizeof( HeaderYM ) );
    memcpy( HeaderYM, ( UBYTE * )"YM5!LeOnArD!", 12 );
    if ( NumTrame )
        {
        // Nb frames (0 - 0xFFFF pour le moment )
        HeaderYM[ 14 ] = ( UBYTE )( NumTrame >> 8 );
        HeaderYM[ 15 ] = ( UBYTE )NumTrame;
        // Attribut (frames entrelacées)
        HeaderYM[ 19 ] = YMEntrelace;
        // Fréquence puce AY
        HeaderYM[ 22 ] = ( UBYTE )( FreqAYCPC >> 24 );
        HeaderYM[ 23 ] = ( UBYTE )( FreqAYCPC >> 16 );
        HeaderYM[ 24 ] = ( UBYTE )( FreqAYCPC >> 8 );
        HeaderYM[ 25 ] = ( UBYTE )FreqAYCPC;
        // Fréquence player
        HeaderYM[ 27 ] = 50; // 50 hz
        fwrite( HeaderYM, sizeof( HeaderYM ), 1, FileYM );
        fprintf( FileYM, "YM Write%c", 0 );
        fprintf( FileYM, "PC-CPC%c", 0 );
        fprintf( FileYM, TITRE "%c", 0 );
        if ( YMEntrelace )
            {
            for ( int r = 0; r < 16; r++ )
                for ( int f = 0; f < NumTrame; f++ )
                    fwrite( &TabTrameReg[ f ][ r ], 1, 1, FileYM );
            }
        else
            fwrite( TabTrameReg, NumTrame * 16, 1, FileYM );

        fprintf( FileYM, "End!" );
        }
    wsprintf( Tmp
            , "Sauvegarde YM terminée. Nombre de trames écrites : %d"
            , NumTrame
            );
    MessageBox( hWnd, Tmp, "Sauvegarde YM", MB_OK );
    YMFileStarted = FALSE;
    fclose( FileYM );
    FileYM = NULL;

#ifdef USE_AYC
    /*******************
     * Create AYC file *
     *******************/
    DWORD BytesWritten;
    HANDLE FileAYC = CreateFile( "Crunched_tune.ayc"
                               , GENERIC_READ | GENERIC_WRITE
                               , 0
                               , NULL
                               , CREATE_ALWAYS
                               , 0
                               , NULL
                               );

    if ( FileAYC != INVALID_HANDLE_VALUE )
        {
        // Process each Register
        int arrCrunLen[14];
        static UBYTE arrCrunchedRegister[14][ 0x10000 ];
        UBYTE arrBuffLen[14];
        
        for (UBYTE curr_register=0;curr_register < 14; curr_register++)
            {                                
            // Crunch register Data 
            USHORT intCrunLen;
            USHORT intCrunLen400;
            UBYTE* arrCrunchedData = doAYCCrunch( curr_register, (USHORT)NumTrame, 0x100, &intCrunLen);
            UBYTE* arrCrunchedData400 = doAYCCrunch( curr_register, (USHORT)NumTrame, 0x400, &intCrunLen400);
                
            if ((intCrunLen400 + 0x300) < intCrunLen)
                {
                // Use a 0x400 bytes buffer rather than a 0x100 one

                // Buffer length
                arrBuffLen[curr_register] = 0x4;

                // Add Crunched register to the list
                memcpy( arrCrunchedRegister[curr_register], arrCrunchedData400, 0x10000 );
                arrCrunLen[curr_register] = intCrunLen400;                
                }
            else
                {
                // Use 0x100 Buffer

                // Buffer length
                arrBuffLen[curr_register] = 0x1;
            
                // Add Crunched register to the list
                memcpy( arrCrunchedRegister[curr_register], arrCrunchedData, 0x10000 );
                arrCrunLen[curr_register] = intCrunLen;                
                }
            }

        MessageBox( hWnd, "Writing file ...", "Sauvegarde AYC", MB_OK );

        /**
         * Write Header
         */
                
        // Time
        WriteFile( FileAYC, &NumTrame, 2, &BytesWritten, NULL );
        
        // Registers' offset
        int offsetBase = 2 + 2;
        int offsetPhysical = 2 + 16*3;
        short offset = ( short )( offsetPhysical - offsetBase );
        WriteFile( FileAYC, arrBuffLen, 1, &BytesWritten, NULL );
        WriteFile( FileAYC, &offset, 2, &BytesWritten, NULL );
        for (int i=1;i<14;i++)
            {
            // get Relative Offset
            offsetPhysical += arrCrunLen[i-1];
            offsetBase += 3;
            offset = ( short )( offsetPhysical - offsetBase );
            
            WriteFile( FileAYC, arrBuffLen+i, 1, &BytesWritten, NULL );
            WriteFile( FileAYC, &offset, 2, &BytesWritten, NULL );
            }
        
        // Add Dummy values for 14,15 Registers' Offset
        UBYTE dummy = 0xFF;
        for (int b=0;b<6;b++) 
            WriteFile( FileAYC, &dummy, 1, &BytesWritten, NULL );
            
        /**
         * Write Data
         */
        for (int j=0;j<14;j++)
            WriteFile( FileAYC
                     , arrCrunchedRegister[j]
                     , arrCrunLen[j]
                     , &BytesWritten
                     , NULL
                     );

        CloseHandle( FileAYC );
        }
    else
        Log( MODULENAME, "Impossible de créer le fichier AYC", LOG_ERROR );
#endif
}


/********************************************************* !NAME! **************
* Nom : WriteYMStarted
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\\/FLE!, ligne : !./LN!
*
* Description : Indique si un fichier YM est en cours de traitement
*
* Résultat    : TRUE si en cours, FALSE sinon
*
* Variables globales modifiées : 
*
********************************************************** !0! ****************/
BOOL WriteYMStarted( void )
{
    return( YMFileStarted );
}
