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

#ifdef _MSC_VER
#include <direct.h>
#else // !_MSC_VER
#include <unistd.h>
#include <sys/stat.h>
#endif // _MSC_VER
#include <string.h>
#include <stdio.h>
#include <string>
#include "wadext.h"
#include "resourcefile.h"
#include "fileformat.h"

#pragma warning(disable:4996)

bool isLevel(WadItemList * we);
char profile[256];
char maindir[128];

bool pstartfound=false;
WadItemList PNames(-1);

#ifndef _MSC_VER

static char* strlwr(char* str)
{
	for (char* ch = str; '\0' != *ch; ++ch)
	{
		*ch = tolower(*ch);
	}
	
	return str;
}

static char* strupr(char* str)
{
	for (char* ch = str; '\0' != *ch; ++ch)
	{
		*ch = toupper(*ch);
	}
	
	return str;
}

static int mkdir(const char *path)
{
	return mkdir(path, 0755);
}

#endif // !_MSC_VER

char * getdir(const char * lump)
{
	static char d[2]="";

	if (*lump>='A' && *lump<='Z') d[0]=*lump;
	else if (*lump>='a' && *lump<='z') d[0]=*lump-32;
	else if (*lump>='0' && *lump<='9') d[0]='0';
	else d[0]='@';
	return d;
}

bool isPatch(const char * n)
{
	if (!pstartfound || PNames.mLump < 0) return false;
	uint32_t * l = (uint32_t*)PNames.Address();
	char * c = (char*)(l + 1);
	char nn[9];


	if (!l) return false;

	strncpy(nn, n, 8);
	nn[8] = 0;
	strupr(nn);

	for (int i = 0; i<*l; i++)
	{
		if (!strncmp(c, nn, 8)) return true;
		c += 8;
	}
	return false;
}


const char * MakeFileName(const char * base, const char * ext)
{
	char basebuffer[20];
	static char buffer[40];

	strncpy(basebuffer,base,8);
	basebuffer[8]=0;

	for (int i = 0; i < 8; i++)
	{
		// replace special path characters in the lump name
		if (basebuffer[i] == '\\') basebuffer[i] = '^';
		if (basebuffer[i] == '/') basebuffer[i] = '\xA7';
		if (basebuffer[i] == ':') basebuffer[i] = '\xA1';
		if (basebuffer[i] == '*') basebuffer[i] = '\xB2';
		if (basebuffer[i] == '?') basebuffer[i] = '\xBF';
	}

	for(int i=0;;i++)
	{
		if (i > 0) sprintf(buffer,"%s(%d)%s", basebuffer, i, ext);
		else sprintf(buffer, "%s%s", basebuffer, ext);
		strlwr(buffer);
		FILE *f = fopen(buffer, "rb");
		if (f == NULL) return buffer;
		fclose(f);
	}
}

void Extract(WadItemList *w,const char * dir,const char * ext, int options, bool isflat)	//It is completely impossible to detect flats, so we have to flag it when extracting the F_ namespace
{
	FileType ft = IdentifyFileType(w->Name(), (uint8_t*)w->Address(), w->Length());
	const char *filename;

	if (dir == NULL)
	{
		switch (ft.type & FG_MASK)
		{
		case FG_GFX:
			if (isPatch(w->Name())) dir = "patches";
			else dir = "graphics";
			break;

		case FG_SND:
			dir = "sounds";
			break;

		case FG_MUS:
			dir = "music";
			break;

		case FG_VOX:
			dir = "voxels";
			break;

		}
	}
	// Flats are easy to misidentify so if we are in the flat namespace and the size is exactly 4096 bytes, let it pass as flat
	if (isflat && (ft.type & FG_MASK) != FG_GFX && (ft.type == FT_DOOMSND || ft.type == FT_MP3 || w->Length() == 4096))
	{
		ft.type = FT_BINARY;
		ft.extension = ".LMP";
	}

	if (dir != NULL)
	{
		mkdir(dir);
		chdir(dir);
	}
	printf("processing %.8s\n", w->Name());

	if (ft.type == FT_DOOMGFX && !(options & NO_CONVERT_GFX))
	{
		filename = MakeFileName(w->Name(), ".png");
		PatchToPng(options, (const uint8_t*)w->Address(), w->Length(), filename);
	}
	else if (w->Length() > 0 && ft.type == FT_BINARY && !(options & NO_CONVERT_GFX) && (isflat || (w->Length() == 64000 && options&(DO_HEXEN_PAL | DO_HERETIC_PAL))))
	{
		filename = MakeFileName(w->Name(), ".png");
		FlatToPng(options, (const uint8_t*)w->Address(), w->Length(), filename);
	}
	else if (ft.type == FT_DOOMSND && !(options & NO_CONVERT_SND))
	{
		filename = MakeFileName(w->Name(), ".wav");
		DoomSndToWav((const uint8_t*)w->Address(), w->Length(), filename);
	}
	else
	{
		filename = MakeFileName(w->Name(), ft.extension);

		FILE *f = fopen(filename, "wb");
		if (f == NULL)
		{
			printf("%s: Unable to create file\n", filename);
			chdir(maindir);
			return;
		}
		fwrite(w->Address(), 1, w->Length(), f);
		fclose(f);
	}
	chdir(maindir);
}

int ListExtract(int startmarker,const char * dir,const char * endm,const char * endm2,bool isflat,int options,int pack=0)
{
	char dirbuf[200];

	mkdir(dir);
	strcpy(dirbuf,dir);
	WadItemList ww(startmarker + 1);
	WadItemList *w = &ww;
	while (strncmp(w->Name(),endm,8) && strncmp(w->Name(),endm2,8)) 
	{
		Extract(w, dir, NULL, options, isflat);
		ww = ww.mLump + 1;
	}
	return ww.mLump;
}

bool isLevel(int lump)
{
	if (lump >= mainwad->NumLumps() - 3) return false;	// any level is at least 3 lumps
	WadItemList w(lump + 1);
	if (!stricmp(w.Name(), "THINGS")) return true;
	if (!stricmp(w.Name(), "TEXTMAP")) return true;
	return false;
}

int MapExtract(int startmarker,const char *dir,int options)
{
	int i;
	char buffer[40];
	TArray<CWADLump> directory;

	mkdir(dir);
	chdir(dir);

	WadItemList ww(startmarker);
	sprintf(buffer,"%.8s.wad", ww.Name());
	printf("processing %s\n",buffer);
	FILE *f=fopen(buffer, "wb");

	fwrite("PWAD\xb\0\0\0\0\0\0\0",1, 12, f);


	CWADLump lmp;
	lmp.offset = ftell(f);
	strncpy(lmp.name, ww.Name(), 8);
	lmp.length = ww.Length();
	fwrite(ww.Address(), 1, ww.Length(), f);
	directory.Push(lmp);
	ww = ww.mLump + 1;

	if (!stricmp(ww.Name(), "TEXTMAP"))
	{
		// UDMF
		while (true)
		{
			if (!(options & DO_STRIP) || (stricmp(ww.Name(), "ZNODES") && stricmp(ww.Name(), "BLOCKMAP") && stricmp(ww.Name(), "REJECT")))
			{
				CWADLump lmp;
				lmp.offset = ftell(f);
				strncpy(lmp.name, ww.Name(), 8);
				lmp.length = ww.Length();
				fwrite(ww.Address(), 1, ww.Length(), f);
				directory.Push(lmp);
			}

			if (!stricmp(ww.Name(), "ENDMAP"))
			{
				goto finalize;
			}
			int lump = ww.mLump + 1;
			if (lump == mainwad->NumLumps())
			{
				printf("Unexpected end of UDMF map found\n");
				return lump;
			}
			ww = lump;
		}
	}
	else
	{
		static const char *maplumps[] = { "THINGS", "LINEDEFS", "SIDEDEFS", "VERTEXES", "SEGS", "SSECTORS", "NODES", "SECTORS", "REJECT", "BLOCKMAP", "BEHAVIOR", "SCRIPTS" };

		for (i = 0; i < 12; i++)
		{
			if (ww.Name() != NULL && !stricmp(ww.Name(), maplumps[i]))
			{
				CWADLump lmp;
				lmp.offset = ftell(f);
				strncpy(lmp.name, ww.Name(), 8);
				if ((options & DO_STRIP) && (i == 4 || i == 5 || i == 6 || i == 8 || i == 9))
				{
					lmp.length = 0;
				}
				else
				{
					lmp.length = ww.Length();
					fwrite(ww.Address(), 1, ww.Length(), f);
				}
				directory.Push(lmp);
				ww = ww.mLump + 1;
			}
		}
	}
finalize:
	uint32_t lumpc = directory.Size();
	uint32_t dpos = ftell(f);
	for (auto &p : directory)
	{
		fwrite(&p, 1, 16, f);
	}
	fseek(f, 4, 0);
	fwrite(&lumpc, 1, 4, f);
	fwrite(&dpos, 1, 4, f);
	fclose(f);
	chdir("..");
	return directory.Size();
}


typedef char pname[8];

bool AddWallTexture(FILE * fo,maptexture_t * wt,pname * pNames, bool first)
{
	int k;
	char buffer[9];

	sprintf(buffer,"%.8s",wt->name);
	strupr(buffer);
	bool cc = strspn(buffer, "ABCDEFGHIJKLMNOPQRSTUVWXYZ_0123456789") < strlen(buffer) || *buffer <= '9';
	fprintf(fo,"WallTexture %s%s%s, %d, %d\n{\n",cc? "\"":"", buffer,cc? "\"":"", wt->width,wt->height);
	if (wt->ScaleX) fprintf(fo,"\tXScale %f\n",wt->ScaleX/8.0f);
	if (wt->ScaleY) fprintf(fo,"\tYScale %f\n",wt->ScaleY/8.0f);
	if (wt->Flags&0x8000) fprintf(fo,"\tWorldPanning\n");
	if (first) fprintf(fo,"\tNullTexture\n");
	for(k=0;k<wt->patchcount;k++)
	{
		sprintf(buffer,"%.8s",pNames[wt->patches[k].patch]);
		strupr(buffer);
		cc = strspn(buffer, "ABCDEFGHIJKLMNOPQRSTUVWXYZ_0123456789") < strlen(buffer) || *buffer <= '9';
		fprintf(fo,"\tPatch %s%.8s%s, %d, %d\n",cc? "\"":"", buffer,cc? "\"":"", wt->patches[k].originx,wt->patches[k].originy);
	}
	fprintf(fo,"}\n");
	return true;
}

bool AddWallTexture(FILE * fo,strifemaptexture_t * wt,pname * pNames, bool first)
{
	int k;
	char buffer[9];

	sprintf(buffer,"%.8s",wt->name);
	strupr(buffer);
	bool cc = strspn(buffer, "ABCDEFGHIJKLMNOPQRSTUVWXYZ_0123456789") < strlen(buffer) || *buffer <= '9';
	fprintf(fo,"WallTexture %s%s%s, %d, %d\n{\n",cc? "\"":"", buffer,cc? "\"":"", wt->width,wt->height);
	if (wt->ScaleX) fprintf(fo,"\tXScale %f\n",wt->ScaleX/8.0f);
	if (wt->ScaleY) fprintf(fo,"\tYScale %f\n",wt->ScaleY/8.0f);
	if (wt->Flags&0x8000) fprintf(fo,"\tWorldPanning\n");
	for(k=0;k<wt->patchcount;k++)
	{
		sprintf(buffer,"%.8s",pNames[wt->patches[k].patch]);
		strupr(buffer);
		cc = strspn(buffer, "ABCDEFGHIJKLMNOPQRSTUVWXYZ_0123456789") < strlen(buffer) || *buffer <= '9';
		fprintf(fo,"\tPatch %s%.8s%s, %d, %d\n",cc? "\"":"", buffer,cc? "\"":"", wt->patches[k].originx,wt->patches[k].originy);
	}
	fprintf(fo,"}\n");
	return true;
}

void AnimExtract(WadItemList *w)
{
	int i,numentries;
	char * c;
	char buffer[30];

	mkdir("decompiled");
	chdir("decompiled");

	strcpy(buffer,"animdefs.animated");
	printf("processing %s\n",buffer);

	numentries=w->Length()/23;

	c=(char*)w->Address();

	FILE * fo=fopen(buffer,"wt");
	for(i=0;i<numentries;i++)
	{
		fprintf(fo,"%s %s Range %s Tics %d\n", *c? "Texture":"Flat",c+10, c+1,*(int*)(c+19));
		c+=23;
	}

	fclose(fo);
	chdir(maindir);
}

void SwitchExtract(WadItemList *w)
{
	int i,numentries;
	char * c;
	char buffer[30];

	mkdir("decompiled");
	chdir("decompiled");


	strcpy(buffer, "animdefs.switches");
	printf("processing %s\n",buffer);

	numentries=w->Length()/20;

	c=(char*)w->Address();

	FILE * fo=fopen(buffer,"wt");
	for(i=0;i<numentries;i++)
	{
		if (*(short*)(c+18)==0) break;
		//fprintf(fo,"%s,%s,%d\n",c,c+9,*(short*)(c+18));
		fprintf(fo,"Switch %s on pic %s tics 0\n", c, c+9);
		c+=20;
	}

	fclose(fo);
	chdir(maindir);
	//Extract(w, dir);
}


void GenerateTextureFile(const char *name, const char * pTex,const char * pPNam, int options, bool nullfirst)
{
	const char * tr=pTex;
	int tc =*(int*)tr;
	int * to =(int*)(tr+4);
	pname * pr=(pname*)((char*)pPNam+4);


	FILE * fo=fopen(name,"wt");

	// Todo: Be more thorough than this!
	if (!(options&DO_STRIFE))
	{
		for(int i=0;i<tc;i++)
			AddWallTexture(fo, (maptexture_t*)(tr+to[i]),pr, i == 0 && nullfirst);

	}
	else
	{
		for(int i=0;i<tc;i++)
			AddWallTexture(fo, (strifemaptexture_t*)((char*)tr+to[i]),pr, i == 0 && nullfirst);
	}
	
	fclose(fo);
}

void GenerateTextureFile(WadItemList * pTex,WadItemList * pPNam, int options)
{
	char buffer[40];
	mkdir("decompiled");
	sprintf(buffer,"decompiled\\textures.%c", pTex->Name()[7]);
	bool nulltex = !strnicmp(pTex->Name(), "TEXTURE1", 8);
	GenerateTextureFile(buffer, (const char*)pTex->Address(), (const char *)pPNam->Address(), options, nulltex);
}

//==========================================================================
//
// IsSeparator (taken from ZDoom)
//
// Returns true if the character is a path separator.
//
//==========================================================================

static inline bool IsSeparator(int c)
{
	if (c == '/')
		return true;
#ifdef WIN32
	if (c == '\\' || c == ':')
		return true;
#endif
	return false;
}

//==========================================================================
//
// ExtractFileBase (taken from ZDoom)
//
// Returns the file part of a pathname, optionally including the extension.
//
//==========================================================================

std::string ExtractFileBase(const char *path, bool include_extension)
{
	const char *src, *dot;

	src = path + strlen(path) - 1;

	if (src >= path)
	{
		// back up until a / or the start
		while (src != path && !IsSeparator(*(src - 1)))
			src--;

		// Check for files with drive specification but no path
#if defined(_WIN32)
		if (src == path && src[0] != 0)
		{
			if (src[1] == ':')
				src += 2;
		}
#endif

		if (!include_extension)
		{
			dot = src;
			while (*dot && *dot != '.')
			{
				dot++;
			}
			return std::string(src, dot - src);
		}
		else
		{
			return std::string(src);
		}
	}
	return std::string();
}


void ExtractWad(char * wadfilename,int options)
{
	WadItemList pTex1(-1), pTex2(-1), pPnam(-1);

	OpenMainWad(wadfilename);
	auto name = ExtractFileBase(wadfilename, false);
	mkdir(name.c_str());
	chdir(name.c_str());
	
	PNames = mainwad->FindLump("PNAMES");
	getcwd(maindir, 128);
	for(int i=0;i<mainwad->NumLumps();i++)
	{
		WadItemList ww(i);
		WadItemList *w = &ww;
		{
			if (!strcmp(w->Name(),"."))
			{
				w->Release();
			}
			else if (!strnicmp(w->Name(),"S_START",8) || !strnicmp(w->Name(),"SS_START",8) )
			{
				i=ListExtract(i,"SPRITES","S_END","SS_END",false,options,false);
			}
			else if (!strnicmp(w->Name(),"F_START",8) || !strnicmp(w->Name(),"FF_START",8) )
			{
				i=ListExtract(i,"FLATS","F_END","FF_END",true,options,false);
			}
			else if (!strnicmp(w->Name(),"C_START",8))
			{
				i=ListExtract(i,"COLORMAPS","C_END","CC_END",false,false,options);
			}
			else if (!strnicmp(w->Name(),"A_START",8))
			{
				i=ListExtract(i,"ACS","A_END","AA_END",false,false,options);
			}
			else if (!strnicmp(w->Name(),"P_START",8) || !strnicmp(w->Name(),"PP_START",8))
			{
				pstartfound=true;
			}
			else if (!strnicmp(w->Name(),"P_END",8) || !strnicmp(w->Name(),"PP_END",8))	// ignore
			{
			}
			else if (!strnicmp(w->Name(),"TX_START",8))
			{
				i=ListExtract(i,"TEXTURES","TX_END","TX_END",false,options);
			}
			else if (!strnicmp(w->Name(),"HI_START",8))
			{
				i=ListExtract(i,"HIRES","HI_END","HI_END",false,options);
			}
			else if (!strnicmp(w->Name(),"VX_START",8))
			{
				i=ListExtract(i,"VOXELS","VX_END","VX_END",false,false,options);
			}
			else if (!strnicmp(w->Name(),"GL_VERT",8))	// ignore
			{
				// skip GL nodes
			}
			else if (!strnicmp(w->Name(),"GL_SEGS",8))	// ignore
			{
				// skip GL nodes
			}
			else if (!strnicmp(w->Name(),"GL_SSECT",8))	// ignore
			{
				// skip GL nodes
			}
			else if (!strnicmp(w->Name(),"GL_NODES",8))	// ignore
			{
				// skip GL nodes
			}
			else if (!strnicmp(w->Name(), "GL_PVS", 8))	// ignore
			{
				// skip GL nodes
			}
			else if (isLevel(i))
			{
				i += MapExtract(i,"MAPS",options) - 1;
			}
			else 
			{
				if (!strnicmp(w->Name(), "TEXTURE1", 8)) pTex1 = *w;
				else if (!strnicmp(w->Name(), "TEXTURE2", 8)) pTex2 = *w;
				else if (!strnicmp(w->Name(), "PNAMES", 8)) pPnam = *w;
				else if (!strnicmp(w->Name(), "ANIMATED", 8))
				{
					AnimExtract(w);
				}
				else if (!strnicmp(w->Name(), "SWITCHES", 8))
				{
					SwitchExtract(w);
				}

				Extract(w, NULL, NULL, options, false);
			}

			if (pTex1.mLump >= 0 && pPnam.mLump >= 0)
			{
				GenerateTextureFile(&pTex1,&pPnam, options);
				pTex1 = -1;
			}
			if (pTex2.mLump >= 0 && pPnam.mLump >= 0)
			{
				GenerateTextureFile(&pTex2,&pPnam, options);
				pTex2=-1;
			}
		}
	}
	chdir("..");
}			

void ConvertTextureX()
{
	FILE *ft1 = fopen("texture1", "rb");
	if (!ft1) ft1 = fopen("texture1.lmp", "rb");

	FILE *ft2 = fopen("texture2", "rb");
	if (!ft2) ft2 = fopen("texture2.lmp", "rb");

	FILE *pnm = fopen("pnames", "rb");
	if (!pnm) pnm = fopen("pnames.lmp", "rb");

	if (!pnm) exit(1);

	static char bt1[100000], bt2[100000], bpn[100000];

	if (ft1) fread(bt1, 1, 100000, ft1), fclose(ft1);
	if (ft2) fread(bt2, 1, 100000, ft2), fclose(ft2);
	if (pnm) fread(bpn, 1, 100000, pnm), fclose(pnm);

	if (ft1) GenerateTextureFile("textures.txt", bt1, bpn, 0, true);
	if (ft2) GenerateTextureFile("textures.txt2", bt2, bpn, 0, false);
}

