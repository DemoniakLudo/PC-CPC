#include  "Types.h"
#include  "Plateform.h"
#include  "SnapShot.h"
#include  "GestDsk.h"
#include  "Log.h"
#include  "Z80.h"


#define     MODULENAME      "Upd"

#define DEBUG_UPD

#define TPS_MOTOR_ON    400000      // temps ready après un motor on (en µs)
#define TPS_DATA_READ   34          // temps entre chaque "read" de datas
#define TPS_OVER_RUN    68          // temps entre chaque accès secteur avant overrun

// Sens des commades
enum { SENS_NONE = 0, SENS_RD, SENS_WR };


// Définition des fonctions UPD : de type int fct( int OctetEcris, int sens, int TpsCycle )
// Si sens == 0, alors exécution depuis Cycle, sinon exécution IO<->Z80
typedef int ( * pFctExec )( int, int, int );


static int CmdInvalid( int, int, int );
static int CmdReadTrack( int, int, int );
static int CmdSpecify( int, int, int );
static int CmdSenseDriveStatus( int, int, int );
static int CmdWriteData( int, int, int );
static int CmdReadData( int, int, int );
static int CmdRecalibrate( int, int, int );
static int CmdSenseInterStatus( int, int, int );
static int CmdReadID( int, int, int );
static int CmdReadDelData( int, int, int );
static int CmdFormatTrack( int, int, int );
static int CmdSeek( int, int, int );
static int CmdWriteDelData( int, int, int );
static int CmdScanLow( int, int, int );
static int CmdScanLowEq( int, int, int );
static int CmdScanHighEq( int, int, int );

// Fonction EXECUTE
static pFctExec FctExec;

// Position dans la pile
static int PosStack;

// Liste des commandes
static pFctExec TabCmdUPD[ 0x20 ] = {
	CmdInvalid,            // 0x00
	CmdInvalid,            // 0x01
	CmdReadTrack,          // 0x02 READ TRACK
	CmdSpecify,            // 0x03 SPECIFY
	CmdSenseDriveStatus,   // 0x04 SENSE DRIVE STATUS
	CmdWriteData,          // 0x05 WRITE DATA
	CmdReadData,           // 0x06 READ DATA
	CmdRecalibrate,        // 0x07 RECALIBRATE
	CmdSenseInterStatus,   // 0x08 SENSE INTERRUPT STATUS
	CmdWriteDelData,       // 0x09 WRITE DELETED DATA
	CmdReadID,             // 0x0A READ ID
	CmdInvalid,            // 0x0B
	CmdReadDelData,        // 0x0C READ DELETED DATA
	CmdFormatTrack,        // 0x0D FORMAT TRACK
	CmdInvalid,            // 0x0E
	CmdSeek,               // 0x0F SEEK
	CmdInvalid,            // 0x10
	CmdScanLow,            // 0x11 SCAN LOW
	CmdInvalid,            // 0x12
	CmdInvalid,            // 0x13
	CmdInvalid,            // 0x14
	CmdInvalid,            // 0x15
	CmdInvalid,            // 0x16
	CmdInvalid,            // 0x17
	CmdInvalid,            // 0x18
	CmdScanLowEq,          // 0x19 SCAN LOW OR EQUAL
	CmdInvalid,            // 0x1A
	CmdInvalid,            // 0x1B
	CmdInvalid,            // 0x1C
	CmdScanHighEq,         // 0x1D SCAN HIGH OR EQUAL
	CmdInvalid,            // 0x1E
	CmdInvalid             // 0x1F
};


#ifdef DEBUG_UPD
static int LastFctUpd, LastPC;

static char * TabNomCmdUPD[ 0x20 ] = {
	"CmdInvalid         ", // 0x00
	"CmdInvalid         ", // 0x01
	"CmdReadTrack       ", // 0x02 READ TRACK
	"CmdSpecify         ", // 0x03 SPECIFY
	"CmdSenseDriveStatus", // 0x04 SENSE DRIVE STATUS
	"CmdWriteData       ", // 0x05 WRITE DATA
	"CmdReadData        ", // 0x06 READ DATA
	"CmdRecalibrate     ", // 0x07 RECALIBRATE
	"CmdSenseInterStatus", // 0x08 SENSE INTERRUPT STATUS
	"CmdWriteDelData    ", // 0x09 WRITE DELETED DATA
	"CmdReadID          ", // 0x0A READ ID
	"CmdInvalid         ", // 0x0B
	"CmdReadDelData     ", // 0x0C READ DELETED DATA
	"CmdFormatTrack     ", // 0x0D FORMAT TRACK
	"CmdInvalid         ", // 0x0E
	"CmdSeek            ", // 0x0F SEEK
	"CmdInvalid         ", // 0x10
	"CmdScanLow         ", // 0x11 SCAN LOW
	"CmdInvalid         ", // 0x12
	"CmdInvalid         ", // 0x13
	"CmdInvalid         ", // 0x14
	"CmdInvalid         ", // 0x15
	"CmdInvalid         ", // 0x16
	"CmdInvalid         ", // 0x17
	"CmdInvalid         ", // 0x18
	"CmdScanLowEq       ", // 0x19 SCAN LOW OR EQUAL
	"CmdInvalid         ", // 0x1A
	"CmdInvalid         ", // 0x1B
	"CmdInvalid         ", // 0x1C
	"CmdScanHighEq      ", // 0x1D SCAN HIGH OR EQUAL
	"CmdInvalid         ", // 0x1E
	"CmdInvalid         "  // 0x1F
};
#endif

// Status registrer
static int Status;

// Bits de Status
#define STATUS_CB       0x10        // Si positionné = UPD Occupé (exécution commande)
#define STATUS_EXE      0x20        // Si positionné = phase EXECUTION
#define STATUS_DIO      0x40        // Si positionné = sens UPD->Z80, sinon Z80->UPD
#define STATUS_RQM      0x80        // Si positionné = UPD Ready

static int ST0, ST1, ST2, ST3;

static int UnitSel;                 // Drive sélectionné
static int HeadSel;                 // Tête sélectionnée

// ST0
#define ST0_NR          0x08        // Not ready
#define ST0_EC          0x10        // Equipment Check (erreur de seek)
#define ST0_SE          0x20        // Seek end
#define ST0_IC1         0x40        // Interrupt Code (1)
#define ST0_IC2         0x80        // Interrupt Code (2)

// ST1
#define ST1_MA          0x01        // Missing Adress mark
#define ST1_ND          0x04        // No data
#define ST1_OR          0x10        // Over Run
#define ST1_DE          0x20        // Data Error
#define ST1_EN          0x80        // End of cylinder

// ST2
#define ST2_MD          0x01        // No Data Address Mark
#define ST2_BC          0x02        // Bad cylinder
#define ST2_WC          0x10        // Wrong cylinder
#define ST2_CRC         0x20        // CRC Error
#define ST2_CM          0x40        // Deleted Address mark

// ST3
#define ST3_HD          0x04        // Head address
#define ST3_TS          0x08        // Double-sided
#define ST3_T0          0x10        // Tête en Track 0
#define ST3_RY          0x20        // Drive ready


// Temps
static int SRT, HUT, HLT;

static int TpsReady;
static int TpsNextByte;
static int TpsOverRun;
static int CptOverRun;
static int CptRotation[ 4 ]; // Compteur rotation pour chaque disquette


// Position demandée
static int NewCylinder;
static int NbSteps;

static int IndexSecteur = 0;

static int PosData = 0;

static int TailleSect = 0;

static int Weak = 1;

static CPCEMUTrack * CurrTrackDatasDSK[ MAX_DSK ][ 2 ];

static int Break = 0;


static void StartExePhase( void ) {
	Status |= STATUS_EXE;
	SetLedFDC( 1 );
}

static void EndExePhase( void ) {
	Status &= ~STATUS_EXE;
	SetLedFDC( 0 );
}


static int CalcST0( void ) {
	Status &= 0xF0;
	ST0 = UnitSel; // drive A ou B ###
	if ( ! Cpc.FddMotor || UnitSel > 1 || ! Dsk[ UnitSel ].Image )
		ST0 |= ST0_IC1 | ST0_NR;
	else
		ST0 &= ~ST0_IC1 & ~ST0_NR;
	//ST0 &= 0xFC;
	//ST0 |= UnitSel;
	//if ( ! Dsk[ UnitSel ].Image )
	//    ST0 |= ST0_NR;
	//else
	//    ST0 &= ~ST0_NR;

	return( ST0 );
}

static void CalcTailleSect( CPCEMUSect * Sect ) {
	TailleSect = Sect->SectSize ? Sect->SectSize : 128 << Sect->N;
	Weak = ( Sect->SectSize / TailleSect ) - 1;

	if ( TailleSect > 0x1800 )
		TailleSect = 0x1800;
}

static int NextSecteur( int * sectC, int * sectH, int * sectR, int * sectN ) {
	static int PosWeak = 0;

	if ( UnitSel < 2 && Dsk[ UnitSel ].Image && CurrTrackDatasDSK[ UnitSel ][ HeadSel ] ) {
		// Calcule la "position" du secteur actuel
		int i, tps = 146 * 32; // 146 octets avant le premier secteur
		for ( i = 0; i <= IndexSecteur; i++ ) {
			tps += 0; // ###
		}
		ST1 &= ~ST1_ND;
		CPCEMUSect * Sect = &CurrTrackDatasDSK[ UnitSel ][ HeadSel ]->Sect[ IndexSecteur ];
		CalcTailleSect( Sect );
		PosData += TailleSect + PosWeak;
		PosWeak += TailleSect;
		if ( PosWeak > TailleSect * Weak )
			PosWeak = 0;

		if ( ++IndexSecteur >= CurrTrackDatasDSK[ UnitSel ][ HeadSel ]->NbSect ) {
			IndexSecteur = 0;
			PosData = 0;
		}
		Sect = &CurrTrackDatasDSK[ UnitSel ][ HeadSel ]->Sect[ IndexSecteur ];
		* sectC = Sect->C;
		* sectH = Sect->H;
		* sectR = Sect->R;
		* sectN = Sect->N;
	}
	else
		* sectC = * sectH = * sectR = * sectN = 0xFF;

	return( IndexSecteur );
}

static void SetSecteur( int index ) {
	int HoleIndex = 0;
	int tmpC, tmpH, tmpR, tmpN;

	if ( UnitSel < 2 && Dsk[ UnitSel ].Image && CurrTrackDatasDSK[ UnitSel ][ HeadSel ] )
	{
		while( HoleIndex < 2 ) {
			CPCEMUSect * Sect = &CurrTrackDatasDSK[ UnitSel ][ HeadSel ]->Sect[ IndexSecteur ];
			if ( IndexSecteur == index ) {
				ST0 &= ~ST0_IC1;
				ST1 = Sect->ST1 & ( ST1_DE | ST1_ND | ST1_MA );
				ST2 = Sect->ST2 & ( ST2_CM | ST2_CRC | ST2_MD );
				CalcTailleSect( Sect );
				return;
			}
			if ( ! NextSecteur( &tmpC, &tmpH, &tmpR, &tmpN ) )
				HoleIndex++;
		}
	}
	ST0 |= ST0_IC1;
	ST1 |= ST1_ND;
	Log( MODULENAME, "index %d non trouvé", index, LOG_INFO );
}

static void RechercheSecteur( int findC, int findH, int findR, int findN, int Eot ) {
	int HoleIndex = 0;
	int tmpC, tmpH, tmpR, tmpN;

	if ( UnitSel < 2 && Dsk[ UnitSel ].Image && CurrTrackDatasDSK[ UnitSel ][ HeadSel ] ) {
		while( HoleIndex < 2 ) {
			CPCEMUSect * Sect = &CurrTrackDatasDSK[ UnitSel ][ HeadSel ]->Sect[ IndexSecteur ];
			if ( Sect->R == findR ) {
				if ( Sect->C == findC ) {
					ST2 &= ~ST2_WC;
					ST2 &= ~ST2_BC;
					if ( Sect->H == findH && Sect->N == findN ) {
						ST0 &= ~ST0_IC1;
						ST1 = Sect->ST1 & ( ST1_DE | ST1_ND | ST1_MA );
						ST2 = Sect->ST2 & ( ST2_CM | ST2_CRC | ST2_MD );
						CalcTailleSect( Sect );
						if ( findR == Eot )
							ST1 |= ST1_EN;

						return;
					}
				}
				else {
					ST2 |= ST2_WC;
					if ( Sect->C == 0xFF )
						ST2 |= ST2_BC;

					break;
				}
			}
			if ( ! NextSecteur( &tmpC, &tmpH, &tmpR, &tmpN ) )
				HoleIndex++;
		}
	}
	ST0 |= ST0_IC1;
	ST1 |= ST1_ND;
	Log( MODULENAME, "secteur (CHRN:#%08X) non trouvé", ( findC << 24 ) + ( findH << 16 ) + ( findR << 8 ) + findN, LOG_INFO );
}

static void MoveTrack( int tps, int maxMove ) {
	static int TimeMoveTrack;

	if ( ! tps ) {
		TimeMoveTrack = 0;
		NbSteps = maxMove;
		ST0 &= ~ST0_SE & ~ST0_EC;
		ST3 &= ~ST3_RY;
		Status &= ~STATUS_RQM;          // UPD Occupé à déplacer la tête
		StartExePhase();
		TpsNextByte += SRT;
	}
	else
		if ( Cpc.FddCurrTrack[ UnitSel ] == NewCylinder || ! NbSteps ) {
			FctExec = 0;
			ST0 &= ~ST0_IC1 & ~ST0_IC2;
			ST0 |= ST0_SE;
			if ( UnitSel < 2 && Dsk[ UnitSel ].Image ) {
				AffichePiste();
				CurrTrackDatasDSK[ UnitSel ][ 0 ] = Dsk[ UnitSel ].Tracks[ Cpc.FddCurrTrack[ UnitSel ] ][ 0 ];
				CurrTrackDatasDSK[ UnitSel ][ 1 ] = Dsk[ UnitSel ].Tracks[ Cpc.FddCurrTrack[ UnitSel ] ][ 1 ];
				ST3 |= ST3_RY;
			}

			if ( ! Cpc.FddCurrTrack[ UnitSel ] )
				ST3 |= ST3_T0;
			else
				ST3 &= ~ST3_T0;

			TpsNextByte = 0;
			IndexSecteur = PosData = 0;
			if ( Cpc.FddCurrTrack[ UnitSel ] != NewCylinder )
				ST0 |= ST0_EC;

			Status &= ~STATUS_CB & ~STATUS_DIO; // ## pourquoi mettre le ~STATUS_CB en commentaire ???
			Status |= STATUS_RQM;
			EndExePhase();
		}
		else {
			TpsNextByte += tps;
			TimeMoveTrack += tps;
			if ( TimeMoveTrack > SRT ) {
				TimeMoveTrack -= SRT;
				if ( Cpc.FddCurrTrack[ UnitSel ] > NewCylinder )
					Cpc.FddCurrTrack[ UnitSel ]--;
				else
					Cpc.FddCurrTrack[ UnitSel ]++;

				NbSteps--;
				AffichePiste();
			}
		}
}

static int CmdInvalid( int val, int sens, int tps ) {
	if ( sens == SENS_WR && ! ( Status & STATUS_DIO ) )
		Status |= STATUS_DIO; // phase result

	if ( sens == SENS_RD && ( Status & STATUS_DIO ) ) {
		FctExec = 0;            // Prêt pour une nouvelle commande
		Status &= ~STATUS_DIO & ~STATUS_CB;  // Repositionné en Z80->UPD (phase cmd)
		EndExePhase();
		ST0 = ST0_IC2;  // ### ST0 forcé à 0x80
		return( ST0 );
	}
	Log( MODULENAME, "Commande Invalide appelée.", LOG_INFO );
	return( 0xFF );
}

//
// Données inter-secteur (pour CmdReadTrack)
//
static int GetExtraData( int * posExtra, int gap ) {
	static UBYTE BufExtra[ 0x100 ];
	static int posBuf;

	if ( * posExtra == -1 ) { // Initialisation
		posBuf = 0;
		BufExtra[ posBuf++ ] = 0x00; // ### crc secteur
		BufExtra[ posBuf++ ] = 0x00; // ### crc secteur
		memset( &BufExtra[ posBuf ], 0x4E, gap );
		posBuf += gap;
		memset( &BufExtra[ posBuf ], 0x00, 12 );
		posBuf += 12;
		BufExtra[ posBuf++ ] = 0xA1;
		BufExtra[ posBuf++ ] = 0xA1;
		BufExtra[ posBuf++ ] = 0xA1;
		BufExtra[ posBuf++ ] = 0xFE;
		BufExtra[ posBuf++ ] = 0x00; // ### C;
		BufExtra[ posBuf++ ] = 0x00; // ### H;
		BufExtra[ posBuf++ ] = 0x00; // ### R;
		BufExtra[ posBuf++ ] = 0x00; // ### N;
		BufExtra[ posBuf++ ] = 0x00; // ### crc id
		BufExtra[ posBuf++ ] = 0x00; // ### crc id
		memset( &BufExtra[ posBuf ], 0x4E, 22 );
		posBuf += 22;
		memset( &BufExtra[ posBuf ], 0x00, 12 );
		posBuf += 12;
		BufExtra[ posBuf++ ] = 0xA1;
		BufExtra[ posBuf++ ] = 0xA1;
		BufExtra[ posBuf++ ] = 0xA1;
		BufExtra[ posBuf++ ] = 0xFB;
		* posExtra = posBuf;
		posBuf = 0;
	}
	(* posExtra)--;
	return( BufExtra[ posBuf++ ] );
}


static int CmdReadTrack( int val, int sens, int tps ) {
	static int cntdata = 0, newPos, posSect = 0, posExtra = -1;
	static int sectC, sectH, sectR, sectN, sectEOT, sectGAP, sectSize;
	int ret;

	if ( sens == SENS_WR && ! ( Status & STATUS_DIO ) ) {
		switch( PosStack++ ) {
		case 1 :
			UnitSel = val & 3;
			HeadSel = ( val >> 2 ) & 1;
			ST0 &= ~ST0_IC1 & ~ST0_IC2;
			break;

		case 2 :
			sectC = val;
			break;

		case 3 :
			sectH = val;
			break;

		case 4 :
			sectR = val;
			break;

		case 5 :
			sectN = val;
			break;

		case 6 :
			sectEOT = val;
			break;

		case 7 :
			sectGAP = val;
			break;

		case 8 :
			sectSize = val;
#ifdef DEBUG_UPD
			Log( MODULENAME, "PC=#%04X %s(#%02X)\t\tparamètres = (Unit=#%02X,C=#%02X,H=#%02X,R=#%02X,N=#%02X,EOT=#%02X,GAP=#%02X,Len=#%02X)", RegPC, TabNomCmdUPD[ LastFctUpd & 0x1F ], LastFctUpd, UnitSel + ( HeadSel << 2 ), sectC, sectH, sectR, sectN, sectEOT, sectGAP, sectSize, LOG_INFO );
#endif
			SetSecteur( posSect ); // Se positionne sur le premier secteur après le trou d'index"
			cntdata = PosData;
			Status |= STATUS_DIO;
			if ( ! ( ST1 & ST1_ND ) ) {
				StartExePhase();
				TpsOverRun = TPS_OVER_RUN * 8000; //### Valeur arbitraire, temps entre id et données secteurs
				CptOverRun = 1;
				if ( ! sectN )
					TailleSect = sectSize;
			}
			else {
				PosStack++;
				Break = 1; // Ne peut pas passer en EXECUTE
			}
			break;
		}
	}
	if ( sens == SENS_RD && ( Status & STATUS_DIO ) ) {
		switch( PosStack++ ) {
		case 9 :
			TpsOverRun = TPS_OVER_RUN ;//* 6250; // ##### GRRR
			posExtra = -1;
			ret = Dsk[ UnitSel ].Data[ Cpc.FddCurrTrack[ UnitSel ] ][ HeadSel ][ cntdata++ ];
			if ( Status & STATUS_EXE ) {
				if ( --TailleSect ) // Tant que fin de secteur pas atteinte
					PosStack--;
			}
			else
				PosStack++;
#ifdef DEBUG_UPD
			Log( MODULENAME, "PC=#%04X %s(#%02X)\t\t case  9  ret=#%02X,TailleSect=#%04X,Status=#%02X", RegPC, TabNomCmdUPD[ LastFctUpd & 0x1F ], LastFctUpd, ret, TailleSect, Status, 0, 0, 0, 0, LOG_INFO );
#endif
			return( ret );
			break;

		case 10 :
			TpsOverRun = TPS_OVER_RUN * 6250;
			ret = GetExtraData( &posExtra, CurrTrackDatasDSK[ UnitSel ][ HeadSel ]->Gap3 );
			if ( Status & STATUS_EXE ) {
				if ( posExtra )
					PosStack--;
				else {
					if ( posSect++ < CurrTrackDatasDSK[ UnitSel ][ HeadSel ]->NbSect ) { // Encore des secteurs à transférer ?
						SetSecteur( posSect );
						cntdata = PosData;
						PosStack -= 2;
					}
					else {
						CptOverRun = 0;
						EndExePhase(); // Dernier octet à envoyer avant résultat
					}
				}
			}
#ifdef DEBUG_UPD
			Log( MODULENAME, "PC=#%04X %s(#%02X)\t\t case 10  ret=#%02X,  posExtra=#%04X,Status=#%02X", RegPC, TabNomCmdUPD[ LastFctUpd & 0x1F ], LastFctUpd, ret, posExtra, Status, 0, 0, 0, 0, LOG_INFO );
#endif
			return( ret );
			break;

		case 11 :
			ST0 &= ~ST0_SE;
			ST0 |= ST0_IC1; // ### à voir
			CptOverRun = 0;
			return( ST0 );

		case 12 :
			return( ST1 );

		case 13 :
			return( ST2 );

		case 14 :
			return( sectC );

		case 15 :
			return( sectH );

		case 16 :
			return( sectR );

		case 17 :
			FctExec = 0;            // Prêt pour une nouvelle commande
			Status &= ~STATUS_DIO & ~STATUS_CB;  // Repositionné en Z80->UPD
#ifdef DEBUG_UPD
			Log( MODULENAME, "PC=#%04X %s(#%02X)\t\trésultats  = (ST0=#%02X,ST1=#%02X,ST2=#%02X,C=#%02X,H=#%02X,R=#%02X,N=#%02X)", RegPC, TabNomCmdUPD[ LastFctUpd & 0x1F ], LastFctUpd, ST0, ST1, ST2, sectC, sectH, sectR, sectN, LOG_INFO );
#endif
			return( sectN );
		}
	}
	return( 0xFF );
}

static int CmdSpecify( int val, int sens, int tps ) {
	if ( sens == SENS_WR && ! ( Status & STATUS_DIO ) ) {
		switch( PosStack++ ) {
		case 1:
			SRT = ( 16 - ( val >> 4 ) ) * 2000; // Temps déplacement d'un pas de la tête en µs
			HUT = val & 0x0F;
			break;
		case 2:
			HLT = val & 0xFE;
			FctExec = 0;
			Status &= ~STATUS_DIO & ~STATUS_CB;  // Repositionné en Z80->UPD (phase cmd)
			EndExePhase();
#ifdef DEBUG_UPD
			Log( MODULENAME, "PC=#%04X %s(#%02X)\t\tparamètres = (SRT(ms):%d,HLT:#%02X)", RegPC, TabNomCmdUPD[ LastFctUpd & 0x1F ], LastFctUpd, SRT / 1000, HLT, LOG_INFO );
#endif
			break;
		}
	}
	return( 0xFF );
}

static int CmdSenseDriveStatus( int val, int sens, int tps ) {
	if ( sens == SENS_WR && ! ( Status & STATUS_DIO ) ) {
		if ( PosStack++ ) {
			UnitSel = val & 3;
			HeadSel = ( val >> 2 ) & 1;
			ST3 &= 0xF8;
			ST3 |= UnitSel | ( HeadSel << 2 );
			Status |= STATUS_DIO; // Phase result
		}
	}
	if ( sens == SENS_RD && ( Status & STATUS_DIO ) ) {
		FctExec = 0;            // Prêt pour une nouvelle commande
		Status &= ~STATUS_DIO & ~STATUS_CB;  // Repositionné en Z80->UPD
		EndExePhase();
#ifdef DEBUG_UPD
		Log( MODULENAME, "PC=#%04X %s(#%02X)\t\trésultats  = (ST3=#%02X)", RegPC, TabNomCmdUPD[ LastFctUpd & 0x1F ], LastFctUpd, ST3, LOG_INFO );
#endif
		return( ST3 );
	}
	return( 0xFF );
}

static int ReadDataWithCM( int val, int sens, int tps, int mask ) {
	static int cntdata = 0, newPos;
	static int sectC, sectH, sectR, sectN, sectEOT, sectGAP, sectSize;
	int ret, tmpC, tmpH, tmpR, tmpN;

	if ( sens == SENS_WR && ! ( Status & STATUS_DIO ) ) {
		switch( PosStack++ ) {
		case 1 :
			UnitSel = val & 3;
			HeadSel = ( val >> 2 ) & 1;
			ST0 &= ~ST0_IC1 & ~ST0_IC2;
			break;

		case 2 :
			sectC = val;
			break;

		case 3 :
			sectH = val;
			break;

		case 4 :
			sectR = val;
			break;

		case 5 :
			sectN = val;
			break;

		case 6 :
			sectEOT = val;
			break;

		case 7 :
			sectGAP = val;
			break;

		case 8 :
			sectSize = val;
#ifdef DEBUG_UPD
			Log( MODULENAME, "PC=#%04X %s(#%02X)\t\tparamètres = (Unit=#%02X,C=#%02X,H=#%02X,R=#%02X,N=#%02X,EOT=#%02X,GAP=#%02X,Len=#%02X)", RegPC, TabNomCmdUPD[ LastFctUpd & 0x1F ], LastFctUpd, UnitSel + ( HeadSel << 2 ), sectC, sectH, sectR, sectN, sectEOT, sectGAP, sectSize, LOG_INFO );
#endif
			RechercheSecteur( sectC, sectH, sectR, sectN, sectEOT );
			ST2 ^= mask;
			cntdata = PosData;
			Status |= STATUS_DIO;
			if ( ! ( ST1 & ST1_ND ) ) {
				StartExePhase();
				TpsOverRun = TPS_OVER_RUN * 2000; //### Valeur arbitraire, temps entre id et données secteurs
				CptOverRun = 1;
				if ( ! sectN )
					TailleSect = sectSize;
			}
			else {
				PosStack++;
				Break = 1; // Ne peut pas passer en EXECUTE
			}
			break;
		}
	}
	if ( sens == SENS_RD && ( Status & STATUS_DIO ) ) {
		switch( PosStack++ ) {
		case 9 :
			TpsOverRun = TPS_OVER_RUN;
			ret = Dsk[ UnitSel ].Data[ Cpc.FddCurrTrack[ UnitSel ] ][ HeadSel ][ cntdata++ ];
			if ( Status & STATUS_EXE ) {
				if ( --TailleSect ) // Tant que fin de secteur pas atteinte
					PosStack--;
				else {
					NextSecteur( &tmpC, &tmpH, &tmpR, &tmpN );
					if ( sectR < sectEOT ) {// Encore des secteurs à transférer ?
						RechercheSecteur( sectC, sectH, ++sectR, sectN, sectEOT );
						cntdata = PosData;
						PosStack--;
					}
					else {
						CptOverRun = 0;
						EndExePhase(); // Dernier octet à envoyer avant résultat
					}
				}
			}
			return( ret );
			break;

		case 10 :
			ST0 &= ~ST0_SE;
			ST0 |= ST0_IC1; // ### à voir
			CptOverRun = 0;
			return( ST0 );

		case 11 :
			return( ST1 );

		case 12 :
			return( ST2 );

		case 13 :
			return( sectC );

		case 14 :
			return( sectH );

		case 15 :
			return( sectR );

		case 16 :
			FctExec = 0;            // Prêt pour une nouvelle commande
			Status &= ~STATUS_DIO & ~STATUS_CB;  // Repositionné en Z80->UPD
#ifdef DEBUG_UPD
			Log( MODULENAME, "PC=#%04X %s(#%02X)\t\trésultats  = (ST0=#%02X,ST1=#%02X,ST2=#%02X,C=#%02X,H=#%02X,R=#%02X,N=#%02X)", RegPC, TabNomCmdUPD[ LastFctUpd & 0x1F ], LastFctUpd, ST0, ST1, ST2, sectC, sectH, sectR, sectN, LOG_INFO );
#endif
			return( sectN );
		}
	}
	return( 0xFF );
}

static int CmdReadData( int val, int sens, int tps ) {
	return( ReadDataWithCM( val, sens, tps, 0x00 ) );
}

static int WriteDataWithCM( int val, int sens, int tps, int mask ) {
	static int cntdata = 0, newPos;
	static int sectC, sectH, sectR, sectN, sectEOT, sectGAP, sectSize;
	int tmpC, tmpH, tmpR, tmpN;

	if ( sens == SENS_WR && ! ( Status & STATUS_DIO ) ) {
		switch( PosStack++ ) {
		case 1 :
			UnitSel = val & 3;
			HeadSel = ( val >> 2 ) & 1;
			ST0 &= ~ST0_IC1 & ~ST0_IC2;
			break;

		case 2 :
			sectC = val;
			break;

		case 3 :
			sectH = val;
			break;

		case 4 :
			sectR = val;
			break;

		case 5 :
			sectN = val;
			break;

		case 6 :
			sectEOT = val;
			break;

		case 7 :
			sectGAP = val;
			break;

		case 8 :
			sectSize = val;
#ifdef DEBUG_UPD
			Log( MODULENAME, "PC=#%04X %s(#%02X)\t\tparamètres = (Unit=#%02X,C=#%02X,H=#%02X,R=#%02X,N=#%02X,EOT=#%02X,GAP=#%02X,Len=#%02X)", RegPC, TabNomCmdUPD[ LastFctUpd & 0x1F ], LastFctUpd, UnitSel + ( HeadSel << 2 ), sectC, sectH, sectR, sectN, sectEOT, sectGAP, sectSize, LOG_INFO );
#endif
			RechercheSecteur( sectC, sectH, sectR, sectN, sectEOT );
			cntdata = PosData;
			if ( ! ( ST1 & ST1_ND ) ) {
				StartExePhase();
				if ( ! sectN )
					TailleSect = sectSize;
			}
			else {
				Status |= STATUS_DIO;
				PosStack++;
			}
			break;

		case 9 :
			Dsk[ UnitSel ].Data[ Cpc.FddCurrTrack[ UnitSel ] ][ HeadSel ][ cntdata++ ] = ( UBYTE )val;
			Dsk[ UnitSel ].FlagWrite = 1;
			if ( Status & STATUS_EXE ) {
				if ( --TailleSect ) // Tant que fin de secteur pas atteinte
					PosStack--;
				else {
					NextSecteur( &tmpC, &tmpH, &tmpR, &tmpN );
					if ( sectR++ < sectEOT ) { // Encore des secteurs à transférer ?
						RechercheSecteur( sectC, sectH, sectR, sectN, sectEOT );
						cntdata = PosData;
						PosStack--;
					}
					else {
						Status |= STATUS_DIO;
						EndExePhase(); // Dernier octet à envoyer avant résultat
					}
				}
			}
			break;
		}
	}
	if ( sens == SENS_RD && ( Status & STATUS_DIO ) ) {
		switch( PosStack++ ) {
		case 10 :
			ST0 &= ~ST0_SE;
			ST0 |= ST0_IC1; // ### à voir
			CptOverRun = 0;
			return( ST0 );

		case 11 :
			return( ST1 );

		case 12 :
			return( ST2 );

		case 13 :
			return( sectC );

		case 14 :
			return( sectH );

		case 15 :
			return( sectR );

		case 16 :
			FctExec = 0;            // Prêt pour une nouvelle commande
			Status &= ~STATUS_DIO & ~STATUS_CB;  // Repositionné en Z80->UPD
			EndExePhase();
#ifdef DEBUG_UPD
			Log( MODULENAME, "PC=#%04X %s(#%02X)\t\trésultats  = (ST0=#%02X,ST1=#%02X,ST2=#%02X,C=#%02X,H=#%02X,R=#%02X,N=#%02X)", RegPC, TabNomCmdUPD[ LastFctUpd & 0x1F ], LastFctUpd, ST0, ST1, ST2, sectC, sectH, sectR, sectN, LOG_INFO );
#endif
			return( sectN );
		}
	}
	return( 0xFF );
}

static int CmdWriteData( int val, int sens, int tps ) {
	return( WriteDataWithCM( val, sens, tps, 0x00 ) );
}

static int CmdRecalibrate( int val, int sens, int tps ) {
	if ( sens == SENS_WR && ! ( Status & STATUS_DIO ) ) {
		if ( PosStack++ ) {
			UnitSel = val & 3;
			NewCylinder = 0;
			MoveTrack( 0, 77 );
#ifdef DEBUG_UPD
			Log( MODULENAME, "PC=#%04X %s(#%02X)\t\tparamètre  = (Unit=#%02X)", RegPC, TabNomCmdUPD[ LastFctUpd & 0x1F ], LastFctUpd, UnitSel, LOG_INFO );
#endif
		}
	}
	if ( sens == SENS_NONE && ( Status & STATUS_EXE ) )
		MoveTrack( tps, 0 );

	return( 0xFF );
}

static int CmdSenseInterStatus( int val, int sens, int tps ) {
	if ( sens == SENS_WR && ! ( Status & STATUS_DIO ) )
		Status |= STATUS_DIO; // Phase result

	if ( sens == SENS_RD && ( Status & STATUS_DIO ) ) {
		switch( PosStack++ ) {
		case 0:
			if ( Break ) {
				ST0 &= ~ST0_IC1;
				ST0 |= ST0_IC2;
				Break = 0;
			}
			return( ST0 );

		case 1:
#ifdef DEBUG_UPD
			Log( MODULENAME, "PC=#%04X %s(#%02X)\t\trésultats  = (ST0=#%02X, C=#%02X)", RegPC, TabNomCmdUPD[ LastFctUpd & 0x1F ], LastFctUpd, ST0, Cpc.FddCurrTrack[ UnitSel ], LOG_INFO );
#endif
			FctExec = 0;            // Prêt pour une nouvelle commande
			Status &= ~STATUS_DIO & ~STATUS_CB;  // Repositionné en Z80->UPD
			if ( ST0 & ( ST0_SE | ST0_IC1 ) )
				ST0 = ST0_IC2;

			EndExePhase();
			return( Cpc.FddCurrTrack[ UnitSel ] );
		}
	}
	return( 0xFF );
}

static int CmdReadID( int val, int sens, int tps ) {
	static int sectC, sectH, sectR, sectN;

	if ( sens == SENS_WR && ! ( Status & STATUS_DIO ) ) {
		if ( PosStack++ ) {
			UnitSel = val & 3;
			HeadSel = ( val >> 2 ) & 1;
			Status |= STATUS_DIO;
		}
	}
	if ( sens == SENS_RD && ( Status & STATUS_DIO ) ) {
		//        TpsNextByte += 16000;
		switch( PosStack++ ) {
		case 2 :
			return( CalcST0() );

		case 3 :
			return( ST1 );

		case 4 :
			return( ST2 );

		case 5 :
			NextSecteur( &sectC, &sectH, &sectR, &sectN );
			return( sectC );

		case 6 :
			return( sectH );

		case 7 :
			return( sectR );

		case 8 :
			FctExec = 0;            // Prêt pour une nouvelle commande
			Status &= ~STATUS_CB & ~STATUS_DIO;
			EndExePhase();
#ifdef DEBUG_UPD
			Log( MODULENAME, "PC=#%04X %s(#%02X)\t\trésultats  = (ST0=#%02X,ST1=#%02X,ST2=#%02X,C=#%02X,H=#%02X,R=#%02X,N=#%02X)", RegPC, TabNomCmdUPD[ LastFctUpd & 0x1F ], LastFctUpd, ST0, ST1, ST2, sectC, sectH, sectR, sectN, LOG_INFO );
#endif
			return( sectN );
		}
	}
	return( 0xFF );
}

static int CmdReadDelData( int val, int sens, int tps ) {
	return( ReadDataWithCM( val, sens, tps, ST2_CM ) );
}

static int CmdFormatTrack( int val, int sens, int tps ) {
	Log( MODULENAME, "Commande CmdFormatTrack pas implémentée.", LOG_WARNING );
	return( 0 );
}

static int CmdSeek( int val, int sens, int tps ) {
	if ( sens == SENS_WR && ! ( Status & STATUS_DIO ) ) {
		switch( PosStack++ ) {
		case 1:
			UnitSel = val & 3;
			break;

		case 2:
			NewCylinder = val;
			MoveTrack( 0, 255 );
#ifdef DEBUG_UPD
			Log( MODULENAME, "PC=#%04X %s(#%02X)\t\tparamètres = (Unité:#%02X,Piste:#%02X)", RegPC, TabNomCmdUPD[ LastFctUpd & 0x1F ], LastFctUpd, UnitSel, NewCylinder, LOG_INFO );
#endif
		}
	}
	if ( sens == SENS_NONE && ( Status & STATUS_EXE ) )
		MoveTrack( tps, 0 );

	return( 0xFF );
}

static int CmdWriteDelData( int val, int sens, int tps ) {
	return( WriteDataWithCM( val, sens, tps, ST2_CM ) );
}

static int CmdScanLow( int val, int sens, int tps ) {
	Log( MODULENAME, "Commande CmdScanLow pas implémentée.", LOG_WARNING );
	return( 0 );
}
static int CmdScanLowEq( int val, int sens, int tps ) {
	Log( MODULENAME, "Commande CmdScanLowEq pas implémentée.", LOG_WARNING );
	return( 0 );
}
static int CmdScanHighEq( int val, int sens, int tps ) {
	Log( MODULENAME, "Commande CmdScanHighEq pas implémentée.", LOG_WARNING );
	return( 0 );
}

int ReadUPD( int port ) {
	int ret = 0xFF;

	if ( port & 0x101 ) {
		if ( port & 1 ) { // Read Data Register
			Status &= ~STATUS_RQM;
			TpsNextByte += TPS_DATA_READ;
			if ( ( Status & STATUS_DIO ) )
				ret = FctExec( 0, SENS_RD, 0 );

#ifdef DEBUG_UPD
			Log( MODULENAME, "ReadUPD  (DATA) = %02X", ret, LOG_DEBUG_FULL );
#endif
		}
		else {
			ret = Status;
#ifdef DEBUG_UPD
			Log( MODULENAME, "ReadUPD  (STATUS) = %02X", ret, LOG_DEBUG_FULL );
#endif
		}
	}
	return( ret );
}

void WriteUPD( int port, int val ) {
	port &= 0x101;
	if ( port == 0x101 ) { // Write Data Register
#ifdef DEBUG_UPD
		Log( MODULENAME, "WriteUPD (%04X) = %02X", port, val, LOG_DEBUG_FULL );
#endif
		if ( ! ( Status & STATUS_DIO ) ) {
			if ( ! FctExec ) {
				FctExec = TabCmdUPD[ val & 0x1F ];
				Status |= STATUS_CB;
				PosStack = 0;
#ifdef DEBUG_UPD
				LastFctUpd = val;
				RegPC = RegPC;
#endif
			}
			FctExec( val, SENS_WR, 0 );
		}
	}
	else
		if ( port <= 1 ) {
			if ( ! Cpc.FddMotor )
				TpsReady = TPS_MOTOR_ON;

			Cpc.FddMotor = val & 1;
			if ( ! Cpc.FddMotor ) {
				ST3 &= ~ST3_RY;
				Break = 1;
			}
		}
}

void CycleUPD( int us ) {
	if ( CptOverRun ) {
		TpsOverRun -= us;
		if ( TpsOverRun < 0 ) {
#ifdef DEBUG_UPD
			Log( MODULENAME, "PC=#%04X %s(#%02X) - OverRun ", RegPC, TabNomCmdUPD[ LastFctUpd & 0x1F ], LastFctUpd, LOG_INFO );
#endif
			ST1 |= ST1_OR;
			CptOverRun = 0;
			EndExePhase();
		}
	}
	if ( ( Status & ( STATUS_EXE | STATUS_DIO ) ) && FctExec )
		FctExec( 0, SENS_NONE, us );

	if ( TpsNextByte > 0 && ! ( Status & STATUS_RQM ) )
		TpsNextByte -= us;
	else
		Status |= STATUS_RQM;

	if ( Cpc.FddMotor ) {
		if ( Dsk[ UnitSel ].Image ) {
			CptRotation[ UnitSel ] += us;
			if ( CptRotation[ UnitSel ] > 200000 )
				CptRotation[ UnitSel ] -= 200000;
		}
		if ( ! ( ST3 & ST3_RY ) ) {
			if ( TpsReady > 0 )
				TpsReady -= us;
			else
				if ( Dsk[ UnitSel ].Image )
					ST3 |= ST3_RY;
		}
	}
}

void ResetUPD( void ) {
	Status = STATUS_RQM;
	PosStack = 0;
	FctExec = 0;
	Cpc.FddMotor = 0;
	IndexSecteur = PosData = 0;
	ST0 = ST1 = ST2 = ST3 = 0;
	SRT = 12000;
	Break = 0;
	HeadSel = 0;
	NbSteps = 0;
	for ( UnitSel = 4; --UnitSel; ) {
		NewCylinder = Cpc.FddCurrTrack[ UnitSel ];
		MoveTrack( 1, 0 );
	}
}

int GetCurrTrack( void ) {
	return( Cpc.FddCurrTrack[ UnitSel ] );
}
