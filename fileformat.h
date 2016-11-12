#include <stdint.h>

enum
{
	FG_UNKNOWN = 0,
	FG_GFX = 0x10000,
	FG_SND = 0x20000,
	FG_MUS = 0x30000,
	FG_VOX = 0x40000,
	FG_FONT = 0x50000,
	FG_OTHER = 0x60000,
	FG_MASK = 0xff0000,
};

enum
{
	FT_MP3BASE = 1,
	FT_OGGBASE = 2,

	FT_DOOMGFX = FG_GFX + 1,
	FT_PNG = FG_GFX + 2,
	FT_JPG = FG_GFX + 3,
	FT_PCX = FG_GFX + 4,
	FT_TGA = FG_GFX + 5,
	FT_DDS = FG_GFX + 6,
	FT_IMGZ = FG_GFX + 7,

	FT_MP3 = FG_SND + FT_MP3BASE,
	FT_OGG = FG_SND + FT_OGGBASE,
	FT_DOOMSND = FG_SND + 3,
	FT_WAV = FG_SND + 4,
	FT_VOC = FG_SND + 5,
	FT_AIFF = FG_SND + 6,
	FT_FLAC = FG_SND + 7,

	FT_MP3MUS = FG_MUS + FT_MP3BASE,
	FT_OGGMUS = FG_MUS + FT_OGGBASE,
	FT_MUS = FG_MUS + 3,
	FT_MID = FG_MUS + 4,
	FT_XMI = FG_MUS + 5,
	FT_HMP = FG_MUS + 6,
	FT_HMI = FG_MUS + 7,
	FT_SPC = FG_MUS + 8,
	FT_MOD = FG_MUS + 9,
	FT_IT = FG_MUS + 10,
	FT_XM = FG_MUS + 11,
	FT_S3M = FG_MUS + 12,
	// Todo: add more specialized formats if needed

	FT_KVX = FG_VOX + 1,

	FT_FON1 = FG_FONT + 1,
	FT_FON2 = FG_FONT + 2,
	FT_BMF = FG_FONT + 3,

	FT_TEXT = FG_OTHER + 1,
	FT_BINARY = FG_OTHER + 2,
	FT_LEVEL = FG_OTHER + 3,
	FT_MARKER = FG_OTHER + 4,
};

struct FileType
{
	int type;
	const char *extension;
};

// various file format headers for type detection.

// posts are runs of non masked source pixels
struct column_t
{
	uint8_t		topdelta;		// -1 is the last post in a column
	uint8_t		length; 		// length data bytes follows
};

struct patch_t
{
	uint16_t		width;			// bounding box size 
	uint16_t		height;
	int16_t			leftoffset; 	// pixels to the left of origin 
	int16_t			topoffset;		// pixels below the origin 
	uint32_t 		columnofs[1];	// only [width] used
									// the [0] is &columnofs[width] 
};

#pragma pack(1)

struct TGAHeader
{
	uint8_t		id_len;
	uint8_t		has_cm;
	uint8_t		img_type;
	int16_t		cm_first;
	int16_t		cm_length;
	uint8_t		cm_size;

	int16_t		x_origin;
	int16_t		y_origin;
	int16_t		width;
	int16_t		height;
	uint8_t		bpp;
	uint8_t		img_desc;
};

struct PCXHeader
{
	uint8_t manufacturer;
	uint8_t version;
	uint8_t encoding;
	uint8_t bitsPerPixel;

	uint16_t xmin, ymin;
	uint16_t xmax, ymax;
	uint16_t horzRes, vertRes;

	uint8_t palette[48];
	uint8_t reserved;
	uint8_t numColorPlanes;

	uint16_t uint8_tsPerScanLine;
	uint16_t paletteType;
	uint16_t horzSize, vertSize;

	uint8_t padding[54];

};

#ifndef __BIG_ENDIAN__
#define MAKE_ID(a,b,c,d)	((uint32_t)((a)|((b)<<8)|((c)<<16)|((d)<<24)))
#else
#define MAKE_ID(a,b,c,d)	((uint32_t)((d)|((c)<<8)|((b)<<16)|((a)<<24)))
#endif

#pragma pack()

#define ID_DDS						MAKE_ID('D','D','S',' ')
#define ID_DXT1						MAKE_ID('D','X','T','1')
#define ID_DXT2						MAKE_ID('D','X','T','2')
#define ID_DXT3						MAKE_ID('D','X','T','3')
#define ID_DXT4						MAKE_ID('D','X','T','4')
#define ID_DXT5						MAKE_ID('D','X','T','5')

// Bits in dwFlags
#define DDSD_CAPS					0x00000001
#define DDSD_HEIGHT					0x00000002
#define DDSD_WIDTH					0x00000004
#define DDSD_PITCH					0x00000008
#define DDSD_PIXELFORMAT			0x00001000
#define DDSD_MIPMAPCOUNT			0x00020000
#define DDSD_LINEARSIZE				0x00080000
#define DDSD_DEPTH					0x00800000

// Bits in ddpfPixelFormat
#define DDPF_ALPHAPIXELS			0x00000001
#define DDPF_FOURCC					0x00000004
#define DDPF_RGB					0x00000040

// Bits in DDSCAPS2.dwCaps1
#define DDSCAPS_COMPLEX				0x00000008
#define DDSCAPS_TEXTURE				0x00001000
#define DDSCAPS_MIPMAP				0x00400000

// Bits in DDSCAPS2.dwCaps2
#define DDSCAPS2_CUBEMAP			0x00000200
#define DDSCAPS2_CUBEMAP_POSITIVEX	0x00000400
#define DDSCAPS2_CUBEMAP_NEGATIVEX	0x00000800
#define DDSCAPS2_CUBEMAP_POSITIVEY	0x00001000
#define DDSCAPS2_CUBEMAP_NEGATIVEY	0x00002000
#define DDSCAPS2_CUBEMAP_POSITIVEZ	0x00004000
#define DDSCAPS2_CUBEMAP_NEGATIZEZ	0x00008000
#define DDSCAPS2_VOLUME				0x00200000

struct DDPIXELFORMAT
{
	uint32_t			Size;		// Must be 32
	uint32_t			Flags;
	uint32_t			FourCC;
	uint32_t			RGBBitCount;
	uint32_t			RBitMask, GBitMask, BBitMask;
	uint32_t			RGBAlphaBitMask;
};

struct DDCAPS2
{
	uint32_t			Caps1, Caps2;
	uint32_t			Reserved[2];
};

struct DDSURFACEDESC2
{
	uint32_t		Size;		// Must be 124. DevIL claims some writers set it to 'DDS ' instead.
	uint32_t		Flags;
	uint32_t		Height;
	uint32_t		Width;
	union
	{
		int32_t		Pitch;
		uint32_t	LinearSize;
	};
	uint32_t		Depth;
	uint32_t		MipMapCount;
	uint32_t		Reserved1[11];
	DDPIXELFORMAT	PixelFormat;
	DDCAPS2			Caps;
	uint32_t		Reserved2;
};

struct DDSFileHeader
{
	uint32_t		Magic;
	DDSURFACEDESC2	Desc;
};

#define MAXVOXMIPS 5

struct kvxslab_t
{
	uint8_t		ztop;			// starting z coordinate of top of slab
	uint8_t		zleng;			// # of uint8_ts in the color array - slab height
	uint8_t		backfacecull;	// low 6 bits tell which of 6 faces are exposed
	uint8_t		col[1/*zleng*/];// color data from top to bottom
};

struct FVoxelMipLevel
{
	unsigned int			SizeX;
	unsigned int			SizeY;
	unsigned int			SizeZ;
	int			*OffsetX;
	short		*OffsetXY;
	uint8_t		*SlabData;
};

inline int GetInt(const unsigned char *foo)
{
	return *(const int *)foo;
}

inline int GetShort(const unsigned char *foo)
{
	return *(const short *)foo;
}

FileType IdentifyFileType(const char *name, const uint8_t *data, int length);

bool FlatToPng(int options, const uint8_t *data, int length, const char *pngpath);
bool PatchToPng(int options, const uint8_t *ldata, int length, const char *pngpath);
bool DoomSndToWav(const uint8_t *data, int length, const char *filename);
