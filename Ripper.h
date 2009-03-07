#ifndef RIPPER_H
#define RIPPER_H

#define FROM_BIG_ENDIAN(a) ((((a)>>24)&0xFF) | (((a)>>8)&0xFF00) | (((a)<<8)&0xFF0000) | (((a)<<24)&0xFF000000)) 

enum FoundCriterium
{
	CRIT_NONE = 0x00,
	CRIT_WEAK = 0x01,    // not sure if file is really of expected type; flag it accordingly
	CRIT_STRONG = 0x02   // file is almost certainly correct
};

// defines one header to search for
struct HeaderStruct
{
  // the pattern to look for
	char *header;

  // the length of the pattern
	int length;

  // number of bytes in the file _before_ the pattern
  // (normally this is 0, see MODRipper for an example where it's not)
  int extra_len_before;

  // even more user data
  void *user_data;
};

struct FoundStruct
{
	// proposed extension for this file
	char extension[10];

	// the start offset where the file begins
	unsigned char *startoffset;

	// the total length of the file
	unsigned long length;

	// how sure are we that we have the expected file?
	FoundCriterium criterium;
};

class Ripper
{
protected:
	unsigned char *m_start;
	unsigned long m_length;

public:
	// returns a list of headers to look out for
	virtual const HeaderStruct *getHeaders() const = 0;

	// return the name of this module
	virtual const char *getName() const = 0;

	// prepare for scanning a file
	virtual void setupFile(unsigned char *start, unsigned long length)
	{
		m_start = start;
		m_length = length;
	}

	// check if this offset really contains a valid file
	virtual bool checkLocation(unsigned char *pos, const HeaderStruct *header, FoundStruct *found) = 0;
};

#endif
