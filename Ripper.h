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

#ifndef RIPPER_H
#define RIPPER_H

#define FROM_BIG_ENDIAN(a) ((((a)>>24)&0xFF) | (((a)>>8)&0xFF00) | (((a)<<8)&0xFF0000) | (((a)<<24)&0xFF000000)) 

enum FoundCriterium
{
	CRIT_NONE = 0x00,
	CRIT_WEAK = 0x01,    // not sure if file is really of expected type; flag it accordingly
	CRIT_STRONG = 0x02   // file is almost certainly correct
};

#define HS_END \
	{0, 0, 0, 0},

#define HS(h, l) \
	{h, l, 0, 0},

#define HS_U(h, l, u) \
	{h, l, 0, u},

#define HS_X(h, l, e) \
	{h, l, e, 0},

#define HS_F(h, l, e, u) \
	{h, l, e, u},

// defines one header to search for
struct HeaderStruct
{
	// the pattern to look for
	const char *header;

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

	// this can be used to check the size of some structs, to see whether compilation bugs arise
	// the default just returns true, subclasses should overload this and return false if something went
	// wrong. The main program will then abort and print a message
	virtual bool checkCompileAssertions()
	{
		return true;
	}
};


// for smaller .h files implementing ripper modules:

#define IMPLEMENT_DEFAULT_RIPPER_INTERFACE \
	virtual const HeaderStruct *getHeaders() const \
	{ \
		return &s_headers[0]; \
	} \
 \
	virtual const char *getName() const \
	{ \
		return s_name; \
	}

#endif
