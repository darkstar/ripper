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

#include "GIFRipper.h"
#include <cstring>

using namespace std;

const char *GIFRipper::s_name = "GIF 87a/89a Ripper v1.0";

const HeaderStruct GIFRipper::s_headers[] = {
	HS_F("GIF87a", 6, 0, (void *)87)
	HS_F("GIF89a", 6, 0, (void *)89)
	HS_END
};

#pragma pack(push, 1)

struct GIFHeader
{
	char ID[6]; // the ID
	unsigned short width;
	unsigned short height;
	unsigned char _flags;
	unsigned char backcolor;
	unsigned char pixelaspect; // 89a only
};

struct GIFImageDesc
{
	char ID; // must be ',' == 0x02c
	unsigned short left;
	unsigned short top;
	unsigned short width;
	unsigned short height;
	unsigned char _flags;
};
#pragma pack(pop)

#define GH_BPP(gh)	(gh->_flags & 0x07)
#define GH_SORT(gh)	((gh->_flags >> 3) & 0x01)
#define GH_COLORRES(gh)	((gh->_flags >> 4) & 0x07)
#define GH_COLORMAP(gh)	((gh->_flags >> 7) & 0x01)

#define GID_BPP(gid)		(gid->_flags & 0x07)
#define GID_SORT(gid)		((gid->_flags >> 5) & 0x01)
#define GID_INTERLACE(gid)	((gid->_flags >> 6) & 0x01)
#define GID_LOCALCOLORMAP(gid)	((gid->_flags >> 7) & 0x01)

bool GIFRipper::checkCompileAssertions()
{
	if (sizeof(GIFHeader) != 13)
		return false;

	if (sizeof(GIFImageDesc) != 10)
		return false;

	return true;
}

bool GIFRipper::checkLocation(unsigned char *pos, const HeaderStruct *header, FoundStruct *found)
{
	GIFHeader *hdr;
	GIFImageDesc *idesc;
	unsigned int version;

	// setup default values
	found->criterium = CRIT_STRONG;
	found->startoffset = pos;
	strcpy(found->extension, "gif");

	hdr = (GIFHeader *)pos;

	pos += 13; // skip the header and screen descriptor

	version = (int)(header->user_data);

	// check header for gif89 features in a gif87 image
	if ((version == 87) && (GH_SORT(hdr) == 1))
		found->criterium = CRIT_WEAK; // sort flag only available in GIF89a

	if ((version == 87) && (hdr->pixelaspect == 1))
		found->criterium = CRIT_WEAK; // pixel aspect ratio only available in GIF89a

	if ((GH_COLORMAP(hdr) == 0) && (hdr->backcolor != 0))
		found->criterium = CRIT_WEAK; // backcolor != 0 makes no sense without global color map

	if (GH_COLORMAP(hdr) == 1)
	{
		// skip the color map
		pos += 3 * (1 << (GH_BPP(hdr) + 1));
	}

	while ((*pos == ',') || (*pos == '!'))
	{
		if (*pos == '!')
		{
			// skip EXTENSION stuff

			pos += 2; // skip header and extension label
			// skip data blocks until terminator block reached
			while (*pos != 0)
			{
				pos += *pos + 1;
			}
			pos++; // skip last terminator byte
		}
		else
		{
			// skip IMAGE DESCRIPTOR
			idesc = (GIFImageDesc *)pos;

			if ((GID_SORT(idesc) == 1) && (version = 87))
				found->criterium = CRIT_WEAK;

			// maybe this should be CRIT_WEAK instead of failure?
			if ((idesc->width > hdr->width) || (idesc->height > hdr->height) || (idesc->left + idesc->width > hdr->width) || (idesc->top + idesc->height > hdr->height))
				return false;

			pos += 10; // size of image descriptor

			// skip LOCAL COLOR TABLE
			if (GID_LOCALCOLORMAP(idesc) == 1)
			{
				// skip local color map
				pos += 3 * (1 << (GID_BPP(idesc) + 1));
			}

			pos++; // skip code size

			// skip IMAGE DATA blocks
			while (*pos != 0)
			{
				pos += *pos + 1;
			}
			pos++;
		}
	}

	// see if we're at the end
	if (*pos != ';')
		return false;

	pos++; // skip final trailer

	found->length = pos - found->startoffset;
	return true;
};
