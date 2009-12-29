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
#include "GlobalDefs.h"
#include "RIFFRipper.h"

const char *RIFFRipper::s_name = "RIFF AVI/WAV/ANI/CDR Ripper v1.0";

const HeaderStruct RIFFRipper::s_headers[] = {
	HS("RIFF", 4)
	HS("RIFX", 4)
	HS_END
};

#pragma pack(push, 1)

struct SimpleRiffHeader {
	uint64 RIFF;
	uint64 chunksize;
	char format[4];
};

#pragma pack(pop)

bool RIFFRipper::checkCompileAssertions()
{
	if (sizeof(SimpleRiffHeader) != 20)
		return false;

	return true;
}

bool RIFFRipper::checkLocation(unsigned char *pos, const HeaderStruct * /*header*/, FoundStruct *found)
{
	SimpleRiffHeader *hdr = (SimpleRiffHeader *)pos;

	// check chunk size
	if (pos + hdr->chunksize + 8 > m_start + m_length)
		return false;

	found->criterium = CRIT_WEAK;
	found->startoffset = pos;
	found->length = hdr->chunksize + 8;

	if (strncmp(hdr->format, "WAVE", 4) == 0)
	{
		strcpy(found->extension, "WAV");
		found->criterium = CRIT_STRONG;
	}
	else if (strncmp(hdr->format, "AVI ", 4) == 0)
	{
		strcpy(found->extension, "AVI");
		found->criterium = CRIT_STRONG;
	}
	else if (strncmp(hdr->format, "ACON", 4) == 0)
	{
		strcpy(found->extension, "ANI");
		found->criterium = CRIT_STRONG;
	}
	else if (strncmp(hdr->format, "CDRA", 4) == 0)
	{
		strcpy(found->extension, "CDR");
		found->criterium = CRIT_STRONG;
	}
	else if (strncmp(hdr->format, "PAL ", 4) == 0)
	{
		strcpy(found->extension, "PAL");
		found->criterium = CRIT_STRONG;
	}
	else if (strncmp(hdr->format, "RMID", 4) == 0)
	{
		strcpy(found->extension, "RMI");
		found->criterium = CRIT_STRONG;
	}
	else if (strncmp(hdr->format, "CDXA", 4) == 0)
	{
		strcpy(found->extension, "STR");
		found->criterium = CRIT_STRONG;
	}
	else
	{
		return false;  // this generated a lot of false positives
	}

	return true;
}
