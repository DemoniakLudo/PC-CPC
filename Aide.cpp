/******************************************************************************/
/* Configuration pour l'archivage des différents éléments du fichier source   */
/******************************************************************************/
// !CONFIG!=/L/* /R/* /W"* Nom : "
// Définition du système       !CONFIG!=/V1!EMULATEUR CPC!
// Définition du sous système  !CONFIG!=/V2!PC-CPC!
// Définition du sous ensemble !CONFIG!=/V3!Accessoires!
// Définition du module        !CONFIG!=/V4!Aide!
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
*  Fichier     : AIDE.C                | Version : 0.1at
* ------------------------------------------------------------------------------
*  Date        : 10/05/2007            | Auteur  : L.DEPLANQUE
* ------------------------------------------------------------------------------
*  Description : Affiche le menu d'aide
*
* ------------------------------------------------------------------------------
*  Historique  :
*           Date           |         Auteur          |       Description
* ------------------------------------------------------------------------------
*  10/05/2007              | L.DEPLANQUE             | creation
* ------------------------------------------------------------------------------
*  18/02/2008              | L.DEPLANQUE             | Version 0.1ao :
*                          |                         | Ajout de la touche
*                          |                         | SHIFT + F7
* ------------------------------------------------------------------------------
*  16/04/2010              | L.DEPLANQUE             | Version 0.1ap :
*                          |                         | Ajout de la touche
*                          |                         | SHIFT + 12 (Assembleur)
* ------------------------------------------------------------------------------
*  16/02/2011              | L.DEPLANQUE             | Version 0.1at :
*                          |                         | Ajout de la touche
*                          |                         | SHIFT + F1
* ------------------------------------------------------------------------------
********************************************************** !END! **************/

#include  "types.h"

#include  <windows.h>
#include  "Plateform.h"

static HDC dc;

static void Affiche( int x, int y, char * Msg ) {
	TextOut( dc, x, y, Msg, ( int )strlen( Msg ) );
}

static void AfficheAide( void ) {
	Affiche( 0x00, 0x16, "F1: Aide" );
	Affiche( 0x00, 0x24, "F2: Turbo ON/OFF" );
	Affiche( 0x00, 0x32, "F3: Bp Multiface" );
	Affiche( 0x00, 0x40, "F4: Lecture SNA" );
	Affiche( 0x00, 0x4E, "F5: Sauvegarde SNA" );
	Affiche( 0x00, 0x5C, "F6: Debugger" );
	Affiche( 0x00, 0x6A, "F7: Lecture DSK drive A:" );
	Affiche( 0x00, 0x78, "F8: Reset" );
	Affiche( 0x00, 0x86, "F9: Lecture Cassette" );
	Affiche( 0x00, 0xA2, "F11:Ecriture Cassette" );
	Affiche( 0x00, 0xB0, "F12:Sauve BMP" );
	Affiche( 0xAA, 0x16, "Shift+F1: Phenixinformatique.com" );
	Affiche( 0xAA, 0x24, "Shift+F2: Lecture/ecriture PC" );
	Affiche( 0xAA, 0x32, "Shift+F3: Taille fenetre" );
	Affiche( 0xAA, 0x40, "Shift+F4: Mono/Couleur" );
	Affiche( 0xAA, 0x4E, "Shift+F5: Sauvegarde YM" );
	Affiche( 0xAA, 0x5C, "Shift+F6: Minimiser fenetre" );
	Affiche( 0xAA, 0x6A, "Shift+F7: lecture DSK drive B:" );
	Affiche( 0xAA, 0x78, "Shift+F8: ROMPACK ON/OFF" );
	Affiche( 0xAA, 0x86, "Shift+F9: Position Cassette" );
	Affiche( 0xAA, 0xA2, "Shift+F11:Sauvegarde AVI" );
	Affiche( 0xAA, 0xB0, "Shift+F12:Assembleur" );
}

static long FAR PASCAL AideWndProc( HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam )
{
	switch( iMessage ) {
	case WM_PAINT :
		AfficheAide();
		break;

	case WM_DESTROY :
	case WM_CLOSE :
		PostQuitMessage( 0 );
		break;

	case WM_KEYDOWN :
	case WM_KEYUP :
		PostMessage( hWnd, WM_CLOSE, 0, 0L );
		break;
	}
	return( ( long )DefWindowProc( hWnd, iMessage, wParam, lParam ) );
}

void Aide( void ) {
	WNDCLASS wndClass;
	HWND HelpWnd;
	MSG msg;
	HFONT f;

	//
	// Enregistrement de la classe de la fenêtre
	//
	memset( &wndClass, 0, sizeof( WNDCLASS ) );
	wndClass.style          = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc    = AideWndProc;
	wndClass.hbrBackground  = CreateSolidBrush( BK_COLOR );
	wndClass.lpszMenuName   = "AideMenu";
	wndClass.lpszClassName  = "Aide";
	if ( RegisterClass( &wndClass ) ) {
		int x = 35, y = 35;
		WINDOWPLACEMENT Pos;

		if ( GetWindowPlacement( hWnd, &Pos ) ) {
			x = Pos.rcNormalPosition.left + 5;
			y = Pos.rcNormalPosition.top + 25;
		}
		//
		// Création de la fenêtre
		//
		HelpWnd = CreateWindow("Aide", "Aide", WS_SYSMENU | WS_OVERLAPPED, x, y, 380, 250, hWnd, NULL, NULL, NULL );

		// Création d'une police
		f = CreateFont( 16, 7, 0, 0, FW_THIN, 0, 0, 0, OEM_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FIXED_PITCH | FF_DONTCARE, NULL );
		if ( f ) {
			dc = GetDC( HelpWnd );
			SelectObject( dc, f );
			SetTextColor( dc, TEXT_COLOR );
			SetBkColor( dc, BK_COLOR );
			AfficheAide();
			ShowWindow( HelpWnd, SW_SHOW );
			UpdateWindow( HelpWnd );
			while( GetMessage( &msg, NULL, 0,0 ) ) {
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}
			DeleteObject( f );
		}
		UnregisterClass( "Aide", NULL );
	}
}
