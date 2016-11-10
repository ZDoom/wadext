
#pragma pack(1)
#pragma warning(disable:4200)

#include "ResourceFile.h"

extern CWADFile *mainwad;

struct CVertex
{
	short x;
	short y;
};


struct CThing
{
   short xpos;      /* x position */
   short ypos;      /* y position */
   short angle;     /* facing angle */
   short type;      /* thing type */
   short when;      /* appears when? */
};



/*
   this data structure contains the information about the LINEDEFS
*/
struct CLineDef
{
   unsigned short start;     /* from this vertex ... */
   unsigned short end;       /* ... to this vertex */
   short flags;     /* see NAMES.C for more info */
   short type;      /* see NAMES.C for more info */
   short tag;       /* crossing this linedef activates the sector wi*/
   short sidedef1;  /* sidedef */
   short sidedef2;  /* only if this line adjoins 2 sectors */
};



/*
   this data structure contains the information about the SIDEDEFS
*/
struct CSideDef
{
   short xoff;      /* X offset for texture */
   short yoff;      /* Y offset for texture */
   char texUpper[8];  /* texture name for the part above */
   char texLower[8];  /* texture name for the part below */
   char texNormal[8];  /* texture name for the regular part */
   short sector;    /* adjacent sector */
};


struct CSector
{
   short floorh;
   short ceilh;
   char floort[8];
   char ceilt[8];
   short light;
   short special;
   short tag;
};


struct CWadFileHeader
{
	char id[4];
	long numEntries;
	long DirOffset;
};

struct CWadFileEntry
{
	long FileOffset;
	long EntryLength;
	char name[8];
};

struct CWad3FileEntry
{
	long FileOffset;
	long EntryLength;
	long EntryLength2;
	long type;
	char name[16];
};

struct WadItemList;

struct CLevelInfo
{
	CThing * pThings;
	CLineDef * pLineDefs;
	CSideDef * pSideDefs;
	CVertex * pVertexes;
	CSector * pSectors;

	char * pNodes;
	char * pSegs;
	char * pSSector;

	char * pBlockmap;
	char * pReject;

	char * pBehavior;
	char * pScript;

	int nThings;
	int nLineDefs;
	int nSideDefs;
	int nVertexes;
	int nSectors;

	int nNodes;
	int nSegs;
	int nSSectors;

	int cReject;
	int cBlockmap;
	int cBehavior;
	int cScript;

	char * pGLVerts;
	char * pGLSegs;
	char * pGLSSectors;
	char * pGLNodes;

	int nGLVertexes;
	int nGLNodes;
	int nGLSegs;
	int nGLSSectors;

	int NameTag;

	CLevelInfo()
	{
		NameTag=0;
	}

	void Release();

	~CLevelInfo() 
	{
		Release();
	}

	bool HasBehavior()
	{
		return pBehavior!=NULL;
	}
	bool HasScript()
	{
		return pBehavior!=NULL;
	}
};



/*
struct WadInfo
{
	HANDLE hFile;
	HANDLE hMapping;
	CWadFileHeader * pwAddr;
	union
	{
		CWadFileEntry  * pwDir;
		CWad3FileEntry * pw3Dir;
	};
	int wtype;
	char filename[256];
};


struct WadList
{
	WadInfo * pwInfo;
	WadList * pNext;
};

struct WadItemList
{
	CWadFileEntry * pwInfo;
	int type;

	WadInfo * pwWad;
	WadItemList * pSuccessor;	// the overloaded entry
	WadItemList * pNext;

	WadItemList * Next() { return pNext; }
	void* PackedAddress() const { return (void*)(((char*)pwWad->pwAddr)+pwInfo->FileOffset); }

	bool isPacked() const
	{
		return false;
	}

	char * Address() 
	{
		return (char *)PackedAddress();
	}

	int Length() const 
	{ 
		return pwInfo->EntryLength; 
	}

	void Release()
	{
	}
	char * Name() const { return pwInfo->name; }
	char * WadFileName() { return pwWad->filename; }

	WadItemList() { }
	~WadItemList() { }
};
*/

struct ImportInf
{
	char name[9];
	int insX,insY;
	int transparentCol;
};

//extern WadItemList * GlobalDirectory;



//void CloseWadFile(WadInfo * wi);
void OpenMainWad(char * filename);
void ClosePatchWad(const char * filename);
void * GetWadItem(char * name,int * pLength);
void FreeWadItem(char * name,char * blockstart=NULL);
void NameError(char * level,char * section);
bool GetLevelInfo(char * name,CLevelInfo * pLi);
//int  GetLevelFileInfo(char * fn,CLevelInfo * pli,WadInfo **pp=NULL);


inline void CLevelInfo::Release()
{
	NameTag=-1;
}

