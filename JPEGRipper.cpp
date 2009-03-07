#include <cstring>
#include "JPEGRipper.h"

const char *JPEGRipper::s_name = "JFIF/JPG Ripper v1.0beta";

const HeaderStruct JPEGRipper::s_headers[] = {
	{"\xff\xd8\xff\xe0", 4},
  {"", 0}
};

// this needs to be more robust. First we should detect non-JFIF JPEG files (without 0xff 0xe0 marker),
// and we should detect the EOI marker more robust (i.e. it might appear inside the compressed stream)
bool JPEGRipper::checkLocation(unsigned char *pos, const HeaderStruct *header, FoundStruct *found)
{
	static const char ID[] = "JFIF";

	if (strncmp((char *)pos + 6, ID, 4) != 0)
		return false;

	found->criterium = CRIT_STRONG;
	strcpy(found->extension, "JPG");
	found->startoffset = pos;

	pos += 10;
	// find EOI marker 0xff 0xd9
	while (pos++ < m_start + m_length)
	{
		if ((*pos == 0xff) && (*(pos+1) == 0xd9))
			break;
	}
	if (pos >= m_start + m_length)
		return false;

	found->length = pos + 2 - found->startoffset;
	return true;
}
