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
#include <cstdio>
#include <locale>
#include "PDFRipper.h"

const char *PDFRipper::s_name = "PDF Ripper v1.0";

const HeaderStruct PDFRipper::s_headers[] = {
	{"%PDF-1.", 7},
	{"", 0}
};

bool PDFRipper::checkLocation(unsigned char *pos, const HeaderStruct *header, FoundStruct *found)
{
	char version;
	unsigned char *scan;
	unsigned char *scan2;
	bool eof_found = false;
	char buffer[20] = { 0 };

	found->startoffset = pos;
	found->criterium = CRIT_STRONG;
	strcpy(found->extension, "PDF");

	// get the version: "%PDF-1.x" ==> x
	version = *((char *)pos + 7);

	if ((version < '1') || (version > '6'))
	{
		fprintf(stderr, "PDFRipper: Unsupported PDF version 1.%c. PLEASE REPORT THIS!\n", version);
		return false;
	}

	if (version == '6')
	{
		// PDF 1.6 seems to have additional data, i.e. data is bigger than "/L <numbytes>" suggests
		fprintf(stderr, "PDFRipper: Warning: PDF v1.6 only partially supported, file may be truncated\n");
		found->criterium = CRIT_WEAK;
	}

	// first check to see if we find a "/Linearized" somewhere...
	for (scan = pos + 8; scan < pos + 1024; scan++)
	{
		if (scan > m_start + m_length)
			return false;

		if (strncmp((char *)scan, "/Linearized", 11) == 0)
			break;
	}
	if (scan >= pos + 1024)
	{
		// we didn't find a "/Linearized", so we just extract until %%EOF{\x0d{\x0a}}
		do
		{
			pos++;
			if (strncmp((char *)pos, "%%EOF", 5) == 0)
			{
				eof_found = true;
				break;
			}
		} while (pos < m_start + m_length);

		if (!eof_found)
			return false;

		pos +=5; // skip the "%%EOF"

		// append trailing \\r\\n if available
		if (*pos == 0x0d)
			pos++;

		if (*pos == 0x0a)
			pos++;
		
		found->length = pos - found->startoffset;
		return true;
	}
	else
	{
		// we found a "/Linearized" header at scan. now use "pos" to find the "/L" string
		for (scan2 = scan; scan2 < scan + 1024; scan2++)
		{
			if (scan2 > m_start + m_length)
				return false;

			if (strncmp((char *)scan2, "/L ", 3) == 0)
			{
				scan2 += 3;
				// copy the numbers out to a buffer
				while ((scan2 <= m_start + m_length) && (isdigit(*scan2)))
					buffer[strlen(buffer)] = *scan2++;

				found->length = atol(buffer);
				return true;
			}
		}
	}
	
	return false;
}
