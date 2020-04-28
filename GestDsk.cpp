/******************************************************************************/
/* Configuration pour l'archivage des différents éléments du fichier source   */
/******************************************************************************/
// !CONFIG!=/L/* /R/* /W"* Nom : "
// Définition du système       !CONFIG!=/V1!EMULATEUR CPC!
// Définition du sous système  !CONFIG!=/V2!PC-CPC!
// Définition du sous ensemble !CONFIG!=/V3!Accessoires!
// Définition du module        !CONFIG!=/V4!Gestion fichiers DSK!
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
*  Fichier     : GESTDSK.CPP           | Version : 0.1at
* ------------------------------------------------------------------------------
*  Date        : 24/01/2011            | Auteur  : L.DEPLANQUE
* ------------------------------------------------------------------------------
*  Description : Gestion des fichiers .DSK
*
* ------------------------------------------------------------------------------
*  Historique  :
*           Date           |         Auteur          |       Description
* ------------------------------------------------------------------------------
*  24/01/2011              | L.DEPLANQUE             | creation
* ------------------------------------------------------------------------------
********************************************************** !END! **************/


#include  <afxwin.h>

#include  "types.h"
#include  "GestDsk.h"
#include  "Upd.h"
#include  "Plateform.h"
#include  "Log.h"
#include  "Resource.h"
#include  "Zip/zip.h"
#include  "Zip/unzip.h"
#include  "DialZip.h"


#define     MODULENAME      "GestDsk"


/********************************************************* !NAME! **************
* Nom : ImgDsk
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Gestion des DSKs : données + structures
*
********************************************************** !0! ****************/
ImgDSK Dsk[ MAX_DSK ];

/********************************************************* !NAME! **************
* Nom : DataDsk
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Stockage des données image disquette
*
********************************************************** !0! ****************/
static UBYTE DataDsk[ MAX_DSK ][ 1024 * 1024 ];

/********************************************************* !NAME! **************
* Nom : EjectDSK
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Termine l'accès à une disquette (ejection)
*
* Résultat    : /
*
* Variables globales modifiées : Dsk
*
********************************************************** !0! ****************/
void EjectDSK( HWND hWnd, int DrvNum ) {
	DWORD BytesWritten;
	ZipOpenFile ZipResult;

	Dsk[ DrvNum ].Image = 0;
	if ( Dsk[ DrvNum ].FlagWrite ) {
		// Ecriture de la disquette ???
		if ( MessageBox( hWnd, "Sauvegarde fichier disquette ?", "Emu-CPC", MB_YESNO ) == IDYES ) {
			HANDLE f = CreateFile( Dsk[ DrvNum ].NomFic, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL );
			if ( f != INVALID_HANDLE_VALUE ) {
				WriteFile( f, &Dsk[ DrvNum ].Infos, sizeof( Dsk[ DrvNum ].Infos ), &BytesWritten, NULL );
				WriteFile( f, DataDsk[ DrvNum ], Dsk[ DrvNum ].LongFic, &BytesWritten, NULL );
				CloseHandle( f );
				// si c'est un fichier ZIP, on le sauve dans le ZIP
				if ( IsInOpendZip( Dsk[ DrvNum ].NomFic, &ZipResult ) ) {
					SaveFileInZip( &ZipResult );
					CloseFileZip( &ZipResult );
				}
			}
		}
		Dsk[ DrvNum ].FlagWrite = 0;
	}
	if ( IsInOpendZip( Dsk[ DrvNum ].NomFic, &ZipResult ) )
		CloseFileZip( &ZipResult );
}

/********************************************************* !NAME! **************
* Nom : SetDSK
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Initialise l'accès à une disquette (insertion)
*
* Résultat    : /
*
* Variables globales modifiées : Dsk
*
********************************************************** !0! ****************/
void SetDSK( HWND hWnd, char * n, int DrvNum )
{
	HANDLE f = CreateFile( n
		, GENERIC_READ
		, 0
		, NULL
		, OPEN_EXISTING
		, 0
		, NULL
		);
	if ( f != INVALID_HANDLE_VALUE )
	{
		ResetUPD();
		DWORD BytesRead;
		EjectDSK( hWnd, DrvNum );

		strcpy( Dsk[ DrvNum ].NomFic, n );
		ReadFile( f, &Dsk[ DrvNum ].Infos, sizeof( Dsk[ DrvNum ].Infos ), &BytesRead, NULL );
		ReadFile( f, &DataDsk[ DrvNum ], sizeof( DataDsk[ DrvNum ] ), &Dsk[ DrvNum ].LongFic, NULL );
		CloseHandle( f );
		int ImageType = ! strncmp( Dsk[ DrvNum ].Infos.debut, "MV - CPC", 8 ) ? 1 : ! strncmp( Dsk[ DrvNum ].Infos.debut, "EXTENDED", 8 ) ? 2 : 0;
		if ( ImageType && Dsk[ DrvNum ].LongFic ) {
			int t, h, s, Pos = 0, PosTable = 0;
			memset( Dsk[ DrvNum ].Tracks, 0, sizeof( Dsk[ DrvNum ].Tracks ) );
			for ( h = 0; h < Dsk[ DrvNum ].Infos.NbHeads; h++ ) {
				for ( t = 0; t < Dsk[ DrvNum ].Infos.NbTracks; t++ ) {
					CPCEMUTrack * CurTrack = ( CPCEMUTrack * )&DataDsk[ DrvNum ][ Pos ];
					if ( ! strncmp( CurTrack->ID, "Track-Info", 10 ) ) {
						int NumTrack = CurTrack->Track;
						int NumHead = CurTrack->Head & 1;
						Dsk[ DrvNum ].Tracks[ NumTrack ][ NumHead ] = CurTrack;
						Dsk[ DrvNum ].Data[ NumTrack ][ NumHead ] = &DataDsk[ DrvNum ][ Pos + sizeof( CPCEMUTrack) ];
						if ( ! NumHead && Dsk[ DrvNum ].Infos.NbHeads == 1 ) {
							Dsk[ DrvNum ].Tracks[ NumTrack ][ 1 ] = CurTrack;
							Dsk[ DrvNum ].Data[ NumTrack ][ 1 ] = &DataDsk[ DrvNum ][ Pos + sizeof( CPCEMUTrack) ];
						}
						if ( ImageType == 2 )
							Pos += Dsk[ DrvNum ].Infos.TrackSizeTable[ PosTable++ ] << 8;
						else
							if ( Dsk[ DrvNum ].Infos.TrackSize )
								Pos += Dsk[ DrvNum ].Infos.TrackSize;
							else {
								for ( s = 0; s < CurTrack->NbSect; s++ ) {
									int n = CurTrack->Sect[ s ].SectSize;
									int addp = n & 0xFF ? ( n + 0xFF ) & 0x1F00 : n ? n : CurTrack->Sect[ s ].N << 6;
									Pos += addp < 0x100 ? 0x100 : addp > 0x1800 ? 0x1800 : addp;
								}
								Pos += sizeof( CPCEMUTrack );
							}
					}
					else
						Log( MODULENAME, "Piste %d non trouvée (%s).", t, Dsk[ DrvNum ].NomFic, LOG_WARNING );
				}
			}
			Dsk[ DrvNum ].Image = 1;
			Dsk[ DrvNum ].FlagWrite = 0;
		}
	}
}
