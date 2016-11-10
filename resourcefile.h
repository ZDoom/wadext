#ifndef __RESOURCEFILE_H
#define __RESOURCEFILE_H

#include <stdio.h>
#include <stdint.h>
#include "tarray.h"

//==========================================================================
//
//
//
//==========================================================================

struct CWADLump
{
	uint32_t offset;
	uint32_t length;
	char name[9];
};

class CWADFile
{
	TArray<CWADLump> lumps;
	FILE *m_File;

	char * m_Filename;
	int m_NumLumps;
	uint64_t m_LumpStart;
	
public:
	CWADFile(const char * filename);
	CWADFile(const char * name, const char * memory);
	~CWADFile(void);

	const char * GetName() const
	{
		return m_Filename;
	}
	int NumLumps()
	{
		return m_NumLumps;
	}
	
	virtual int FindLump(const char * name);
	virtual int GetLumpSize(int lump);
	virtual void * ReadLump(int lump);
	virtual void ReadLump(int lump, void * buffer);
	virtual const char * GetLumpName(int lump);
};

extern CWADFile *mainwad;

struct WadItemList
{
	int mLump;
	TArray<char> mData;

	WadItemList(int lump)
	{
		mLump = lump;
	}

	char * Address()
	{
		if (mData.Size() != Length())
		{
			mData.Resize(Length());
		}
		mainwad->ReadLump(mLump, &mData[0]);
		return &mData[0];
	}

	int Length() const
	{
		return mainwad->GetLumpSize(mLump);
	}

	void Release()
	{
		mData.Clear();
	}

	const char * Name() const { return mainwad->GetLumpName(mLump); }
};

void OpenMainWad(char * filename);

#endif