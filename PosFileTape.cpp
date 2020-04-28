/******************************************************************************/
/* Configuration pour l'archivage des différents éléments du fichier source   */
/******************************************************************************/
// !CONFIG!=/L/* /R/* /W"* Nom : "
// Définition du système       !CONFIG!=/V1!EMULATEUR CPC!
// Définition du sous système  !CONFIG!=/V2!PC-CPC!
// Définition du sous ensemble !CONFIG!=/V3!Accessoires!
// Définition du module        !CONFIG!=/V4!Gestion cassette!
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
*  Fichier     : POSFILETAPE.C         | Version : 0.1ai
* ------------------------------------------------------------------------------
*  Date        : 06/06/2005            | Auteur  : L.DEPLANQUE
* ------------------------------------------------------------------------------
*  Description : Popup de sélection position fichier cassette
*
* ------------------------------------------------------------------------------
*  Historique  :
*           Date           |         Auteur          |       Description
* ------------------------------------------------------------------------------
*  06/06/2005              | L.DEPLANQUE             | creation
* ------------------------------------------------------------------------------
*  08/12/2005              | L.DEPLANQUE             | Version 0.1ai :
*                          |                         | Mise en conformité des
*                          |                         | commentaires
* ------------------------------------------------------------------------------
********************************************************** !END! **************/


#include  "Types.h"

#include  <windows.h>



/********************************************************* !NAME! **************
* Nom : hEditLeft
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Champ de saisie pour le compteur cassette
*
********************************************************** !0! ****************/
static HANDLE hEditLeft;


/********************************************************* !NAME! **************
* Nom : ValParam
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Récupération sous forme ascii du champ de saisie pour le
*               compteur cassette
*
********************************************************** !0! ****************/
static char ValParam[ 16 ];


/********************************************************* !NAME! **************
* Nom : EditAlignWndProc
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Procédure de traitement des messages de la fenêtre de
*               saisie du compteur position cassette
*
* Résultat    : Résultat du traitement des messages
*
* Variables globales modifiées : ValParam, hEditLeft
*
********************************************************** !0! ****************/
long FAR PASCAL EditAlignWndProc( HWND hWnd
                                , UINT iMessage
                                , WPARAM wParam
                                , LPARAM lParam
                                )
{
    switch( iMessage )
        {
        case WM_DESTROY :
            PostQuitMessage( 0 );
            break;

        case WM_COMMAND :
            if ( ! wParam )
                {
                //
                // Convertir en ascii dans ValParam la valeur saisie
                //
                GetWindowText( ( HWND )hEditLeft
                             , ValParam
                             , sizeof( ValParam )
                             );
                PostMessage( hWnd, WM_CLOSE, 0, 0L );
                }
            break;
        }

    return( ( long )DefWindowProc( hWnd, iMessage, wParam, lParam ) );
}


/********************************************************* !NAME! **************
* Nom : SetPosFileTape
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Ouverture fenêtre saisie compteur position cassette
*
* Résultat    : La valeur du compteur
*
* Variables globales modifiées : ValParam, hEditLeft
*
********************************************************** !0! ****************/
int SetPosFileTape( int Val, HWND hParent )
{
    WNDCLASS wndClass;
    HWND hWnd;
    MSG msg;
    HANDLE hButton;

    //
    // Enregistrement de la classe de la fenêtre
    //
    memset( &wndClass, 0, sizeof( WNDCLASS ) );
    wndClass.style          = CS_HREDRAW | CS_VREDRAW;
    wndClass.lpfnWndProc    = EditAlignWndProc;
    wndClass.hbrBackground  = ( HBRUSH )GetStockObject( WHITE_BRUSH );
    wndClass.lpszMenuName   = "EditAlignMenu";
    wndClass.lpszClassName  = "EditAlign";
    itoa( Val, ValParam, 10 );
    if ( RegisterClass( &wndClass ) )
        {
        int x = 35, y = 35;
        WINDOWPLACEMENT Pos;

        if ( GetWindowPlacement( hParent, &Pos ) )
            {
            x = Pos.rcNormalPosition.left + 30;
            y = Pos.rcNormalPosition.top + 30;
            }
        //
        // Création de la fenêtre
        //
        hWnd = CreateWindow("EditAlign"
                           , "Saisie valeur compteur"
                           , WS_SYSMENU | WS_OVERLAPPED
                           , x, y, 180, 80
                           , hParent
                           , NULL
                           , NULL
                           , NULL
                           );
        ShowWindow( hWnd, SW_SHOW );
        UpdateWindow( hWnd );

        //
        // Création du champ de saisie
        //
        hEditLeft = CreateWindowEx( WS_EX_NOPARENTNOTIFY | WS_EX_CLIENTEDGE
                                  , "edit"
                                  , ValParam
                                  , WS_BORDER | WS_VISIBLE | WS_CHILD | ES_LEFT
                                  , 10, 10, 44, 26
                                  , hWnd
                                  , NULL
                                  , NULL
                                  , NULL
                                  );

        //
        // Création du bouton de validation
        //
        hButton = CreateWindowEx( BS_PUSHBUTTON
                                , "button"
                                , "Valider"
                                , WS_BORDER | WS_VISIBLE | WS_CHILD | ES_LEFT
                                , 60, 10, 100, 26
                                , hWnd
                                , NULL
                                , NULL
                                , NULL
                                );

        strcpy( ValParam, "-1" );
        while( GetMessage( &msg, NULL, 0,0 ) )
            {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
            }
        UnregisterClass( "EditAlign", NULL );
        }
    return( atoi( ValParam ) );
}
