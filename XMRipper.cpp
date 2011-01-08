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
#include "GlobalDefs.h"
#include "XMRipper.h"

const char *XMRipper::s_name = "XM module ripper v1.0";

const HeaderStruct XMRipper::s_headers[] = {
	{"Extended Module: ", 17},
	{"", 0}
};

#pragma pack(push, 1)
struct XMHeader
{
	char id[17];
	char modname[20];
	char eof;
	char trackername[20];
	uint16 trackerversion;
	uint32 headersize;
	uint16 songlen; // in patterns
	uint16 restartpos;
	uint16 channels;
	uint16 numpatterns; // (< 256)
	uint16 numinstruments; // <128
	uint16 flags;  // bit 0: linear instead of amiga freq. table
	uint16 tempo;
	uint16 bpm;
	char pattern_table[256];
};

struct XMPattern
{
	uint32 hdrlength;
	char packing;
	uint16 rows;
	uint16 datasize;
};

struct XMInstrument
{
	uint32 headersize;
	char instrname[22];
	char insttype;
	uint16 numsamples;
	uint32 sampleheader_size;
	char sample_numbers[96];
	char vol_envelope[48];
	char pan_envelope[48];
	char vol_points;
	char pan_points;
	char vol_sustain;
	char vol_loop_start;
	char vol_loop_end;
	char pan_sustain;
	char pan_loop_start;
	char pan_loop_end;
	char vol_type;
	char pan_type;
	char vibrato_type;
	char vibrato_sweep;
	char vibrato_depth;
	char vibrato_rate;
	uint16 vol_fadeout;
	uint16 vol_reserved;
};

struct XMSample
{
	uint32 length;
	uint32 loop_start;
	uint32 loop_end;
	char volume;
	char finetune;
	char type;
	char pan;
	char note;
	char reserved;
	char name[22];
};

#pragma pack(pop)

static bool valid_ascii(char *x, int len)
{
	for (int i = 0; i < len; i++)
	{
		if (x[i] == 0)
			return true;
		if (*x < 32)
			return false;
	}

	return true;
}

bool XMRipper::checkLocation(unsigned char *pos, const HeaderStruct *header, FoundStruct *found)
{
	XMHeader *hdr = (XMHeader *)pos;
	XMPattern *phdr;
	XMInstrument *ihdr;
	XMSample *shdr;
	uint32 smp_len;
	unsigned char *pos2 = pos + sizeof(XMHeader);

	if (hdr->eof != 0x1a)
		return false;

	if (!valid_ascii(hdr->modname, 20))
		return false;

	if (!valid_ascii(hdr->trackername, 20))
		return false;

	if ((hdr->numpatterns > 255) || (hdr->numinstruments > 127))
		return false;

	found->criterium = CRIT_STRONG;
	strcpy(found->extension, "XM");

	if (hdr->trackerversion != 0x0104)
		found->criterium = CRIT_WEAK;

	for (int i = 0; i < hdr->numpatterns; i++)
	{
		phdr = (XMPattern *)pos2;
		if ((phdr->packing != 0) || (phdr->hdrlength != 9))
			return false;

		pos2 += sizeof(XMPattern);
		pos2 += phdr->datasize;
	}

	for (int i = 0; i < hdr->numinstruments; i++)
	{
		ihdr = (XMInstrument *)pos2;
		pos2 += ihdr->headersize;
		smp_len = 0;
		for (int j = 0; j < ihdr->numsamples; j++)
		{
			shdr = (XMSample *)pos2;
			pos2 += ihdr->sampleheader_size;
			smp_len += shdr->length;
		}
		pos2 += smp_len;
	}

	found->startoffset = pos;
	found->length = pos2 - pos;
	
	return true;
}

bool XMRipper::checkCompileAssertions()
{
  if (sizeof(XMHeader) != 0x100 + 0x50)
    return false;

  if (sizeof(XMPattern) != 9)
    return false;

  if (sizeof(XMSample) != 0x28)
    return false;

  return true;
}

