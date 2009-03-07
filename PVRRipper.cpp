#include <cstring>
#include <cstdio>
#include <cstdlib>

#include "PVRRipper.h"

/* Ripper for PVR files (PowerVR Textures, commonly used on the SEGA Dreamcast) */

const char *PVRRipper::s_name = "PVR/PVRT/GBIX Ripper v1.0";

const HeaderStruct PVRRipper::s_headers[] = {
  { "PVRT", 4 },
  { "", 0 }
};

bool PVRRipper::checkLocation(unsigned char *pos, const HeaderStruct *header, FoundStruct *found)
{
  unsigned char *startpos = pos;
  unsigned long total_len = 0;
  unsigned long len;
  unsigned char type1, type2;
  unsigned short width, height;
  
  found->criterium = CRIT_STRONG;

  if (pos - m_start > 16)
  {
    // check for valid GBIX header before the PVRT header
    if ( (memcmp(pos - 16, "GBIX", 4) == 0) && (*((unsigned long *)(pos - 12)) == 0x00000008))
    {
      // if we have a GBIX header, then adjust the start position
      startpos -= 16;
      total_len += 16;
    }
  }

  // continue checking the PVRT fields
  len = *((unsigned long *)(pos+4));

  if (m_start + m_length < pos + len + 8) // file goes beyond EOF
    return false;

  total_len += len;
  total_len += 8; // 8 header bytes

  type1 = *(pos+8);
  type2 = *(pos+9);
  width = *((unsigned short *)(pos+12));
  height = *((unsigned short *)(pos+14));

  // Type 1: 0=ARGB1555 1=RGB565 2=ARGB444 3=UNKNOWN(Soul Calibur has them, for example)
  if ((type1 != 0) && (type1 != 1) && (type1 != 2))
    found->criterium = CRIT_WEAK;

  if ((type2 != 0) && (type2 != 1) && (type2 != 2) && (type2 != 3) && (type2 != 4)
        && (type2 != 9) && (type2 != 16) && (type2 != 17))
    found->criterium = CRIT_WEAK;

  strcpy(found->extension, "PVR");
  found->startoffset = startpos;
  found->length = total_len;

  return true;
};

