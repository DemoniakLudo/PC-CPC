#include  <afxwin.h>
#include  <stdio.h>

#include  "Resource.h"
#include  "AsmOutput.h"
#include  "Types.h"
#include  "Z80.h"
#include  "VGA.h"
#include  "Crtc.h"
#include  "Snapshot.h"


#define     MODULENAME      "Assemble"


#define LENFN           128

#define COMMENT         ';'
#define LINCOM          '*'
#define LABSEP          ':'
#define STRSEP          '"'
#define ENDFILE         "END"
#define MAXLINE         128
#define SYMSIZE         32
#define INCNEST         5
#define IFNEST          5
#define MAX_SYM         1024
#define OPCARRAY        0x10000


typedef struct 
    {
    char * name;
    int ( * fun )( int, int );
    int c1, c2;
    } StOpc;

typedef struct
    {
    char *name;
    int sym;
    } StOpe;

typedef struct
    {
    char name[ 128 ];
    int  val;
    } StSym;

static int NbSymboles;

static CMyListBox * liste;

#define REGB            0
#define REGC            1
#define REGD            2
#define REGE            3
#define REGH            4
#define REGL            5
#define REGIHL          6
#define REGA            7
#define REGI            8
#define REGR            9
#define REGAF           10
#define REGBC           11
#define REGDE           12
#define REGHL           13
#define REGIX           0xDD
#define REGIY           0xFD
#define REGSP           16
#define REGIBC          17
#define REGIDE          18
#define REGIIX          19
#define REGIIY          20
#define REGISP          21
#define FLGNC           30
#define FLGNZ           31
#define FLGZ            32
#define FLGM            33
#define FLGP            34
#define FLGPE           35
#define FLGPO           36
#define REGLX           40
#define REGLY           41
#define REGHX           42
#define REGHY           43
#define NOOPERA         98
#define NOREG           99

#define E_ILLOPC        0
#define E_ILLOPE        1
#define E_MISOPE        2
#define E_MULSYM        3
#define E_UNDSYM        4
#define E_VALOUT        5
#define E_MISPAR        6
#define E_MISHYP        7
#define E_MEMOVR        8
#define E_MISIFF        9
#define E_IFNEST        10
#define E_MISEIF        11
#define E_INCNEST       12

#define F_OUTMEM        0
#define F_FOPEN         1

#define OPEDEC          1
#define OPEHEX          2
#define OPEOCT          3
#define OPEBIN          4
#define OPESUB          5
#define OPEADD          6
#define OPEMUL          7
#define OPEDIV          8
#define OPEMOD          9
#define OPESHL          10
#define OPESHR          11
#define OPELOR          12
#define OPELAN          13
#define OPEXOR          14
#define OPECOM          15
#define OPESYM          99

static char  *srcfn,
     line[ MAXLINE ],
     tmp[ MAXLINE ],
     label[ SYMSIZE+1 ],
     opcode[ MAXLINE ],
     operand[ MAXLINE ];

static UBYTE ops[ OPCARRAY ];

static int  pc, run,
     pass,
     iflevel,
     gencode,
     errors,
     sd_val;

static FILE *srcfp;

static unsigned c_line;

static BOOL DoList = TRUE;

static StSym symtab[ MAX_SYM ];

static int op_1b( int,int ), op_2b( int,int ), op_pupo( int,int ), op_ex( int,int ), op_ld( int,int );
static int op_call( int,int ), op_ret( int,int ), op_jp( int,int ), op_jr( int,int ), op_djnz( int,int ), op_rst( int,int );
static int op_add( int,int ), op_adc( int,int ), op_sub( int,int ), op_sbc( int,int ), op_cp( int,int );
static int op_inc( int,int ), op_dec( int,int ), op_or( int,int ), op_xor( int,int ), op_and( int,int );
static int op_rl( int,int ), op_rr( int,int ), op_sla( int,int ), op_sra( int,int ), op_srl( int,int ), op_rlc( int,int ), op_rrc( int,int );
static int op_out( int,int ), op_in( int,int ), op_im( int,int );
static int op_org( int,int ), op_dl( int,int ), op_equ( int,int ), op_run( int, int );
static int op_ds( int,int ), op_db( int,int ), op_dw( int,int ), op_dm( int,int );
static int op_misc( int,int );
static int op_incbin( int, int );
static int op_cond( int,int );
static int op_cbbit( int, int );

static StOpc opctab[] = 
{
    { "ADC",        op_adc,         0,    0    },
    { "ADD",        op_add,         0,    0    },
    { "AND",        op_and,         0,    0    },
    { "BANK",       op_misc,        3,    0    },
    { "BIT",        op_cbbit,       0x40, 0    },
    { "CALL",       op_call,        0,    0    },
    { "CCF",        op_1b,          0x3f, 0    },
    { "CP",         op_cp,          0,    0    },
    { "CPD",        op_2b,          0xed, 0xa9 },
    { "CPDR",       op_2b,          0xed, 0xb9 },
    { "CPI",        op_2b,          0xed, 0xa1 },
    { "CPIR",       op_2b,          0xed, 0xb1 },
    { "CPL",        op_1b,          0x2f, 0    },
    { "DAA",        op_1b,          0x27, 0    },
    { "DB",         op_db,          0,    0    },
    { "DEC",        op_dec,         0,    0    },
    { "DEFB",       op_db,          0,    0    },
    { "DEFL",       op_dl,          0,    0    },
    { "DEFM",       op_dm,          0,    0    },
    { "DEFS",       op_ds,          0,    0    },
    { "DEFW",       op_dw,          0,    0    },
    { "DI",         op_1b,          0xf3, 0    },
    { "DJNZ",       op_djnz,        0,    0    },
    { "DL",         op_dl,          0,    0    },
    { "DM",         op_dm,          0,    0    },
    { "DS",         op_ds,          0,    0    },
    { "DW",         op_dw,          0,    0    },
    { "EI",         op_1b,          0xfb, 0    },
    { "ELSE",       op_cond,        98,   0    },
    { "ENDIF",      op_cond,        99,   0    },
    { "EQU",        op_equ,         0,    0    },
    { "EX",         op_ex,          0,    0    },
    { "EXX",        op_1b,          0xd9, 0    },
    { "HALT",       op_1b,          0x76, 0    },
    { "IFDEF",      op_cond,        1,    0    },
    { "IFEQ",       op_cond,        3,    0    },
    { "IFNDEF",     op_cond,        2,    0    },
    { "IFNEQ",      op_cond,        4,    0    },
    { "IM",         op_im,          0,    0    },
    { "IN",         op_in,          0,    0    },
    { "INC",        op_inc,         0,    0    },
    { "INCBIN",     op_incbin,      0,    0    },
    { "INCLUDE",    op_misc,        4,    0    },
    { "IND",        op_2b,          0xed, 0xaa },
    { "INDR",       op_2b,          0xed, 0xba },
    { "INI",        op_2b,          0xed, 0xa2 },
    { "INIR",       op_2b,          0xed, 0xb2 },
    { "JP",         op_jp,          0,    0    },
    { "JR",         op_jr,          0,    0    },
    { "LD",         op_ld,          0,    0    },
    { "LDD",        op_2b,          0xed, 0xa8 },
    { "LDDR",       op_2b,          0xed, 0xb8 },
    { "LDI",        op_2b,          0xed, 0xa0 },
    { "LDIR",       op_2b,          0xed, 0xb0 },
    { "LIST",       op_misc,        1,    0    },
    { "NEG",        op_2b,          0xed, 0x44 },
    { "NOLIST",     op_misc,        2,    0    },
    { "NOP",        op_1b,          0,    0    },
    { "OR",         op_or,          0,    0    },
    { "ORG",        op_org,         0,    0    },
    { "OTDR",       op_2b,          0xed, 0xbb },
    { "OTIR",       op_2b,          0xed, 0xb3 },
    { "OUT",        op_out,         0,    0    },
    { "OUTD",       op_2b,          0xed, 0xab },
    { "OUTI",       op_2b,          0xed, 0xa3 },
    { "POP",        op_pupo,        0,    0    },
    { "PUSH",       op_pupo,        4,    0    },
    { "READ",       op_misc,        4,    0    },
    { "RES",        op_cbbit,       0x80, 0    },
    { "RET",        op_ret,         0,    0    },
    { "RETI",       op_2b,          0xed, 0x4d },
    { "RETN",       op_2b,          0xed, 0x45 },
    { "RL",         op_rl,          0,    0    },
    { "RLA",        op_1b,          0x17, 0    },
    { "RLC",        op_rlc,         0,    0    },
    { "RLCA",       op_1b,          0x07, 0    },
    { "RLD",        op_2b,          0xed, 0x6f },
    { "RR",         op_rr,          0,    0    },
    { "RRA",        op_1b,          0x1f, 0    },
    { "RRC",        op_rrc,         0,    0    },
    { "RRCA",       op_1b,          0x0f, 0    },
    { "RRD",        op_2b,          0xed, 0x67 },
    { "RST",        op_rst,         0,    0    },
    { "RUN",        op_run,         0,    0    },
    { "SBC",        op_sbc,         0,    0    },
    { "SCF",        op_1b,          0x37, 0    },
    { "SET",        op_cbbit,       0xC0, 0    },
    { "SLA",        op_sla,         0,    0    },
    { "SRA",        op_sra,         0,    0    },
    { "SRL",        op_srl,         0,    0    },
    { "SUB",        op_sub,         0,    0    },
    { "WRITESNA",   op_misc,        5,    0    },
    { "XOR",        op_xor,         0,    0    }
};

static int no_opcodes = sizeof( opctab ) / sizeof( StOpc );

static StOpe opetab[] = 
{
    { "(BC)",       REGIBC },
    { "(DE)",       REGIDE },
    { "(HL)",       REGIHL },
    { "(IX)",       REGIIX },
    { "(IY)",       REGIIY },
    { "(SP)",       REGISP },
    { "A",          REGA   },
    { "AF",         REGAF  },
    { "B",          REGB   },
    { "BC",         REGBC  },
    { "C",          REGC   },
    { "D",          REGD   },
    { "DE",         REGDE  },
    { "E",          REGE   },
    { "H",          REGH   },
    { "HL",         REGHL  },
    { "HX",         REGHX  },
    { "HY",         REGHY  },
    { "I",          REGI   },
    { "IX",         REGIX  },
    { "IXH",        REGHX  },
    { "IXL",        REGLX  },
    { "IY",         REGIY  },
    { "IYH",        REGHY  },
    { "IYL",        REGLY  },
    { "L",          REGL   },
    { "LX",         REGLX  },
    { "LY",         REGLY  },
    { "M",          FLGM   },
    { "NC",         FLGNC  },
    { "NZ",         FLGNZ  },
    { "P",          FLGP   },
    { "PE",         FLGPE  },
    { "PO",         FLGPO  },
    { "R",          REGR   },
    { "SP",         REGSP  },
    { "Z",          FLGZ   }
};

static int no_operands = sizeof( opetab ) / sizeof( StOpe );


static void AddList( char * Txt, char * arg1, int arg2, char * arg3, int itemVal )
{
    static char Msg[ 128 ];
    static int cnt = 0;

    sprintf( Msg, Txt, arg1, arg2, arg3 );
    int index = liste->AddString( Msg );
    if ( cnt++ > 50 )
        {
        liste->SetCurSel( liste->GetCount() - 1 );
        cnt = 0;
        }
    liste->SetItemData( index, itemVal );
}

static void fatal( int i, char *arg )
{
    static char * errmsg[] =
        {
        "out of memory : %s",
        "can't open file %s",
        };

    AddList( errmsg[ i ], arg, 0, 0, ITEM_BREAK );
}

static int asmerr( int i )
{
    static char * errmsg[] =
        {
        "illegal opcode",
        "illegal operand",
        "missing operand",
        "multiply defined symbol",
        "undefined symbol",
        "value out of range",
        "missing )",
        "missing string separator",
        "memory override",
        "missing IF",
        "IF nesting to deep",
        "missing ENDIF",
        "INCLUDE nesting to deep"
        };

    if ( pass == 2 )
        {
        AddList( "Erreur fichier %s Ligne %d : %s", srcfn, c_line, errmsg[ i ], ITEM_BREAK );
        errors++;
        }
    return( 0 );
}

static char *get_label( char * s, char * l )
{
    int i = 0;

    if ( *l != LINCOM )
        while ( !isspace( *l ) && *l != COMMENT && *l != LABSEP && i < SYMSIZE )
            {
            *s++ = ( char )( islower( *l ) ? toupper( *l++ ) : *l++ );
            i++;
            }
    *s = 0;
    return( l );
}

static char * get_opcode( char * s, char * l )
{
    if ( *l != LINCOM )
        {
        while( ! isspace( *l ) && *l != COMMENT && *l != LABSEP )
            l++;

        if ( *l == LABSEP )
            l++;

        while ( *l == ' ' || *l == '\t' )
            l++;

        while ( ! isspace( *l ) && *l != COMMENT )
            *s++ = ( char )toupper( *l++ );
        }
    *s = 0;
    return( l );
}

static char *get_arg( char *s, char *l )
{
    if ( *l != LINCOM )
        {
        while ( *l == ' ' || *l == '\t' )
            l++;

        while ( * l && *l != '\n' && *l != COMMENT ) {
            if ( isspace( *l ) )
                {
                l++;
                continue;
                }
            if ( *l != STRSEP )
                {
                *s++ = ( char )( islower( *l ) ? toupper( *l ) : *l );
                l++;
                continue;
                }
            *s++ = *l++;
            if ( *( s - 2 ) == 'F' )
                continue;

            while ( *l != STRSEP )
                {
                if ( *l == '\n' || ! *l || *l == COMMENT )
                    {
                    * s = 0;
                    return( l );
                    }
                *s++ = *l++;
                }
            *s++ = *l++;
        }
    }
    *s = 0;
    return( l );
}


static StOpc *search_op( char * op_name )
{
    int cond;

    StOpc * low = &opctab[ 0 ];
    StOpc * high = &opctab[ no_opcodes - 1 ];
    while ( low <= high ) 
        {
        StOpc * mid = low + ( high - low ) / 2;
        if ( ( cond = strcmp( op_name, mid->name ) ) < 0 )
            high = mid - 1;
        else 
            if ( cond > 0 )
                low = mid + 1;
            else
                return( mid );
        }
    return( NULL );
}


static StSym *get_sym( char * sym_name )
{
    for ( int i = 0; i < NbSymboles; i++ )
        {
        StSym * np = &symtab[ i ];
        if ( ! strcmp( sym_name, np->name ) )
            return( np );
        }
    return( NULL );
}


static int put_sym( char * sym_name, int sym_val )
{
    StSym *np;

    if ( gencode )
        {
        if ( ( np = get_sym( sym_name ) ) == NULL )
            {
            if ( NbSymboles == MAX_SYM )
                return( 1 );

            np = &symtab[ NbSymboles ];
            strcpy( symtab[ NbSymboles ].name, sym_name );
            symtab[ NbSymboles ].val = sym_val;
            NbSymboles++;
            }
        else
            np->val = sym_val;
        }
    return( 0 );
}

static void put_label( void )
{
    if ( get_sym( label ) == NULL )
        {
        if ( put_sym( label, pc ) )
            fatal( F_OUTMEM, "symbols" );
        }
    else
        asmerr( E_MULSYM );
}

static int strval( char * str )
{
    int num = 0;

    while ( *str ) 
        {
        num <<= 8;
        num += ( int ) *str++;
        }
    return( num );
}

static int isari( int c )
{
    return( ( c ) == '+' || ( c ) == '-' || ( c ) == '*' ||
           ( c ) == '/' || ( c ) == '%' || ( c ) == '<' ||
           ( c ) == '>' || ( c ) == '|' || ( c ) == '&' ||
           ( c ) == '~' || ( c ) == '^' );
}

static int get_type( char * s )
{
    if ( isdigit( *s ) )
        {
        if ( isdigit( *( s + strlen( s ) - 1 ) ) )
            return( OPEDEC );
        else if ( *( s + strlen( s ) - 1 ) == 'H' )
            return( OPEHEX );
        else if ( *( s + strlen( s ) - 1 ) == 'B' )
            return( OPEBIN );
        else if ( *( s + strlen( s ) - 1 ) == 'O' )
            return( OPEOCT );
        }
    else if ( *s == '-' )
        return( OPESUB );
    else if ( *s == '#' )
        return( OPEHEX );
    else if ( *s == '%' && ( * ( s + 1 ) == '0' || * ( s + 1 ) == '1' ) )
        return( OPEBIN );
    else if ( *s == '+' )
        return( OPEADD );
    else if ( *s == '*' )
        return( OPEMUL );
    else if ( *s == '/' )
        return( OPEDIV );
    else if ( *s == '%' )
        return( OPEMOD );
    else if ( *s == '<' )
        return( OPESHL );
    else if ( *s == '>' )
        return( OPESHR );
    else if ( *s == '|' )
        return( OPELOR );
    else if ( *s == '&' )
        return( OPELAN );
    else if ( *s == '^' )
        return( OPEXOR );
    else if ( *s == '~' )
        return( OPECOM );
    return( OPESYM );
}

static int axtoi( char * str )
{
    int num = 0;

    while ( isxdigit( *str ) )
        {
        num *= 16;
        num += *str - ( ( *str <= '9' ) ? '0' : '7' );
        str++;
        }
    return( num );
}

static int aotoi( char * str )
{
    int num = 0;

    while ( '0' <= *str && *str <= '7' )
        {
        num *= 8;
        num += ( *str++ ) - '0';
        }
    return( num );
}

static int abtoi( char * str )
{
    int num = 0;

    while ( '0' <= *str && *str <= '1' )
        {
        num *= 2;
        num += ( *str++ ) - '0';
        }
    return( num );
}

static int eval( char * s )
{
    char * p;
    int val = 0, ev = 0;
    char word[ MAXLINE ];
    StSym *sp;

    while ( *s )
        {
        p = word;
        if ( *s == '(' )
            {
            s++;
            while ( *s != ')' )
                {
                if ( ! *s )
                    {
                    asmerr( E_MISPAR );
                    return( val );
                    }
                *p++ = *s++;
                }
            *p = 0;
            s++;
            val = eval( word );
            continue;
            }
        if ( *s == STRSEP )
            {
            s++;
            while ( *s != STRSEP )
                {
                if ( *s == '\n' || ! *s )
                    {
                    asmerr( E_MISHYP );
                    break;
                    }
                *p++ = *s++;
                }
            s++;
            *p = 0;
            val = strval( word );
            continue;
            }
        if ( isari( *s ) )
            *p++ = *s++;
        else
            while ( !isspace( *s ) && !isari( *s ) && *s )
                *p++ = *s++;

        *p = 0;
        switch( get_type( word ) )
            {
            case OPESYM :
                if ( ! strcmp( word, "$" ) )
                    {
                    val = pc;
                    break;
                    }
                if ( strlen( word ) > SYMSIZE )
                    word[ SYMSIZE ] = 0;
                if ( ( sp = get_sym( word ) ) != NULL )
                    val = sp->val;
                else
                    asmerr( E_UNDSYM );
                break;

            case OPEDEC :
                val = atoi( word );
                break;

            case OPEHEX :
                val = axtoi( word + 1 );
                break;

            case OPEBIN :
                val = abtoi( word );
                break;

            case OPEOCT :
                val = aotoi( word );
                break;

            case OPESUB :
                val -= eval( s );
                return( val );

            case OPEADD :
                val += eval( s );
                return( val );

            case OPEMUL :
                val *= eval( s );
                return( val );

            case OPEDIV :
                ev = eval( s );
                if ( ev )
                    val /= ev;
                return( val );

            case OPEMOD :
                ev = eval( s );
                if ( ev )
                    val %= ev;
                return( val );

            case OPESHL :
                val <<= eval( s );
                return( val );

            case OPESHR :
                val >>= eval( s );
                return( val );

            case OPELOR :
                val |= eval( s );
                return( val );

            case OPELAN :
                val &= eval( s );
                return( val );

            case OPEXOR :
                val ^= eval( s );
                return( val );

            case OPECOM :
                val = ~( eval( s ) );
                return( val );
            }
        }
    return( val );
}

static UBYTE chk_v1( int i )
{
    if ( i >= -255 && i <= 255 )
        return( ( UBYTE )i );

    return( ( UBYTE )asmerr( E_VALOUT ) );
}

static UBYTE chk_v2( int i )
{
    if ( i >= -127 && i <= 127 )
        return( ( UBYTE )i );

    return( ( UBYTE )asmerr( E_VALOUT ) );
}

static void obj_fill( int count, UBYTE val )
{
    for ( int i = 0; i < count; i++ )
        Poke8Ext( ( USHORT )( pc + i ), val );
}

static int op_org( int a, int b )
{
    if ( gencode )
        pc = eval( operand );

    return( 0 );
}

static int op_run( int a, int b )
{
    if ( gencode && pass == 2 )
        run = eval( operand );

    return( 0 );
}

static int op_equ( int a, int b )
{
    if ( gencode )
        {
        if ( pass == 1 )
            {
            if ( get_sym( label ) == NULL ) 
                {
                sd_val = eval( operand );
                if ( put_sym( label, sd_val ) )
                    fatal( F_OUTMEM, "symbols" );
                }
            else
                asmerr( E_MULSYM );
            }
        else 
            sd_val = eval( operand );
        }
    return( 0 );
}

static int op_dl( int a, int b )
{
    if ( gencode )
        {
        sd_val = eval( operand );
        if ( put_sym( label, sd_val ) )
            fatal( F_OUTMEM, "symbols" );
        }
    return( 0 );
}

static int op_ds( int a, int b )
{
    int val;

    if ( gencode )
        {
        if ( pass == 1 )
            if ( *label )
                put_label();

        sd_val = pc;
        val = eval( operand );
        if ( pass == 2 )
            obj_fill( val, 0 );

        pc += val;
        }
    return( 0 );
}

static int op_db( int a, int b )
{ 
    char * s;
    int i = 0;
    char * p = operand;

    if ( !gencode )
        return( 0 );

    if ( pass == 1 )
        if ( *label )
            put_label();

    while ( * p )
        {
        if ( * p == '"' )
            {
            p++;
            while( * p && * p != '"' )
                {
                ops[ i++ ]= *p++;
                }
            if ( * p == '"' )
                p++;

            if ( * p == '+' )
                {
                ops[ i - 1 ] = ( UBYTE )( ops[ i - 1 ] + eval( ++p ) );
                break;
                }
            }
        else
            {
            if ( *p == STRSEP )
                {
                p++;
                while ( *p != STRSEP ) 
                    {
                    if ( *p == '\n' || ! *p )
                        {
                        asmerr( E_MISHYP );
                        return( i );
                        }
                    ops[ i++ ] = *p++;
                    }
                p++;
                }
            else 
                {
                s = tmp;
                while ( *p != ',' && *p )
                    *s++ = *p++;
                *s = 0;
                ops[ i++ ] = ( UBYTE )eval( tmp );
                }
            }
        if ( *p == ',' )
            p++;
        }
    return( i );
}

static int op_dm( int a, int b )
{
    int i = 0;
    char *p = operand;

    if ( !gencode )
        return( 0 );

    if ( pass == 1 )
        if ( *label )
            put_label();

    if ( *p != STRSEP )
        return( asmerr( E_MISHYP ) );

    p++;
    while ( *p != STRSEP )
        {
        if ( *p == '\n' || ! *p )
            {
            asmerr( E_MISHYP );
            break;
            }
        ops[ i++ ] = *p++;
        }
    return( i );
}

static int op_dw( int a, int b )
{
    int i = 0, len = 0, temp;
    char *p = operand;
    char *s;

    if ( !gencode )
        return( 0 );

    if ( pass == 1 )
        if ( *label )
            put_label();

    while ( *p )
        {
        s = tmp;
        while ( *p != ',' && *p )
            *s++ = *p++;

        *s = 0;
        if ( pass == 2 )
            {
            temp = eval( tmp );
            ops[ i++ ] = ( UBYTE )( temp & 0xff );
            ops[ i++ ] = ( UBYTE )( temp >> 8 );
            }
        len += 2;
        if ( *p == ',' )
            p++;
        }
    return( len );
}


static int op_cond( int op_code, int dummy )
{
    char *p, *p1, *p2;
    static int condnest[ IFNEST ];
    int equal;

    switch( op_code )
        {
        case 1 : // IFDEF
        case 2 : // IFNDEF
            if ( iflevel >= IFNEST )
                return( asmerr( E_IFNEST ) );

            condnest[ iflevel++ ] = gencode;
            if ( gencode )
                {
                equal = get_sym( operand ) == NULL;
                if ( ( equal && op_code == 1 ) || ( ! equal && op_code == 2 ) )
                    gencode = 0;
                }
            break;

        case 3 : // IFEQ
        case 4 : // IFNEQ
            if ( iflevel >= IFNEST )
                return( asmerr( E_IFNEST ) );

            condnest[ iflevel++ ] = gencode;
            p = operand;
            if ( ! * p || ! ( p1 = strchr( operand, ',' ) ) )
                return( asmerr( E_MISOPE ) );

            if ( gencode )
                {
                p2 = tmp;
                while ( *p != ',' )
                    *p2++ = *p++;
                *p2 = 0;
                equal= eval( tmp ) == eval( ++p1 );
                if ( ( ! equal && op_code == 3 ) || ( equal && op_code == 4 ) )
                    gencode = 0;
                }
            break;


        case 98 : // ELSE
            if ( !iflevel )
                return( asmerr( E_MISIFF ) );

            if ( ! iflevel  || ( condnest[ iflevel - 1 ] == 1 ) )
                gencode = !gencode;
            break;

        case 99 : // ENDIF
            if ( !iflevel )
                return( asmerr( E_MISIFF ) );

            gencode = condnest[ --iflevel ];
            break;
        }
    return( 0 );
}

static int op_1b( int b1, int dummy )
{
    if ( pass == 1 )
        {
        if ( * label )
            put_label();
        }
    else
        ops[ 0 ] = ( UBYTE )b1;

    return( 1 );
}

static int op_2b( int b1, int b2 )
{
    if ( pass == 1 )
        {
        if ( * label )
            put_label();
        }
    else
        {
        ops[ 0 ] = ( UBYTE )b1;
        ops[ 1 ] = ( UBYTE )b2;
        }
    return( 2 );
}

static int op_im( int a, int b )
{
    if ( pass == 1 )
        if ( *label )
            put_label();

    if ( pass == 2 ) 
        {
        ops[ 0 ] = 0xed;
        switch( eval( operand ) )
            {
            case 0 :
                ops[ 1 ] = 0x46;
                break;

            case 1 :
                ops[ 1 ] = 0x56;
                break;

            case 2 :
                ops[ 1 ] = 0x5e;
                break;

            default :
                ops[ 1 ] = 0;
                asmerr( E_ILLOPE );
                break;
            }
        }
    return( 2 );
}

static int get_reg( char * s )
{
    int cond;
    StOpe *low, *high, *mid;

    if ( s == NULL || ! *s )
        return( NOOPERA );

    low = &opetab[ 0 ];
    high = &opetab[ no_operands - 1 ];
    while ( low <= high )
        {
        mid = low + ( high - low ) / 2;
        if ( ( cond = strcmp( s, mid->name ) ) < 0 )
            high = mid - 1;
        else 
            if ( cond > 0 )
                low = mid + 1;
            else
                return( mid->sym );
        }
    return( NOREG );
}

static int op_pupo( int op_code, int dummy )
{
    int len = 1, op;
    if ( pass == 1 )
        if ( *label )
            put_label();

    switch( op = get_reg( operand ) )
        {
        case REGAF :
            if ( pass == 2 )
                ops[ 0 ] = ( UBYTE )( 0xF1 + op_code );
            break;

        case REGBC :
            if ( pass == 2 )
                ops[ 0 ] = ( UBYTE )( 0xC1 + op_code );
            break;

        case REGDE :
            if ( pass == 2 )
                ops[ 0 ] = ( UBYTE )( 0xD1 + op_code );
            break;

        case REGHL :
            if ( pass == 2 )
                ops[ 0 ] = ( UBYTE )( 0xE1 + op_code );
            break;

        case REGIX :
        case REGIY :
            if ( pass == 2 )
                {
                ops[ 0 ] = ( UBYTE )op;
                ops[ 1 ] = ( UBYTE )( 0xE1 + op_code );
                }
            len = 2;
            break;

        case NOOPERA :
            ops[ 0 ] = 0;
            asmerr( E_MISOPE );
            break;

        default :
            ops[ 0 ] = 0;
            asmerr( E_ILLOPE );
        }
    return( len );
}

static int op_ex( int a, int b )
{
    int len = 1;

    if ( pass == 1 )
        if ( *label )
            put_label();

    if ( ! strncmp( operand, "DE,HL", 5 ) ) 
        ops[ 0 ] = 0xeb;
    else
        if ( !strncmp( operand, "AF,AF'", 7 ) ) 
            ops[ 0 ] = 0x08;
        else
            if ( !strncmp( operand, "(SP),HL", 7 ) )
                ops[ 0 ] = 0xe3;
            else
                if ( !strncmp( operand, "(SP),IX", 7 ) )
                    {
                    ops[ 0 ] = 0xdd;
                    ops[ 1 ] = 0xe3;
                    len = 2;
                    }
                else
                    if ( !strncmp( operand, "(SP),IY", 7 ) ) 
                        {
                        ops[ 0 ] = 0xfd;
                        ops[ 1 ] = 0xe3;
                        len = 2;
                        }
                    else 
                        {
                        ops[ 0 ] = 0;
                        asmerr( E_ILLOPE );
                        }

    return( len );
}

static void SetOpAdr( int op, int adr )
{
    ops[ 0 ] = ( UBYTE )op;
    ops[ 1 ] = ( UBYTE )( adr & 0xff );
    ops[ 2 ] = ( UBYTE )( adr >> 8 );
}


static void SetOp2Adr( int op1, int op2, int adr )
{
    ops[ 0 ] = ( UBYTE )op1;
    ops[ 1 ] = ( UBYTE )op2;
    ops[ 2 ] = ( UBYTE )( adr & 0xff );
    ops[ 3 ] = ( UBYTE )( adr >> 8 );
}


static int op_call( int a, int b )
{
    char *p1, *p2;

    if ( pass == 1 )
        {
        if ( *label )
            put_label();
        }
    else
        {
        p1 = operand;
        p2 = tmp;
        while ( *p1 != ',' && *p1 )
            *p2++ = *p1++;

        *p2 = 0;
        switch( get_reg( tmp ) )
            {
            case FLGZ :
                SetOpAdr( 0xCC, eval( strchr( operand, ',' ) + 1 ) );
                break;

            case FLGNZ :
                SetOpAdr( 0xC4, eval( strchr( operand, ',' ) + 1 ) );
                break;

            case REGC :
                SetOpAdr( 0xDC, eval( strchr( operand, ',' ) + 1 ) );
                break;

            case FLGNC :
                SetOpAdr( 0xD4, eval( strchr( operand, ',' ) + 1 ) );
                break;

            case FLGPE :
                SetOpAdr( 0xEC, eval( strchr( operand, ',' ) + 1 ) );
                break;

            case FLGPO :
                SetOpAdr( 0xE4, eval( strchr( operand, ',' ) + 1 ) );
                break;

            case FLGM :
                SetOpAdr( 0xFC, eval( strchr( operand, ',' ) + 1 ) );
                break;

            case FLGP :
                SetOpAdr( 0xF4, eval( strchr( operand, ',' ) + 1 ) );
                break;

            case NOREG :
                SetOpAdr( 0xCD, eval( operand ) );
                break;

            case NOOPERA :
                SetOpAdr( 0, 0 );
                asmerr( E_MISOPE );
                break;

            default :
                SetOpAdr( 0, 0 );
                asmerr( E_ILLOPE );
            }
        }
    return( 3 );
}

static int op_rst( int a, int b )
{
    int op;

    if ( pass == 1 )
        {
        if ( *label )
            put_label();
        }
    else
        {
        op = eval( operand );
        if ( ( op / 8 > 7 ) || ( op % 8 ) )
            {
            ops[ 0 ] = 0;
            asmerr( E_VALOUT );
            }
        else
            ops[ 0 ] = ( UBYTE )( 0xc7 + op );
        }
    return( 1 );
}

static int op_ret( int a, int b )
{
    if ( pass == 1 )
        {
        if ( *label )
            put_label();
        }
    else 
        {
        switch( get_reg( operand ) )
            {
            case NOOPERA :
                ops[ 0 ] = 0xc9;
                break;

            case FLGZ :
                ops[ 0 ] = 0xc8;
                break;

            case FLGNZ :
                ops[ 0 ] = 0xc0;
                break;

            case REGC :
                ops[ 0 ] = 0xd8;
                break;

            case FLGNC :
                ops[ 0 ] = 0xd0;
                break;

            case FLGPE :
                ops[ 0 ] = 0xe8;
                break;

            case FLGPO :
                ops[ 0 ] = 0xe0;
                break;

            case FLGM :
                ops[ 0 ] = 0xf8;
                break;

            case FLGP :
                ops[ 0 ] = 0xf0;
                break;

            default :
                ops[ 0 ] = 0;
                asmerr( E_ILLOPE );
            }
        }
    return( 1 );
}

static int op_jp( int a, int b )
{
    char *p1, *p2;
    int len = 3;

    if ( pass == 1 )
        if ( *label )
            put_label();

    p1 = operand;
    p2 = tmp;
    while ( *p1 != ',' && *p1 )
        *p2++ = *p1++;

    *p2 = 0;
    switch( get_reg( tmp ) )
        {
        case REGC :
            if ( pass == 2 )
                SetOpAdr( 0xDA, eval( strchr( operand, ',' ) + 1 ) );
            break;

        case FLGNC :
            if ( pass == 2 )
                SetOpAdr( 0xD2, eval( strchr( operand, ',' ) + 1 ) );
            break;

        case FLGZ :
            if ( pass == 2 )
                SetOpAdr( 0xCA, eval( strchr( operand, ',' ) + 1 ) );
            break;

        case FLGNZ :
            if ( pass == 2 )
                SetOpAdr( 0xC2, eval( strchr( operand, ',' ) + 1 ) );
            break;

        case FLGPE :
            if ( pass == 2 )
                SetOpAdr( 0xEA,  eval( strchr( operand, ',' ) + 1 ) );
            break;

        case FLGPO :
            if ( pass == 2 )
                SetOpAdr( 0xE2, eval( strchr( operand, ',' ) + 1 ) );
            break;

        case FLGM :
            if ( pass == 2 )
                SetOpAdr( 0xFA, eval( strchr( operand, ',' ) + 1 ) );
            break;

        case FLGP :
            if ( pass == 2 )
                SetOpAdr( 0xF2, eval( strchr( operand, ',' ) + 1 ) );
            break;

        case REGIHL :
            ops[ 0 ] = 0xe9;
            len = 1;
            break;

        case REGIIX :
            ops[ 0 ] = 0xdd;
            ops[ 1 ] = 0xe9;
            len = 2;
            break;

        case REGIIY :
            ops[ 0 ] = 0xfd;
            ops[ 1 ] = 0xe9;
            len = 2;
            break;

        case NOREG :
            if ( pass == 2 )
                SetOpAdr( 0xC3, eval( operand ) );
            break;

        case NOOPERA :
            ops[ 0 ] = 0;
            len = 1;
            asmerr( E_MISOPE );
            break;

        default :
            ops[ 0 ] = 0;
            len = 1;
            asmerr( E_ILLOPE );
        }
    return( len );
}

static int op_jr( int a, int b )
{
    char *p1, *p2;

    if ( pass == 1 )
        {
        if ( *label )
            put_label();
        }
    else
        {
        p1 = operand;
        p2 = tmp;
        while ( *p1 != ',' && *p1 )
            *p2++ = *p1++;
        *p2 = 0;
        switch( get_reg( tmp ) )
            {
            case REGC :
                ops[ 0 ] = 0x38;
                ops[ 1 ] = chk_v2( eval( strchr( operand, ',' ) + 1 ) - pc - 2 );
                break;

            case FLGNC :
                ops[ 0 ] = 0x30;
                ops[ 1 ] = chk_v2( eval( strchr( operand, ',' ) + 1 ) - pc - 2 );
                break;

            case FLGZ :
                ops[ 0 ] = 0x28;
                ops[ 1 ] = chk_v2( eval( strchr( operand, ',' ) + 1 ) - pc - 2 );
                break;

            case FLGNZ :
                ops[ 0 ] = 0x20;
                ops[ 1 ] = chk_v2( eval( strchr( operand, ',' ) + 1 ) - pc - 2 );
                break;

            case NOREG :
                ops[ 0 ] = 0x18;
                ops[ 1 ] = chk_v2( eval( operand ) - pc - 2 );
                break;

            case NOOPERA :
                ops[ 0 ] = ops[ 1 ] = 0;
                asmerr( E_MISOPE );
                break;

            default :
                ops[ 0 ] = ops[ 1 ] = 0;
                asmerr( E_ILLOPE );
            }
        }
    return( 2 );
}

static int op_djnz( int a, int b )
{
    if ( pass == 1 )
        {
        if ( *label )
            put_label();
        }
    else 
        {
        ops[ 0 ] = 0x10;
        ops[ 1 ] = chk_v2( eval( operand ) - pc - 2 );
        }
    return( 2 );
}

static char *get_second( char *s )
{
    char *p;

    if ( ( p = strchr( s, ',' ) ) != NULL )
        return( p + 1 );
    else
        return( NULL );
}

static int calc_val( char *s )
{
    char * p;
    int i;

    if ( ( p = strrchr( s, ')' ) ) == NULL ) 
        return( asmerr( E_MISPAR ) );

    i = ( int )( p - s );
    strncpy( tmp, s, i );
    *( tmp + i ) = 0;
    return( eval( tmp ) );
}

static int lda( void )
{
    int op, len = 1;

    char * p = get_second( operand );
    switch( op = get_reg( p ) )
        {
        case REGA :
        case REGB :
        case REGC :
        case REGD :
        case REGE :
        case REGH :
        case REGL :
        case REGIHL :
            ops[ 0 ] = ( UBYTE )( 0x78 + op );
            break;

        case REGI :
            len = 2;
            ops[ 0 ] = 0xed;
            ops[ 1 ] = 0x57;
            break;

        case REGR :
            len = 2;
            ops[ 0 ] = 0xed;
            ops[ 1 ] = 0x5f;
            break;

        case REGIBC :
            ops[ 0 ] = 0x0a;
            break;

        case REGIDE :
            ops[ 0 ] = 0x1a;
            break;

        case NOREG :
            if ( ! strncmp( p, "(IX+", 4 ) )
                {
                len = 3;
                if ( pass == 2 )
                    {
                    ops[ 0 ] = 0xdd;
                    ops[ 1 ] = 0x7e;
                    ops[ 2 ] = chk_v2( calc_val( strchr( p, '+' ) + 1 ) );
                    }
                break;
                }
            if ( ! strncmp( p, "(IY+", 4 ) )
                {
                len = 3;
                if ( pass == 2 )
                    {
                    ops[ 0 ] = 0xfd;
                    ops[ 1 ] = 0x7e;
                    ops[ 2 ] = chk_v2( calc_val( strchr( p, '+' ) + 1 ) );
                    }
                break;
                }
            if ( *p == '(' && *( p + strlen( p ) - 1 ) == ')' )
                {
                len = 3;
                if ( pass == 2 )
                    SetOpAdr( 0x3A, calc_val( p + 1 ) );
                break;
                }
            len = 2;
            if ( pass == 2 )
                {
                ops[ 0 ] = 0x3e;
                ops[ 1 ] = chk_v1( eval( p ) );
                }
            break;

        case NOOPERA :
            ops[ 0 ] = 0;
            asmerr( E_MISOPE );
            break;

        default :
            ops[ 0 ] = 0;
            asmerr( E_ILLOPE );
        }
    return( len );
}

static int ld_rr( int ofs )
{
    int op;
    int len = 1;

    char * p = get_second( operand );
    switch( op = get_reg( p ) )
        {
        case REGA :
        case REGB :
        case REGC :
        case REGD :
        case REGE :
        case REGH :
        case REGL :
        case REGIHL :
            ops[ 0 ] = ( UBYTE )( 0x3A + ofs + op );
            break;

        case NOREG :
            if ( ! strncmp( p, "(IX+", 4 ) )
                {
                len = 3;
                if ( pass == 2 )
                    {
                    ops[ 0 ] = 0xdd;
                    ops[ 1 ] = ( UBYTE )( 0x40 + ofs );
                    ops[ 2 ] = chk_v2( calc_val( strchr( p, '+' ) + 1 ) );
                    }
                break;
                }
            if ( ! strncmp( p, "(IY+", 4 ) )
                {
                len = 3;
                if ( pass == 2 )
                    {
                    ops[ 0 ] = 0xfd;
                    ops[ 1 ] = ( UBYTE )( 0x40 + ofs );
                    ops[ 2 ] = chk_v2( calc_val( strchr( p, '+' ) + 1 ) );
                    }
                break;
                }
            len = 2;
            if ( pass == 2 )
                {
                ops[ 0 ] = ( UBYTE )ofs;
                ops[ 1 ] = chk_v1( eval( p ) );
                }
            break;

        case NOOPERA :
            ops[ 0 ] = 0;
            asmerr( E_MISOPE );
            break;

        default :
            ops[ 0 ] = 0;
            asmerr( E_ILLOPE );
        }
    return( len );
}

static int ldlx( int ofs )
{
    int op;
    int len = 2;

    char * p = get_second( operand );
    switch( op = get_reg( p ) )
        {
        case REGA :
        case REGB :
        case REGC :
        case REGD :
        case REGE :
        case REGH :
        case REGL :
        case REGIHL :
            ops[ 0 ] = ( UBYTE )ofs;
            ops[ 1 ] = ( UBYTE )( 0x60 + op );
            break;

        case NOREG :
            len = 3;
            if ( pass == 2 )
                {
                ops[ 0 ] = ( UBYTE )ofs;
                ops[ 1 ] = ( UBYTE )ofs;
                ops[ 2 ] = chk_v1( eval( p ) );
                }
            break;

        case NOOPERA :
            ops[ 0 ] = 0;
            asmerr( E_MISOPE );
            break;

        default :
            ops[ 0 ] = 0;
            asmerr( E_ILLOPE );
        }
    return( len );
}

static int ldly()
{
    return( 2 );
}

static int ldhx()
{
    return( 2 );
}

static int ldhy()
{
    return( 2 );
}

static int ld_bcde( int ofs )
{
    int len = 1;

    char * p = get_second( operand );
    switch( get_reg( p ) )
        {
        case NOREG :
            if ( *p == '(' && *( p + strlen( p ) - 1 ) == ')' )
                {
                len = 4;
                if ( pass == 2 )
                    SetOp2Adr( 0xED, 0x4A + ofs, calc_val( p + 1 ) );
                break;
                }
            len = 3;
            if ( pass == 2 )
                SetOpAdr( ofs, eval( p ) );
            break;

        case NOOPERA :
            ops[ 0 ] = 0;
            asmerr( E_MISOPE );
            break;

        default :
            ops[ 0 ] = 0;
            asmerr( E_ILLOPE );
        }
    return( len );
}

static int ldhl( void )
{
    int len = 1;

    char * p = get_second( operand );
    switch( get_reg( p ) )
        {
        case NOREG :
            len = 3;
            if ( *p == '(' && *( p + strlen( p ) - 1 ) == ')' )
                {
                if ( pass == 2 )
                    SetOpAdr( 0x2A, calc_val( p + 1 ) );
                break;
                }
            if ( pass == 2 )
                SetOpAdr( 0x21, eval( p ) );
            break;

        case NOOPERA :
            ops[ 0 ] = 0;
            asmerr( E_MISOPE );
            break;

        default :
            ops[ 0 ] = 0;
            asmerr( E_ILLOPE );
        }
    return( len );
}

static int ld_ixiy( int ofs )
{
    int len = 1;

    char * p = get_second( operand );
    switch( get_reg( p ) )
        {
        case NOREG :
            len = 4;
            if ( *p == '(' && *( p + strlen( p ) - 1 ) == ')' )
                {
                if ( pass == 2 )
                    SetOp2Adr( ofs, 0x2A, calc_val( p + 1 ) );
                break;
                }
            if ( pass == 2 )
                SetOp2Adr( ofs, 0x21, eval( p ) );
            break;

        case NOOPERA :
            ops[ 0 ] = 0;
            asmerr( E_MISOPE );
            break;

        default :
            ops[ 0 ] = 0;
            asmerr( E_ILLOPE );
        }
    return( len );
}

static int ldsp( void )
{
    int len = 1, op;

    char * p = get_second( operand );
    switch( op = get_reg( p ) )
        {
        case REGHL :
            ops[ 0 ] = 0xf9;
            break;

        case REGIX :
        case REGIY :
            len = 2;
            ops[ 0 ] = ( UBYTE )op;
            ops[ 1 ] = 0xf9;
            break;

        case NOREG :
            if ( *p == '(' && *( p + strlen( p ) - 1 ) == ')' )
                {
                len = 4;
                if ( pass == 2 )
                    SetOp2Adr( 0xED, 0x7B, calc_val( p + 1 ) );
                break;
                }
            len = 3;
            if ( pass == 2 )
                SetOpAdr( 0x31, eval( p ) );
            break;

        case NOOPERA :
            ops[ 0 ] = 0;
            asmerr( E_MISOPE );
            break;

        default :
            ops[ 0 ] = 0;
            asmerr( E_ILLOPE );
        }
    return( len );
}

static int ldihl( void )
{
    int op;
    int len = 1;

    char * p = get_second( operand );
    switch( op = get_reg( p ) )
        {
        case REGA :
        case REGB :
        case REGC :
        case REGD :
        case REGE :
        case REGH :
        case REGL :
            ops[ 0 ] = ( UBYTE )( 0x70 + op );
            break;

        case NOREG :
            len = 2;
            if ( pass == 2 )
                {
                ops[ 0 ] = 0x36;
                ops[ 1 ] = chk_v1( eval( p ) );
                }
            break;

        case NOOPERA :
            ops[ 0 ] = 0;
            asmerr( E_MISOPE );
            break;

        default :
            ops[ 0 ] = 0;
            asmerr( E_ILLOPE );
        }
    return( len );
}

static int ldi_ixiy( int ofs )
{
    int op;
    int len = 1;

    char * p = get_second( operand );
    switch( op = get_reg( p ) )
        {
        case REGA :
        case REGB :
        case REGC :
        case REGD :
        case REGE :
        case REGH :
        case REGL :
            len = 3;
            if ( pass == 2 )
                {
                ops[ 0 ] = ( UBYTE )ofs;
                ops[ 1 ] = ( UBYTE )( 0x70 + op );
                ops[ 2 ] = chk_v2( calc_val( strchr( operand, '+' ) + 1 ) );
                }
            break;

        case NOREG :
            len = 4;
            if ( pass == 2 )
                {
                ops[ 0 ] = ( UBYTE )ofs;
                ops[ 1 ] = 0x36;
                ops[ 2 ] = chk_v2( calc_val( strchr( operand, '+' ) + 1 ) );
                ops[ 3 ] = chk_v1( eval( p ) );
                }
            break;

        case NOOPERA :
            ops[ 0 ] = 0;
            asmerr( E_MISOPE );
            break;

        default :
            ops[ 0 ] = 0;
            asmerr( E_ILLOPE );
        }
    return( len );
}

static int ldinn( void )
{
    int len = 4, op;

    char * p = get_second( operand );
    switch( op = get_reg( p ) )
        {
        case REGA :
            len = 3;
            if ( pass == 2 )
                SetOpAdr( 0x32, calc_val( operand + 1 ) );
            break;

        case REGBC :
            if ( pass == 2 )
                SetOp2Adr( 0xED, 0x43, calc_val( operand + 1 ) );
            break;

        case REGDE :
            if ( pass == 2 )
                SetOp2Adr( 0xED, 0x53, calc_val( operand + 1 ) );
            break;

        case REGHL :
            len = 3;
            if ( pass == 2 )
                SetOpAdr( 0x22, calc_val( operand + 1 ) );
            break;

        case REGSP :
            if ( pass == 2 )
                SetOp2Adr( 0xED, 0x73, calc_val( operand + 1 ) );
            break;

        case REGIX :
        case REGIY :
            if ( pass == 2 )
                SetOp2Adr( op, 0x22, calc_val( operand + 1 ) );
            break;

        case NOOPERA :
            len = 1;
            ops[ 0 ] = 0;
            asmerr( E_MISOPE );
            break;

        default :
            len = 1;
            ops[ 0 ] = 0;
            asmerr( E_ILLOPE );
        }
    return( len );
}

static int op_ld( int a, int b )
{
    int len, op;
    char *p1, *p2;

    if ( pass == 1 )
        if ( *label )
            put_label();

    p1 = operand;
    p2 = tmp;
    while ( *p1 != ',' && *p1 )
        *p2++ = *p1++;

    *p2 = 0;
    switch( op = get_reg( tmp ) )
        {
        case REGA :
            len = lda();
            break;

        case REGB :
            len = ld_rr( 0x06 );
            break;

        case REGC :
            len = ld_rr( 0x0E );
            break;

        case REGD :
            len = ld_rr( 0x16 );
            break;

        case REGE :
            len = ld_rr( 0x1E );
            break;

        case REGH :
            len = ld_rr( 0x26 );
            break;

        case REGL :
            len = ld_rr( 0x2E );
            break;

        case REGI :
            if ( get_reg( get_second( operand ) ) == REGA )
                {
                len = 2;
                ops[ 0 ] = 0xed;
                ops[ 1 ] = 0x47;
                break;
                }
            len = 1;
            ops[ 0 ] = 0;
            asmerr( E_ILLOPE );
            break;

        case REGR :
            if ( get_reg( get_second( operand ) ) == REGA )
                {
                len = 2;
                ops[ 0 ] = 0xed;
                ops[ 1 ] = 0x4f;
                break;
                }
            len = 1;
            ops[ 0 ] = 0;
            asmerr( E_ILLOPE );
            break;

        case REGBC :
            len = ld_bcde( 0x01 );
            break;

        case REGDE :
            len = ld_bcde( 0x11 );
            break;

        case REGHL :
            len = ldhl();
            break;

        case REGIX :
        case REGIY :
            len = ld_ixiy( op );
            break;

        case REGSP :
            len = ldsp();
            break;

        case REGIHL :
            len = ldihl();
            break;

        case REGIBC :
            if ( get_reg( get_second( operand ) ) == REGA )
                {
                len = 1;
                ops[ 0 ] = 0x02;
                break;
                }
            len = 1;
            ops[ 0 ] = 0;
            asmerr( E_ILLOPE );
            break;

        case REGIDE :
            if ( get_reg( get_second( operand ) ) == REGA )
                {
                len = 1;
                ops[ 0 ] = 0x12;
                break;
                }
            len = 1;
            ops[ 0 ] = 0;
            asmerr( E_ILLOPE );
            break;

        case REGLX :
            len = ldlx( 0xDD );
            break;

        case REGLY :
            len = ldly();
            break;

        case REGHX :
            len = ldhx();
            break;

        case REGHY :
            len = ldhy();
            break;

        case NOOPERA :
            len = 1;
            ops[ 0 ] = 0;
            asmerr( E_MISOPE );
            break;

        default :
            if ( ! strncmp( operand, "(IX+", 4 ) )
                len = ldi_ixiy( 0xDD );
            else
                if ( ! strncmp( operand, "(IY+", 4 ) )
                    len = ldi_ixiy( 0xFD );
                else
                    if ( *operand == '(' )
                        len = ldinn();
                    else 
                        {
                        len = 1;
                        ops[ 0 ] = 0;
                        asmerr( E_ILLOPE );
                        }
        }
    return( len );
}

static int addhl( void )
{
    switch( get_reg( get_second( operand ) ) ) {
    case REGBC :
        ops[ 0 ] = 0x09;
        break;

    case REGDE :
        ops[ 0 ] = 0x19;
        break;

    case REGHL :
        ops[ 0 ] = 0x29;
        break;

    case REGSP :
        ops[ 0 ] = 0x39;
        break;

    case NOOPERA :
        ops[ 0 ] = 0;
        ops[ 1 ] = 0;
        asmerr( E_MISOPE );
        break;

    default :
        ops[ 0 ] = 0;
        ops[ 1 ] = 0;
        asmerr( E_ILLOPE );
    }
    return( 1 );
}

static int add_ixiy( int ofs )
{
    switch( get_reg( get_second( operand ) ) )
        {
        case REGBC :
            ops[ 0 ] = ( UBYTE )ofs;
            ops[ 1 ] = 0x09;
            break;

        case REGDE :
            ops[ 0 ] = ( UBYTE )ofs;
            ops[ 1 ] = 0x19;
            break;

        case REGIX :
            ops[ 0 ] = ( UBYTE )ofs;
            ops[ 1 ] = 0x29;
            break;

        case REGSP :
            ops[ 0 ] = ( UBYTE )ofs;
            ops[ 1 ] = 0x39;
            break;

        case NOOPERA :
            ops[ 0 ] = 0;
            ops[ 1 ] = 0;
            asmerr( E_MISOPE );
            break;

        default :
            ops[ 0 ] = 0;
            ops[ 1 ] = 0;
            asmerr( E_ILLOPE );
        }
    return( 2 );
}

static int adda( void )
{
    int op;
    int len = 1;

    char * p = get_second( operand );
    switch( op = get_reg( p ) )
        {
        case REGA :
        case REGB :
        case REGC :
        case REGD :
        case REGE :
        case REGH :
        case REGL :
        case REGIHL :
            ops[ 0 ] = ( UBYTE )( 0x80 + op );
            break;

        case NOREG :
            if ( ! strncmp( p, "(IX+", 4 ) )
                {
                len = 3;
                if ( pass == 2 )
                    {
                    ops[ 0 ] = 0xdd;
                    ops[ 1 ] = 0x86;
                    ops[ 2 ] = chk_v2( calc_val( strchr( p, '+' ) + 1 ) );
                    }
                }
            else
                if ( ! strncmp( p, "(IY+", 4 ) )
                    {
                    len = 3;
                    if ( pass == 2 )
                        {
                        ops[ 0 ] = 0xfd;
                        ops[ 1 ] = 0x86;
                        ops[ 2 ] = chk_v2( calc_val( strchr( p, '+' ) + 1 ) );
                        }
                    }
                else
                    {
                    len = 2;
                    if ( pass == 2 )
                        {
                        ops[ 0 ] = 0xc6;
                        ops[ 1 ] = chk_v1( eval( p ) );
                        }
                    }
            break;

        case NOOPERA :
            ops[ 0 ] = 0;
            asmerr( E_MISOPE );
            break;

        default :
            ops[ 0 ] = 0;
            asmerr( E_ILLOPE );
        }
    return( len );
}

static int op_alog( int a, int b, int ofs )
{
    int len = 1, op;

    if ( pass == 1 )
        if ( *label )
            put_label();

    switch( op = get_reg( operand ) )
        {
        case REGA :
        case REGB :
        case REGC :
        case REGD :
        case REGE :
        case REGH :
        case REGL :
        case REGIHL :
            ops[ 0 ] = ( UBYTE )( ofs + op );
            break;

        case NOREG :
            if ( ! strncmp( operand, "(IX+", 4 ) )
                {
                len = 3;
                if ( pass == 2 )
                    {
                    ops[ 0 ] = 0xdd;
                    ops[ 1 ] = ( UBYTE )( ofs + 6 );
                    ops[ 2 ] = chk_v2( calc_val( strchr( operand, '+' ) + 1 ) );
                    }
                }
            else
                if ( ! strncmp( operand, "(IY+", 4 ) )
                    {
                    len = 3;
                    if ( pass == 2 )
                        {
                        ops[ 0 ] = 0xfd;
                        ops[ 1 ] = ( UBYTE )( ofs + 6 );
                        ops[ 2 ] = chk_v2( calc_val( strchr( operand, '+' ) + 1 ) );
                        }
                    }
                else
                    {
                    len = 2;
                    if ( pass == 2 )
                        {
                        ops[ 0 ] = ( UBYTE )( ofs + 0x46 );
                        ops[ 1 ] = chk_v1( eval( operand ) );
                        }
                    }
            break;

        case NOOPERA :
            ops[ 0 ] = 0;
            asmerr( E_MISOPE );
            break;

        default :
            ops[ 0 ] = 0;
            asmerr( E_ILLOPE );
        }
    return( len );
}

static int op_add( int a, int b )
{
    int len = 1, op;
    char *p1, *p2;

    if ( pass == 1 )
        if ( *label )
            put_label();

    p1 = operand;
    p2 = tmp;
    while ( *p1 != ',' && *p1 )
        *p2++ = *p1++;

    *p2 = 0;
    switch( op = get_reg( tmp ) )
        {
        case REGA :
            len = adda();
            break;

        case REGHL :
            len = addhl();
            break;

        case REGIX :
        case REGIY :
            len = add_ixiy( op );
            break;
        case NOOPERA :
            ops[ 0 ] = 0;
            asmerr( E_MISOPE );
            break;

        default :
            ops[ 0 ] = 0;
            asmerr( E_ILLOPE );
        }
    return( len );
}

static int adcsbc_hl( int ofs )
{
    switch( get_reg( get_second( operand ) ) )
        {
        case REGBC :
            ops[ 0 ] = 0xed;
            ops[ 1 ] = ( UBYTE )( 0x40 + ofs );
            break;

        case REGDE :
            ops[ 0 ] = 0xed;
            ops[ 1 ] = ( UBYTE )( 0x50 + ofs );
            break;

        case REGHL :
            ops[ 0 ] = 0xed;
            ops[ 1 ] = ( UBYTE )( 0x60 + ofs );
            break;

        case REGSP :
            ops[ 0 ] = 0xed;
            ops[ 1 ] = ( UBYTE )( 0x70 + ofs );
            break;

        case NOOPERA :
            ops[ 0 ] = 0;
            ops[ 1 ] = 0;
            asmerr( E_MISOPE );
            break;

        default :
            ops[ 0 ] = 0;
            ops[ 1 ] = 0;
            asmerr( E_ILLOPE );
        }
    return( 2 );
}

static int op_sub( int a, int b )
{
    return( op_alog( a, b, 0x90 ) );
}

static int adcsbc_a( int ofs )
{
    int op;
    int len = 1;

    char * p = get_second( operand );
    switch( op = get_reg( p ) )
        {
        case REGA :
        case REGB :
        case REGC :
        case REGD :
        case REGE :
        case REGH :
        case REGL :
        case REGIHL :
            ops[ 0 ] = ( UBYTE )( 0x8 + ofs + op );
            break;

        case NOREG :
            if ( ! strncmp( p, "(IX+", 4 ) )
                {
                len = 3;
                if ( pass == 2 )
                    {
                    ops[ 0 ] = 0xdd;
                    ops[ 1 ] = ( UBYTE )( 0xe + ofs );
                    ops[ 2 ] = chk_v2( calc_val( strchr( p, '+' ) + 1 ) );
                    }
                }
            else
                if ( ! strncmp( p, "(IY+", 4 ) )
                    {
                    len = 3;
                    if ( pass == 2 )
                        {
                        ops[ 0 ] = 0xfd;
                        ops[ 1 ] = ( UBYTE )( 0xe + ofs );
                        ops[ 2 ] = chk_v2( calc_val( strchr( p, '+' ) + 1 ) );
                        }
                    }
                else
                    {
                    len = 2;
                    if ( pass == 2 )
                        {
                        ops[ 0 ] = ( UBYTE )( 0x4e + ofs );
                        ops[ 1 ] = chk_v1( eval( p ) );
                        }
                    }
            break;

        case NOOPERA :
            ops[ 0 ] = 0;
            asmerr( E_MISOPE );
            break;

        default :
            ops[ 0 ] = 0;
            asmerr( E_ILLOPE );
        }
    return( len );
}

static int op_adc( int a, int b )
{
    int len = 1;
    char *p1, *p2;

    if ( pass == 1 )
        if ( *label )
            put_label();

    p1 = operand;
    p2 = tmp;
    while ( *p1 != ',' && *p1 )
        *p2++ = *p1++;

    *p2 = 0;
    switch( get_reg( tmp ) )
        {
        case REGA :
            len = adcsbc_a( 0x80 );
            break;

        case REGHL :
            len = adcsbc_hl( 0x0A );
            break;

        case NOOPERA :
            ops[ 0 ] = 0;
            asmerr( E_MISOPE );
            break;

        default :
            ops[ 0 ] = 0;
            asmerr( E_ILLOPE );
        }
    return( len );
}

static int op_sbc( int a, int b )
{
    int len = 1;
    char *p1, *p2;

    if ( pass == 1 )
        if ( *label )
            put_label();

    p1 = operand;
    p2 = tmp;
    while ( *p1 != ',' && *p1 )
        *p2++ = *p1++;

    *p2 = 0;
    switch( get_reg( tmp ) )
        {
        case REGA :
            len = adcsbc_a( 0x90 );
            break;

        case REGHL :
            len = adcsbc_hl( 0x02 );
            break;

        case NOOPERA :
            ops[ 0 ] = 0;
            asmerr( E_MISOPE );
            break;

        default :
            ops[ 0 ] = 0;
            asmerr( E_ILLOPE );
        }
    return( len );
}

static int op_inc( int a, int b )
{
    int len = 1, op;

    if ( pass == 1 )
        if ( *label )
            put_label();
    switch( op = get_reg( operand ) )
        {
        case REGA :
        case REGB :
        case REGC :
        case REGD :
        case REGE :
        case REGH :
        case REGL :
        case REGIHL :
            ops[ 0 ] = ( UBYTE )( 0x04 + ( op << 3 ) );
            break;

        case REGBC :
            ops[ 0 ] = 0x03;
            break;

        case REGDE :
            ops[ 0 ] = 0x13;
            break;

        case REGHL :
            ops[ 0 ] = 0x23;
            break;

        case REGSP :
            ops[ 0 ] = 0x33;
            break;

        case REGIX :
        case REGIY :
            len = 2;
            ops[ 0 ] = ( UBYTE )op;
            ops[ 1 ] = 0x23;
            break;

        case NOREG :
            if ( ! strncmp( operand, "(IX+", 4 ) )
                {
                len = 3;
                if ( pass == 2 )
                    {
                    ops[ 0 ] = 0xdd;
                    ops[ 1 ] = 0x34;
                    ops[ 2 ] = chk_v2( calc_val( strchr( operand, '+' ) + 1 ) );
                    }
                }
            else
                if ( ! strncmp( operand, "(IY+", 4 ) )
                    {
                    len = 3;
                    if ( pass == 2 )
                        {
                        ops[ 0 ] = 0xfd;
                        ops[ 1 ] = 0x34;
                        ops[ 2 ] = chk_v2( calc_val( strchr( operand, '+' ) + 1 ) );
                        }
                    }
                else
                    {
                    ops[ 0 ] = 0;
                    asmerr( E_ILLOPE );
                    }
            break;

        case NOOPERA :
            ops[ 0 ] = 0;
            asmerr( E_MISOPE );
            break;

        default :
            ops[ 0 ] = 0;
            asmerr( E_ILLOPE );
        }
    return( len );
}

static int op_dec( int a, int b )
{
    int len = 1, op;

    if ( pass == 1 )
        if ( *label )
            put_label();

    switch( op = get_reg( operand ) )
        {
        case REGA :
        case REGB :
        case REGC :
        case REGD :
        case REGE :
        case REGH :
        case REGL :
        case REGIHL :
            ops[ 0 ] = ( UBYTE )( 0x05 + ( op << 3 ) );
            break;

        case REGBC :
            ops[ 0 ] = 0x0b;
            break;

        case REGDE :
            ops[ 0 ] = 0x1b;
            break;

        case REGHL :
            ops[ 0 ] = 0x2b;
            break;

        case REGSP :
            ops[ 0 ] = 0x3b;
            break;

        case REGIX :
        case REGIY :
            len = 2;
            ops[ 0 ] = ( UBYTE )op;
            ops[ 1 ] = 0x2b;
            break;

        case NOREG :
            if ( !strncmp( operand, "(IX+", 4 ) )
                {
                len = 3;
                if ( pass == 2 )
                    {
                    ops[ 0 ] = 0xdd;
                    ops[ 1 ] = 0x35;
                    ops[ 2 ] = chk_v2( calc_val( strchr( operand, '+' ) + 1 ) );
                    }
                }
            else
                if ( ! strncmp( operand, "(IY+", 4 ) )
                    {
                    len = 3;
                    if ( pass == 2 )
                        {
                        ops[ 0 ] = 0xfd;
                        ops[ 1 ] = 0x35;
                        ops[ 2 ] = chk_v2( calc_val( strchr( operand, '+' ) + 1 ) );
                        }
                    }
                else
                    {
                    ops[ 0 ] = 0;
                    asmerr( E_ILLOPE );
                    }
            break;

        case NOOPERA :
            ops[ 0 ] = 0;
            asmerr( E_MISOPE );
            break;

        default :
            ops[ 0 ] = 0;
            asmerr( E_ILLOPE );
        }
    return( len );
}

static int op_or( int a, int b )
{
    return( op_alog( a, b, 0xB0 ) );
}

static int op_xor( int a, int b )
{
    return( op_alog( a, b, 0xA8 ) );
}

static int op_and( int a, int b )
{
    return( op_alog( a, b, 0xA0 ) );
}

static int op_cp( int a, int b )
{
    return( op_alog( a, b, 0xB8 ) );
}

static int op_cbrs( int a, int b, int ofs )
{
    int len = 1, op;

    if ( pass == 1 )
        if ( *label )
            put_label();

    switch( op = get_reg( operand ) )
        {
        case REGA :
        case REGB :
        case REGC :
        case REGD :
        case REGE :
        case REGH :
        case REGL :
        case REGIHL :
            len = 2;
            ops[ 0 ] = 0xcb;
            ops[ 1 ] = ( UBYTE )( ofs + op );
            break;

        case NOREG :
            if ( ! strncmp( operand, "(IX+", 4 ) )
                {
                len = 4;
                if ( pass == 2 )
                    {
                    ops[ 0 ] = 0xdd;
                    ops[ 1 ] = 0xcb;
                    ops[ 2 ] = chk_v2( calc_val( strchr( operand, '+' ) + 1 ) );
                    ops[ 3 ] = ( UBYTE )( ofs + 6 );
                    }
                }
            else 
                if ( ! strncmp( operand, "(IY+", 4 ) )
                    {
                    len = 4;
                    if ( pass == 2 )
                        {
                        ops[ 0 ] = 0xfd;
                        ops[ 1 ] = 0xcb;
                        ops[ 2 ] = chk_v2( calc_val( strchr( operand, '+' ) + 1 ) );
                        ops[ 3 ] = ( UBYTE )( ofs + 6 );
                        }
                    }
                else
                    {
                    ops[ 0 ] = 0;
                    asmerr( E_ILLOPE );
                    }
            break;

        case NOOPERA :
            ops[ 0 ] = 0;
            asmerr( E_MISOPE );
            break;

        default :
            ops[ 0 ] = 0;
            asmerr( E_ILLOPE );
        }
    return( len );
}

static int op_rl( int a, int b )
{
    return( op_cbrs( a, b, 0x10 ) );
}

static int op_rr( int a, int b )
{
    return( op_cbrs( a, b, 0x18 ) );
}

static int op_sla( int a, int b )
{
    return( op_cbrs( a, b, 0x20 ) );
}

static int op_sra( int a, int b )
{
    return( op_cbrs( a, b, 0x28 ) );
}

static int op_srl( int a, int b )
{
    return( op_cbrs( a, b, 0x38 ) );
}

static int op_rlc( int a, int b )
{
    return( op_cbrs( a, b, 0x00 ) );
}

static int op_rrc( int a, int b )
{
    return( op_cbrs( a, b, 0x08 ) );
}

static int op_out( int a, int b )
{
    int op;

    if ( pass == 1 )
        {
        if ( *label )
            put_label();
        }
    else
        {
        if ( ! strncmp( operand, "(C),", 4 ) )
            {
            switch( op = get_reg( get_second( operand ) ) )
                {
                case REGA :
                case REGB :
                case REGC :
                case REGD :
                case REGE :
                case REGH :
                case REGL :
                    ops[ 0 ] = 0xed;
                    ops[ 1 ] = ( UBYTE )( 0x41 + ( op << 3 ) );
                    break;

                case NOOPERA :
                    ops[ 0 ] = 0;
                    ops[ 1 ] = 0;
                    asmerr( E_MISOPE );
                    break;

                default :
                    ops[ 0 ] = 0;
                    ops[ 1 ] = 0;
                    asmerr( E_ILLOPE );
                }
            }
        else
            {
            ops[ 0 ] = 0xd3;
            ops[ 1 ] = chk_v1( calc_val( operand + 1 ) );
            }
        }
    return( 2 );
}

static int op_in( int a, int b )
{
    char *p1, *p2;
    int op;

    if ( pass == 1 )
        {
        if ( *label )
            put_label();
        }
    else
        {
        p1 = operand;
        p2 = tmp;
        while ( *p1 != ',' && *p1 )
            *p2++ = *p1++;

        *p2 = 0;
        switch( op = get_reg( tmp ) )
            {
            case REGA :
                if ( ! strncmp( operand, "A,(C)", 5 ) )
                    {
                    ops[ 0 ] = 0xed;
                    ops[ 1 ] = 0x78;
                    }
                else 
                    {
                    ops[ 0 ] = 0xdb;
                    ops[ 1 ] = chk_v1( calc_val( get_second( operand ) + 1 ) );
                    }
                break;

            case REGB :
            case REGC :
            case REGD :
            case REGE :
            case REGH :
            case REGL :
                ops[ 0 ] = 0xed;
                ops[ 1 ] = ( UBYTE )( 0x40 + ( op << 3 ) );
                break;

            default :
                ops[ 0 ] = 0;
                ops[ 1 ] = 0;
                asmerr( E_ILLOPE );
            }
        }
    return( 2 );
}

static int op_cbbit( int ofs, int dummy )
{
    char *p1, *p2;
    int len = 2;
    int i;
    int op;

    i = 0;
    if ( pass == 1 )
        if ( *label )
            put_label();

    ops[ 0 ] = 0xcb;
    p1 = operand;
    p2 = tmp;
    while ( *p1 != ',' && *p1 )
        *p2++ = *p1++;

    *p2 = 0;
    if ( pass == 2 )
        {
        i = eval( tmp );
        if ( i < 0 || i > 7 )
            asmerr( E_VALOUT );
        }
    switch( op = get_reg( ++p1 ) )
        {
        case REGA :
        case REGB :
        case REGC :
        case REGD :
        case REGE :
        case REGH :
        case REGL :
        case REGIHL :
            ops[ 1 ] = ( UBYTE )( ofs + i * 8 + op );
            break;

        case NOREG :
            if ( ! strncmp( p1, "(IX+", 4 ) )
                {
                len = 4;
                if ( pass == 2 )
                    {
                    ops[ 0 ] = 0xdd;
                    ops[ 1 ] = 0xcb;
                    ops[ 2 ] = chk_v2( calc_val( strchr( operand, '+' ) + 1 ) );
                    ops[ 3 ] = ( UBYTE )( ofs + 6 + i * 8 );
                    }
                }
            else 
                if ( ! strncmp( p1, "(IY+", 4 ) )
                    {
                    len = 4;
                    if ( pass == 2 )
                        {
                        ops[ 0 ] = 0xfd;
                        ops[ 1 ] = 0xcb;
                        ops[ 2 ] = chk_v2( calc_val( strchr( operand, '+' ) + 1 ) );
                        ops[ 3 ] = ( UBYTE )( ofs + 6 + i * 8 );
                        }
                    }
                else
                    {
                    ops[ 1 ] = 0;
                    asmerr( E_ILLOPE );
                    }
            break;

        case NOOPERA :
            ops[ 1 ] = 0;
            asmerr( E_MISOPE );
            break;

        default :
            ops[ 1 ] = 0;
            asmerr( E_ILLOPE );
        }
    return( len );
}

static char * GetFileName( char * line )
{
    static char fn[ LENFN ];

    char * p = line;
    char * d = fn;
    while( isspace( *p ) )
        p++;
    while( !isspace( *p ) )
        p++;
    while( isspace( *p ) || *p == '"' )
        p++;
    while( !isspace( *p ) && *p != COMMENT && *p != '"' )
        *d++ = *p++;
    *d = 0;

    return( fn );
}

static int op_incbin( int a, int dummy )
{
    FILE * fp;
    unsigned char b;

    char * p = GetFileName( line );
    if ( fp = fopen( p, "rb" ) )
        {
        if ( gencode )
            while( fread( &b, 1, 1, fp ) )
                if ( pass == 2 )
                    Poke8Ext( ( USHORT )pc++, b );
                else
                    pc++;

        fclose( fp );
        }
    else
        fatal( F_FOPEN, p );

    return( 0 );
}

static void Assemble( char *fn )
{
    char * p;
    char Tmp[ 16 ];

    srcfn = fn;
    if ( srcfp = fopen( fn, "r" ) )
        {
        for ( c_line = 1; p = fgets( line, MAXLINE, srcfp ); c_line++ )
            {
            if ( DoList && pass == 2 )
                AddList( "%-6s\t#%04X  %s", itoa( c_line, Tmp, 10 ), pc, line, 0 );

            p = get_label( label, p );
            p = get_opcode( opcode, p );
            p = get_arg( operand, p );
            if ( ! strcmp( opcode, ENDFILE ) )
                break;

            if ( * opcode )
                {
                StOpc * op = search_op( opcode );
                if ( op )
                    {
                    int op_count = ( *op->fun )( op->c1, op->c2 );
                    if ( gencode ) 
                        {
                        if ( pass == 1 )
                            pc += op_count;
                        else
                            for ( int i = 0; i < op_count; i++ )
                                Poke8Ext( ( USHORT )pc++, ops[ i ] );
                        }
                    }
                }
            else
                if ( * label && pass == 1 )
                    put_label();
            }
        fclose( srcfp );
        }
    else
        fatal( F_FOPEN, fn );
}

static int op_misc( int op_code, int dummy )
{
    static int incnest;
    static struct
        {
        unsigned inc_line;
        char *inc_fn;
        FILE *inc_fp;
        } incl[ INCNEST ];

    if ( gencode )
        {
        switch( op_code )
            {
            case 1 : // LIST
            case 2 : // NOLIST
                DoList = op_code == 1;
                break;

            case 3 : // BANK
                WriteVGA( 0xC0 + ( eval( operand ) & 0x0F ) );
                break;

            case 4 : // READ / INCLUDE
                if ( incnest < INCNEST )
                    {
                    incl[ incnest ].inc_line = c_line;
                    incl[ incnest ].inc_fn = srcfn;
                    incl[ incnest ].inc_fp = srcfp;
                    incnest++;
                    Assemble( GetFileName( line ) );
                    incnest--;
                    c_line = incl[ incnest ].inc_line;
                    srcfn = incl[ incnest ].inc_fn;
                    srcfp = incl[ incnest ].inc_fp;
                    }
                else
                    asmerr( E_INCNEST );

                break;

            case 5 : // WRITESNA
                break;
            }
        }
    return( 0 );
}

static void AssembleFile( char * infile, CMyListBox * l )
{
    int OldDecodeur = Cpc.VGARom;
    int OldRam = Cpc.VGARam;
    NbSymboles = 0;
    errors = 0;
    iflevel = 0;
    gencode = 1;
    run = -1;
    liste = l;
    pass = 1;
    pc = 0;
    Assemble( infile );
    pass = 2;
    pc = 0;
    Assemble( infile );
    WriteVGA( OldDecodeur );
    WriteVGA( 0xC0 + OldRam );
    if ( errors )
        AddList( "->%s%d erreur%s", "", errors, errors > 1 ? "s" : "", ITEM_PC );
    else
        AddList( "Aucune erreur détectée.", "", 0, "", ITEM_PC );

    if ( ! errors && run > -1 )
        {
        WriteVGA( LastMode + 0x8C );
        PUSH( RegPC );
        RegPC = ( USHORT )run;
        }
}


AsmOutput::AsmOutput(CWnd* pParent ) : CDialog(AsmOutput::IDD, pParent) { }

void AsmOutput::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_SORTIE, m_Sortie);
}

BEGIN_MESSAGE_MAP(AsmOutput, CDialog)
    ON_WM_CLOSE()
END_MESSAGE_MAP()


BOOL AsmOutput::OnInitDialog()
{
    CDialog::OnInitDialog();

    font8.CreatePointFont( 80, "Courier New", this->GetDC() );
    m_Sortie.SetFont( &font8 );
    m_Sortie.SetMaxLen( 128 );

    return TRUE;
}


void AsmOutput::Assemble( char * Nom )
{
    MSG msg;

    CDialog::Create( IDD_ASM_OUT, 0 );
    CDialog::ShowWindow( SW_SHOW );
    AssembleFile( Nom, &m_Sortie );
    Close = 0;
    liste->SetCurSel( liste->GetCount() - 1 );
    while( ! Close )
        {
        Sleep( 1 );
        if ( PeekMessage( &msg, NULL, 0,0, PM_REMOVE ) )
            DispatchMessage( &msg );
        }
}


void AsmOutput::OnClose()
{
    Close = 1;
    CDialog::OnClose();
}
