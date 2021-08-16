
#include <stdint.h>

#ifdef _MSC_VER

#include <direct.h>

#else // !_MSC_VER

#include <unistd.h>
#include <sys/stat.h>

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

static int mkdir(const char* path)
{
	return mkdir(path, 0755);
}

#endif // !_MSC_VER

enum ExtractOpt
{
	NO_CONVERT_GFX=1,
	DO_HERETIC_PAL=2,
	DO_HEXEN_PAL=4,
	DO_STRIFE=8,
	DO_STRIP=16,	// strip map lumps ZDoom does not need (nodes, blockmap, reject)
	NO_CONVERT_SND = 32,
};





//--------------------------------------------------------------------------
//
// Texture definition
//
//--------------------------------------------------------------------------

//
// Each texture is composed of one or more patches, with patches being lumps
// stored in the WAD. The lumps are referenced by number, and patched into
// the rectangular texture space using origin and possibly other attributes.
//
typedef struct
{
	short	originx;
	short	originy;
	short	patch;
	short	stepdir;
	short	colormap;
} mappatch_t;

//
// A wall texture is a list of patches which are to be combined in a
// predefined order.
//
typedef struct
{
	char		name[8];
	uint16_t	Flags;				// [RH] Was unused
	uint8_t		ScaleX;				// [RH] Scaling (8 is normal)
	uint8_t		ScaleY;				// [RH] Same as above
	short		width;
	short		height;
	uint8_t		columndirectory[4];	// OBSOLETE
	short		patchcount;
	mappatch_t	patches[1];
} maptexture_t;

#define MAPTEXF_WORLDPANNING	0x8000

// [RH] Just for documentation purposes, here's what I think the
// Strife versions of the above two structures are:

typedef struct
{
	short	originx;
	short	originy;
	short	patch;
} strifemappatch_t;

//
// A wall texture is a list of patches which are to be combined in a
// predefined order.
//
typedef struct
{
	char		name[8];
	uint16_t		Flags;				// [RH] Was nused
	uint8_t		ScaleX;				// [RH] Scaling (8 is normal)
	uint8_t		ScaleY;				// [RH] Same as above
	short		width;
	short		height;
	short		patchcount;
	strifemappatch_t	patches[1];
} strifemaptexture_t;

