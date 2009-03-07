#include <cstring>
#include <cstdio>
#include <ctype.h>

#include "MIDIRipper.h"

#define SWAP_BYTES(a) \
	((((a)>>24)&0xFF) | (((a)>>8)&0xFF00) | (((a)<<8)&0xFF0000) | (((a)<<24)&0xFF000000)) 

const char *MIDIRipper::s_name = "MIDI Ripper v1.0";

const HeaderStruct MIDIRipper::s_headers[] = {
		{"MThd", 4},
    {"", 0}
};

#pragma pack(push, 1)

#pragma pack(pop)

unsigned long MIDIRipper::IsValidChunk(unsigned char *pos)
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

	// treat the next 4 bytes as unsigned long and see if they stil point inside the file
	chunksize = SWAP_BYTES(*((unsigned long *)(pos + 4)));
	if (pos + 8 + chunksize > m_start + m_length)
		return ~0;

	return chunksize + 8;
}

bool MIDIRipper::checkLocation(unsigned char *pos, const HeaderStruct *header, FoundStruct *found)
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
	if (format > 2)
		found->criterium = CRIT_WEAK;
	else
		found->criterium = CRIT_STRONG;

	chunksize = IsValidChunk(pos);
	while (chunksize != ~0)
	{
		filesize += chunksize;
		chunksize = IsValidChunk(pos + filesize);
	}

	found->startoffset = pos;
	strcpy(found->extension, "MID");
	found->length = filesize;

	return true;
}
