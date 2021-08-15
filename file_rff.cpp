/*
** file_rff.cpp
**
**---------------------------------------------------------------------------
** Copyright 1998-2009 Randy Heit
** Copyright 2005-2009 Christoph Oelckers
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
**
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. The name of the author may not be used to endorse or promote products
**    derived from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
** IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
** IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
** NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
** THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**---------------------------------------------------------------------------
**
**
*/

#ifdef _MSC_VER
#include <direct.h>
#else // !_MSC_VER
#include <unistd.h>
#include <sys/stat.h>
#endif // _MSC_VER
#include <string>
#include "resourcefile.h"

std::string ExtractFileBase(const char* path, bool include_extension);

//==========================================================================
//
//
//
//==========================================================================

struct RFFInfo
{
	// Should be "RFF\x18"
	uint32_t		Magic;
	uint32_t		Version;
	uint32_t		DirOfs;
	uint32_t		NumLumps;
};

struct RFFLump
{
	uint32_t		DontKnow1[4];
	uint32_t		FilePos;
	uint32_t		Size;
	uint32_t		DontKnow2;
	uint32_t		Time;
	uint8_t		Flags;
	char		Extension[3];
	char		Name[8];
	uint32_t		IndexNum;	// Used by .sfx, possibly others
};

//==========================================================================
//
// BloodCrypt
//
//==========================================================================

void BloodCrypt (void *data, int key, int len)
{
	int p = (uint8_t)key, i;

	for (i = 0; i < len; ++i)
	{
		((uint8_t *)data)[i] ^= (unsigned char)(p+(i>>1));
	}
}

//==========================================================================
//
// Initializes a Blood RFF file
//
//==========================================================================

void RffExtract(const char* filename, FILE* f)
{
	uint32_t NumLumps;
	
	RFFInfo header;

	fread(&header, 1, sizeof(header), f);

	NumLumps = header.NumLumps;
	header.DirOfs = header.DirOfs;
	TArray<RFFLump> lumps(NumLumps, true);
	fseek (f, header.DirOfs, SEEK_SET);
	fread(lumps.Data(), header.NumLumps, sizeof(RFFLump), f);
	BloodCrypt (lumps.Data(), header.DirOfs, header.NumLumps * sizeof(RFFLump));

	auto name = ExtractFileBase(filename, false);
	mkdir(name.c_str());
	chdir(name.c_str()); 
	
	for (uint32_t i = 0; i < NumLumps; ++i)
	{
		TArray<char> buffer(lumps[i].Size, true);

		fseek(f, lumps[i].FilePos, SEEK_SET);
		fread(buffer.Data(), 1, lumps[i].Size, f);

		// Rearrange the name and extension to construct the fullname.
		char fn[128];

		bool mustuseid = 
			!memcmp(lumps[i].Extension, "KVX", 3) ||
			!memcmp(lumps[i].Extension, "PLU", 3) ||
			!memcmp(lumps[i].Extension, "QAV", 3) ||
			!memcmp(lumps[i].Extension, "PAL", 3) ||
			!memcmp(lumps[i].Extension, "SEQ", 3) ||
			!memcmp(lumps[i].Extension, "SFX", 3); // these are the data types only loaded by ID, so for them even 0 must be emitted.

		if (lumps[i].IndexNum != 0 || mustuseid)
		{
			snprintf(fn, 128, "%.8s.{%d}.%.3s", lumps[i].Name, lumps[i].IndexNum, lumps[i].Extension);
		}
		else
		{
			snprintf(fn, 128, "%.8s.%.3s", lumps[i].Name, lumps[i].Extension);
		}
		
		if (lumps[i].Flags & 0x10)
		{
			int cryptlen = std::min<int> (lumps[i].Size, 256);
			uint8_t *data = (uint8_t *)buffer.Data();
			
			for (int i = 0; i < cryptlen; ++i)
			{
				data[i] ^= i >> 1;
			}
		}
		FILE *fo = fopen(fn, "wb");
		if (fo)
		{
			fwrite(buffer.Data(), 1, lumps[i].Size, fo);
			fclose (fo);
		}
	}
}




