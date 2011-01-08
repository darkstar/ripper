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

#include "GlobalDefs.h"
#include "PNGRipper.h"

#include <cctype>
#include <cstring>

const char *PNGRipper::s_name = "PNG Ripper v1.0";

const HeaderStruct PNGRipper::s_headers[] = {
	HS("\x89PNG\x0d\x0a\x1a\x0a", 8)
	HS_END
};

#pragma pack(push, 1)
struct ChunkHeader
{
	uint32 size;
	unsigned char id[4];
	uint32 crc;
};
#pragma pack(pop)

static bool validChunkName(unsigned char *n)
{
	return isalpha(n[0]) && isalpha(n[1]) && isalpha(n[2]) && isalpha(n[3]);
}

bool PNGRipper::checkLocation(unsigned char *pos, const HeaderStruct *header, FoundStruct *found)
{
	bool ihdr_found = false;
	bool iend_found = false;
	ChunkHeader *chdr;

	// save the start offset for later
	found->startoffset = pos;

	// skip PNG header bytes
	pos += 8;
	while (pos < m_start + m_length)
	{
		// get current chunk
		chdr = (ChunkHeader *)pos;
		if (memcmp(chdr->id, "IHDR", 4) == 0)
		{
			// did we already find another IHDR chunk?
			if (ihdr_found)
				return false;

			ihdr_found = true;
		}
		if (memcmp(chdr->id, "IEND", 4) == 0)
		{
			// check that we encountered IHDR before
			if (!ihdr_found)
				return false;

			// end png processing
			iend_found = true;
			break;
		}
		// this *must* be true as IHDR must be the first header
		if (!ihdr_found)
			return false;

		// if we don't have a valid chunk name, we're also screwed
		if (!validChunkName(chdr->id))
			return false;

		// skip to next header
		pos += 12 + FROM_BIG_ENDIAN(chdr->size);
	}
	// if we neither found IHDR nor IEND then we're screwed
	if (!(ihdr_found && iend_found))
		return false;

	// skip over IEND chunk size
	pos += 12 + FROM_BIG_ENDIAN(chdr->size);

	strcpy(found->extension, "PNG");
	found->criterium = CRIT_STRONG;
	found->length = pos - found->startoffset;

	return true;
}
