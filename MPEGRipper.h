#ifndef MPEG_RIPPER_H
#define MPEG_RIPPER_H

#include "Ripper.h"

class MPEGRipper : public Ripper
{
private:
	static const char *s_name;

	static const HeaderStruct s_headers[];

public:
	MPEGRipper()
	{
	}

	virtual ~MPEGRipper()
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

private:
	bool isValidMP3Header(unsigned char **pos, const HeaderStruct *header);
};

#endif
