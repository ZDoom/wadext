// 
//---------------------------------------------------------------------------
//
// Copyright(C) 2005-2016 Christoph Oelckers
// All rights reserved.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/
//
//--------------------------------------------------------------------------
//

#include <stdlib.h>
#include <string.h>
#include "resourcefile.h"

//==========================================================================
//
//
//
//==========================================================================

CWADFile::CWADFile(const char * filename, FILE* f)
{
	char type[4];
	int diroffset;
	int numlumps;

	m_NumLumps=0;
	m_LumpStart=0;
	
	m_Filename = strdup(filename);

	m_File = f;
	fread(&type, 1, 4, f);

	if (memcmp(type, "IWAD", 4) && memcmp(type, "PWAD", 4))
	{
		printf("%s: not a WAD file\n", filename);
		return;
	}
	fread(&numlumps, 4, 1, f);
	fread(&diroffset, 4, 1, f);
	m_NumLumps = numlumps;

	fseek(f, diroffset, SEEK_SET);

	CWADLump lump;

	for (int i = 0; i < numlumps; i++)
	{
		fread(&lump.offset, 4, 1, f);
		fread(&lump.length, 4, 1, f);
		fread(lump.name, 1, 8, f);
		lump.name[8]=0;

		lumps.Push(lump);

	}
}

//==========================================================================
//
//
//
//==========================================================================

CWADFile::~CWADFile(void)
{
	if (m_File) fclose(m_File);
	if (m_Filename!=NULL) free(m_Filename);
}

//==========================================================================
//
//
//
//==========================================================================

int CWADFile::FindLump(const char * name)
{
	for (int i = lumps.Size()-1; i>=0; i--)
	{
		if (!stricmp(name, lumps[i].name)) return i;
	}
	return -1;
}

//==========================================================================
//
//
//
//==========================================================================

int CWADFile::GetLumpSize(int lump)
{
	if ((unsigned)lump < lumps.Size() ) return lumps[lump].length;
	else return -1;
}

//==========================================================================
//
//
//
//==========================================================================

void * CWADFile::ReadLump(int lump)
{
	if ((unsigned)lump < lumps.Size() )
	{
		void * Ptr= malloc(lumps[lump].length);
		ReadLump(lump, Ptr);
		return Ptr;
	}
	return NULL;
}

//==========================================================================
//
//
//
//==========================================================================

void CWADFile::ReadLump(int lump, void * Ptr)
{
	if ((unsigned)lump < lumps.Size() )
	{
		fseek(m_File, lumps[lump].offset, SEEK_SET);
		fread(Ptr, 1, lumps[lump].length, m_File);
	}
}

//==========================================================================
//
//
//
//==========================================================================

const char * CWADFile::GetLumpName(int lump)
{
	if ((unsigned)lump < lumps.Size() )
	{
		return lumps[lump].name;
	}
	else
	{
		return NULL;
	}
}

//==========================================================================
//
//
//
//==========================================================================

void GrpExtract(const char* filename, FILE* f);
void RffExtract(const char* filename, FILE* f);

//==========================================================================
//
//
//
//==========================================================================
CWADFile *mainwad;

void OpenMainWad(char *filename)
{
	if (filename == NULL)
	{
		printf("No file name specified\n");
		exit(1);
	}

	FILE *f = fopen(filename, "rb");
	if (!f)
	{
		printf("%s: unable to open\n", filename);
		exit(1);
	}


	char type[4];
	fread(&type, 1, 4, f);
	fseek(f, 0, SEEK_SET);

	if (memcmp(type, "IWAD", 4) == 0 || memcmp(type, "PWAD", 4) == 0)
	{
		mainwad = new CWADFile(filename, f);
		if (mainwad->NumLumps() == 0)
		{
			exit(1);
		}
	}
	else if (memcmp(type, "KenS", 4) == 0)
	{
		GrpExtract(filename, f);
		exit(1);
	}
	else if (!memcmp(type, "RFF\x1a", 4))
	{
		RffExtract(filename, f);
		exit(1);
	}
	else
	{
		printf("%s: not a WAD file\n", filename);
		exit(1);
	}
}

