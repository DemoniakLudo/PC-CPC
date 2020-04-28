/******************************************************************************/
/* Configuration pour l'archivage des différents éléments du fichier source   */
/******************************************************************************/
// !CONFIG!=/L/* /R/* /W"* Nom : "
// Définition du système       !CONFIG!=/V1!EMULATEUR CPC!
// Définition du sous système  !CONFIG!=/V2!PC-CPC!
// Définition du sous ensemble !CONFIG!=/V3!Chips!
// Définition du module        !CONFIG!=/V4!CPU Z80!
/******************************************************************************/

/********************************************************* !NAME! **************
* !./FLE!
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\Fichiers
********************************************************** !0! *****************
* ------------------------------------------------------------------------------
*  Fichier     :                       | Version : 0.1z
* ------------------------------------------------------------------------------
*  Date        : 05/11/2002            | Auteur  : L.DEPLANQUE
* ------------------------------------------------------------------------------
*  Description : Définitions pour le module Z80.C
*
* ------------------------------------------------------------------------------
*  Historique  :
*           Date           |         Auteur          |       Description
* ------------------------------------------------------------------------------
*  05/11/2002              | L.DEPLANQUE             | creation
* ------------------------------------------------------------------------------
*  30/03/2004              | L.DEPLANQUE             | Version 0.1z :
*                          |                         | Optimisations fonctions
*                          |                         | émulation Z80 : retour
*                          |                         | du nombre de cycles
*                          |                         | plutôt que incrément
*                          |                         | variable globale
* ------------------------------------------------------------------------------
********************************************************** !END! **************/


#ifndef Z80_H
#define Z80_H


#define     BIT0        0x01
#define     BIT1        0x02
#define     BIT2        0x04
#define     BIT3        0x08
#define     BIT4        0x10
#define     BIT5        0x20
#define     BIT6        0x40
#define     BIT7        0x80

//
// Flags Z80
//
#define     FLAG_0      0x00
#define     FLAG_C      0x01
#define     FLAG_N      0x02
#define     FLAG_V      0x04
#define     FLAG_3      0x08
#define     FLAG_H      0x10
#define     FLAG_5      0x20
#define     FLAG_Z      0x40
#define     FLAG_S      0x80


/********************************************************* !NAME! **************
* Nom : Registre
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Structures
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Structure d'un registre Z80
*
********************************************************** !0! ****************/
typedef union {
    USHORT Word;
    struct {
        UBYTE Low;
        UBYTE High;
        } Byte;
    } Registre;

typedef int ( * pfct )( void );

#define     RegAF           Cpc.AF.Word
#define     RegBC           Cpc.BC.Word
#define     RegDE           Cpc.DE.Word
#define     RegHL           Cpc.HL.Word

#define     Reg_AF          Cpc._AF.Word
#define     Reg_BC          Cpc._BC.Word
#define     Reg_DE          Cpc._DE.Word
#define     Reg_HL          Cpc._HL.Word

#define     RegSP           Cpc.SP.Word
#define     RegPC           Cpc.PC.Word
#define     RegIX           Cpc.IX.Word
#define     RegIY           Cpc.IY.Word
#define     RegIR           Cpc.IR.Word

#define     RegA            Cpc.AF.Byte.High
#define     FLAGS           Cpc.AF.Byte.Low
#define     RegB            Cpc.BC.Byte.High
#define     RegC            Cpc.BC.Byte.Low
#define     RegD            Cpc.DE.Byte.High
#define     RegE            Cpc.DE.Byte.Low
#define     RegH            Cpc.HL.Byte.High
#define     RegL            Cpc.HL.Byte.Low
#define     RegI            Cpc.IR.Byte.High
#define     RegR            Cpc.IR.Byte.Low
#define     RegIXH          Cpc.IX.Byte.High
#define     RegIXL          Cpc.IX.Byte.Low
#define     RegIYH          Cpc.IY.Byte.High
#define     RegIYL          Cpc.IY.Byte.Low

UBYTE Peek8Ext( USHORT adr );

void Poke8Ext( USHORT adr, UBYTE val );

int ExecInstZ80( void );

int Z80_NMI( void );

void ResetZ80( void );

void PUSH( USHORT Reg );

int RET( void );


#endif
