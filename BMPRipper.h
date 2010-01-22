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

#ifndef BMP_RIPPER_H
#define BMP_RIPPER_H

#include "GlobalDefs.h"
#include "Ripper.h"

#pragma pack(push,1)
struct BMPHeader
{
	uint16 bfType;
	uint32 bfSize;
	uint16 bfReserved1;
	uint16 bfReserved2;
	uint32 bfOffBits;
	uint32 biHeaderSize;
	int32 biWidth; 
	int32 biHeight; 
	uint16 biPlanes; 
	uint16 biBitCount; 
	// all after here is optional (?)
	uint32 biCompression; 
	uint32 biSizeImage; 
	int32 biXPelsPerMeter; 
	int32 biYPelsPerMeter; 
	uint32 biClrUsed; 
	uint32 biClrImportant; 
};
#pragma pack(pop)

class BMPRipper : public Ripper
{
private:
	static const char *s_name;

	static const HeaderStruct s_headers[];

public:
	BMPRipper() : Ripper()
	{
	};

	virtual ~BMPRipper()
	{
	};
	
	IMPLEMENT_DEFAULT_RIPPER_INTERFACE

	virtual bool checkLocation(unsigned char *pos, const HeaderStruct *header, FoundStruct *found);

	virtual bool checkCompileAssertions();
};

#endif
