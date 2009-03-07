#ifndef RIFF_RIPPER_H
#define RIFF_RIPPER_H

#include "Ripper.h"

class RIFFRipper : public Ripper
{
private:
	static const char *s_name;

	static const HeaderStruct s_headers[];

public:
	RIFFRipper()
	{
	}

	virtual ~RIFFRipper()
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
