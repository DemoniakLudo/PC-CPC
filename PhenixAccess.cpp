#include  <afxwin.h>
#include  <afxcmn.h>
#include  <afxdlgs.h>
#include  <wininet.h>

typedef unsigned long ULONG_PTR;


#include  "Resource.h"
#include  "Types.h"
#include  "Snapshot.h"
#include  "Plateform.h"
#include  "VGA.h"


#ifdef USE_CIMAGE
#include  <atlimage.h>
#endif

#define     URL_PHENIX      "http://amstrad.eu/modules/pdb/api.php?action="

#define     TYPE_SCR_TXT    "Screenshot"

#define     TYPE_DSK_TXT    "Disquette"

#define     TYPE_SNA_TXT    "Snapshot"


enum { COL_NOM = 0, COL_ID, COL_ANNEE, COL_GENRE, COL_COMPAGNY };

enum { COL_MEDIA_TYPE = 0, COL_MEDIA_ID, COL_MEDIA_GAME, COL_MEDIA_NAME };

enum { TYPE_SCR, TYPE_DSK, TYPE_SNA };

#define SIZE_BUF_RECV   1024

#define MAX_GAME        4096

#define MAX_COMPAGNY    512

#define MAX_GENRE       256

#define MAX_MEDIA       32

#define MAX_CACHE       512

typedef struct {
	int Id;
	int TypeIndex;
	char FileName[ 64 ];
} StMedia;

typedef struct {
	int Id;
	char Titre[ 64 ];
	int CompagnyIndex;
	int Year;
	int GenreIndex;
	int NbScr;
	int NbDsk;
	int NbSna;
	StMedia TabMedias[ MAX_MEDIA ];
	int NbMedia;
} StGame;

typedef struct {
	int id;
	UBYTE * Buff;
	int size;
} StCache;

static HINTERNET Connexion;

static UBYTE Buffer[ 1024 * 1024 ];

static CListCtrl Liste;

static StGame TabGames[ MAX_GAME ];

static char TabCompagny[ MAX_COMPAGNY ][ 32 ];

static char TabGenre[ MAX_GENRE ][ 32 ];

static int NbGames = 0, NbCompagny = 0, NbGenre = 0;
static int NbCache = 0;

static StCache TabCache[ MAX_CACHE ];

static BOOL FindCache( int id, UBYTE ** Buff, DWORD * size ) {
	for ( int i = 0; i < NbCache; i++ )
		if ( TabCache[ i ].id == id ) {
			* size = TabCache[ i ].size;
			* Buff = TabCache[ i ].Buff;
			return( TRUE );
		}
		* size = 0;
		return( FALSE );
}

static BOOL AddCache( int id, UBYTE * Buff, int size ) {
	if (  NbCache < MAX_CACHE - 1 && ( TabCache[ NbCache ].Buff = ( UBYTE * )malloc( size ) ) != NULL ) {
		TabCache[ NbCache ].id = id;
		TabCache[ NbCache ].size = size;
		memcpy( TabCache[ NbCache++ ].Buff, Buff, size );
		return( TRUE );
	}
	return( FALSE );
}

static void ClearCache( void ) {
	for ( int i = 0; i < NbCache; i++ )
		free( TabCache[ i ].Buff );

	NbCache = 0;
}

static int GetCompagnyIdx( char * co ) {
	if ( co ) {
		for ( int i = 0; i < NbCompagny; i++ )
			if ( ! strcmp( co, TabCompagny[ i ] ) )
				return( i );

		if ( NbCompagny < MAX_COMPAGNY - 1 ) {
			strcpy( TabCompagny[ NbCompagny ], co );
			return( NbCompagny++ );
		}
	}
	return( -1 );
}

static char * GetCompagny( int idx ) {
	if ( idx > -1 && idx < NbCompagny )
		return( TabCompagny[ idx ] );

	return( "???" );
}

static int GetGenreIdx( char * ge ) {
	if ( ge ) {
		for ( int i = 0; i < NbGenre; i++ )
			if ( ! strcmp( ge, TabGenre[ i ] ) )
				return( i );

		if ( NbGenre < MAX_COMPAGNY - 1 ) {
			strcpy( TabGenre[ NbGenre ], ge );
			return( NbGenre++ );
		}
	}
	return( -1 );
}

static char * GetGenre( int idx ) {
	if ( idx > -1 && idx < NbGenre )
		return( TabGenre[ idx ] );

	return( "???" );
}

static int GetTypeIdx( char * ty ) {
	if ( ty ) {
		if ( ! strcmp( ty, TYPE_SCR_TXT ) )
			return( TYPE_SCR );

		if ( ! strcmp( ty, TYPE_DSK_TXT ) )
			return( TYPE_DSK );

		if ( ! strcmp( ty, TYPE_SNA_TXT ) )
			return( TYPE_SNA );
	}
	return( -1 );
}

static char * GetType( int idx ) {
	switch( idx ) {
	case TYPE_SCR :
		return( TYPE_SCR_TXT );

	case TYPE_DSK :
		return( TYPE_DSK_TXT );

	case TYPE_SNA :
		return( TYPE_SNA_TXT );
	}
	return( "???" );
}

static int AddGame( char * gi, char * ti, char * co, char * ye, char * ge ) {
	if ( NbGames < MAX_GAME - 1 ) {
		TabGames[ NbGames ].Id = atoi( gi );
		strcpy( TabGames[ NbGames ].Titre, ti );
		TabGames[ NbGames ].CompagnyIndex = GetCompagnyIdx( co );
		TabGames[ NbGames ].Year = ye != NULL ? atoi( ye ) : 0;
		TabGames[ NbGames ].GenreIndex = GetGenreIdx( ge );
		TabGames[ NbGames ].NbMedia = 0;
		TabGames[ NbGames ].NbDsk = 0;
		TabGames[ NbGames ].NbSna = 0;
		TabGames[ NbGames ].NbScr = 0;
		return( NbGames++ );
	}
	return( -1 );
}

static void AddMedia( int g, char * mi, char * ty, char * fi ) {
	int m = TabGames[ g ].NbMedia;
	if ( m < MAX_MEDIA - 1 ) {
		StMedia * med = &TabGames[ g ].TabMedias[ m ];
		med->Id = atoi( mi );
		med->TypeIndex = GetTypeIdx( ty );
		strcpy( med->FileName, fi );
		switch( med->TypeIndex ) {
		case TYPE_DSK:
			TabGames[ g ].NbDsk++;
			break;

		case TYPE_SNA:
			TabGames[ g ].NbSna++;
			break;

		case TYPE_SCR:
			TabGames[ g ].NbScr++;
			break;
		}
		TabGames[ g ].NbMedia++;
	}
}

class CDialListe : public CDialog {
public:
	DECLARE_DYNAMIC(CDialListe)
	enum { IDD = IDD_DIAL_PHENIXINFO };
	CDialListe(CWnd* pParent = NULL) : CDialog(CDialListe::IDD, pParent) {}


protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog( void );
	afx_msg void OnPaint( void );
	afx_msg void OnColumnClickListe( NMHDR * pNMHDR, LRESULT * pResult );
	afx_msg void OnNMClickListe( NMHDR *pNMHDR, LRESULT *pResult );
	afx_msg void OnBpGetDSK( void );
	afx_msg void OnBpGetSNA( void );
	afx_msg void OnBpRunSNA( void );
	afx_msg void OnBpSCRprev( void );
	afx_msg void OnBpSCRnext( void );
	afx_msg void OnBpDSKprev( void );
	afx_msg void OnBpSNAprev( void );
	afx_msg void OnBpDSKnext( void );
	afx_msg void OnBpSNAnext( void );
	afx_msg void OnBpReload( void );
	DECLARE_MESSAGE_MAP()

private:
	void SetEnable( BOOL Enable );
	DWORD GetPhenixFile( char * action, char * param );
	DWORD GetAll( void );
	void RedrawListe( void );
	void GetDetail( int );
	DWORD GetMedia( int numMedia );
	char * SaveMedia( int MediaID, DWORD size );
	void ShowSCR( int numSCR );
	void ClearSCR( void );
	void DownloadMedia( int numMedia );
	BOOL GetPrevMedia( int * pos, int typeMedia );
	BOOL GetNextMedia( int * pos, int typeMedia );

	int CurGame, CurSel;
	int PosDSK, PosSNA, PosSCR;
	CStatic m_Image;
	CButton m_BpGetDsk, m_BpGetSna;
	CButton m_BpSCRprev, m_BpSCRnext;
	CButton m_BpDSKprev, m_BpDSKnext;
	CButton m_BpSNAprev, m_BpSNAnext;
	CButton m_BpRunSNA, m_BpReload;
	CEdit m_NomJeu, m_SnaName, m_DskName;
	CStatic m_Nbre;
};

IMPLEMENT_DYNAMIC(CDialListe, CDialog)
	BEGIN_MESSAGE_MAP(CDialListe, CDialog)
		ON_WM_PAINT()
		ON_NOTIFY( LVN_COLUMNCLICK, IDC_LISTE, OnColumnClickListe )
		ON_NOTIFY( NM_CLICK, IDC_LISTE, OnNMClickListe )
		ON_BN_CLICKED(IDC_BP_GETDSK, OnBpGetDSK)
		ON_BN_CLICKED(IDC_BP_GETSNA, OnBpGetSNA)
		ON_BN_CLICKED(IDC_BP_RUNSNA, OnBpRunSNA)
		ON_BN_CLICKED(IDC_BPSCRPREV, OnBpSCRprev)
		ON_BN_CLICKED(IDC_BPSCRNEXT, OnBpSCRnext)
		ON_BN_CLICKED(IDC_BPDSKPREV, OnBpDSKprev)
		ON_BN_CLICKED(IDC_BPDSKNEXT, OnBpDSKnext)
		ON_BN_CLICKED(IDC_BPSNAPREV, OnBpSNAprev)
		ON_BN_CLICKED(IDC_BPSNANEXT, OnBpSNAnext)
		ON_BN_CLICKED(IDC_RELOAD, OnBpReload)
	END_MESSAGE_MAP()

	void CDialListe::DoDataExchange(CDataExchange* pDX) {
		CDialog::DoDataExchange( pDX );
		DDX_Control( pDX, IDC_LISTE, Liste );
		DDX_Control( pDX, IDC_IMAGE, m_Image);
		DDX_Control( pDX, IDC_BP_GETDSK, m_BpGetDsk );
		DDX_Control( pDX, IDC_BP_GETSNA, m_BpGetSna );

		DDX_Control( pDX, IDC_BP_RUNSNA, m_BpRunSNA );
		DDX_Control( pDX, IDC_RELOAD, m_BpReload );

		DDX_Control( pDX, IDC_BPSCRPREV, m_BpSCRprev );
		DDX_Control( pDX, IDC_BPSCRNEXT, m_BpSCRnext );
		DDX_Control( pDX, IDC_BPDSKPREV, m_BpDSKprev );
		DDX_Control( pDX, IDC_BPDSKNEXT, m_BpDSKnext );
		DDX_Control( pDX, IDC_BPSNAPREV, m_BpSNAprev );
		DDX_Control( pDX, IDC_BPSNANEXT, m_BpSNAnext );
		DDX_Control( pDX, IDC_DSK_NAME, m_DskName );
		DDX_Control( pDX, IDC_SNA_NAME, m_SnaName );
		DDX_Control( pDX, IDC_NBRE, m_Nbre );
		DDX_Control( pDX, IDC_NOM_JEU, m_NomJeu );
	}

	BOOL CDialListe::OnInitDialog( void ) {
		CDialog::OnInitDialog();

		CurGame = CurSel = -1;
		m_BpSCRprev.ShowWindow( SW_HIDE );
		m_BpDSKprev.ShowWindow( SW_HIDE );
		m_BpSNAprev.ShowWindow( SW_HIDE );
		m_BpSCRnext.ShowWindow( SW_HIDE );
		m_BpDSKnext.ShowWindow( SW_HIDE );
		m_BpSNAnext.ShowWindow( SW_HIDE );

		ShowWindow( SW_SHOW );
		CenterWindow();
		if ( ! NbGames )
			GetAll();

		Liste.InsertColumn( COL_NOM, "Nom", LVCFMT_LEFT );
		Liste.InsertColumn( COL_ID, "ID", LVCFMT_LEFT );
		Liste.InsertColumn( COL_ANNEE, "Année", LVCFMT_LEFT );
		Liste.InsertColumn( COL_GENRE, "Genre", LVCFMT_LEFT );
		Liste.InsertColumn( COL_COMPAGNY, "Editeur", LVCFMT_LEFT );
		RedrawListe();
		return( TRUE );
	}

	void CDialListe::SetEnable( BOOL Enable ) {
		m_BpGetDsk.EnableWindow( CurGame != -1 ? Enable : FALSE );
		m_BpGetSna.EnableWindow( CurGame != -1 ? Enable : FALSE );
		m_BpSCRprev.EnableWindow( CurGame != -1 ? Enable : FALSE );
		m_BpSCRnext.EnableWindow( CurGame != -1 ? Enable : FALSE );
		m_BpDSKprev.EnableWindow( CurGame != -1 ? Enable : FALSE );
		m_BpDSKnext.EnableWindow( CurGame != -1 ? Enable : FALSE );
		m_BpSNAprev.EnableWindow( CurGame != -1 ? Enable : FALSE );
		m_BpSNAnext.EnableWindow( CurGame != -1 ? Enable : FALSE );
		m_DskName.EnableWindow( Enable );
		m_SnaName.EnableWindow( Enable );
		m_Nbre.EnableWindow( Enable );
		m_NomJeu.EnableWindow( Enable );
		m_BpRunSNA.EnableWindow( CurGame != -1 ? Enable : FALSE );
		m_BpReload.EnableWindow( Enable );
	}

	void CDialListe::OnPaint( void ) {
		if ( CurGame != -1 && PosSCR != -1 )
			ShowSCR( PosSCR );

		if ( CurSel != -1 )
			Liste.Update( CurSel );

		CDialog::OnPaint();
	}

	static char * ExtraitChamp( char ** src, char * id ) {
		char * p = strstr( * src, id );
		if ( p ) {
			p = strchr( p, '"' );
			if ( p ) {
				char * q = strchr( ++p, '"' );
				if ( q ) {
					* q++ = 0;
					* src = q;
					return( p );
				}
			}
		}
		return( NULL );
	}

	static void DecodePhenixXmlAll( void ) {
		char * game = strstr( ( char * )Buffer, "<games>" );
		if ( game ) {
			game += 7;
			NbGames = 0;
			do {
				char * Limit = strchr( game, '>' );
				if ( Limit )
					* Limit = 0;

				char * gi = ExtraitChamp( &game, "game id" );
				if ( ! gi )
					break;

				char * ti = ExtraitChamp( &game, "title" );
				char * co = ExtraitChamp( &game, "company" );
				char * ye = ExtraitChamp( &game, "year" );
				char * ge = ExtraitChamp( &game, "genre" );
				if ( Limit )
					* Limit = '>';

				if ( ti ) {
					int g = AddGame( gi, ti, co, ye, ge );
					if ( g == -1 )
						break;

					if ( * game == '>' )
						game++;

					while( strncmp( game, "</game>", 7 ) ) {
						char * mi = ExtraitChamp( &game, "media id" );
						char * ty = ExtraitChamp( &game, "type" );
						char * fi = ExtraitChamp( &game, "file" );
						AddMedia( g, mi, ty, fi );
						if ( game[ 1 ] == '/' && game[ 2 ] == '>' )
							game += 3;
					}
				}

				char * p = strstr( game, "</game>" );
				if ( p )
					game = p + 7;
				else {
					p = strstr( game, "/>" );
					if ( p )
						game = p + 3;
					else
						game = NULL;
				}
			}
			while( game );
		}
	}

	void CDialListe::RedrawListe( void ) {
		char Tmp[ 32 ];
		Liste.DeleteAllItems();
		for ( int i = 0; i < NbGames; i++ ) {
			Liste.InsertItem( LVIF_TEXT | LVIF_PARAM, i, TabGames[ i ].Titre, 0, 0, 0, i );
			Liste.SetItemText( i, COL_ID, itoa( TabGames[ i ].Id, Tmp, 10 ) );
			if ( TabGames[ i ].Year )
				Liste.SetItemText( i, COL_ANNEE, itoa( TabGames[ i ].Year, Tmp, 10 ) );

			Liste.SetItemText( i, COL_GENRE, GetGenre( TabGames[ i ].GenreIndex ) );
			Liste.SetItemText( i, COL_COMPAGNY, GetCompagny( TabGames[ i ].CompagnyIndex ) );
		}
		Liste.SetColumnWidth( COL_NOM, LVSCW_AUTOSIZE );
		Liste.SetColumnWidth( COL_ID, 0 );
		Liste.SetColumnWidth( COL_ANNEE, LVSCW_AUTOSIZE_USEHEADER );
		Liste.SetColumnWidth( COL_GENRE, LVSCW_AUTOSIZE_USEHEADER );
		Liste.SetColumnWidth( COL_COMPAGNY, LVSCW_AUTOSIZE_USEHEADER );
		sprintf( Tmp, "Nombre : %d", NbGames );
		m_Nbre.SetWindowText( Tmp );
	}

	DWORD CDialListe::GetAll( void ) {
		DWORD ret = GetPhenixFile( "detailist", "" );
		if ( ret )
			DecodePhenixXmlAll();
		else
			SetWindowText( "Erreur connexion site phenixinformatique.com" );

		return( ret );
	}

	void CDialListe::GetDetail( int id ) {
		CurGame = -1;
		for ( int i = 0; i < NbGames; i++ )
			if ( TabGames[ i ].Id == id ) {
				CurGame = i;
				break;
			}

			if ( CurGame != -1 ) {
				m_NomJeu.SetWindowText( TabGames[ CurGame ].Titre );
				PosDSK = PosSNA = PosSCR = -1;
				m_BpGetDsk.ShowWindow( TabGames[ CurGame ].NbDsk ? SW_SHOW : SW_HIDE );
				m_BpGetSna.ShowWindow( TabGames[ CurGame ].NbSna ? SW_SHOW : SW_HIDE );
				m_BpSCRprev.ShowWindow( SW_HIDE );
				m_BpDSKprev.ShowWindow( SW_HIDE );
				m_BpSNAprev.ShowWindow( SW_HIDE );
				m_BpSCRnext.ShowWindow( TabGames[ CurGame ].NbScr > 1 ? SW_SHOW : SW_HIDE );
				m_BpDSKnext.ShowWindow( TabGames[ CurGame ].NbDsk > 1 ? SW_SHOW : SW_HIDE );
				m_BpSNAnext.ShowWindow( TabGames[ CurGame ].NbSna > 1 ? SW_SHOW : SW_HIDE );
				for ( int num = 0; num < TabGames[ CurGame ].NbMedia; num++ ) {
					StMedia * m = &TabGames[ CurGame ].TabMedias[ num ];
					if ( m->TypeIndex == TYPE_DSK && PosDSK == -1 )
						PosDSK = num;
					else
						if ( m->TypeIndex == TYPE_SNA && PosSNA == -1 )
							PosSNA = num;
						else
							if ( m->TypeIndex == TYPE_SCR && PosSCR == -1 )
								PosSCR = num;
				}
				m_SnaName.SetWindowText( TabGames[ CurGame ].NbSna ? TabGames[ CurGame ].TabMedias[ PosSNA ].FileName : "Aucun" );
				m_DskName.SetWindowText( TabGames[ CurGame ].NbDsk ? TabGames[ CurGame ].TabMedias[ PosDSK ].FileName : "Aucun" );
				if ( PosSCR != -1 )
					ShowSCR( PosSCR );
				else
					ClearSCR();
			}
	}

	DWORD CDialListe::GetMedia( int MediaID ) {
		static char Tmp[ 256 ];
		DWORD ret = 0;
		UBYTE * Buff;

		int id = TabGames[ CurGame ].TabMedias[ MediaID ].Id;
		if ( FindCache( id, &Buff, &ret ) )
			memcpy( Buffer, Buff, ret );
		else {
			sprintf( Tmp, "get&id=%d", id );
			ret = GetPhenixFile( Tmp, "" );
			if ( ret && ! AddCache( id, Buffer, ret ) )
				ret = 0;
		}
		return( ret );
	}

	char * CDialListe::SaveMedia( int MediaID, DWORD size ) {
		static char Tmp[ 256 ];
		static char FileName[ 512 ];

		GetTempPath( sizeof( Tmp ), Tmp );
		if ( Tmp[ strlen( Tmp ) - 1 ] == '\\' )
			Tmp[ strlen( Tmp ) - 1 ] =0;

		sprintf( FileName, "%s\\%s", Tmp, TabGames[ CurGame ].TabMedias[ MediaID ].FileName );
		HANDLE f = CreateFile( FileName, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL );
		if ( f != INVALID_HANDLE_VALUE ) {
			DWORD BytesWritten;
			WriteFile( f, Buffer, size, &BytesWritten, NULL );
			CloseHandle( f );
			return( FileName );
		}
		return( NULL );
	}

	void CDialListe::ShowSCR( int numSCR ) {
		DWORD ret = GetMedia( numSCR );
		if ( ret ) {
			char * FileName = SaveMedia( numSCR, ret );
			if ( FileName ) {
#ifdef USE_CIMAGE
				CImage img;
				if ( img.Load( FileName ) == S_OK ) {
					img.StretchBlt( m_Image.GetDC()->m_hDC, 0, 0, 0x180, 0x110 );
					m_Image.Invalidate();
				}
#endif
			}
		}
	}

	void CDialListe::ClearSCR( void ) {
#ifdef USE_CIMAGE
		CImage img;
		img.Create( 0x180, 0x110, 1, 0 );
		img.StretchBlt( m_Image.GetDC()->m_hDC, 0, 0, 0x180, 0x110 );
		m_Image.Invalidate();
#endif
	}

	void CDialListe::DownloadMedia( int numMedia ) {
		DWORD ret = GetMedia( numMedia );
		if ( ret ) {
			CFileDialog d( FALSE, NULL, TabGames[ CurGame ].TabMedias[ numMedia ].FileName, OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT, NULL, this );
			if ( d.DoModal() == IDOK ) {
				HANDLE f = CreateFile( d.GetPathName(), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL );
				if ( f != INVALID_HANDLE_VALUE ) {
					DWORD BytesWritten;
					WriteFile( f, Buffer, ret, &BytesWritten, NULL );
					CloseHandle( f );
				}
			}
		}
	}

	DWORD CDialListe::GetPhenixFile( char * action, char * param ) {
		static char FullUrl[ 256 ];
		static char * Anim[ 8 ] = {
			"........",
			".......",
			"......",
			".....",
			"....",
			"...",
			".."
			"."
		};
		DWORD ret =0, lgRet = 0, posAnim = 0;

		SetEnable( FALSE );
		SetWindowText( "Connexion en cours..." );
		sprintf( FullUrl, "%s%s%s", URL_PHENIX, action, param );
		HINTERNET f = InternetOpenUrl( Connexion, FullUrl, NULL, 0, INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_PASSIVE, 0 );
		if ( f ) {
			memset( Buffer, 0, sizeof( Buffer ) );
			for ( lgRet = 0; lgRet < sizeof( Buffer ); ) {
				SetWindowText( Anim[ posAnim++ & 7 ] );
				InternetReadFile( f, &Buffer[ lgRet ], SIZE_BUF_RECV, &ret );
				lgRet += ret;
				if ( ! ret )
					break;
			}
			InternetCloseHandle( f );
			SetWindowText( lgRet ? "" : "Erreur connexion site..." );
		}
		SetEnable( TRUE );
		return( lgRet );
	}

	static int CALLBACK fctCompare( LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort ) {
		static char Text1[ 128 ], Text2[ 128 ];
		int i1 = 0, i2 = 0;

		for ( int i = 0; i < Liste.GetItemCount(); i++ ) {
			if ( Liste.GetItemData( i ) == ( DWORD )lParam1 )
				i1 = i;
			else
				if ( Liste.GetItemData( i ) == ( DWORD )lParam2 )
					i2 = i;
		}
		Liste.GetItemText( i1, ( int )lParamSort, Text1, sizeof( Text1 ) );
		Liste.GetItemText( i2, ( int )lParamSort, Text2, sizeof( Text2 ) );
		int r = strcmp( Text1, Text2 );
		if ( ! r ) {
			Liste.GetItemText( i1, 0, Text1, sizeof( Text1 ) );
			Liste.GetItemText( i2, 0, Text2, sizeof( Text2 ) );
			r = strcmp( Text1, Text2 );
		}
		return( r );
	}

	void CDialListe::OnColumnClickListe( NMHDR *pNMHDR, LRESULT *pResult ) {
		LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>( pNMHDR );
		DWORD numCol = pNMLV->iSubItem;
		Liste.SortItems( fctCompare, numCol );
		* pResult = 0;
	}

	/* Clique sur un jeu dans la liste */
	void CDialListe::OnNMClickListe( NMHDR * pNMHDR, LRESULT * pResult ) {
		static char Id[ 64 ];

		LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>( pNMHDR );
		CurSel = pNMItemActivate->iItem;
		if ( CurSel > -1 ) {
			Liste.GetItemText( CurSel, COL_ID, Id, sizeof( Id ) );
			GetDetail( atoi( Id ) );
		}
		* pResult = 0;
	}

	void CDialListe::OnBpGetDSK( void ) {
		DownloadMedia( PosDSK );
	}

	void CDialListe::OnBpGetSNA( void ) {
		DownloadMedia( PosSNA );
	}

	void CDialListe::OnBpRunSNA( void ) {
		DWORD ret1 = GetMedia( PosDSK );
		char * FileName = ret1 ? SaveMedia( PosDSK, ret1 ) : NULL;
		DWORD ret2 = GetMedia( PosSNA );
		if ( ret2 && SetSnap( ( StCpc * )Buffer ) )
			memcpy( MemCPC, &Buffer[ sizeof( StCpc ) ], sizeof( MemCPC ) );

		if ( FileName )
			LoadDSK( FileName, 0 );

		if ( ret1 || ret2 ) {
			CloseWindow();
			OnOK();
		}
	}

	BOOL CDialListe::GetPrevMedia( int * pos, int typeMedia ) 	{
		BOOL Trouve1 = FALSE, Trouve2 = FALSE;

		for ( int m = * pos; m--; )
			if ( TabGames[ CurGame ].TabMedias[ m ].TypeIndex == typeMedia ) {
				if ( ! Trouve1 ) {
					* pos = m;
					Trouve1 = TRUE;
				}
				else
					Trouve2 = TRUE;
			}
			return( Trouve2 );
	}

	BOOL CDialListe::GetNextMedia( int * pos, int typeMedia ) {
		BOOL Trouve1 = FALSE, Trouve2 = FALSE;

		for ( int m = * pos + 1; m < TabGames[ CurGame ].NbMedia; m++ )
			if ( TabGames[ CurGame ].TabMedias[ m ].TypeIndex == typeMedia ) {
				if ( ! Trouve1 ) {
					* pos = m;
					Trouve1 = TRUE;
				}
				else
					Trouve2 = TRUE;
			}

			return( Trouve2 );
	}

	void CDialListe::OnBpSCRprev( void ) {
		BOOL ret = GetPrevMedia( &PosSCR, TYPE_SCR );
		ShowSCR( PosSCR );
		m_BpSCRprev.ShowWindow( ret ? SW_SHOW : SW_HIDE );
		m_BpSCRnext.ShowWindow( SW_SHOW );
	}

	void CDialListe::OnBpSCRnext( void ) {
		BOOL ret = GetNextMedia( &PosSCR, TYPE_SCR );
		ShowSCR( PosSCR );
		m_BpSCRnext.ShowWindow( ret ? SW_SHOW : SW_HIDE );
		m_BpSCRprev.ShowWindow( SW_SHOW );
	}

	void CDialListe::OnBpDSKprev( void ) {
		BOOL ret = GetPrevMedia( &PosDSK, TYPE_DSK );
		m_DskName.SetWindowText( TabGames[ CurGame ].TabMedias[ PosDSK ].FileName );
		m_BpDSKprev.ShowWindow( ret ? SW_SHOW : SW_HIDE );
		m_BpDSKnext.ShowWindow( SW_SHOW );
	}

	void CDialListe::OnBpDSKnext( void ){
		BOOL ret = GetNextMedia( &PosDSK, TYPE_DSK );
		m_DskName.SetWindowText( TabGames[ CurGame ].TabMedias[ PosDSK ].FileName );
		m_BpDSKnext.ShowWindow( ret ? SW_SHOW : SW_HIDE );
		m_BpDSKprev.ShowWindow( SW_SHOW );
	}

	void CDialListe::OnBpSNAprev( void ) {
		BOOL ret = GetPrevMedia( &PosSNA, TYPE_SNA );
		m_SnaName.SetWindowText( TabGames[ CurGame ].TabMedias[ PosSNA ].FileName );
		m_BpSNAprev.ShowWindow( ret ? SW_SHOW : SW_HIDE );
		m_BpSNAnext.ShowWindow( SW_SHOW );
	}

	void CDialListe::OnBpSNAnext( void ) {
		BOOL ret = GetNextMedia( &PosSNA, TYPE_SNA );
		m_SnaName.SetWindowText( TabGames[ CurGame ].TabMedias[ PosSNA ].FileName );
		m_BpSNAnext.ShowWindow( ret ? SW_SHOW : SW_HIDE );
		m_BpSNAprev.ShowWindow( SW_SHOW );
	}

	void CDialListe::OnBpReload( void ) {
		GetAll();
		RedrawListe();
	}

	void InitPhenixAccess( void ) {
		Connexion = InternetOpen( "PC-CPC", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0 );
		if ( Connexion ) {
			CDialListe dlg;
			dlg.DoModal();
			InternetCloseHandle( Connexion );
		}
	}
