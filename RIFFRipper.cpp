#include <cstring>

#include "RIFFRipper.h"

const char *RIFFRipper::s_name = "RIFF AVI/WAV/ANI/CDR Ripper v1.0";

const HeaderStruct RIFFRipper::s_headers[] = {
	{"RIFF", 4},
  {"RIFX", 4},
  {"", 0}
};

#pragma pack(push, 1)

struct SimpleRiffHeader {
	unsigned long RIFF;
	unsigned long chunksize;
	char format[4];
};

#pragma pack(pop)

bool RIFFRipper::checkLocation(unsigned char *pos, const HeaderStruct *header, FoundStruct *found)
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
	else
	{
		return false;  // this generated a lot of false positives
	}

	return true;
}
