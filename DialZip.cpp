#include  <afxwin.h>

#include  "Resource.h"
#include  "Log.h"
#include  "Zip/zip.h"
#include  "Zip/unzip.h"
#include  "DialZip.h"


#define     MODULENAME      "ZIP"


/********************************************************* !NAME! **************
* Nom : ZipInit
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Initalisation pour le ZIP
*               NE PAS APPELER, CELA EST FAIT PAR LES FONCTION DE GESTION DU ZIP
*
* Résultat    : /
*
* Variables globales modifiées : TabZipOpen
*
********************************************************** !0! ****************/
void ZipInit()
{
    if ( ZipIsInit > 0 )
        return;

    // on initalise les structures
    for (int iPos = 0; iPos < MAX_ZIP_FILE_USING; iPos++)
        memset( (ZipOpenFile*)&TabZipOpen[ iPos ], 0, sizeof(ZipOpenFile) );

    ZipIsInit = 1;
}


/********************************************************* !NAME! **************
* Nom : OpenZip
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Lecture d'un fichier ZIP
*               Avec gestion du type de fichier à prendre dans le ZIP (DSK, SNA, WAV, ...)
*               Avec gestion du choix du fichier à prendre si plusieurs fichier dans le ZIP
*
* Résultat    : /
*
* Variables globales modifiées : /
*
********************************************************** !0! ****************/
int OpenZip( char * Nom, int ForceType, ZipOpenFile* ZipResult )
{
	HZIP hz;
	ZRESULT zr;
	ZIPENTRY ze;
	TCHAR m[1024];
	char NomF[ MAX_PATH ];
	char szTmpZipFile[MAX_PATH];
	int icurTypeFile = Z_TYPE_UNKNOWN;
	int curPosTabTypeFile = 0;

	ZipInit();

	if ( ZipResult == NULL )
	{
		return -1;
	}

	hz=OpenZip( Nom, 0 );
	if ( hz==0 )
	{
		Log(MODULENAME, _T("* Failed to open zip"), LOG_WARNING);
		return -1;
	}

	// on vas travailler avec le rep tmp de windows (celui du profile)
	GetTempPath (256, szTmpZipFile);
	strcpy( ZipResult->Name,  szTmpZipFile );
	zr=SetUnzipBaseDir( hz, szTmpZipFile );
	if (zr!=ZR_OK)
		TRACE(_T("* Failed to set base dir for folders.zip"));

	zr=GetZipItem( hz, -1, &ze);
	if (zr!=ZR_OK)
	{
		Log(MODULENAME, _T("* Failed to GetZipItem"), LOG_WARNING);
		CloseZip(hz);
		return -1;
	}

	pTabFileInZip = (ZIPENTRYEX*)malloc( (ze.index+1) * sizeof(ZIPENTRYEX) );
	memset(pTabFileInZip, 0xFF, (ze.index+1) * sizeof(ZIPENTRYEX) );

	// la on vas conté le nombre de fichiers que l'ont peut utiliser dans PC-CPC
	// s'il n'y a qu'un seul fichier d'un type connu, on le charge.
	// sinon, on demade ce qu'il faut faire ...
	int numitems=ze.index;
	for (int i=0; i<numitems; i++)
	{
		zr=GetZipItem(hz,i,&ze);
		if (zr!=ZR_OK)
		{
			wsprintf(m, _T("* Failed to get index %i of zip"), i);
			Log(MODULENAME, m, LOG_WARNING);
			CloseZip(hz);
			return -1;
		}

		// c'est un répertoire, non n'a rien à faire....
		//		if ( (ze.attr&FILE_ATTRIBUTE_DIRECTORY) == 0 )
		//		{
		//			continue;
		//		}

		strcpy( NomF, ze.name );
		//_strupr_s( NomF, MAX_PATH );

		char * pch;
		pch=strrchr( NomF , '.' );
		if ( pch != NULL )
		{
			icurTypeFile = Z_TYPE_UNKNOWN;

			if ( ! strcmp( &NomF[ pch-NomF ], ".DSK" ) )
			{
				// c'est un fichier DSK
				icurTypeFile = Z_TYPE_DSK;
			}
			else if ( ! strcmp( &NomF[ pch-NomF ], ".SNA" ) )
			{
				icurTypeFile = Z_TYPE_SNA;
			}
			else if ( ! strcmp( &NomF[ pch-NomF ], ".WAV" ) )
			{
				icurTypeFile = Z_TYPE_WAV;
			}
			else if ( ! strcmp( &NomF[ pch-NomF ], ".ASM" ) )
			{
				icurTypeFile = Z_TYPE_ASM;
			}

			if ( (icurTypeFile != Z_TYPE_UNKNOWN) && ((ForceType == Z_TYPE_ALL) || (ForceType == icurTypeFile)) )
			{
				// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				// !!! BON, J'ARRIVE PAS A FAIRE LE memcpy, A VOIRE, SE SERAIS BIEN MIEUX !!!
				// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				//memcpy( (void*)(pTabFileInZip[curPosTabTypeFile].EntryZip), ze, sizeof(ze) );
				
				pTabFileInZip[curPosTabTypeFile].EntryZip.atime = ze.atime;
				pTabFileInZip[curPosTabTypeFile].EntryZip.attr = ze.attr;
				pTabFileInZip[curPosTabTypeFile].EntryZip.comp_size = ze.comp_size;
				pTabFileInZip[curPosTabTypeFile].EntryZip.ctime = ze.ctime;
				pTabFileInZip[curPosTabTypeFile].EntryZip.index = ze.index;
				pTabFileInZip[curPosTabTypeFile].EntryZip.mtime = ze.mtime;
				strcpy( pTabFileInZip[curPosTabTypeFile].EntryZip.name, ze.name);
				pTabFileInZip[curPosTabTypeFile].EntryZip.unc_size = ze.unc_size;

				pTabFileInZip[curPosTabTypeFile].Type = icurTypeFile;
				pTabFileInZip[curPosTabTypeFile].indexInTab = curPosTabTypeFile;

				curPosTabTypeFile++;
			}
		}
	}

	// la soit on a un seul fichier (de type connu) dans la ZIP dans se cas on le charge.
	// sinon il faut faire choisir l'utilisateur
	if ( curPosTabTypeFile == 1 )
	{
		// aller on décopresse vraiment....
		// le fichier
		zr = UnzipItem( hz, pTabFileInZip[ 0 ].EntryZip.index, pTabFileInZip[ 0 ].EntryZip.name );
		if (zr!=ZR_OK)
		{
			wsprintf( m, _T("* Failed to unzip item %i of zip %s"), pTabFileInZip[ 0 ].EntryZip.index, pTabFileInZip[ 0 ].EntryZip.name );
			Log(MODULENAME, m, LOG_WARNING);
		}

		// renseigne la structure de retour....
		ZipResult->indexInZip = pTabFileInZip[ 0 ].EntryZip.index;
		strcat( ZipResult->Name, pTabFileInZip[ 0 ].EntryZip.name );
		ZipResult->Type = pTabFileInZip[ 0 ].Type;
		strcpy( ZipResult->NameZIP, Nom);

		// décompression en mémoire
		/*
		char *ibuf = (char*) malloc( pTabFileInZip[ 0 ].EntryZip.unc_size );
		UnzipItem( hz, pTabFileInZip[ 0 ].EntryZip.index, pTabFileInZip[ 0 ].EntryZip.name );
		*/

	}
	else
	{
		CDialZip dz;
		dz.pTabFileInZip = pTabFileInZip;
		dz.ZipResult = ZipResult;

		if ( dz.DoModal() == IDOK )
		{
			zr=GetZipItem(hz, ZipResult->indexInZip ,&ze);
			if (zr!=ZR_OK)
			{
				wsprintf(m, _T("* Failed to get index %i of zip"), ZipResult->indexInZip);
				Log(MODULENAME, m, LOG_WARNING);
				CloseZip(hz);
				return -1;
			}

			// aller on décopresse vraiment....
			// le fichier
			zr = UnzipItem( hz, ze.index, ze.name );
			if (zr!=ZR_OK)
			{
				wsprintf( m, _T("* Failed to unzip item %i of zip %s"), ze.index, ze.name );
				Log(MODULENAME, m, LOG_WARNING);
			}

			// renseigne la structure de retour....
			//strcat_s( ZipResult->Name, ze.name );
			strcat( ZipResult->Name, ze.name );
			strcpy( ZipResult->NameZIP, Nom);
		}
	}

	zr=CloseZip(hz);
	if (zr!=ZR_OK)
	{
		Log(MODULENAME, _T("* Failed to close zip"), LOG_WARNING);
	}

	delete (pTabFileInZip);
	return 0;
}


/********************************************************* !NAME! **************
* Nom : MemZipIsOpen
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Mémorise l'utilisation d'un fichier contenu dans un ZIP
*
* Résultat    : /
*
* Variables globales modifiées : TabZipOpen
*
********************************************************** !0! ****************/
void MemZipIsOpen( ZipOpenFile* ZipInfoOpen, int Drive )
{
	int iPos;

	// Pas d'initalisation du ZIP
	// c'est pas normal, on devrais l'avoir fait !!!
	if ( ZipIsInit == 0 )
	{
		Log(MODULENAME, _T("* Memorisation d'un ZIP sans initalisation"), LOG_ERROR);
		return;
	}

	if ( ZipInfoOpen == NULL )
	{
		return;
	}

	// on recherche le dernié emplacement dispo dans le tableau
	for (iPos = 0; iPos < MAX_ZIP_FILE_USING; iPos++)
	{
		if ( TabZipOpen[ iPos ].NameZIP[0] == 0 )
		{
			break;
		}

		iPos++;
	}

	// on mes les info à jour
	memcpy( TabZipOpen + (iPos*sizeof(ZipOpenFile)), ZipInfoOpen, sizeof(ZipOpenFile) );

}

/********************************************************* !NAME! **************
* Nom : CloseFileZip
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : fermeture d'un fichier contenue dans un ZIP
*
* Résultat    : /
*
* Variables globales modifiées : TabZipOpen
*
********************************************************** !0! ****************/
int CloseFileZip( ZipOpenFile* ZipInfo )
{
	int iPos;

	// on recherche dans le tableau si le fichier vient d'un fichier ZIP
	for (iPos = 0; iPos < MAX_ZIP_FILE_USING; iPos++)
	{
		if ( ! strcmp( TabZipOpen[ iPos ].Name, ZipInfo->Name)  )
		{
			memset( (ZipOpenFile*)&TabZipOpen[ iPos ], 0, sizeof(ZipOpenFile) );
			return true;
		}

		iPos++;
	}

	return 0;
}

/********************************************************* !NAME! **************
* Nom : OpenZipFile
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Le fichier vient est t'il contenue dans un fichier ZIP ouver
*
* Résultat    : /
*
* Variables globales modifiées : /
*
********************************************************** !0! ****************/
bool IsInOpendZip( char* File, ZipOpenFile* ZipResult )
{
	int iPos;

	if ( File[0] == 0)
	{
		return false;
	}

	// on recherche dans le tableau si le fichier vient d'un fichier ZIP
	for (iPos = 0; iPos < MAX_ZIP_FILE_USING; iPos++)
	{
		if ( ! strcmp( TabZipOpen[ iPos ].Name, File)  )
		{
			memcpy( ZipResult, TabZipOpen + (iPos*sizeof(ZipOpenFile)), sizeof(ZipOpenFile) );
			return true;
		}

		iPos++;
	}

	return false;
}

// AddFileToZip: adds a file to a zip, possibly replacing what was there before
// zipfn ="c:\\archive.zip"             (the fn of the zip file) 
// zefn  ="c:\\my documents\\file.txt"  (the fn of the file to be added)
// zename="file.txt"                    (the name that zefn will take inside the zip)
// If zefn is empty, we just delete zename from the zip archive.
// The way it works is that we create a temporary zipfile, and copy the original
// contents into the new one (with the appropriate addition or substitution)
// and then remove the old one and rename the new one. NB. we are case-insensitive.
ZRESULT AddFileToZip(const TCHAR *zipfn, const TCHAR *zename, const TCHAR *zefn)
{
	if (GetFileAttributes(zipfn)==0xFFFFFFFF || (zefn!=0 && GetFileAttributes(zefn)==0xFFFFFFFF))
		return ZR_NOFILE;

	// Expected size of the new zip will be the size of the old zip plus the size of the new file
	HANDLE hf=CreateFile(zipfn,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,0,0);
	if (hf==INVALID_HANDLE_VALUE)
		return ZR_NOFILE;
	
	DWORD size=GetFileSize(hf,0);
	CloseHandle(hf);

	if (zefn!=0)
	{
		hf=CreateFile(zefn,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,0,0);
		if (hf==INVALID_HANDLE_VALUE)
			return ZR_NOFILE;
		
		size+=GetFileSize(hf,0);
		CloseHandle(hf);
	}

	size*=2; // just to be on the safe side.
	//
	HZIP hzsrc=OpenZip(zipfn,0);
	if (hzsrc==0)
		return ZR_READ;

	HZIP hzdst=CreateZip(0,size,0);
	if (hzdst==0)
	{
		CloseZip(hzsrc);
		return ZR_WRITE;
	}

	// hzdst is created in the system pagefile
	// Now go through the old zip, unzipping each item into a memory buffer, and adding it to the new one
	char *buf=0;
	unsigned int bufsize=0; // we'll unzip each item into this memory buffer
	ZIPENTRY ze;
	ZRESULT zr=GetZipItem(hzsrc,-1,&ze);
	int numitems=ze.index;
	if (zr!=ZR_OK)
	{
		CloseZip(hzsrc);
		CloseZip(hzdst);
		return zr;
	}

	for (int i=0; i<numitems; i++)
	{
		zr=GetZipItem(hzsrc,i,&ze);
		if (zr!=ZR_OK)
		{
			CloseZip(hzsrc);
			CloseZip(hzdst);
			return zr;
		}

		if (stricmp(ze.name,zename)==0)
			continue; // don't copy over the old version of the file we're changing

		if (ze.attr&FILE_ATTRIBUTE_DIRECTORY)
		{
			zr=ZipAddFolder(hzdst,ze.name);
			if (zr!=ZR_OK)
			{
				CloseZip(hzsrc);
				CloseZip(hzdst);
				return zr;
			}
			continue;
		}

		if (ze.unc_size>(long)bufsize)
		{
			if (buf!=0)
				delete[] buf;

			bufsize=ze.unc_size*2;
			buf=new char[bufsize];
		}

		zr=UnzipItem(hzsrc,i,buf,bufsize);
		if (zr!=ZR_OK)
		{
			CloseZip(hzsrc);
			CloseZip(hzdst);
			return zr;
		}

		zr=ZipAdd(hzdst,ze.name,buf,bufsize);
		if (zr!=ZR_OK)
		{
			CloseZip(hzsrc);
			CloseZip(hzdst);
			return zr;
		}
	}

	delete[] buf;
	// Now add the new file

	if (zefn!=0)
	{
		zr=ZipAdd(hzdst,zename,zefn);
		if (zr!=ZR_OK)
		{
			CloseZip(hzsrc);
			CloseZip(hzdst);
			return zr;
		}
	}

	zr=CloseZip(hzsrc);
	if (zr!=ZR_OK)
	{
		CloseZip(hzdst);
		return zr;
	}
	
	//
	// The new file has been put into pagefile memory. Let's store it to disk, overwriting the original zip
	zr=ZipGetMemory(hzdst,(void**)&buf,&size);
	if (zr!=ZR_OK)
	{
		CloseZip(hzdst);
		return zr;
	}

	hf=CreateFile(zipfn,GENERIC_WRITE,0,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
	if (hf==INVALID_HANDLE_VALUE)
	{
		CloseZip(hzdst);
		return ZR_WRITE;
	}

	DWORD writ;
	WriteFile(hf,buf,size,&writ,0);
	CloseHandle(hf);
	zr=CloseZip(hzdst);
	if (zr!=ZR_OK)
		return zr;

	return ZR_OK;
}


bool SaveFileInZip( ZipOpenFile* ZipInfo )
{
    static char Nom[ MAX_PATH ];

    //strcpy( Nom, ZipInfo->Name );
    char * pch = strrchr( ZipInfo->Name , '\\' );
    if ( pch == NULL )
        return false;

    int ileng = strlen( ZipInfo->Name );
    int ileng2 = strlen( pch );
    int result = (int)( strlen( ZipInfo->Name ) - strlen( pch ) + 1);
    strcpy( Nom, &ZipInfo->Name[result] );
    AddFileToZip( ZipInfo->NameZIP, Nom, ZipInfo->Name );
    return true;
}


// CDialZip dialog

IMPLEMENT_DYNAMIC(CDialZip, CDialog)

CDialZip::CDialZip(CWnd* pParent /*=NULL*/)
	: CDialog(CDialZip::IDD, pParent)
{

}

CDialZip::~CDialZip()
{
}

void CDialZip::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_ZIP_FILE, m_ListFileInZip);
}


BEGIN_MESSAGE_MAP(CDialZip, CDialog)
	ON_LBN_SELCHANGE(IDC_LIST_ZIP_FILE, CDialZip::OnLbnSelchangeListZipFile)
	ON_BN_CLICKED(IDOK, CDialZip::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, CDialZip::OnCancel)
	ON_LBN_DBLCLK(IDC_LIST_ZIP_FILE, CDialZip::OnLbnDblclkListZipFile)
END_MESSAGE_MAP()

BOOL CDialZip::OnInitDialog()
{
	int nIndex;

	CDialog::OnInitDialog();

	m_ListFileInZip.ResetContent() ;
	int pos = 0;
	while ( pTabFileInZip[ pos ].indexInTab != -1 )
	{
		nIndex = m_ListFileInZip.AddString( pTabFileInZip[ pos ].EntryZip.name );
		m_ListFileInZip.SetItemData( nIndex, (DWORD_PTR)(ZIPENTRYEX*)&pTabFileInZip[ pos ] );
		pos++;
	}

	m_ListFileInZip.SetCurSel( 0 );

	return TRUE;
}


// CDialZip message handlers

void CDialZip::OnLbnSelchangeListZipFile()
{

}

void CDialZip::OnLbnDblclkListZipFile()
{
	OnBnClickedOk();
}


void CDialZip::OnBnClickedOk()
{
	// on récupre l'item sélectionée.
	int nIndex = m_ListFileInZip.GetCurSel();

	ZipResult->indexInZip = -1;
	ZipResult->Type = Z_TYPE_UNKNOWN;

	if ( nIndex != LB_ERR )
	{
		ZIPENTRYEX* pZipTypeFile = (ZIPENTRYEX*)m_ListFileInZip.GetItemData( nIndex );
		ZipResult->indexInZip = pZipTypeFile->EntryZip.index;
		ZipResult->Type = pZipTypeFile->Type;
	}

	OnOK();
}

