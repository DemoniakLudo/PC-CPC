/******************************************************************************/
/* Configuration pour l'archivage des différents éléments du fichier source   */
/******************************************************************************/
// !CONFIG!=/L/* /R/* /W"* Nom : "
// Définition du système       !CONFIG!=/V1!EMULATEUR CPC!
// Définition du sous système  !CONFIG!=/V2!PC-CPC!
// Définition du sous ensemble !CONFIG!=/V3!Chips!
// Définition du module        !CONFIG!=/V4!CRTC 6845!
/******************************************************************************/

/********************************************************* !NAME! **************
* !./FLE!
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\Fichiers
********************************************************** !0! *****************
* ------------------------------------------------------------------------------
*          SYSTEME         |      SOUS SYSTEME       |      SOUS ENSEMBLE
* ------------------------------------------------------------------------------
*  EMULATEUR CPC           | PC-CPC                  | Chips
* ------------------------------------------------------------------------------
*  Fichier     : CRTC.C                | Version : 0.1at
* ------------------------------------------------------------------------------
*  Date        : 05/11/2002            | Auteur  : L.DEPLANQUE
* ------------------------------------------------------------------------------
*  Description : Emulation du CRTC 6845
*
* ------------------------------------------------------------------------------
*  Historique  :
*           Date           |         Auteur          |       Description
* ------------------------------------------------------------------------------
*  05/11/2002              | L.DEPLANQUE             | creation
* ------------------------------------------------------------------------------
*  13/11/2002              | L.DEPLANQUE             | Changement du type de
*                          |                         | la variable TabAdrCrtc : 
*                          |                         | passage en USHORT, pour
*                          |                         | optimiser le memset
*                          |                         | dans CalcAdrEcrCPC()
* ------------------------------------------------------------------------------
*  08/02/2006              | L.DEPLANQUE             | Version 0.1ak :
*                          |                         | Réécriture complète du
*                          |                         | code
* ------------------------------------------------------------------------------
*  06/12/2006              | L.DEPLANQUE             | Version 0.1am :
*                          |                         | Variable "VBL" remplacée
*                          |                         | par "VSync" et supprimée
*                          |                         | du module PPI.
*                          |                         | Correction problème de
*                          |                         | synchro multiples.
* ------------------------------------------------------------------------------
*  01/03/2007              | L.DEPLANQUE             | Version 0.1an :
*                          |                         | Réécriture complète de
*                          |                         | la fonction principale
*                          |                         | du CRTC, prise en compte
*                          |                         | d'un cycle (1 mot vidéo)
*                          |                         | au lieu d'une ligne HBL
* ------------------------------------------------------------------------------
*  22/05/2007              | L.DEPLANQUE             | Version 0.1ao :
*                          |                         | Ajout appel fonction
*                          |                         | "DelockAsic"
* ------------------------------------------------------------------------------
*  05/03/2011              | L.DEPLANQUE             | Version 0.1at :
*                          |                         | Correction bugs prise en
*                          |                         | compte taille VBL et
*                          |                         | taille HBL
* ------------------------------------------------------------------------------
********************************************************** !END! **************/


#include  <memory.h>

#include  "Types.h"
#include  "Config.h"
#include  "PlateForm.h"
#include  "SnapShot.h"
#include  "VGA.H"
#include  "Z80.h"
#include  "Asic.h"


#define     MODULENAME      "CRTC"


#define     TAILLE_X_ECR    384*2
#define     TAILLE_Y_ECR    272*2

#define     TAILLE_Y_CRTC   312     // Le crtc génère 312 lignes verticales

#define     TAILLE_VBL      26      // VBL = 26 lignes



static int xEcr, yEcr;
static int LigneCRTC, MaCRTC, MaHi, HDelay, VDelay, VswCount, HDT, VDT, HS, MR, VT, OldVSync;
static int VCharCount, VHCount;


/********************************************************* !NAME! **************
* Nom : VSync
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Signal VSync
*
********************************************************** !0! ****************/
int VSync;


/********************************************************* !NAME! **************
* Nom : LastMode
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Mode écran sélectionné
*
********************************************************** !0! ****************/
int LastMode;


/********************************************************* !NAME! **************
* Nom : ResetCRTC
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Effectue un reset du CRTC
*
* Résultat    : /
*
* Variables globales modifiées : Cpc.CRTCReg
*
********************************************************** !0! ****************/
void ResetCRTC( void ) {
	xEcr = yEcr = LigneCRTC = HDelay = VDelay = VswCount = HDT = VDT = MaCRTC = MaHi = HS = MR = VT = OldVSync = VCharCount = VHCount = 0;
}


/********************************************************* !NAME! **************
* Nom : ReadCRTC
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Lecture d'un registre du CRTC
*
* Résultat    : Le registre du CRTC selectionné
*
* Variables globales modifiées : /
*
********************************************************** !0! ****************/
int ReadCRTC( int port ) {
	port &= 0x300;
	return port == 0x300 ? Cpc.CRTCIndex >= 10 ? Cpc.CRTCReg[ Cpc.CRTCIndex ] : 0 : port == 0x200 ? VSync << 5 : 0xFF;
}


/********************************************************* !NAME! **************
* Nom : WriteCRTC
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Ecriture d'un registre du CRTC
*
* Résultat    : /
*
* Variables globales modifiées : Cpc.CRTCReg, RegCRTCSel, XStart, XEnd
*
********************************************************** !0! ****************/
void WriteCRTC( int adr, int val ) {
	adr &= 0x300;
	if ( ! adr )
		Cpc.CRTCIndex = val & 0x1F;
	else
		if ( adr == 0x100 ) {
			switch( Cpc.CRTCIndex ) {
			case 4 :
			case 6 :
			case 7 :
				val &= 0x7F;
				break;

			case 5 :
			case 9 :
				val &= 0x1F;
				break;

			case 12 :
				MaHi = ( ( val << 8 ) & 0x0F00 ) | ( ( val << 9 ) & 0x6000 );
				break;
			}
			Cpc.CRTCReg[ Cpc.CRTCIndex ] = val;
		}
}


/********************************************************* !NAME! **************
* Nom : CycleCRTC
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Effectue n cycles CRTC, trace les mots en mémoire vidéo
*
* Résultat    : 1 si demande de redessiner l'écran, 0 sinon
*
* Variables globales modifiées : /
*
********************************************************** !0! ****************/
int CycleCRTC( int NbCycles ) {
	int CalcMa = 0, DoResync = 0;
	static int HT = 0;

	for ( ; NbCycles--; ) {
		VCharCount++;
		if ( ++Cpc.HCC > Cpc.CRTCReg[ 0 ] ) {
			Cpc.HCC = 0;
			Cpc.VLC = ++Cpc.VLC & 0x1F;
			if ( VSync && ++Cpc.TailleVBL >= ( ( Cpc.CRTCReg[ 3 ] & 0xF0 ) ? Cpc.CRTCReg[ 3 ] >> 4 : 16 ) ) {
				OldVSync = 1;
				VSync = 0;
			}
			if ( MR ) {
				MR = 0;
				Cpc.VLC = 0;
				MaCRTC += HDT ? 0 : Cpc.CRTCReg[ 1 ];
				Cpc.VCC = ++Cpc.VCC & 0x7F;
			}
			CalcMa = Cpc.VtAdj ? ! --Cpc.VtAdj : CalcMa;
			if ( VT ) {
				VT = 0;
				if ( Cpc.CRTCReg[ 5 ] )
					Cpc.VtAdj = Cpc.CRTCReg[ 5 ];
				else
					CalcMa = 1;
			}
			if ( CalcMa ) {
				CalcMa = OldVSync = Cpc.VLC = Cpc.VCC = 0;
				VDT = 1;
				MaCRTC = Cpc.CRTCReg[ 13 ] + MaHi;
			}
			if ( Cpc.VLC == Cpc.CRTCReg[ 9 ] ) {
				MR = 1;
				VT = ! Cpc.VtAdj && Cpc.VCC == Cpc.CRTCReg[ 4 ] ? 1 : VT;
			}
			VDT = Cpc.VCC == Cpc.CRTCReg[ 6 ] ? 0 : VDT;
			if ( Cpc.VCC == Cpc.CRTCReg[ 7 ] && ! VSync && ! OldVSync ) {
				Cpc.TailleVBL = 0;
				VSync = 1;
				Cpc.SyncCount = 2;
				VDelay = 2;
				VswCount = 4;
			}
			HDT = 1;
		}
		HDT = Cpc.HCC == Cpc.CRTCReg[ 1 ] ? 0 : HDT;
		if ( HS ) {
			if ( HDelay != 2 )
				HDelay++;
			else
				if ( ! --VHCount ) {
					LigneCRTC++;
					VCharCount = 0;
					HDelay++;
				}
		}
		if ( Cpc.TailleHBL && ! --Cpc.TailleHBL ) {
			HS = 0;
			VDelay -= VDelay != 0;
			if ( ! VDelay && VswCount && ! --VswCount ) {
				LigneCRTC = 0;
				DoResync = 1;
			}
			LastMode = Cpc.VGARom & 3;
			Cpc.CntHSync++;
			if ( ! Cpc.SyncCount ) {
				if ( Cpc.CntHSync == 52 ) {
					Cpc.CntHSync = 0;
					Cpc.IRQ = 1;
				}
			}
			else
				if ( ! --Cpc.SyncCount ) {
					if ( Cpc.CntHSync & 32 )
						Cpc.IRQ = 1;

					Cpc.CntHSync = 0;
				}
		}
		if ( Cpc.HCC == Cpc.CRTCReg[ 2 ] ) {
			HDelay = 0;
			HS = 1;
			Cpc.TailleHBL = ( Cpc.CRTCReg[ 3 ] & 0x0F ) ? Cpc.CRTCReg[ 3 ] & 0x0F : 16;
			VHCount = Cpc.TailleHBL > 2 ? Cpc.TailleHBL < 6 ? Cpc.TailleHBL - 2 : 4 : 0;
		}
		if ( yEcr && yEcr < TAILLE_Y_ECR && xEcr < TAILLE_X_ECR ) {
			TraceMot32B640( xEcr, yEcr, HDT && VDT ? ( ( ( ( Cpc.HCC + MaCRTC ) << 1 ) + ( ( ( Cpc.HCC + MaCRTC ) & 0x1000 ) << 2 ) ) & 0xC7FF ) | ( Cpc.VLC << 11 ) : -1, LigneCRTC < TAILLE_VBL );
			xEcr += 16;
		}
		else {
			VGASyncColor();
			if ( VCharCount == 7 ) {
				xEcr = 0;
				yEcr = yEcr >= TAILLE_Y_ECR && LigneCRTC == TAILLE_VBL ? 0 : yEcr + 2;
			}
			if ( LigneCRTC == TAILLE_Y_CRTC ) {
				LigneCRTC = 0;
				DoResync = 1;
			}
		}
	}
	return( DoResync );
}
