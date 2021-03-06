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
#include <cstdlib>

#include "GlobalDefs.h"
#include "MODRipper.h"

#define SWAP(x) ((((x) & 0x00ff) << 8) | (((x) & 0xff00) >> 8))

const char *MODRipper::s_name = "MOD/669 Ripper v1.1";

const HeaderStruct MODRipper::s_headers[] = {
	// 4ch 31in
	HS_F("M.K.", 4, 1080, new ModInfoStruct(MT_MOD, 4))
	HS_F("M!K!", 4, 1080, new ModInfoStruct(MT_MOD, 4))
	HS_F("FLT4", 4, 1080, new ModInfoStruct(MT_MOD, 4))
	HS_F("4CHN", 4, 1080, new ModInfoStruct(MT_MOD, 4))
	// 6ch 31in
	HS_F("6CHN", 4, 1080, new ModInfoStruct(MT_MOD, 6))
	// 8ch 31in
	HS_F("8CHN", 4, 1080, new ModInfoStruct(MT_MOD, 8))
	HS_F("OCTA", 4, 1080, new ModInfoStruct(MT_MOD, 8))
	HS_F("FLT8", 4, 1080, new ModInfoStruct(MT_MOD, 8, 31, true))
	HS_F("CD81", 4, 1080, new ModInfoStruct(MT_MOD, 4))
	// others :)
	HS_F("2CHN", 4, 1080, new ModInfoStruct(MT_MOD, 2))
	HS_F("TDZ1", 4, 1080, new ModInfoStruct(MT_MOD, 1))
	HS_F("TDZ2", 4, 1080, new ModInfoStruct(MT_MOD, 2))
	HS_F("TDZ3", 4, 1080, new ModInfoStruct(MT_MOD, 3))
	HS_F("5CHN", 4, 1080, new ModInfoStruct(MT_MOD, 5))
	HS_F("7CHN", 4, 1080, new ModInfoStruct(MT_MOD, 7))
	HS_F("9CHN", 4, 1080, new ModInfoStruct(MT_MOD, 9))
	HS_F("10CH", 4, 1080, new ModInfoStruct(MT_MOD, 10))
	HS_F("11CH", 4, 1080, new ModInfoStruct(MT_MOD, 11))
	HS_F("12CH", 4, 1080, new ModInfoStruct(MT_MOD, 12))
	HS_F("13CH", 4, 1080, new ModInfoStruct(MT_MOD, 13))
	HS_F("14CH", 4, 1080, new ModInfoStruct(MT_MOD, 14))
	HS_F("15CH", 4, 1080, new ModInfoStruct(MT_MOD, 15))
	HS_F("16CH", 4, 1080, new ModInfoStruct(MT_MOD, 16))
	HS_F("18CH", 4, 1080, new ModInfoStruct(MT_MOD, 18))
	HS_F("20CH", 4, 1080, new ModInfoStruct(MT_MOD, 20))
	HS_F("22CH", 4, 1080, new ModInfoStruct(MT_MOD, 22))
	HS_F("24CH", 4, 1080, new ModInfoStruct(MT_MOD, 24))
	HS_F("26CH", 4, 1080, new ModInfoStruct(MT_MOD, 26))
	HS_F("28CH", 4, 1080, new ModInfoStruct(MT_MOD, 28))
	HS_F("30CH", 4, 1080, new ModInfoStruct(MT_MOD, 30))
	HS_F("32CH", 4, 1080, new ModInfoStruct(MT_MOD, 32))
	// non-mod file formats
	HS_F("if", 2, 0, new ModInfoStruct(MT_669))    // 669 files have embedded channel info
	HS_F("JN", 2, 0, new ModInfoStruct(MT_669))
	HS_END
};

#pragma pack(push, 1)
struct SampleHeader
{
	char name[22];
	uint16 wordlength;
	uint8 finetune;
	uint8 volume;
	uint16 repeatoffset;
	uint16 repeatlength;
};
struct SampleHeader669
{
	char name[13];
	uint32 length;
	uint32 loopStart;
	uint32 loopEnd;
};

struct PatternTable
{
	uint8 songlen;
	uint8 songrepeat;
	uint8 table[128];
};
#pragma pack(pop)

bool MODRipper::checkCompileAssertions()
{
	if (sizeof(SampleHeader) != 30)
		return false;

	if (sizeof(SampleHeader669) != 25)
		return false;
	
	if (sizeof(PatternTable) != 130)
		return false;

	return true;
}

bool MODRipper::IsValidMODSampleHeader(unsigned char *pos, unsigned long *totalSampleSize)
{
	SampleHeader *header = (SampleHeader *)pos;

	// check to see if the file name is valid (i.e. ASCII >= 32)
	for (int i = 0; i < 22; i++)
	{
		if (header->name[i] == 0)
			break; // 0-terminated: pass
		if (header->name[i] < 32)
			return false; // invalid ASCII char: bail out
	}
	// then we check the finetune (only lower nibble is used!)
	if ((header->finetune & 0xf0) != 0)
		return false;
	// then we check the linear volume
	if (header->volume > 64)
		return false;
	// then we check repeat offset and -length (but only if repeat is enabled)
	if (SWAP(header->repeatlength) > 2)
	{
		if (SWAP(header->repeatoffset) + SWAP(header->repeatlength) > SWAP(header->wordlength))
			return false;
	}

	// seems this sample is valid...
	*totalSampleSize += 2 * SWAP(header->wordlength);

	return true;
}

bool MODRipper::IsValid669SampleHeader(unsigned char *pos, unsigned long *totalSampleSize)
{
	SampleHeader669 *header = (SampleHeader669 *)pos;
	// check to see if the file name is valid (i.e. ASCII >= 32)
	for (int i = 0; i < 13; i++)
	{
		if (header->name[i] == 0)
			break; // 0-terminated: pass
		if (header->name[i] < 32)
			return false; // invalid ASCII char: bail out
	}

	// check loop offsets
	if (header->loopStart > 0)
	{
		if (header->loopStart > header->loopEnd)
			return false;
		if (header->loopStart > header->length)
			return false;
		if (header->loopEnd > header->length)
			return false;
	}

	*totalSampleSize += header->length;

	return true;
}

bool MODRipper::checkLocation(unsigned char *pos, const HeaderStruct *header, FoundStruct *found)
{
	unsigned char *modstart;
	const ModInfoStruct *info;
	unsigned long sampleSize = 0;
	unsigned long patternSize;
	PatternTable *ptbl;
	unsigned char lastPattern = 0;
	int numSamples, numPatterns;

	// find the channel info struct describing this module format
	info = (ModInfoStruct *)(header->user_data);

	// the module ID is at file offset 1080, so we need at least these many bytes BEFORE pos
	if (pos - header->extra_len_before < m_start)
		return false;

	modstart = pos - header->extra_len_before;

	if (info->m_modType == MT_MOD)
	{
		// check module file name for valid ASCII chars
		for (int i = 0; i < 20; i++)
		{
			if (modstart[i] == 0)
				break;
			if (modstart[i] < 32)
				return false;
		}

		// check sample names
		for (int i = 0; i < info->m_instruments; i++)
		{
			if (!IsValidMODSampleHeader(modstart + 20 + i * sizeof(SampleHeader), &sampleSize))
				return false;
		}
		// sample names seem to be OK, scan pattern table
		ptbl = (PatternTable *)(pos - sizeof(PatternTable));

		if (ptbl->songlen > 128)
			return false; // more than 128 patterns cannot be there

		for (int i = 0; i < ptbl->songlen; i++)
		{
			if (ptbl->table[i] > lastPattern)
			{
				lastPattern = ptbl->table[i];
			}
		}
		if (lastPattern > 128)
			return false; // again, more than 128 patterns is a no-no

		patternSize = info->m_channels * 4 * 64; // n channels * 4 bytes/channel,line, * 64lines/pattern

		// there still seems to be an error lurking somewhere here... some MOD files
		// are detected a few bytes (<16 noticed so far) too short. dunno why. both were
		// 6CHN files from Terminal Velocity
		found->criterium = CRIT_STRONG;
		strcpy(found->extension, "mod");
		found->startoffset = modstart;
		found->length = (lastPattern + 1) * patternSize + sampleSize + header->extra_len_before + 4; // the 4 is the length of the header ID

		return true;
	}
	else if (info->m_modType == MT_669)
	{
		found->criterium = CRIT_STRONG;

		if (modstart + 0x1f0 >= m_start + m_length)
			return false;

		// check module message for valid ASCII chars
		for (int i = 2; i < 110; i++)
		{
			if (modstart[i] == 0)
				break;
			if (modstart[i] < 32)
				found->criterium = CRIT_WEAK;  // some 669's do exactly that. ouch.
		}

		// number of samples
		numSamples = modstart[0x6e];
		numPatterns = modstart[0x6f];

		if ((numSamples == 0) || (numPatterns == 0) || (numPatterns > 0x80))
			return false;

		for (int i = 0; i < numSamples; i++)
		{
			if (!IsValid669SampleHeader(modstart + 0x1f1 + i * 0x19, &sampleSize))
				return false;
		}

		// another check to reduce false positives: the total sample data should be >0 bytes
		// TODO: probably it should even be >512 bytes for example, see if that makes a difference
		if (sampleSize == 0)
			return false;

		if (modstart + 0x1f1 + (numSamples * 0x19) + (numPatterns * 0x600) + sampleSize > m_start + m_length)
			return false;

		strcpy(found->extension, "669");
		found->startoffset = modstart;
		found->length = 0x1f1 + (numSamples * 0x19) + (numPatterns * 0x600) + sampleSize;

		return true;
	}

	return false;
}
