#ifndef MIDI_RIPPER_H
#define MIDI_RIPPER_H

#include "Ripper.h"

class MIDIRipper : public Ripper
{
private:
	static const char *s_name;

	static const HeaderStruct s_headers[];

	unsigned long IsValidChunk(unsigned char *pos);

public:
	MIDIRipper()
	{
	}

	virtual ~MIDIRipper()
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
