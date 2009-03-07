#include <cstring>

#include "ICORipper.h"

const char *ICORipper::s_name = "Windows ICO Ripper v1.0";

const HeaderStruct ICORipper::s_headers[] = {
	{"\0\0\1\0", 4},
  {"", 0}
};

#pragma pack(push, 1)
struct GlobalHeader
{
	unsigned long id; // \0\0\1\0
	unsigned short count; // number of icons in this file
};

struct EntryHeader
{
	unsigned char width;
	unsigned char height;
	unsigned char colors;
	unsigned char reserved;
	unsigned short planes;
	unsigned short bitcount;
	unsigned long size;
	unsigned long offset;
};

struct LocalHeader
{
	unsigned long size;
	unsigned long width;
	unsigned long height;
	unsigned short planes;
	unsigned short bitcount;
	unsigned long compression;
	unsigned long imagesize;
	unsigned long xdpi;
	unsigned long ydpi;
	unsigned long colorsused;
	unsigned long colorsimportant;
};
#pragma pack(pop)

// to check if we're still inside the file limits
#define CHECK_POS(x) \
	if (x > m_start + m_length) return false

bool ICORipper::checkLocation(unsigned char *pos, const HeaderStruct *header, FoundStruct *found)
{
	int i;
	unsigned long filesize = 0;
	unsigned char *curPos = pos + sizeof(GlobalHeader);
	GlobalHeader *gheader = (GlobalHeader *)pos;
	EntryHeader *eheader;
	LocalHeader *lheader;
	
	if (gheader->count == 0)
		return false;

	for (i = 0; i < gheader->count; i++)
	{
		CHECK_POS(curPos);
		// check each entry
		eheader = (EntryHeader *)curPos;
		curPos += 16;
		// first check if the width is valid
		if ((eheader->width != 16) && (eheader->width != 32) && (eheader->width != 48) && (eheader->width != 64))
			return false;
		// then check the height
		if ((eheader->height != 16) && (eheader->height != 32) && (eheader->height != 48) && (eheader->height != 64))
			return false;
/*		// then check colorcount (WEAK???)
		if ((eheader->colors != 2) && (eheader->colors != 16) && (eheader->colors != 256))
			return false;*/
		// check reserved and planes
		if ((eheader->reserved != 0))
			return false;
		if ((eheader->planes != 1) && (eheader->planes != 0))
			return false;
		if ((eheader->bitcount != 1) && (eheader->bitcount != 4) && (eheader->bitcount != 8) &&
			(eheader->bitcount != 15) && (eheader->bitcount != 16) && (eheader->bitcount != 24) &&
			(eheader->bitcount != 32) && (eheader->bitcount != 0))
			return false;
		// check if icon data is inside our file
		CHECK_POS(pos + eheader->size + eheader->offset);

		// check local header
		lheader = (LocalHeader *)(pos + eheader->offset);
		if (lheader->size != 40)
			return false;
		if ((lheader->width != 16) && (lheader->width != 32) && (lheader->width != 48) && (lheader->width != 64))
			return false;
		if ((lheader->height != 16) && (lheader->height != 32) && (lheader->height != 48) && (lheader->height != 64))
			return false;
		if (lheader->planes != 1)
			return false;
		if ((lheader->bitcount != 1) && (lheader->bitcount != 4) && (lheader->bitcount != 8) &&
			(lheader->bitcount != 15) && (lheader->bitcount != 16) && (lheader->bitcount != 24) &&
			(lheader->bitcount != 32))
			return false;
		if ((lheader->compression != 0) || (lheader->xdpi != 0) ||
			(lheader->ydpi != 0) || (lheader->colorsused != 0) || (lheader->colorsimportant != 0))
			return false;

		filesize = eheader->offset + eheader->size;
	}

	// here we're pretty confident that we have an ICO file

	found->startoffset = pos;
	found->length = filesize;
	strcpy(found->extension, "ICO");
	found->criterium = CRIT_STRONG;
	return true;
}
