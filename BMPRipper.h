#ifndef BMP_RIPPER_H
#define BMP_RIPPER_H

#include "Ripper.h"

#pragma pack(push, 1)

struct BMPHeader
{
	WORD bfType;
	DWORD bfSize;
	WORD bfReserved1;
	WORD bfReserved2;
	DWORD bfOffBits;
  DWORD biHeaderSize;
  LONG biWidth; 
  LONG biHeight; 
  WORD biPlanes; 
  WORD biBitCount; 
	// all after here is optional (?)
  DWORD biCompression; 
  DWORD biSizeImage; 
  LONG biXPelsPerMeter; 
  LONG biYPelsPerMeter; 
  DWORD biClrUsed; 
  DWORD biClrImportant; 
};

#pragma pack(pop)

class BMPRipper : public Ripper
{
private:
	static const char *s_name;

	static const HeaderStruct s_headers[];

public:
	BMPRipper()
	{
	};

	virtual ~BMPRipper()
	{
	};

	virtual const HeaderStruct *getHeaders() const
	{
		return &s_headers[0];
	}

	virtual const char *getName() const
	{
		return s_name;
	}

	virtual bool checkLocation(unsigned char *pos, const HeaderStruct *header, FoundStruct *found);
};

#endif
