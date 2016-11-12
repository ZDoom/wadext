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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wadext.h"
#include "fileformat.h"

#pragma warning(disable:4996)

//============================================================================
//
//
//
//============================================================================

struct Detector
{
	bool(*checker)(const uint8_t *pbuffer, int len);
	FileType ret;
};

bool IsMagic(const uint8_t *s, const char *magic, int len = 4)
{
	return !memcmp(s, magic, len);
}

//============================================================================
//
// Doom patch detector
//
//============================================================================

bool isPatch(const uint8_t *data, int length)
{
	if (length < 13) return false;	// minimum length of a valid Doom patch

	const patch_t *foo = (const patch_t *)data;

	int height = foo->height;
	int width = foo->width;

	if (height <= 4096 && width <= 4096 && width < length / 4 && abs(foo->leftoffset) < 4096 && abs(foo->topoffset) < 4096)
	{
		// The dimensions seem like they might be valid for a patch, so
		// check the column directory for extra security. At least one
		// column must begin exactly at the end of the column directory,
		// and none of them must point past the end of the patch.
		bool gapAtStart = true;
		int x;

		for (x = 0; x < width; ++x)
		{
			uint32_t ofs = foo->columnofs[x];
			if (ofs == (uint32_t)width * 4 + 8)
			{
				gapAtStart = false;
			}
			else if (ofs >= (uint32_t)length)	// Need one byte for an empty column (but there's patches that don't know that!)
			{
				return false;
			}
		}
		return !gapAtStart;
	}
	return false;
}

//============================================================================
//
//
//
//============================================================================

bool isPng(const uint8_t *data, int length)
{
	const uint32_t *dwdata = (const uint32_t*)data;
	if (length < 16) return false;
	// Check up to the IHDR so that this rejects Apple's homegrown image format that masquerades as a PNG but isn't one.
	return
		dwdata[0] == MAKE_ID(137, 'P', 'N', 'G') &&
		dwdata[1] == MAKE_ID(13, 10, 26, 10) &&
		dwdata[2] == MAKE_ID(0, 0, 0, 13) &&
		dwdata[3] == MAKE_ID('I', 'H', 'D', 'R');
}

//============================================================================
//
//
//
//============================================================================

bool isJpg(const uint8_t *data, int length)
{
	return (*data == 0xff && IsMagic(data + 6, "JFIF"));
}

//============================================================================
//
//
//
//============================================================================

bool isTGA(const uint8_t *data, int length)
{
	const TGAHeader *hdr = (const TGAHeader *)data;
	if (length < sizeof(TGAHeader)) return false;

	// Not much that can be done here because TGA does not have a proper
	// header to be identified with.
	if (hdr->has_cm != 0 && hdr->has_cm != 1) return false;
	if (hdr->width <= 0 || hdr->height <= 0) return false;
	if (hdr->bpp != 8 && hdr->bpp != 15 && hdr->bpp != 16 && hdr->bpp != 24 && hdr->bpp != 32) return false;
	if (hdr->img_type <= 0 || hdr->img_type > 11) return false;
	if (hdr->img_type >= 4 && hdr->img_type <= 8) return false;
	if ((hdr->img_desc & 16) != 0) return false;
	return true;
}

//============================================================================
//
//
//
//============================================================================

bool isPCX(const uint8_t *data, int length)
{
	const PCXHeader *hdr = (const PCXHeader*)data;

	if (length < sizeof(PCXHeader)) return false;

	if (hdr->manufacturer != 10 || hdr->encoding != 1) return false;
	if (hdr->version != 0 && hdr->version != 2 && hdr->version != 3 && hdr->version != 4 && hdr->version != 5) return false;
	if (hdr->bitsPerPixel != 1 && hdr->bitsPerPixel != 8) return false;
	if (hdr->bitsPerPixel == 1 && hdr->numColorPlanes != 1 && hdr->numColorPlanes != 4) return false;
	if (hdr->bitsPerPixel == 8 && hdr->uint8_tsPerScanLine != ((hdr->xmax - hdr->xmin + 2)&~1)) return false;

	for (int i = 0; i < 54; i++)
	{
		if (hdr->padding[i] != 0) return false;
	}
	return true;
}

//============================================================================
//
//
//
//============================================================================

bool isDDS(const uint8_t *data, int length)
{
	const DDSFileHeader *Header = (const DDSFileHeader*)data;
	if (length < sizeof(DDSFileHeader)) return false;

	return Header->Magic == ID_DDS &&
		(Header->Desc.Size == sizeof(DDSURFACEDESC2) || Header->Desc.Size == ID_DDS) &&
		Header->Desc.PixelFormat.Size == sizeof(DDPIXELFORMAT) &&
		(Header->Desc.Flags & (DDSD_CAPS | DDSD_PIXELFORMAT | DDSD_WIDTH | DDSD_HEIGHT)) == (DDSD_CAPS | DDSD_PIXELFORMAT | DDSD_WIDTH | DDSD_HEIGHT) &&
		Header->Desc.Width != 0 &&
		Header->Desc.Height != 0;
}


//============================================================================
//
//
//
//============================================================================

bool isImgz(const uint8_t *pbuffer, int len)
{
	return IsMagic(pbuffer, "IMGZ");
}

//============================================================================
//
//
//
//============================================================================

bool isMid(const uint8_t *pbuffer, int len)
{
	return IsMagic(pbuffer, "MThd");
}

//============================================================================
//
//
//
//============================================================================

bool isMus(const uint8_t *pbuffer, int len)
{
	return IsMagic(pbuffer, "MUS\x1a");
}

//============================================================================
//
//
//
//============================================================================

bool isIt(const uint8_t *pbuffer, int len)
{
	return IsMagic(pbuffer, "IMPM");
}

//============================================================================
//
//
//
//============================================================================

bool isS3m(const uint8_t *pbuffer, int len)
{
	return len >= 0x30 && IsMagic(pbuffer+0x3c, "SCRM");
}

//============================================================================
//
//
//
//============================================================================

bool isXt(const uint8_t *pbuffer, int len)
{
	return len >= 0x34 && IsMagic(pbuffer + 0x26, "FastTracker", 11);
}

//============================================================================
//
//
//
//============================================================================

bool isMod(const uint8_t *pbuffer, int len)
{
	if (len < 1084) return false;

	const uint8_t *s = pbuffer + 1080;
	return (IsMagic(s, "M.K.") || IsMagic(s, "M!K!") || IsMagic(s, "M&K!") || IsMagic(s, "N.T.") ||
		IsMagic(s, "CD81") || IsMagic(s, "OKTA") || IsMagic(s, "16CN") || IsMagic(s, "32CN") ||
		((s[0] == 'F') && (s[1] == 'L') && (s[2] == 'T') && (s[3] >= '4') && (s[3] <= '9')) ||
		((s[0] >= '2') && (s[0] <= '9') && (s[1] == 'C') && (s[2] == 'H') && (s[3] == 'N')) ||
		((s[0] == '1') && (s[1] >= '0') && (s[1] <= '9') && (s[2] == 'C') && (s[3] == 'H')) ||
		((s[0] == '2') && (s[1] >= '0') && (s[1] <= '9') && (s[2] == 'C') && (s[3] == 'H')) ||
		((s[0] == '3') && (s[1] >= '0') && (s[1] <= '2') && (s[2] == 'C') && (s[3] == 'H')) ||
		((s[0] == 'T') && (s[1] == 'D') && (s[2] == 'Z') && (s[3] >= '4') && (s[3] <= '9'))
		);
}

//============================================================================
//
//
//
//============================================================================

bool isSpc(const uint8_t *pbuffer, int len)
{
	return len >= 27 && IsMagic(pbuffer, "SNES - SPC700 Sound File Data v0.30\x1A\x1A", 27);
}

//============================================================================
//
//
//
//============================================================================

bool isHmi(const uint8_t *pbuffer, int len)
{
	return len >= 12 && IsMagic(pbuffer, "HMI-MIDISONG", 12);
}

//============================================================================
//
//
//
//============================================================================

bool isHmp(const uint8_t *pbuffer, int len)
{
	return len >= 12 && IsMagic(pbuffer, "HMIMIDIP", 8);
}

//============================================================================
//
//
//
//============================================================================

bool isXmi(const uint8_t *pbuffer, int len)
{
	return len >= 12 &&
		((IsMagic(pbuffer, "FORM") && IsMagic(pbuffer + 8, "XDIR")) ||
			(IsMagic(pbuffer, "FORM") && IsMagic(pbuffer + 8, "XMID")) ||
			(IsMagic(pbuffer, "CAT ") && IsMagic(pbuffer + 8, "XMID")));
}

//============================================================================
//
// Doom sound format has virtually no reliable features to identify. 
// All we can go by is to check if the little info in the header makes sense.
//
//============================================================================

bool isDoomSound(const uint8_t *data, int len)
{
	if (len < 8) return false;
	uint32_t dmxlen = GetInt(data + 4);
	int32_t freq = GetShort(data + 2) & 0xffff;
	return data[0] == 3 && data[1] == 0 /*&& (freq == 0 || freq == 11025 || freq == 22050 || freq == 44100)*/ && dmxlen <= unsigned(len - 8);
}

//============================================================================
//
//
//
//============================================================================

bool isWav(const uint8_t *pbuffer, int len)
{
	return len > 16 && IsMagic(pbuffer, "RIFF") && IsMagic(pbuffer + 8, "WAVE");
}

//============================================================================
//
//
//
//============================================================================

bool isAiff(const uint8_t *pbuffer, int len)
{
	return len > 12 && IsMagic(pbuffer, "FORM") && IsMagic(pbuffer+8, "AIFF");
}

bool isAifc(const uint8_t *pbuffer, int len)
{
	return len > 12 && IsMagic(pbuffer, "FORM") && IsMagic(pbuffer + 8, "AIFC");
}

//============================================================================
//
//
//
//============================================================================

bool isFlac(const uint8_t *pbuffer, int len)
{
	return IsMagic(pbuffer, "fLaC");
}

//============================================================================
//
//
//
//============================================================================

bool isOgg(const uint8_t *pbuffer, int len)
{
	return IsMagic(pbuffer, "OggS");
}

//============================================================================
//
// Almost nothing to check here, so this format is checked last of all.
//
//============================================================================

bool isMP3(const uint8_t *data, int len)
{
	if (IsMagic(data, "ID3\3")) return true;
	return data[0] == 0xff && ((data[1] & 0xfe) == 0xfa/*MPEG-1*/ || (data[1] & 0xfe) == 0xf2/*MPEG-2*/);
}



//============================================================================
//
//
//
//============================================================================

bool isVoc(const uint8_t *pbuffer, int len)
{
	return len > 19 && IsMagic(pbuffer, "Creative Voice File", 19);
}

//============================================================================
//
//
//
//============================================================================

bool isFon1(const uint8_t *pbuffer, int len)
{
	return IsMagic(pbuffer, "FON1") && pbuffer[5] == 0 && pbuffer[7] == 0;
}

//============================================================================
//
//
//
//============================================================================

bool isFon2(const uint8_t *pbuffer, int len)
{
	return IsMagic(pbuffer, "FON2") && pbuffer[5] == 0 && pbuffer[6] <= pbuffer[7];
}

//============================================================================
//
//
//
//============================================================================

bool isBmf(const uint8_t *data, int len)
{
	return (data[0] == 0xE1 && data[1] == 0xE6 && data[2] == 0xD5 && data[3] == 0x1A);
}

//==========================================================================
//
//
//
//==========================================================================

bool isKVX(const uint8_t *data, int length)
{
	// This isn't reliable enough.
	return false;
#if 0
	const kvxslab_t *slabs[MAXVOXMIPS];
	const uint8_t *rawmip;
	int mip, maxmipsize;
	int i, j, n;

	const uint8_t *rawvoxel = data;
	int voxelsize = length - 1;

	// Oh, KVX, why couldn't you have a proper header? We'll just go through
	// and collect each MIP level, doing lots of range checking, and if the
	// last one doesn't end exactly 768 uint8_ts before the end of the file,
	// we'll reject it.

	for (mip = 0, rawmip = rawvoxel, maxmipsize = voxelsize - 768 - 4;
		mip < MAXVOXMIPS;
		mip++)
	{
		int numuint8_ts = GetInt(rawmip);
		if (numuint8_ts > maxmipsize || numuint8_ts < 24)
		{
			break;
		}
		rawmip += 4;

		FVoxelMipLevel mipl;

		// Load header data.
		mipl.SizeX = GetInt(rawmip + 0);
		mipl.SizeY = GetInt(rawmip + 4);
		mipl.SizeZ = GetInt(rawmip + 8);
		if (mipl.SizeX > 255 || mipl.SizeY > 255 || mipl.SizeZ > 255) return false;

		// How much space do we have for voxdata?
		int offsetsize = (mipl.SizeX + 1) * 4 + mipl.SizeX * (mipl.SizeY + 1) * 2;
		int voxdatasize = numuint8_ts - 24 - offsetsize;
		if (voxdatasize < 0)
		{ // Clearly, not enough.
			return false;
		}
		if (voxdatasize != 0)
		{	// This mip level is not empty.
			// Allocate slab data space.
			mipl.OffsetX = new int[(numuint8_ts - 24 + 3) / 4];
			mipl.OffsetXY = (short *)(mipl.OffsetX + mipl.SizeX + 1);
			mipl.SlabData = (uint8_t *)(mipl.OffsetXY + mipl.SizeX * (mipl.SizeY + 1));

			// Load x offsets.
			for (i = 0, n = mipl.SizeX; i <= n; ++i)
			{
				// The X offsets stored in the KVX file are relative to the start of the
				// X offsets array. Make them relative to voxdata instead.
				mipl.OffsetX[i] = GetInt(rawmip + 24 + i * 4) - offsetsize;
			}

			// The first X offset must be 0 (since we subtracted offsetsize), according to the spec:
			//		NOTE: xoffset[0] = (xsiz+1)*4 + xsiz*(ysiz+1)*2 (ALWAYS)
			if (mipl.OffsetX[0] != 0)
			{
				return false;
			}
			// And the final X offset must point just past the end of the voxdata.
			if (mipl.OffsetX[mipl.SizeX] != voxdatasize)
			{
				break;
			}

			// Load xy offsets.
			i = 24 + i * 4;
			for (j = 0, n *= mipl.SizeY + 1; j < n; ++j)
			{
				mipl.OffsetXY[j] = GetShort(rawmip + i + j * 2);
			}

			// Ensure all offsets are within bounds.
			for (i = 0; i < (int)mipl.SizeX; ++i)
			{
				int xoff = mipl.OffsetX[i];
				for (j = 0; j < (int)mipl.SizeY; ++j)
				{
					int yoff = mipl.OffsetXY[(mipl.SizeY + 1) * i + j];
					if (unsigned(xoff + yoff) > unsigned(voxdatasize))
					{
						return false;
					}
				}
			}

			// Record slab location for the end.
			slabs[mip] = (kvxslab_t *)(rawmip + 24 + offsetsize);
		}

		// Time for the next mip Level.
		rawmip += numuint8_ts;
		maxmipsize -= numuint8_ts + 4;
	}
	// Did we get any mip levels, and if so, does the last one leave just
	// enough room for the palette after it?
	if (mip == 0 || rawmip != rawvoxel + voxelsize - 768)
	{
		return false;
	}
	return true;
#endif
}


// formats are listed in descending order of detection reliability, not grouped by types!
static Detector detectors[] =
{
	// format which have an identifier at the beginning of the file
	{ isPng,{ FT_PNG, ".PNG" } },
	{ isJpg,{ FT_JPG, ".JPG" }},
	{ isMid, { FT_MID, ".MID" }},
	{ isOgg, { FT_OGGBASE, ".OGG" }},
	{ isMus, { FT_MUS, ".MUS" }},
	{ isFlac,{ FT_FLAC, ".FLAC" }},
	{ isAiff, { FT_AIFF, ".AIF" }},
	{ isAifc, { FT_AIFF, ".AIFC" }},
	{ isIt,{ FT_AIFF, ".IT" } },
	{ isSpc,{ FT_SPC, ".SPC" } },
	{ isVoc,{ FT_VOC, ".VOC" }},
	{ isWav,{ FT_WAV, ".WAV" } },
	{ isImgz,{ FT_IMGZ ".IMGZ" } },
	{ isHmi, { FT_HMI, ".HMI" }},
	{ isHmp,{ FT_HMP, ".HMP" } },
	{ isXmi,{ FT_XMI, ".XMI" } },
	{ isBmf,{ FT_BMF, ".BMF" } },
	{ isFon1,{ FT_FON1, ".FON1" } },
	{ isFon2,{ FT_FON2, ".FON2" } },
	
	// formats which have an identifier near the beginning of a file
	{ isS3m, { FT_S3M, ".S3M" }},
	{ isXt, { FT_XM, ".XM" }},

	// format which can only be detected by checking the header (MOD has an identifier in the middle, but too far from the start to be considered safe.)
	{ isTGA,{ FT_TGA, ".TGA" } },
	{ isPCX,{ FT_PCX, ".PCX" } },
	{ isDDS,{ FT_DDS, ".DDS" } },
	{ isMod,{ FT_MOD, ".MOD" } },
	{ isPatch,{ FT_DOOMGFX, ".GFX" } },

	// formats which cannot easily be detected reliably
	{ isKVX, { FT_KVX, ".KVX" }},
	{ isDoomSound, { FT_DOOMSND, ".DMX" }},
	{ isMP3,{ FT_MP3BASE, ".MP3" } },
};


FileType resolveUnknown(FileType ft, const char *name, int length)
{
	switch (ft.type)
	{
	case FT_OGGBASE:
	case FT_MP3BASE:
		// todo: Try to be a bit smarter about deciding whether this is a music or sound lump.
		if (!strnicmp(name, "DS", 2)) ft.type |= FG_SND;
		else if (!strnicmp(name, "D_", 2)) ft.type |= FG_MUS;
		else if (length < 400000) ft.type |= FG_SND;
		else ft.type |= FG_MUS;
		break;
	}
	return ft;
}

FileType IdentifyFileType(const char *name, const uint8_t *data, int length)
{
	if (length > 4)
	{
		for (auto &t : detectors)
		{
			if (t.checker(data, length))
			{
				return resolveUnknown(t.ret, name, length);
			}
		}
	}
	if (length == 0)
	{
		return{ FT_BINARY, ".LMP" };
	}
	// Identify pure text files. If any character < 32, except \t, \n, \r is present, a binary file is assumed
	for (int i = 0; i < length; i++)
	{
		if (data[i] < 9 || data[i] == 11 || data[i] == 12 || (data[i] >= 14 && data[i] < 32))
		{
			return{ FT_BINARY, ".LMP" };
		}
	}
	return{ FT_TEXT, ".TXT" };
}


