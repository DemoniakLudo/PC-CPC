#include  <string.h>
#include  <stdlib.h>
#include  <ctype.h>

#include  "Types.h"
#include  "Log.h"
#include  "Z80.h"
#include  "SnapShot.h"

#define     MODULENAME      "BreakPoints"

#define     MAX_BREAKPOINTS 1024

typedef struct {
	USHORT  Adr;
	UBYTE   Enable;
	UBYTE   PokeCdt;
	UBYTE   PeekCdt;
	char    Condition[ 128 ];
} StBreakPoint;

static int NbBreakPoints;

static StBreakPoint TabBreakPoints[ MAX_BREAKPOINTS ];

static int GetValeur( char * Registre ) {
	typedef struct {
		char * Name;
		USHORT * Reg;
	} StValReg16;

	typedef struct {
		char * Name;
		UBYTE * Reg;
	} StValReg8;

	typedef struct {
		char * Name;
		int Bit;
	} StValFlag;

	static StValReg16 TabValReg16[] = {
		{ "AF", &RegAF },
		{ "BC", &RegBC },
		{ "DE", &RegDE },
		{ "HL", &RegHL },
		{ "IX", &RegIX },
		{ "IY", &RegIY },
		{ "SP", &RegSP },
		{ "PC", &RegPC },
		{ 0, 0 }
	};

	static StValReg8 TabValReg8[] = {
		{ "A", &RegA },
		{ "B", &RegB },
		{ "C", &RegA },
		{ "D", &RegB },
		{ "E", &RegA },
		{ "H", &RegB },
		{ "L", &RegA },
		{ "I", &RegB },
		{ "R", &RegA },
		{ 0, 0 }
	};

	static StValFlag TabValFlag[]= {
		{ "FLAG_S", 7 },
		{ "FLAG_Z", 6 },
		{ "FLAG_H", 4 },
		{ "FLAG_V", 2 },
		{ "FLAG_N", 1 },
		{ "FLAG_C", 0 },
		{ 0, 0 }
	};
	int i;

	for( i = 0; TabValReg16[ i ].Name; i++ )
		if ( ! strcmp( Registre, TabValReg16[ i ].Name ) )
			return( * TabValReg16[ i ].Reg );

	for ( i = 0; TabValReg8[ i ].Name; i++ )
		if ( ! strcmp( Registre, TabValReg8[ i ].Name ) )
			return( * TabValReg8[ i ].Reg );

	for ( i = 0; TabValFlag[ i ].Name; i++ )
		if ( ! strcmp( Registre, TabValFlag[ i ].Name ) )
			return( ( FLAGS >> TabValFlag[ i ].Bit ) & 1 );

	Log( MODULENAME, "Registre/Flag inconnu : %s", Registre, LOG_WARNING );
	return( -1 );
}

//
// Evaluation d'un terme de l'expression
//
static int EvalTerme( char * Expression ) {
	static char VarTemp[ 128 ];
	int l = 0;
	int Valeur = 0;

	while( Expression[ l ] && Expression[ l ] <= ' ' )
		l++;

	// Traitement valeurs en hexa (#xxxx)
	if ( Expression[ l ] == '#' ) {
		int ValHex = 0, ValFull = 0;
		while( 1 ) {
			l++;
			if ( isdigit( Expression[ l ] ) )
				ValHex = Expression[ l ] - '0';
			else
				if ( toupper( Expression[ l ] ) >= 'A' && toupper( Expression[ l ] <= 'F' ) )
					ValHex = toupper( Expression[ l ]  ) - 'A' + 10;
				else
					break;

			ValFull = ValFull << 4;
			ValFull += ValHex;
		}
		Valeur = ValFull;
	}
	else
		if ( isdigit( Expression[ l ] ) ) {
			Valeur = atoi( &Expression[ l ] );
			while( isdigit( Expression[ l ] ) || Expression[ l ] == '.' )
				l++;
		}
		else {
			int m = 0;
			strcpy( VarTemp, &Expression[ l ] );
			while( __iscsym( Expression[ l ] ) ) {
				l++;
				m++;
			}
			//
			// Evite la boucle infinie si une expression ne peut être évaluée
			//
			if ( ! m ) {
				Log( MODULENAME, "Erreur d'expression : %s", Expression, LOG_WARNING );
				* Expression = 0;           // Pour sortir de la boucle d'évaluation
				//ErreurExpression = TRUE;
			}
			VarTemp[ m ] = 0;
			Valeur = GetValeur( VarTemp );
		}
		strcpy( Expression, &Expression[ l ] );
		return( Valeur );
}


//
// Calcule le résultat d'une expression arithmétique et logique
//
static int CalcExpression( int Resultat, char * Expression )
{
	while( * Expression ) {
		if ( ! strncmp( Expression, "!=", 2 ) ) {
			strcpy( Expression, &Expression[ 2 ] );
			Resultat = ( Resultat != CalcExpression( Resultat, Expression ) );
		}
		else
			if ( ! strncmp( Expression, "==", 2 ) ) {
				strcpy( Expression, &Expression[ 2 ] );
				Resultat = ( Resultat == CalcExpression( Resultat, Expression ) );
			}
			else
				if ( ! strncmp( Expression, "<=", 2 ) ) {
					strcpy( Expression, &Expression[ 2 ] );
					Resultat = ( Resultat <= CalcExpression( Resultat, Expression ) );
				}
				else
					if ( ! strncmp( Expression, ">=", 2 ) ) {
						strcpy( Expression, &Expression[ 2 ] );
						Resultat = ( Resultat >= CalcExpression( Resultat, Expression ) );
					}
					else {
						switch( * Expression ) {
						case '(' :
							Resultat = CalcExpression( Resultat, ++Expression );
							while( * Expression && * Expression != ')' )
								Expression++;

							strcpy( Expression, &Expression[ 1 ] );
							return( Resultat );
							break;

						case '<' :
							strcpy( Expression, &Expression[ 1 ] );
							Resultat = ( Resultat < CalcExpression( Resultat, Expression ) );
							break;

						case '>' :
							strcpy( Expression, &Expression[ 1 ] );
							Resultat = ( Resultat > CalcExpression( Resultat, Expression ) );
							break;

						case '+' :
							strcpy( Expression, &Expression[ 1 ] );
							Resultat += CalcExpression( Resultat, Expression );
							break;

						case '-' :
							strcpy( Expression, &Expression[ 1 ] );
							Resultat -= CalcExpression( Resultat, Expression );
							break;

						case '*' :
							strcpy( Expression, &Expression[ 1 ] );
							Resultat *= CalcExpression( Resultat, Expression );
							break;

						case '/' :
							strcpy( Expression, &Expression[ 1 ] );
							Resultat /= CalcExpression( Resultat, Expression );
							break;

						case '%' :
							strcpy( Expression, &Expression[ 1 ] );
							Resultat = ( int )Resultat % ( int )CalcExpression( Resultat, Expression );
							break;

						case '&' :
							strcpy( Expression, &Expression[ 1 ] );
							Resultat = ( int )Resultat & ( int )CalcExpression( Resultat, Expression );
							break;

						case '|' :
							strcpy( Expression, &Expression[ 1 ] );
							Resultat = ( int )Resultat | ( int )CalcExpression( Resultat, Expression );
							break;

						case '^' :
							strcpy( Expression, &Expression[ 1 ] );
							Resultat = ( int )Resultat ^ ( int )CalcExpression( Resultat, Expression );
							break;

						case '~' :
							strcpy( Expression, &Expression[ 1 ] );
							Resultat = ~ ( int )CalcExpression( Resultat, Expression );
							break;

						case '!' :
							strcpy( Expression, &Expression[ 1 ] );
							Resultat = ! ( int )CalcExpression( Resultat, Expression );
							break;

						case ')' :
							strcpy( Expression, &Expression[ 1 ] );
							return( Resultat );

						case ' ' :
							strcpy( Expression, &Expression[ 1 ] );
							break;

						default :
							Resultat = EvalTerme( Expression );
						}
					}
	}
	return( Resultat );
}


void AddOrToogleBreakPoint( int Adr ) {
	BOOL Trouve = FALSE;

	for ( int i = 0; i < NbBreakPoints; i++ )
		if ( TabBreakPoints[ i ].Adr == Adr ) {
			TabBreakPoints[ i ].Enable ^= 1;
			Trouve = TRUE;
			break;
		}
		if ( ! Trouve ) {
			TabBreakPoints[ NbBreakPoints ].Adr = ( USHORT )Adr;
			TabBreakPoints[ NbBreakPoints ].Enable = 1;
			NbBreakPoints++;
		}
}

void SetPokeAdr( int Adr ){
}

void SetPeekAdr( int Adr ){
}

int CheckBreakPoint( int Adr ) {
	for ( int i = 0; i < NbBreakPoints; i++ ) {
		if ( TabBreakPoints[ i ].PokeCdt ) {
			TabBreakPoints[ i ].PokeCdt = FALSE;
			return( 1 );
		}
		if ( TabBreakPoints[ i ].PeekCdt ) {
			TabBreakPoints[ i ].PeekCdt = FALSE;
			return( 1 );
		}

		if ( TabBreakPoints[ i ].Adr == Adr && TabBreakPoints[ i ].Enable )
			return( 1 );
	}
	return( 0 );
}
