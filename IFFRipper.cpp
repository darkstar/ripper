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

#include "IFFRipper.h"
#include <cstring>

using namespace std;

const char *IFFRipper::s_name = "Generic IFF/LBM/XMI/AIFF/8SV Ripper v1.2";

const HeaderStruct IFFRipper::s_headers[] = {
	HS("FORM", 4)
	HS_END
};

#define ISNUMBER(x) (((x) >= '0') && ((x) <= '9'))
#define ISUPPER(x) (((x) >= 'A') && ((x) <= 'Z'))
#define ISOTHER(x) ((x) == ' ')
#define ISVALID(x) (ISNUMBER(x) || ISUPPER(x) || ISOTHER(x))

bool IFFRipper::validChunkName(unsigned char *pos, unsigned char *end)
{
	if (pos + 3 >= end)
		return false;
	return ISVALID(pos[0]) && ISVALID(pos[1]) && ISVALID(pos[2]) && ISVALID(pos[3]);
};

#undef ISVALID
#undef ISOTHER
#undef ISUPPER
#undef ISNUMBER

bool IFFRipper::checkLocation(unsigned char *pos, const HeaderStruct * /*header*/, FoundStruct *found)
{
	unsigned int form_size;
	unsigned char *secondaryHeader1 = pos + 8;
	unsigned char *secondaryHeader2; // this keeps the last secondary header encountered (usually == header1 except for XMI so far)
	bool endReached = false;

	found->startoffset = pos;
	found->length = 0;

	do
	{
		// keep track of the last secondary header encountered. this is needed by XMI which have "FORM" "XDIR" followed by "CAT " "XMID" in ONE file
		secondaryHeader2 = pos + 8;
		form_size = FROM_BIG_ENDIAN(*((unsigned int *)(pos + 4)));
		// check FORM chunk size
		if (pos + form_size + 8 > m_start + m_length)
			return false;

		// arbitrary 1G limit on FORM sizes, should be replaced by better sanity checks elsewhere
		if (form_size > 1000000000)
			return false;

		// the end of this chunk
		pos += form_size + 8;
		found->length += form_size + 8;

		// we're at the end if we either have no valid chunk name or found another FORM chunk (next file)
		if ( (!validChunkName(pos, m_start + m_length)) || (strncmp((char *)pos, "FORM", 4) == 0))
		{
			endReached = true;
		}
	} while (!endReached);

	found->criterium = CRIT_STRONG;

	if (strncmp((char *)secondaryHeader1, "ILBM", 4) == 0)
	{
		strcpy(found->extension, "lbm");
	}
	else if (strncmp((char *)secondaryHeader1, "PBM ", 4) == 0)
	{
		strcpy(found->extension, "lbm");
	}
	else if (strncmp((char *)secondaryHeader1, "AIFF", 4) == 0)
	{
		strcpy(found->extension, "aiff");
	}
	else if (strncmp((char *)secondaryHeader1, "8SVX", 4) == 0)
	{
		strcpy(found->extension, "8sv");
	}
	else if ((strncmp((char *)secondaryHeader1, "XDIR", 4) == 0) && (strncmp((char *)secondaryHeader2, "XMID", 4) == 0))
	{
		strcpy(found->extension, "xmi");
	}
	else
	{
		strcpy(found->extension, "iff");
	}

	return true;
}

