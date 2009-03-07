#include <cstring>

#define WINDOWS_LEAN_AND_MEAN
#include <windows.h>

#include "BMPRipper.h"

const char *BMPRipper::s_name = "BMP Ripper v1.0";

const HeaderStruct BMPRipper::s_headers[] = {
    {"BM", 2},
    {"", 0}
};

bool BMPRipper::checkLocation(unsigned char *pos, const HeaderStruct *header, FoundStruct *found)
{
	BMPHeader *hdr = (BMPHeader *)pos;

	// the bfType has already been checked so we need only check the other fields

	found->criterium = CRIT_NONE;

	// check if the file goes beyond the end of our file
	if (pos + hdr->bfSize > m_start + m_length)
		return false;

	// do the bits start after the end of file?
	if (hdr->bfOffBits > hdr->bfSize)
		return false;

	// arbitrary value for sanity reasons
	if (hdr->biHeaderSize > 1024)
		return false;

	// check image dimensions
	if ((hdr->biHeight <= 0) || (hdr->biWidth <= 0))
		return false;

	// check bit count, set WEAK if we're not so sure anymore
	if ((hdr->biBitCount != 1) && (hdr->biBitCount != 4) && (hdr->biBitCount != 8)
		&& (hdr->biBitCount != 24) && (hdr->biBitCount != 32))
		return false;

	// here, we're pretty sure we have a BMP file, so fill up the 
	found->criterium = CRIT_STRONG;

	// check if the reserved fields are really zero (this is a weak check, maybe remove?)
	if ((hdr->bfReserved1 != 0) || (hdr->bfReserved2 != 0))
		found->criterium = CRIT_WEAK;

	// check planes (weak check)
	if (hdr->biPlanes != 1)
		found->criterium = CRIT_WEAK;

	strcpy(found->extension, "BMP");
	found->startoffset = pos;
	found->length = hdr->bfSize;
	return true;
}

