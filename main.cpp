// 
//---------------------------------------------------------------------------
//
// Copyright(C) 2016 Christoph Oelckers
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

#include <direct.h>
#include <io.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "wadext.h"

void BuildWad(char * wadfilename,int options);
void ExtractWad(char * wadfilename,int options);
void ConvertTextureX();

int ParseOptions(int argc,char ** argv,int & index)
{
	struct parseinf
	{
		char * opt;
		int flag;
		int argc;
	} pi[]={
		{"-nogfxconvert",NO_CONVERT_GFX},
		{"-nosndconvert",NO_CONVERT_SND },
		{"-heretic",DO_HERETIC_PAL},
		{"-hexen",DO_HEXEN_PAL},
		{"-strife",DO_STRIFE},
		{"-strip",DO_STRIP},
		{NULL,0}
	};

	int def_opt=0;

	for(int i=2;i<argc;i++)
	{
		for(int j=0;pi[j].opt;j++)
		{
			if (!stricmp(argv[i],pi[j].opt)) 
			{
				def_opt|=pi[j].flag;
			}
		}
	}
	return def_opt;
}

int main(int argc, char ** argv)
{
	if (argc>1 && !strcmp(argv[1], "-tx"))
	{
		ConvertTextureX();
	}
	else if (argc>2)
	{
		int index;
		int f=ParseOptions(argc,argv,index);
		ExtractWad(argv[1],f);
	}
	else if (argc > 1)
	{
#ifdef DEFAULT_STRIP
		ExtractWad(argv[1],DO_STRIP);
#else
		ExtractWad(argv[1], 0);
#endif
	}
	else
	{
		printf("WadExt v1.0 (c) 2016 Christoph Oelckers\nUsage: wadext [options] filename\n");
		printf("Options:\n");
		printf("   -nogfxconvert : Leave Doom format patches and flats in their original form, if not specified they will be converted to PNG.\n");
		printf("   -nosndconvert : Leave Doom format sounds in their original form, if not specified they will be converted to WAV.\n");
		printf("   -heretic, -hexen, -strife: Force usage of the named game's palette if the WAD does not contain one. Default is Doom palette\n");
		printf("   -strip: Remove node lumps from extracted maps.\n");
		printf("   -tx: Converts a set of TEXTURE1/TEXTURE2/PNAMES in the current directory to a textures.txt file.\n");
	}
	return 0;
}