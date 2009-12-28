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
#include "JPEGRipper.h"

const char *JPEGRipper::s_name = "JFIF/JPG Ripper v1.0beta";

const HeaderStruct JPEGRipper::s_headers[] = {
	HS("\xff\xd8\xff\xe0", 4)
	HS_END
};

// this needs to be more robust. First we should detect non-JFIF JPEG files (without 0xff 0xe0 marker),
// and we should detect the EOI marker more robust (i.e. it might appear inside the compressed stream)
bool JPEGRipper::checkLocation(unsigned char *pos, const HeaderStruct * /*header*/, FoundStruct *found)
{
	static const char ID[] = "JFIF";

	if (strncmp((char *)pos + 6, ID, 4) != 0)
		return false;

	found->criterium = CRIT_STRONG;
	strcpy(found->extension, "JPG");
	found->startoffset = pos;

	pos += 10;
	// find EOI marker 0xff 0xd9
	while (pos++ < m_start + m_length)
	{
		if ((*pos == 0xff) && (*(pos+1) == 0xd9))
			break;
	}
	if (pos >= m_start + m_length)
		return false;

	found->length = pos + 2 - found->startoffset;
	return true;
}
