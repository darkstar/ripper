#include <cstring>
#include "RTFRipper.h"

const char *RTFRipper::s_name = "RTF Ripper v1.0";

const HeaderStruct RTFRipper::s_headers[] = {
	{"{\\rtf1\\", 7},
  {"", 0}
};

bool RTFRipper::checkLocation(unsigned char *pos, const HeaderStruct *header, FoundStruct *found)
{
	bool ansi = false;
	int numbraces = 0;

	found->criterium = CRIT_STRONG;
	strcpy(found->extension, "RTF");
	found->startoffset = pos;

	if (strncmp((char *)pos + 7, "ansi\\", 5) == 0)
	{
		ansi = true;
	}

	do
	{
		if (pos > m_start + m_length)
			return false;

		if (*pos == '{')
			numbraces++;

		if (*pos == '}')
			numbraces--;

		if (ansi && (*pos > 0x7f))
			found->criterium = CRIT_WEAK;

		pos++;
	} while (numbraces > 0);

	found->length = pos - found->startoffset;
	return true;
}
