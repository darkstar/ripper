#ifndef MOD_RIPPER_H
#define MOD_RIPPER_H

#include "Ripper.h"

struct ModInfoStruct {
  int m_channels;
  int m_instruments;
  bool m_alt8chn;// for FLT8 -- 2x4 channels instead of 8channels
  //long m_magic_offset;// start offset of header in file, default 1080

  ModInfoStruct(int channels, int instruments = 31, bool alt8chn = false, long magic_offset = 1080)
  {
    m_channels = channels;
    m_instruments = instruments;
    m_alt8chn = alt8chn;
    //m_magic_offset = magic_offset;
  }
};

class MODRipper : public Ripper
{
private:
	static const char *s_name;

	static const HeaderStruct s_headers[];

	bool IsValidMODSampleHeader(unsigned char *pos, unsigned long *totalSampleSize);

public:
	MODRipper()
	{
	}

	virtual ~MODRipper()
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
