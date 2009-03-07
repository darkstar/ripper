#include <cstring>
#include "VOCRipper.h"

const char *VOCRipper::s_name = "Creative VOC Ripper v1.0";

const HeaderStruct VOCRipper::s_headers[] = {
	{"Creative Voice File\x1a", 20},
  {"", 0}
};

#pragma pack(push, 1)

struct VOCHeader
{
	char ID[20]; // the ID
	unsigned short dataOffset;
	unsigned short ver;
	unsigned short ver2;
};

#pragma pack(pop)

bool VOCRipper::checkLocation(unsigned char *pos, const HeaderStruct *header, FoundStruct *found)
{
	VOCHeader *hdr = (VOCHeader *)pos;
	unsigned int blockLength;
	unsigned char *pos2;
	unsigned char blockType;

	// check simple checksum
	if ((~hdr->ver) + 0x1234 != hdr->ver2)
		return false;

	// scan through all data blocks until we find the terminator block
	pos2 = pos + hdr->dataOffset;

	blockType = *pos2;
	while ((blockType != 0) && (pos2 < m_start + m_length))
	{
		pos2++;
		// first: read 3 bytes blocklength and skip those
		blockLength = (unsigned int)(pos2[0]) | (unsigned int)(pos2[1] << 8) | (unsigned int)(pos2[2] << 16);
		pos2 += 3;

		// skip length of block
		pos2 += blockLength;

		blockType = *pos2;
	}

	// check to see if we ran past the end of the file
	if (pos2 >= m_start + m_length)
		return false;

	found->criterium = CRIT_STRONG;
	strcpy(found->extension, "VOC");
	found->startoffset = pos;
	found->length = pos2 - pos + 1;

	return true;
}
