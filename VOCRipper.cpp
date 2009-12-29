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

#include <cstring>
#include "VOCRipper.h"

const char *VOCRipper::s_name = "Creative VOC Ripper v1.0";

const HeaderStruct VOCRipper::s_headers[] = {
	HS("Creative Voice File\x1a", 20)
	HS_END
};

#pragma pack(push, 1)

struct VOCHeader
{
	char ID[20]; // the ID
	unsigned short dataOffset;
	unsigned short ver;
	unsigned short ver2;
};

#pragma pack(pop)

bool VOCRipper::checkLocation(unsigned char *pos, const HeaderStruct * /*header*/, FoundStruct *found)
{
	VOCHeader *hdr = (VOCHeader *)pos;
	unsigned int blockLength;
	unsigned char *pos2;
	unsigned char blockType;

	// check simple checksum
	if ((~hdr->ver) + 0x1234 != hdr->ver2)
		return false;

	// scan through all data blocks until we find the terminator block
	pos2 = pos + hdr->dataOffset;

	blockType = *pos2;
	while ((blockType != 0) && (pos2 + 2 < m_start + m_length))
	{
		pos2++;
		// first: read 3 bytes blocklength and skip those
		blockLength = (unsigned int)(pos2[0]) | (unsigned int)(pos2[1] << 8) | (unsigned int)(pos2[2] << 16);
		pos2 += 3;

		// skip length of block
		pos2 += blockLength;

		blockType = *pos2;
	}

	// check to see if we ran past the end of the file
	if (pos2 >= m_start + m_length)
		return false;

	found->criterium = CRIT_STRONG;
	strcpy(found->extension, "voc");
	found->startoffset = pos;
	found->length = pos2 - pos + 1;

	return true;
}
