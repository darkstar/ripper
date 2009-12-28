/***********************************************************************
* Copyright 2007 Michael Drueing <michael@drueing.de>
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License as
* published by the Free Software Foundation; either version 2 of
* the License or (at your option) version 3 or any later version
* accepted by the membership of KDE e.V. (or its successor approved
* by the membership of KDE e.V.), which shall act as a proxy 
* defined in Section 14 of version 3 of the license.
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

// bitfields are not portable! 
// TODO: change to one field and use masks in the code to access the bits
struct GIFHeader
{
	char ID[6]; // the ID
	unsigned short width;
	unsigned short height;
	unsigned int bpp:3;
	unsigned int sort:1; // 89a only
	unsigned int colorres:3;
	unsigned int colormap:1;
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
	unsigned int bpp:3;
	unsigned int reserved:2;
	unsigned int sort:1;
	unsigned int interlace:1;
	unsigned int localcolormap:1;
};

#pragma pack(pop)


bool GIFRipper::checkLocation(unsigned char *pos, const HeaderStruct *header, FoundStruct *found)
{
	GIFHeader *hdr;
	GIFImageDesc *idesc;
	unsigned int version;

	// setup default values
	found->criterium = CRIT_STRONG;
	found->startoffset = pos;
	strcpy(found->extension, "GIF");

	hdr = (GIFHeader *)pos;

	pos += 13; // skip the header and screen descriptor

	version = (int)(header->user_data);

	// check header for gif89 features in a gif87 image
	if ((version == 87) && (hdr->sort == 1))
		found->criterium = CRIT_WEAK; // sort flag only available in GIF89a

	if ((version == 87) && (hdr->pixelaspect == 1))
		found->criterium = CRIT_WEAK; // pixel aspect ratio only available in GIF89a

	if ((hdr->colormap == 0) && (hdr->backcolor != 0))
		found->criterium = CRIT_WEAK; // backcolor != 0 makes no sense without global color map

	if (hdr->colormap == 1)
	{
		// skip the color map
		pos += 3 * (1 << (hdr->bpp + 1));
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

			if ((idesc->sort == 1) && (version = 87))
				found->criterium = CRIT_WEAK;

			// maybe this should be CRIT_WEAK instead of failure?
			if ((idesc->width > hdr->width) || (idesc->height > hdr->height) || (idesc->left + idesc->width > hdr->width) || (idesc->top + idesc->height > hdr->height))
				return false;

			pos += 10; // size of image descriptor

			// skip LOCAL COLOR TABLE
			if (idesc->localcolormap == 1)
			{
				// skip local color map
				pos += 3 * (1 << (idesc->bpp + 1));
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
