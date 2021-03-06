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
#include <cstdio>
#include <ctype.h>

#include "MIDIRipper.h"

/* TODO: Make this endian-safe! */

#define SWAP_BYTES(a) \
	((((a)>>24)&0xFF) | (((a)>>8)&0xFF00) | (((a)<<8)&0xFF0000) | (((a)<<24)&0xFF000000)) 

#define SWAP_BYTES2(a) \
	((((a)>>8)&0xff00)|(((a)<<8)&0x00ff))

const char *MIDIRipper::s_name = "MIDI Ripper v1.0";

const HeaderStruct MIDIRipper::s_headers[] = {
	HS("MThd", 4)
	HS_END
};

unsigned long MIDIRipper::IsValidChunk(unsigned char *pos, bool allowMThd)
{
	unsigned long chunksize;

	// we need at least 8 bytes, so...
	if (pos + 8 > m_start + m_length)
		return ~0;

	// check the first 4 bytes to see if they are ascii
	for (int i = 0; i < 4; i++)
	{
		if (!isalnum(pos[i]))
			return ~0;
	}

	// if the first 4 bytes spell "MThd" then we're actually at the start of a new file
	// (this happens when MIDI files are concatenated without padding inbetween)
	if (!allowMThd && (strncmp((char *)pos, "MThd", 4) == 0))
	{
		return ~0;
	}

	// treat the next 4 bytes as unsigned long and see if they stil point inside the file
	chunksize = SWAP_BYTES(*((unsigned long *)(pos + 4)));
	if (pos + 8 + chunksize > m_start + m_length)
		return ~0;

	return chunksize + 8;
}

bool MIDIRipper::checkLocation(unsigned char *pos, const HeaderStruct * /*header*/, FoundStruct *found)
{
	unsigned long filesize = 0;
	unsigned long chunksize = 0;
	unsigned short format;

	if (pos + 8 > m_start + m_length)
		return false;

	// we found an mthd chunk, now check size of this chunk
	chunksize = SWAP_BYTES(*((unsigned long *)(pos + 4)));

	// MThd chunk data must be 6 bytes 
	if (chunksize != 6)
		return false;

	// check if we have a valid MIDI format (0, 1 or 2)
	format = *((unsigned short *)(pos + 8));
	format = SWAP_BYTES2(format);
	if (format > 2)
		found->criterium = CRIT_WEAK;
	else
		found->criterium = CRIT_STRONG;

	// the first chunk is allowed to be "MThd"
	chunksize = IsValidChunk(pos, true);
	while (chunksize != (unsigned long)~0)
	{
		filesize += chunksize;
		chunksize = IsValidChunk(pos + filesize, false);
	}

	found->startoffset = pos;
	strcpy(found->extension, "mid");
	found->length = filesize;

	return true;
}
