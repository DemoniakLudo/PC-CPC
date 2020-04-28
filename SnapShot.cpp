/******************************************************************************/
/* Configuration pour l'archivage des différents éléments du fichier source   */
/******************************************************************************/
// !CONFIG!=/L/* /R/* /W"* Nom : "
// Définition du système       !CONFIG!=/V1!EMULATEUR CPC!
// Définition du sous système  !CONFIG!=/V2!PC-CPC!
// Définition du sous ensemble !CONFIG!=/V3!Accessoires!
// Définition du module        !CONFIG!=/V4!Snapshots!
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
*  Fichier     : SNAPSHOT.C            | Version : 0.1am
* ------------------------------------------------------------------------------
*  Date        : 05/11/2002            | Auteur  : L.DEPLANQUE
* ------------------------------------------------------------------------------
*  Description : Lecture/écriture des fichiers snapshots
*
* ------------------------------------------------------------------------------
*  Historique  :
*           Date           |         Auteur          |       Description
* ------------------------------------------------------------------------------
*  05/11/2002              | L.DEPLANQUE             | creation
* ------------------------------------------------------------------------------
*  13/11/2002              | L.DEPLANQUE             | Passage du nom de fichier
*                          |                         | snapshot en paramètre aux
*                          |                         | fonctions LireSnap() et
*                          |                         | SauveSnap().  
* ------------------------------------------------------------------------------
*  20/10/2006              | L.DEPLANQUE             | Version 0.1am:
*                          |                         | Sauvegarde seulement de
*                          |                         | 128Ko de mémoire.
*                          |                         | Il manquait la couleur
*                          |                         | Du border lors de la
*                          |                         | sauvegarde du sna
* ------------------------------------------------------------------------------
*  03/03/2011              | L.DEPLANQUE             | Version 0.1at :
*                          |                         | Passage "partiel" en
*                          |                         | version 3 de .SNA
* ------------------------------------------------------------------------------
********************************************************** !END! **************/


#include  <afxwin.h>

#include  "types.h"
#include  "config.h"
#include  "AY8912.h"
#include  "crtc.h"
#include  "ppi.h"
#include  "upd.h"
#include  "vga.h"
#include  "z80.h"
#include  "log.h"
#include  "SnapShot.h"
#include  "Resource.h"
#include  "Zip/zip.h"
#include  "Zip/unzip.h"
#include  "DialZip.h"


#define     MODULENAME      "SnapShot"


/********************************************************* !NAME! **************
* Nom : Cpc
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Structure Snapshot
*
********************************************************** !0! ****************/
StCpc Cpc;


/********************************************************* !NAME! **************
* Nom : SauveSnap
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Sauvegarde d'un fichier snapshot
*
* Résultat    : /
*
* Variables globales modifiées : /
*
********************************************************** !0! ****************/
void SauveSnap( char * Nom ) {

	DWORD BytesWritten;

	HANDLE f = CreateFile( Nom, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL );
	Log( MODULENAME, "Sauvegarde snapshot : %s", Nom, LOG_INFO );
	if ( f != INVALID_HANDLE_VALUE ) {
		WriteFile( f, &Cpc, sizeof( Cpc ), &BytesWritten, NULL );
		WriteFile( f, MemCPC, 0x20000, &BytesWritten, NULL );
		CloseHandle( f );
		// si c'est un fichier ZIP, on le sauve dans le ZIP
		ZipOpenFile ZipResult;
		if ( IsInOpendZip( Nom, &ZipResult ) )
		{
			SaveFileInZip( &ZipResult );
			CloseFileZip( &ZipResult );
		}
	}
	else
		Log( MODULENAME, "Erreur écriture fichier %s.", Nom, LOG_ERROR );
}


/********************************************************* !NAME! **************
* Nom : SetSnap
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Initialise un snapshot à partir d'une en-tête SNA
*
* Résultat    : TRUE si initialisation ok, FALSE sinon
*
* Variables globales modifiées : Cpc
*
********************************************************** !0! ****************/
BOOL SetSnap( StCpc * Entete )
{
	int i;

	if ( ! strncmp( Entete->Id, "MV - SNA", 8 ) ) {
		memcpy( &Cpc, Entete, sizeof( Cpc ) );
		Cpc.VGARam &= 7;
		for ( i = 0; i < 17; i++ ) {
			WriteVGA( i );
			WriteVGA( 0x40 | Cpc.InkData[ i ] );
			VGASyncColor();
		}
		for ( i = 0; i < 4; i++ )
			WritePPI( i + 0xF400 + ( i << 8 ), Cpc.PPI[ i ] );

		for ( i = 0; i < 16; i++ )
			Write8912( i, Cpc.PsgData[ i ] );

		SetMemCPC();
		for ( i = 0; i < 18; i++ ) {
			WriteCRTC( 0xBC00, i );
			WriteCRTC( 0xBD00, Cpc.CRTCReg[ i ] );
		}
		return( TRUE );
	}
	return( FALSE );
}


/********************************************************* !NAME! **************
* Nom : LireSnap
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Lecture d'un fichier snapshot
*
* Résultat    : /
*
* Variables globales modifiées : MemCpc
*
********************************************************** !0! ****************/
void LireSnap( char * Nom ) {
	StCpc tmpCpc;

	HANDLE f = CreateFile( Nom, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL );
	Log( MODULENAME, "Lecture snapshot : %s", Nom, LOG_INFO );
	if ( f != INVALID_HANDLE_VALUE ) {
		DWORD BytesRead;
		ReadFile( f, &tmpCpc, sizeof( tmpCpc ), &BytesRead, NULL );
		if ( SetSnap( &tmpCpc ) )
			ReadFile( f, MemCPC, 0x20000, &BytesRead, NULL );
		else
			Log( MODULENAME, "Snapshot (%s) non reconnu.", Nom, LOG_ERROR );

		CloseHandle( f );
	}
	else
		Log( MODULENAME, "Erreur lecture fichier %s.", Nom, LOG_ERROR );
}
