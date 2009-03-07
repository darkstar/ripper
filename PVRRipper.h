#ifndef PVR_RIPPER_H
#define PVR_RIPPER_H

#include "Ripper.h"

class PVRRipper : public Ripper
{
private:
	static const char *s_name;

	static const HeaderStruct s_headers[];

public:
	PVRRipper()
	{
	}

	virtual ~PVRRipper()
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

