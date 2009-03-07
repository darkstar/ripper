/***********************************************************************
* Copyright 2007 Michael Drueing <michael@drueing.de>
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License as
* published by the Free Software Foundation; either version 2 of
* the License or (at your option) version 3 or any later version
* accepted by the membership of KDE e.V. (or its successor approved
* by the membership of KDE e.V.), which shall act as a proxy 
* defined in Section 14 of version 3 of the license.
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

#include "DDSRipper.h"

const char *DDSRipper::s_name = "DDS Texture Ripper v1.0";

const HeaderStruct DDSRipper::s_headers[] = {
		{"DDS ", 4},
    {"", 0}
};

#define DWORD unsigned int

struct DDPIXELFORMAT
{
  DWORD dwSize; //	Size of structure. This member must be set to 32.
  DWORD dwFlags; //	Flags to indicate valid fields. Uncompressed formats will usually use DDPF_RGB to indicate an RGB format, while compressed formats will use DDPF_FOURCC with a four-character code.
  DWORD dwFourCC; //	This is the four-character code for compressed formats. dwFlags should include DDPF_FOURCC in this case. For DXTn compression, this is set to "DXT1", "DXT2", "DXT3", "DXT4", or "DXT5".
  DWORD dwRGBBitCount; //	For RGB formats, this is the total number of bits in the format. dwFlags should include DDPF_RGB in this case. This value is usually 16, 24, or 32. For A8R8G8B8, this value would be 32.
  DWORD dwRBitMask; //
  DWORD dwGBitMask; //
  DWORD dwBBitMask; //	For RGB formats, these three fields contain the masks for the red, green, and blue channels. For A8R8G8B8, these values would be 0x00ff0000, 0x0000ff00, and 0x000000ff respectively.
  DWORD dwRGBAlphaBitMask; //	For RGB formats, this contains the mask for the alpha channel, if any. dwFlags should include DDPF_ALPHAPIXELS in this case. For A8R8G8B8, this value would be 0xff000000.
};

struct DDCAPS2
{
  DWORD dwCaps1; //	DDS files should always include DDSCAPS_TEXTURE. If the file contains mipmaps, DDSCAPS_MIPMAP should be set. For any DDS file with more than one main surface, such as a mipmaps, cubic environment map, or volume texture, DDSCAPS_COMPLEX should also be set.
  DWORD dwCaps2; //	For cubic environment maps, DDSCAPS2_CUBEMAP should be included as well as one or more faces of the map (DDSCAPS2_CUBEMAP_POSITIVEX, DDSCAPS2_CUBEMAP_NEGATIVEX, DDSCAPS2_CUBEMAP_POSITIVEY, DDSCAPS2_CUBEMAP_NEGATIVEY, DDSCAPS2_CUBEMAP_POSITIVEZ, DDSCAPS2_CUBEMAP_NEGATIVEZ). For volume textures, DDSCAPS2_VOLUME should be included.
  DWORD Reserved[2]; //
};

struct DDSURFACEDESC2
{
  DWORD dwSize; // == 124
  DWORD dwFlags; //	Flags to indicate valid fields. Always include DDSD_CAPS, DDSD_PIXELFORMAT, DDSD_WIDTH, DDSD_HEIGHT.
  DWORD dwHeight; //	Height of the main image in pixels
  DWORD dwWidth; //	Width of the main image in pixels
  DWORD dwPitchOrLinearSize; //	For uncompressed formats, this is the number of bytes per scan line (DWORD> aligned) for the main image. dwFlags should include DDSD_PITCH in this case. For compressed formats, this is the total number of bytes for the main image. dwFlags should be include DDSD_LINEARSIZE in this case.
  DWORD dwDepth; //	For volume textures, this is the depth of the volume. dwFlags should include DDSD_DEPTH in this case.
  DWORD dwMipMapCount; //	For items with mipmap levels, this is the total number of levels in the mipmap chain of the main image. dwFlags should include DDSD_MIPMAPCOUNT in this case.
  DWORD dwReserved1[11];
  DDPIXELFORMAT ddpfPixelFormat; //	32-byte value that specifies the pixel format structure.
  DDCAPS2 ddsCaps; //	16-byte value that specifies the capabilities structure.
  DWORD dwReserved2;
};

long RoundUp(long i)
{
  while (i % 4)
    i++;

  return i;
}

bool DDSRipper::checkLocation(unsigned char *pos, const HeaderStruct *header, FoundStruct *found)
{
  DDSURFACEDESC2 *hd = (DDSURFACEDESC2 *)(pos+4);
  long w, h;
  unsigned long i;
  long datasize = 0;
  long mainsize = 0;
  bool compressed = false;
  long minmipmap = 4;

  found->criterium = CRIT_STRONG;
  found->length = 4 + sizeof(DDSURFACEDESC2); // +4 for the "DDS " id
  found->startoffset = pos;

  //fprintf(stderr, " *DDS check A\n");
  if (pos + 4 + sizeof(DDSURFACEDESC2) >= m_start + m_length)
    return false; // the header is 124 bytes; at least so many bytes must be available

  if (hd->dwSize != 124)
    return false;  // size must be 124 if this structure is valid

  if (hd->ddpfPixelFormat.dwSize != 32)
    return false; // this structure must be valid

  if ((hd->dwFlags & 0x1007) != 0x1007)
    return false; // must have CAPS, WIDTH, HEIGHT, PIXELFORMAT bits set

  if ((hd->dwHeight == 0) || (hd->dwWidth == 0))
    return false; // 0-pixel width or 0-pixel height is not good...

  if ((hd->dwHeight > 0x100000) || (hd->dwWidth > 0x100000))
    return false; // some arbitrary size limits: 1M x 1M pixels

  w = hd->dwWidth;
  h = hd->dwHeight;

  if (hd->dwFlags & 0x0008) // DDSD_PITCH
  {
    if ((hd->dwPitchOrLinearSize == 0) || (hd->dwPitchOrLinearSize > 0x100000))
      return false; // scanline size == 0 or too big

    w = hd->dwPitchOrLinearSize; // new width for calculations
    datasize = RoundUp(w*h);
  }
  else if (hd->dwFlags & 0x80000) // DDSD_LINEARSIZE
  {
    compressed = true;
    datasize = mainsize = hd->dwPitchOrLinearSize;
    if (hd->dwPitchOrLinearSize == 0) 
      return false; // linear size == 0
  }
  else
  {
    // neither DDSD_PITCH nor DDSD_LINEARSIZE set. Error.
    //fprintf(stderr, " *DDS check FAILED, flags=0x%08x\n", hd->dwFlags);
    return false;
  }

  if ((hd->dwFlags & 0x00800000) && (hd->dwDepth == 0)) // DDSD_DEPTH
  {
    // depth flag set, but depth = 0.
    return false;
  }

  if ((hd->dwFlags &  0x00020000) && (hd->dwMipMapCount == 0)) // DDSD_MIPMAPCOUNT
  {
    // MipMap flag set but no mipmaps in file.
    return false;
  }

  //fprintf(stderr, " - mainsize: %d (%dx%d)%s\n", mainsize, w, h, compressed?" (COMPRESSED)":"");

  if (hd->ddpfPixelFormat.dwFlags & 0x00000004) // DDPF_FOURCC
  {
    if (strncmp((const char *)&(hd->ddpfPixelFormat.dwFourCC), "DXT1", 4) == 0)
    {
      minmipmap = 8;
    }
    else if (strncmp((const char *)&(hd->ddpfPixelFormat.dwFourCC), "DXT", 3) == 0)
    {
      minmipmap = 16;
    }
  }

  if (hd->dwFlags & 0x00020000) // has mipmaps?
  {
    //fprintf(stderr, " - hasMipMaps(%2d): ", hd->dwMipMapCount);
    // calc mipmap size
    for (i = 1; i < hd->dwMipMapCount; i++)
    {
      if (compressed)
      {
        mainsize /= 4;
        if (mainsize < minmipmap)
          mainsize = minmipmap;

        datasize += mainsize;
        //fprintf(stderr, "%d, ", mainsize);
      }
      else
      {
        w/=2;
        h/=2;
        datasize += RoundUp(w*h);
        //fprintf(stderr, "(%dx%d) %d, ", w, h, RoundUp(w*h));
      }
    }
    //fprintf(stderr, "\n");
  }

  found->length += datasize;

  strcpy(found->extension, "DDS");

  return true;
}
