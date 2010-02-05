/***********************************************************************
* Copyright 2007-2010 Michael Drueing <michael@drueing.de>
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License as
* published by the Free Software Foundation; either version 2 of
* the License or (at your option) version 3 or any later version
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
***********************************************************************/

#include <cstring>

#include "GlobalDefs.h"
#include "ICORipper.h"

const char *ICORipper::s_name = "Windows ICO Ripper v1.0";

const HeaderStruct ICORipper::s_headers[] = {
	HS("\0\0\1\0", 4)
	HS_END
};

#pragma pack(push, 1)
struct GlobalHeader
{
	uint32 id; // \0\0\1\0
	uint16 count; // number of icons in this file
};

struct EntryHeader
{
	uint8 width;
	uint8 height;
	uint8 colors;
	uint8 reserved;
	uint16 planes;
	uint16 bitcount;
	uint32 size;
	uint32 offset;
};

struct LocalHeader
{
	uint32 size;
	uint32 width;
	uint32 height;
	uint16 planes;
	uint16 bitcount;
	uint32 compression;
	uint32 imagesize;
	uint32 xdpi;
	uint32 ydpi;
	uint32 colorsused;
	uint32 colorsimportant;
};
#pragma pack(pop)

// to check if we're still inside the file limits
#define CHECK_POS(x) \
	if (x > m_start + m_length) return false

bool ICORipper::checkCompileAssertions()
{
	if (sizeof(GlobalHeader) != 6)
	  	return false;

	if (sizeof(EntryHeader) != 16)
		return false;

	if (sizeof(LocalHeader) != 40)
		return false;

	return true;
}

bool ICORipper::checkLocation(unsigned char *pos, const HeaderStruct * /*header*/, FoundStruct *found)
{
	int i;
	unsigned long filesize = 0;
	unsigned char *curPos = pos + sizeof(GlobalHeader);
	GlobalHeader *gheader = (GlobalHeader *)pos;
	EntryHeader *eheader;
	LocalHeader *lheader;
	
	if (gheader->count == 0)
		return false;

	found->criterium = CRIT_STRONG;
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
			found->criterium = CRIT_WEAK;

		filesize = eheader->offset + eheader->size;
	}

	// here we're pretty confident that we have an ICO file

	found->startoffset = pos;
	found->length = filesize;
	strcpy(found->extension, "ico");
	return true;
}
