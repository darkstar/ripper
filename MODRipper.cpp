#include <cstring>
#include <cstdio>
#include <cstdlib>

#include "MODRipper.h"

#define SWAP(x) ((((x) & 0x00ff) << 8) | (((x) & 0xff00) >> 8))

const char *MODRipper::s_name = "MOD Ripper v1.0";

const HeaderStruct MODRipper::s_headers[] = {
		// 4ch 31in
		{"M.K.", 4, 1080, new ModInfoStruct(4)}, 
		{"M!K!", 4, 1080, new ModInfoStruct(4)}, 
		{"FLT4", 4, 1080, new ModInfoStruct(4)}, 
		{"4CHN", 4, 1080, new ModInfoStruct(4)},
		// 6ch 31in
		{"6CHN", 4, 1080, new ModInfoStruct(6)}, 
		// 8ch 31in
		{"8CHN", 4, 1080, new ModInfoStruct(8)}, 
		{"OCTA", 4, 1080, new ModInfoStruct(8)}, 
		{"FLT8", 4, 1080, new ModInfoStruct(8, 31, true)},
		{"CD81", 4, 1080, new ModInfoStruct(4)}, 
		// others :)
		{"2CHN", 4, 1080, new ModInfoStruct(2)}, 
		{"TDZ1", 4, 1080, new ModInfoStruct(1)}, 
		{"TDZ2", 4, 1080, new ModInfoStruct(2)}, 
		{"TDZ3", 4, 1080, new ModInfoStruct(3)}, 
		{"5CHN", 4, 1080, new ModInfoStruct(5)}, 
		{"7CHN", 4, 1080, new ModInfoStruct(7)}, 
		{"9CHN", 4, 1080, new ModInfoStruct(9)}, 
		{"10CH", 4, 1080, new ModInfoStruct(10)}, 
		{"11CH", 4, 1080, new ModInfoStruct(11)}, 
		{"12CH", 4, 1080, new ModInfoStruct(12)}, 
		{"13CH", 4, 1080, new ModInfoStruct(13)}, 
		{"14CH", 4, 1080, new ModInfoStruct(14)}, 
		{"15CH", 4, 1080, new ModInfoStruct(15)}, 
		{"16CH", 4, 1080, new ModInfoStruct(16)}, 
		{"18CH", 4, 1080, new ModInfoStruct(18)}, 
		{"20CH", 4, 1080, new ModInfoStruct(20)}, 
		{"22CH", 4, 1080, new ModInfoStruct(22)}, 
		{"24CH", 4, 1080, new ModInfoStruct(24)}, 
		{"26CH", 4, 1080, new ModInfoStruct(26)}, 
		{"28CH", 4, 1080, new ModInfoStruct(28)}, 
		{"30CH", 4, 1080, new ModInfoStruct(30)}, 
		{"32CH", 4, 1080, new ModInfoStruct(32)}, 
		{"", 0}
};

#pragma pack(push, 1)
struct SampleHeader
{
	char name[22];
	unsigned short wordlength;
	unsigned char finetune;
	unsigned char volume;
	unsigned short repeatoffset;
	unsigned short repeatlength;
};

struct PatternTable
{
	unsigned char songlen;
	unsigned char songrepeat;
	unsigned char table[128];
};
#pragma pack(pop)

bool MODRipper::IsValidMODSampleHeader(unsigned char *pos, unsigned long *totalSampleSize)
{
	SampleHeader *header = (SampleHeader *)pos;

	// first we check if this sample is maybe unused, then it's valid
//  if (header->name[0] == 0)
//		return true;

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

bool MODRipper::checkLocation(unsigned char *pos, const HeaderStruct *header, FoundStruct *found)
{
	unsigned char *modstart;
	const ModInfoStruct *info;
	unsigned long sampleSize = 0;
	unsigned long patternSize;
	PatternTable *ptbl;
	unsigned char lastPattern = 0;

	// find the channel info struct describing this module format
	info = (ModInfoStruct *)(header->user_data);

	// the module ID is at file offset 1080, so we need at least these many bytes BEFORE pos
//  if (pos - info->m_magic_offset < m_start)
  if (pos - header->extra_len_before < m_start)
		return false;

//  modstart = pos - info->m_magic_offset;
  modstart = pos - header->extra_len_before;

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
	strcpy(found->extension, "MOD");
	found->startoffset = modstart;
//  found->length = (lastPattern + 1) * patternSize + sampleSize + info->m_magic_offset + 4; // the 4 is the length of the header ID
  found->length = (lastPattern + 1) * patternSize + sampleSize + header->extra_len_before + 4; // the 4 is the length of the header ID

	return true;
}
