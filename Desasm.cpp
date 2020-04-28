/******************************************************************************/
/* Configuration pour l'archivage des différents éléments du fichier source   */
/******************************************************************************/
// !CONFIG!=/L/* /R/* /W"* Nom : "
// Définition du système       !CONFIG!=/V1!EMULATEUR CPC!
// Définition du sous système  !CONFIG!=/V2!PC-CPC!
// Définition du sous ensemble !CONFIG!=/V3!Accessoires!
// Définition du module        !CONFIG!=/V4!Dessassembleur!
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
*  Fichier     : DESASM.C              | Version : 0.1at
* ------------------------------------------------------------------------------
*  Date        : 05/11/2002            | Auteur  : L.DEPLANQUE
* ------------------------------------------------------------------------------
*  Description : Dessassemblage d'instructions
*
* ------------------------------------------------------------------------------
*  Historique  :
*           Date           |         Auteur          |       Description
* ------------------------------------------------------------------------------
*  05/11/2002              | L.DEPLANQUE             | creation
* ------------------------------------------------------------------------------
*  20/09/2006              | L.DEPLANQUE             | Version 0.1am :
*                          |                         | Ajout profiling
* ------------------------------------------------------------------------------
*  29/06/2011              | L.DEPLANQUE             | Version 0.1at :
*                          |                         | Correction pb sur instr.
*                          |                         | #DD #CB et #FD #CB
* ------------------------------------------------------------------------------
********************************************************** !END! **************/


#include  <string.h>

#include  "Types.h"
#include  "Z80.h"


#define BUF_OFFSET  8
#define BUF_SIZE    64


/********************************************************* !NAME! **************
* Nom : TabInstrCB
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Descriptions des instructions après suffixe CB
*
********************************************************** !0! ****************/
static char * TabInstrCB[ 256 ] =
    {
    "RLC B",        "RLC C",        "RLC D",        "RLC E",
    "RLC H",        "RLC L",        "RLC (HL)",     "RLC A",
    "RRC B",        "RRC C",        "RRC D",        "RRC E",
    "RRC H",        "RRC L",        "RRC (HL)",     "RRC A",
    "RL B",         "RL C",         "RL D",         "RL E",
    "RL H",         "RL L",         "RL (HL)",      "RL A",
    "RR B",         "RR C",         "RR D",         "RR E",
    "RR H",         "RR L",         "RR (HL)",      "RR A",
    "SLA B",        "SLA C",        "SLA D",        "SLA E",
    "SLA H",        "SLA L",        "SLA (HL)",     "SLA A",
    "SRA B",        "SRA C",        "SRA D",        "SRA E",
    "SRA H",        "SRA L",        "SRA (HL)",     "SRA A",
    "SLL B",        "SLL C",        "SLL D",        "SLL E",
    "SLL H",        "SLL L",        "SLL (HL)",     "SLL A",
    "SRL B",        "SRL C",        "SRL D",        "SRL E",
    "SRL H",        "SRL L",        "SRL (HL)",     "SRL A",
    "BIT 0,B",      "BIT 0,C",      "BIT 0,D",      "BIT 0,E", 
    "BIT 0,H",      "BIT 0,L",      "BIT 0,(HL)",   "BIT 0,A", 
    "BIT 1,B",      "BIT 1,C",      "BIT 1,D",      "BIT 1,E", 
    "BIT 1,H",      "BIT 1,L",      "BIT 1,(HL)",   "BIT 1,A", 
    "BIT 2,B",      "BIT 2,C",      "BIT 2,D",      "BIT 2,E", 
    "BIT 2,H",      "BIT 2,L",      "BIT 2,(HL)",   "BIT 2,A", 
    "BIT 3,B",      "BIT 3,C",      "BIT 3,D",      "BIT 3,E", 
    "BIT 3,H",      "BIT 3,L",      "BIT 3,(HL)",   "BIT 3,A", 
    "BIT 4,B",      "BIT 4,C",      "BIT 4,D",      "BIT 4,E", 
    "BIT 4,H",      "BIT 4,L",      "BIT 4,(HL)",   "BIT 4,A", 
    "BIT 5,B",      "BIT 5,C",      "BIT 5,D",      "BIT 5,E", 
    "BIT 5,H",      "BIT 5,L",      "BIT 5,(HL)",   "BIT 5,A", 
    "BIT 6,B",      "BIT 6,C",      "BIT 6,D",      "BIT 6,E", 
    "BIT 6,H",      "BIT 6,L",      "BIT 6,(HL)",   "BIT 6,A", 
    "BIT 7,B",      "BIT 7,C",      "BIT 7,D",      "BIT 7,E", 
    "BIT 7,H",      "BIT 7,L",      "BIT 7,(HL)",   "BIT 7,A", 
    "RES 0,B",      "RES 0,C",      "RES 0,D",      "RES 0,E", 
    "RES 0,H",      "RES 0,L",      "RES 0,(HL)",   "RES 0,A", 
    "RES 1,B",      "RES 1,C",      "RES 1,D",      "RES 1,E", 
    "RES 1,H",      "RES 1,L",      "RES 1,(HL)",   "RES 1,A", 
    "RES 2,B",      "RES 2,C",      "RES 2,D",      "RES 2,E", 
    "RES 2,H",      "RES 2,L",      "RES 2,(HL)",   "RES 2,A", 
    "RES 3,B",      "RES 3,C",      "RES 3,D",      "RES 3,E", 
    "RES 3,H",      "RES 3,L",      "RES 3,(HL)",   "RES 3,A", 
    "RES 4,B",      "RES 4,C",      "RES 4,D",      "RES 4,E", 
    "RES 4,H",      "RES 4,L",      "RES 4,(HL)",   "RES 4,A", 
    "RES 5,B",      "RES 5,C",      "RES 5,D",      "RES 5,E", 
    "RES 5,H",      "RES 5,L",      "RES 5,(HL)",   "RES 5,A", 
    "RES 6,B",      "RES 6,C",      "RES 6,D",      "RES 6,E", 
    "RES 6,H",      "RES 6,L",      "RES 6,(HL)",   "RES 6,A", 
    "RES 7,B",      "RES 7,C",      "RES 7,D",      "RES 7,E", 
    "RES 7,H",      "RES 7,L",      "RES 7,(HL)",   "RES 7,A", 
    "SET 0,B",      "SET 0,C",      "SET 0,D",      "SET 0,E", 
    "SET 0,H",      "SET 0,L",      "SET 0,(HL)",   "SET 0,A", 
    "SET 1,B",      "SET 1,C",      "SET 1,D",      "SET 1,E", 
    "SET 1,H",      "SET 1,L",      "SET 1,(HL)",   "SET 1,A", 
    "SET 2,B",      "SET 2,C",      "SET 2,D",      "SET 2,E", 
    "SET 2,H",      "SET 2,L",      "SET 2,(HL)",   "SET 2,A", 
    "SET 3,B",      "SET 3,C",      "SET 3,D",      "SET 3,E", 
    "SET 3,H",      "SET 3,L",      "SET 3,(HL)",   "SET 3,A", 
    "SET 4,B",      "SET 4,C",      "SET 4,D",      "SET 4,E", 
    "SET 4,H",      "SET 4,L",      "SET 4,(HL)",   "SET 4,A", 
    "SET 5,B",      "SET 5,C",      "SET 5,D",      "SET 5,E", 
    "SET 5,H",      "SET 5,L",      "SET 5,(HL)",   "SET 5,A", 
    "SET 6,B",      "SET 6,C",      "SET 6,D",      "SET 6,E", 
    "SET 6,H",      "SET 6,L",      "SET 6,(HL)",   "SET 6,A", 
    "SET 7,B",      "SET 7,C",      "SET 7,D",      "SET 7,E", 
    "SET 7,H",      "SET 7,L",      "SET 7,(HL)",   "SET 7,A" 
    };


/********************************************************* !NAME! **************
* Nom : TabInstrED
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Descriptions des instructions après suffixe ED
*
********************************************************** !0! ****************/
static char * TabInstrED[ 256 ] =
    {
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    "IN B,(C)",     "OUT (C),B",    "SBC HL,BC",    "LD (#nnnn),BC", 
    "NEG",          "RETN",         "IM 0",         "LD I,A", 
    "IN C,(C)",     "OUT (C),C",    "ADC HL,BC",    "LD BC,(#nnnn)",
    "NEG",          "RETI",         NULL,           "LD R,A", 
    "IN D,(C)",     "OUT (C),D",    "SBC HL,DE",    "LD (#nnnn),DE", 
    "NEG",          NULL,           "IM 1",         "LD A,I", 
    "IN E,(C)",     "OUT (C),E",    "ADC HL,DE",    "LD DE,(#nnnn)",
    "NEG",          NULL,           "IM 2",         "LD A,R", 
    "IN H,(C)",     "OUT (C),H",    "SBC HL,HL",    NULL,
    "NEG",          NULL,           NULL,           "RRD",
    "IN L,(C)",     "OUT (C),L",    "ADC HL,HL",    NULL,
    "NEG",          NULL,           NULL,           "RLD",
    "IN F,(C)",     "OUT (C),0",    "SBC HL,SP",    "LD (#nnnn),SP", 
    "NEG",          NULL,           NULL,           NULL,
    "IN A,(C)",     "OUT (C),A",    "ADC HL,SP",    "LD SP,(#nnnn)",
    "NEG",          NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    "LDI",          "CPI",          "INI",          "OUTI",
    NULL,           NULL,           NULL,           NULL,
    "LDD",          "CPD",          "IND",          "OUTD",
    NULL,           NULL,           NULL,           NULL,
    "LDIR",         "CPIR",         "INIR",         "OTIR",
    NULL,           NULL,           NULL,           NULL,
    "LDDR",         "CPDR",         "INDR",         "OTDR",
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL
    };


/********************************************************* !NAME! **************
* Nom : TabInstrXX
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Descriptions des instructions après suffixe DD ou FD
*
********************************************************** !0! ****************/
static char * TabInstrXX[ 256 ] =
    {
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           "ADD IX,BC",    NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           "ADD IX,DE",    "LD A,(DE)",    NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           "LD IX,#nnnn",  "LD (#nnnn),IX","INC IX",
    "INC IXh",      "DEC IXh",      "LD IXh,#nn",   NULL,
    NULL,           "ADD IX,HL",    "LD IX,(#nnnn)","DEC IX",
    "INC IXl",      "DEC IXl",      "LD IXl,#nn",   NULL,
    NULL,           NULL,           NULL,           NULL,
    "INC (IX+#nn)", "DEC (IX+#nn)", "LD (IX+#nn),#nn",NULL,
    NULL,           "ADD IX,SP",    NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    "LD B,IXh",     "LD B,IXl",     "LD B,(IX+#nn)",NULL,
    NULL,           NULL,           NULL,           NULL,
    "LD C,IXh",     "LD C,IXl",     "LD C,(IX+#nn)",NULL,
    NULL,           NULL,           NULL,           NULL,
    "LD D,IXh",     "LD D,IXl",     "LD D,(IX+#nn)",NULL,
    NULL,           NULL,           NULL,           NULL,
    "LD E,IXh",     "LD E,IXl",     "LD E,(IX+#nn)",NULL,
    NULL,           NULL,           NULL,           NULL,
    "LD IXh,IXh",   "LD IXh,IXl",   "LD H,(IX+#nn)","LD IXh,A",
    NULL,           NULL,           NULL,           NULL,
    "LD IXl,IXh",   "LD IXl,IXl",   "LD L,(IX+#nn)","LD IXl,A",
    "LD (IX+#nn),B","LD (IX+#nn),C","LD (IX+#nn),D","LD (IX+#nn),E",
    "LD (IX+#nn),H","LD (IX+#nn),L",NULL,           "LD (IX+#nn),A",
    NULL,           NULL,           NULL,           NULL,
    "LD A,IXh",     "LD A,IXl",     "LD A,(IX+#nn)",NULL,
    NULL,           NULL,           NULL,           NULL,
    "ADD A,IXh",    "ADD A,IXl",    "ADD A,(IX+#nn)",NULL,
    NULL,           NULL,           NULL,           NULL,
    "ADC A,IXh",    "ADC A,IXl",    "ADC A,(IX+#nn)",NULL,
    NULL,           NULL,           NULL,           NULL,
    "SUB IXh",      "SUB IXl",      "SUB (IX+#nn)", NULL,
    NULL,           NULL,           NULL,           NULL,
    "SBC A,IXh",    "SBC A,IXl",    "SBC A,(IX+#nn)",NULL,
    NULL,           NULL,           NULL,           NULL,
    "AND IXh",      "AND IXl",      "AND (IX+#nn)", NULL,
    NULL,           NULL,           NULL,           NULL,
    "XOR IXh",      "XOR IXl",      "XOR (IX+#nn)", NULL,
    NULL,           NULL,           NULL,           NULL,
    "OR IXh",       "OR IXl",       "OR (IX+#nn)",  NULL,
    NULL,           NULL,           NULL,           NULL,
    "CP IXh",       "CP IXl",       "CP (IX+#nn)",  NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           "POP IX",       NULL,           "EX (SP),IX",
    NULL,           "PUSH IX",      NULL,           NULL,
    NULL,           "JP (IX)",      NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           "LD SP,IX",     NULL,           NULL,
    NULL,           NULL,           NULL,           NULL
    };


/********************************************************* !NAME! **************
* Nom : TabInstrXXCB
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Descriptions des instructions après suffixe DD CB ou FD CB
*
********************************************************** !0! ****************/
static char * TabInstrXXCB[ 256 ] =
    {
    NULL, NULL, NULL, NULL, NULL, NULL, "RLC (IX+#nn)", "LD A,RLC (IX+#nn)",
    NULL, NULL, NULL, NULL, NULL, NULL, "RRC (IX+#nn)", "LD A,RRC (IX+#nn)",
    NULL, NULL, NULL, NULL, NULL, NULL, "RL (IX+#nn)", "LD A,RL (IX+#nn)",
    NULL, NULL, NULL, NULL, NULL, NULL, "RR (IX+#nn)", "LD A,RR (IX+#nn)",
    NULL, NULL, NULL, NULL, NULL, NULL, "SLA (IX+#nn)", "LD A,SLA (IX+#nn)",
    NULL, NULL, NULL, NULL, NULL, NULL, "SRA (IX+#nn)", "LD A,SRA (IX+#nn)",
    NULL, NULL, NULL, NULL, NULL, NULL, "SLL (IX+#nn)", "LD A,SLL (IX+#nn)",
    NULL, NULL, NULL, NULL, NULL, NULL, "SRL (IX+#nn)", "LD A,SRL (IX+#nn)",
    NULL, NULL, NULL, NULL, NULL, NULL, "BIT 0,(IX+#nn)", NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, "BIT 1,(IX+#nn)", NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, "BIT 2,(IX+#nn)", NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, "BIT 3,(IX+#nn)", NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, "BIT 4,(IX+#nn)", NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, "BIT 5,(IX+#nn)", NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, "BIT 6,(IX+#nn)", NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, "BIT 7,(IX+#nn)", NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, "RES 0,(IX+#nn)", NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, "RES 1,(IX+#nn)", NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, "RES 2,(IX+#nn)", NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, "RES 3,(IX+#nn)", NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, "RES 4,(IX+#nn)", NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, "RES 5,(IX+#nn)", NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, "RES 6,(IX+#nn)", NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, "RES 7,(IX+#nn)", NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, "SET 0,(IX+#nn)", NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, "SET 1,(IX+#nn)", NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, "SET 2,(IX+#nn)", NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, "SET 3,(IX+#nn)", NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, "SET 4,(IX+#nn)", NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, "SET 5,(IX+#nn)", NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, "SET 6,(IX+#nn)", NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, "SET 7,(IX+#nn)", NULL
    };


/********************************************************* !NAME! **************
* Nom : TabInstr
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Descriptions des instructions "standard"
*
********************************************************** !0! ****************/
static char * TabInstr[ 256 ] =
    {
    "NOP",          "LD BC,#nnnn",  "LD (BC),A",    "INC BC",
    "INC B",        "DEC B",        "LD B,#nn",     "RLCA",
    "EX AF, AF",    "ADD HL,BC",     "LD A,(BC)",   "DEC BC",
    "INC C",        "DEC C",        "LD C,#nn",     "RRCA",
    "DJNZ #eeee",   "LD DE,#nnnn",  "LD (DE),A",    "INC DE",
    "INC D",        "DEC D",        "LD D,#nn",     "RLA",
    "JR #eeee",     "ADD HL,DE",    "LD A,(DE)",    "DEC DE",
    "INC E",        "DEC E",        "LD E,#nn",     "RRA",
    "JR NZ,#eeee",  "LD HL,#nnnn",  "LD (#nnnn),HL","INC HL",
    "INC H",        "DEC H",        "LD H,#nn",     "DAA",
    "JR Z,#eeee",   "ADD HL,HL",    "LD HL,(#nnnn)","DEC HL",
    "INC L",        "DEC L",        "LD L,#nn",     "CPL",
    "JR NC,#eeee",  "LD SP,#nnnn",  "LD (#nnnn),A", "INC SP",
    "INC (HL)",     "DEC (HL)",     "LD (HL),#nn",  "SCF",
    "JR C,#eeee",   "ADD HL,SP",    "LD A,(#nnnn)", "DEC SP",
    "INC A",        "DEC A",        "LD A,#nn",     "CCF",
    "LD B,B",       "LD B,C",       "LD B,D",       "LD B,E",
    "LD B,H",       "LD B,L",       "LD B,(HL)",    "LD B,A",
    "LD C,B",       "LD C,C",       "LD C,D",       "LD C,E",
    "LD C,H",       "LD C,L",       "LD C,(HL)",    "LD C,A",
    "LD D,B",       "LD D,C",       "LD D,D",       "LD D,E",
    "LD D,H",       "LD D,L",       "LD D,(HL)",    "LD D,A",
    "LD E,B",       "LD E,C",       "LD E,D",       "LD E,E",
    "LD E,H",       "LD E,L",       "LD E,(HL)",    "LD E,A",
    "LD H,B",       "LD H,C",       "LD H,D",       "LD H,E",
    "LD H,H",       "LD H,L",       "LD H,(HL)",    "LD H,A",
    "LD L,B",       "LD L,C",       "LD L,D",       "LD L,E",
    "LD L,H",       "LD L,L",       "LD L,(HL)",    "LD L,A",
    "LD (HL),B",    "LD (HL),C",    "LD (HL),D",    "LD (HL),E",
    "LD (HL),H",    "LD (HL),L",    "HALT",         "LD (HL),A",
    "LD A,B",       "LD A,C",       "LD A,D",       "LD A,E",
    "LD A,H",       "LD A,L",       "LD A,(HL)",    "LD A,A",
    "ADD A,B",      "ADD A,C",      "ADD A,D",      "ADD A,E",
    "ADD A,H",      "ADD A,L",      "ADD A,(HL)",   "ADD A,A",
    "ADC A,B",      "ADC A,C",      "ADC A,D",      "ADC A,E",
    "ADC A,H",      "ADC A,L",      "ADC A,(HL)",   "ADC A,A",
    "SUB B",        "SUB C",        "SUB D",        "SUB E",
    "SUB H",        "SUB L",        "SUB (HL)",     "SUB A",
    "SBC A,B",      "SBC A,C",      "SBC A,D",      "SBC A,E",
    "SBC A,H",      "SBC A,L",      "SBC A,(HL)",   "SBC A,A",
    "AND B",        "AND C",        "AND D",        "AND E",
    "AND H",        "AND L",        "AND (HL)",     "AND A",
    "XOR B",        "XOR C",        "XOR D",        "XOR E",
    "XOR H",        "XOR L",        "XOR (HL)",     "XOR A",
    "OR B",         "OR C",         "OR D",         "OR E",
    "OR H",         "OR L",         "OR (HL)",      "OR A",
    "CP B",         "CP C",         "CP D",         "CP E",
    "CP H",         "CP L",         "CP (HL)",      "CP A",
    "RET NZ",       "POP BC",       "JP NZ,#nnnn",  "JP #nnnn",
    "CALL NZ,#nnnn","PUSH BC",      "ADD A,#nn",    "RST #00",
    "RET Z",        "RET",          "JP Z,#nnnn",   NULL,
    "CALL Z,#nnnn", "CALL #nnnn",   "ADC A,#nn",    "RST #08",
    "RET NC",       "POP DE",       "JP NC,#nnnn",  "OUT (#nn),A",
    "CALL NC,#nnnn","PUSH DE",      "SUB #nn",      "RST #10",
    "RET C",        "EXX",          "JP C,#nnnn",   "IN A,(#nn)",
    "CALL C,#nnnn", NULL,           "SBC A,#nn",    "RST #18",
    "RET PO",       "POP HL",       "JP PO,#nnnn",  "EX (SP),HL",
    "CALL PO,#nnnn","PUSH HL",      "AND #nn",      "RST #20",
    "RET PE",       "JP (HL)",      "JP PE,#nnnn",  "EX DE,HL",
    "CALL PE,#nnnn",NULL,           "XOR #nn",      "RST #28",
    "RET P",        "POP AF",       "JP P,#nnnn",   "DI",
    "CALL P,#nnnn", "PUSH AF",      "OR #nn",       "RST #30",
    "RET M",        "LD SP,HL",     "JP M,#nnnn",   "EI",
    "CALL M,#nnnn", NULL,           "CP #nn",       "RST #38"
    };


static char TabDigit[ 17 ] = "0123456789ABCDEF";


void Hex4( char Chaine[],  int Valeur )
{
    Chaine[ 0 ] = TabDigit[ ( Valeur >> 12 ) & 15 ];
    Chaine[ 1 ] = TabDigit[ ( Valeur >> 8 ) & 15 ];
    Chaine[ 2 ] = TabDigit[ ( Valeur >> 4 ) & 15 ];
    Chaine[ 3 ] = TabDigit[ Valeur & 15 ];
}


void Hex2( char Chaine[], int Valeur )
{
    Chaine[ 0 ] = TabDigit[ ( Valeur >> 4 ) & 15 ];
    Chaine[ 1 ] = TabDigit[ Valeur & 15 ];
}


void InitDesasm( void )
{
    for ( int i = 0; i < 256; i++ )
        if ( TabInstrXX[ i ] == NULL )
            TabInstrXX[ i ] = TabInstr[ i ];
}


int SetLigne( USHORT Adr, char * Buffer )
{
    int Instr, Inst2 = 0, Inst3, Ad16, PosBuf = 0;
    char * Chaine, * p;
    static char Inst[ 1024 ];
    char Ad8;

    memset( Buffer, ' ', BUF_SIZE );

    Hex4( Buffer, Adr );
    Instr = Peek8Ext( Adr++ );
    Hex2( &Buffer[ BUF_OFFSET + PosBuf ], Instr );
    PosBuf += 3;
    Chaine = TabInstr[ Instr ];
    if ( Instr == 0xCB )
        {
        Inst2 = Peek8Ext( Adr++ );
        Hex2( &Buffer[ BUF_OFFSET + PosBuf ], Inst2 );
        Chaine = TabInstrCB[ Inst2 ];
        PosBuf += 3;
        }
    else
        if ( Instr == 0xED )
            {
            Inst2 = Peek8Ext( Adr++ );
            Hex2( &Buffer[ BUF_OFFSET + PosBuf ], Inst2 );
            PosBuf += 3;
            Chaine = TabInstrED[ Inst2 ];
            }
        else
            if ( Instr == 0xDD || Instr == 0xFD )
                {
                Inst2 = Peek8Ext( Adr++ );
                Hex2( &Buffer[ BUF_OFFSET + PosBuf ], Inst2 );
                PosBuf += 3;
                if ( Inst2 == 0xCB )
                    {
                    Inst3 = Peek8Ext( ( USHORT )( Adr + 1 ) );
                    Chaine = TabInstrXXCB[ Inst3 ];
                    }
                else
                    Chaine = TabInstrXX[ Inst2 ];
                }

    if ( ! Chaine )
        Chaine = "????";

    strcpy( Inst, Chaine );
    if ( Instr == 0xFD )
        {
        p = strstr( Inst, "IX" );
        if ( p )
            p[ 1 ] = 'Y';
        }
    p = strstr( Inst, "nnnn" );
    Ad16 = Peek8Ext( Adr );
    Ad16 += Peek8Ext( ( USHORT )( Adr + 1 ) ) << 8;
    Ad8 = ( char )Ad16;
    if ( p )
        {
        Hex4( p, Ad16 );
        Buffer[ BUF_OFFSET + PosBuf++ ] = p[ 2 ];
        Buffer[ BUF_OFFSET + PosBuf++ ] = p[ 3 ];
        PosBuf++;
        Buffer[ BUF_OFFSET + PosBuf++ ] = p[ 0 ];
        Buffer[ BUF_OFFSET + PosBuf ] = p[ 1 ];
        Adr += 2;
        }
    else
        {
        p = strstr( Inst, "nn" );
        if ( p )
            {
            Adr++;
            Hex2( p, Ad16 );
            Buffer[ BUF_OFFSET + PosBuf++ ] = * p++;
            Buffer[ BUF_OFFSET + PosBuf++ ] = * p++;
            PosBuf++;
            if ( Inst2 == 0xCB )
                {
                Hex2( &Buffer[ BUF_OFFSET + PosBuf ], Ad16 >> 8 );
                Adr++;
                PosBuf += 3;
                }
            p = strstr( Inst, "nn" );
            if ( p )
                {
                Hex2( p, Ad16 >> 8 );
                Buffer[ BUF_OFFSET + PosBuf++ ] = * p++;
                Buffer[ BUF_OFFSET + PosBuf++ ] = * p++;
                }
            }
        else
            {
            p = strstr( Inst, "eeee" );
            if ( p )
                {
                Hex4( p, ++Adr + Ad8  );
                Hex2( &Buffer[ BUF_OFFSET + PosBuf ], Ad8 );
                }
            }
        }
    memcpy( &Buffer[ BUF_OFFSET + 20 ], Inst, strlen( Inst ) );
    return( Adr );
}
