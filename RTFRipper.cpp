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
#include "RTFRipper.h"

const char *RTFRipper::s_name = "RTF Ripper v1.0";

const HeaderStruct RTFRipper::s_headers[] = {
	HS("{\\rtf1\\", 7)
	HS_END
};

bool RTFRipper::checkLocation(unsigned char *pos, const HeaderStruct * /*header*/, FoundStruct *found)
{
	bool ansi = false;
	int numbraces = 0;

	found->criterium = CRIT_STRONG;
	strcpy(found->extension, "rtf");
	found->startoffset = pos;

	if (strncmp((char *)pos + 7, "ansi\\", 5) == 0)
	{
		ansi = true;
	}

	do
	{
		if (pos > m_start + m_length)
			return false;

		if (*pos == '{')
			numbraces++;

		if (*pos == '}')
			numbraces--;

		if (ansi && (*pos > 0x7f))
			found->criterium = CRIT_WEAK;

		pos++;
	} while (numbraces > 0);

	found->length = pos - found->startoffset;
	return true;
}
