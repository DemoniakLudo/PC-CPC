#ifndef DIALZIP_H
#define DIALZIP_H

//
// Type de fichier pour fichier ZIP
//
#define		Z_TYPE_ALL		-1
#define		Z_TYPE_UNKNOWN	0
#define		Z_TYPE_DSK		1
#define		Z_TYPE_SNA		2
#define		Z_TYPE_WAV		3
#define		Z_TYPE_ASM		4


#define MAX_ZIP_FILE_USING  6  //On de devrais avoir que 6 fichier ouvrable en même temps !

/********************************************************* !NAME! **************
* Nom : ZipIsInit
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Structures
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Variable pour l'initalisation des données pour le ZIP
*
********************************************************** !0! ****************/
static int ZipIsInit = 0;



/********************************************************* !NAME! **************
* Nom : ZipTypeFile
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Structures
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Structure type de fichier contenue dans un ZIP
*
********************************************************** !0! ****************/
typedef struct
{
	ZIPENTRY EntryZip;	// Structure pour le fichier ZIP dans le fichier
	int Type;			// Type de ficheir
	int indexInTab;		// Position dans la Tableux pour le choix du ZIP
} ZIPENTRYEX;




/********************************************************* !NAME! **************
* Nom : ZipOpenFile
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Structures
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Structure de mémorisation des fichiers ZIP ouvert
*
********************************************************** !0! ****************/
typedef struct
{
	char NameZIP[MAX_PATH];	// Non du fichier ZIP
	char Name[MAX_PATH];	// Nom dans le ZIP puis nom du fichier déconprésé
	int Type;				// Type de ficheir
	int indexInZip;			// Position dans le ZIP
	int Drive;				// Numéro du lecteur de disquette pour les fichier de type DSK
} ZipOpenFile;


/********************************************************* !NAME! **************
* Nom : TabZipTypeFile
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Structures
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : tableau de structure type de fichier contenue dans un ZIP
*
********************************************************** !0! ****************/
static ZIPENTRYEX* pTabFileInZip; 


/********************************************************* !NAME! **************
* Nom : TabZipOpen
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Structures
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : tableau de mémorisation des fichiers ZIP ouvert
*               On de devrais avoir que 6 fichier ouvrable en même temps !
*
********************************************************** !0! ****************/
static ZipOpenFile TabZipOpen[ MAX_ZIP_FILE_USING ];


void ZipInit();

int OpenZip( char * Nom, int ForceType, ZipOpenFile* ZipResult );

void MemZipIsOpen( ZipOpenFile* ZipInfoOpen, int Drive = -1  );

bool IsInOpendZip( char* File, ZipOpenFile* ZipResult );

bool SaveFileInZip( ZipOpenFile* ZipInfo );

int CloseFileZip( ZipOpenFile* ZipInfo );


// CDialZip dialog

class CDialZip : public CDialog
{
	DECLARE_DYNAMIC(CDialZip)

public:
	CDialZip(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDialZip();

	ZipOpenFile* ZipResult;
	ZIPENTRYEX* pTabFileInZip;

// Dialog Data
	enum { IDD = IDD_DIAL_ZIP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog() ;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLbnSelchangeListZipFile();

protected:
	CListBox m_ListFileInZip;
	
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnLbnDblclkListZipFile();
};

#endif // DIALZIP_H