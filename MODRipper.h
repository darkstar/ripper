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

#ifndef MOD_RIPPER_H
#define MOD_RIPPER_H

#include "Ripper.h"

struct ModInfoStruct {
	int m_channels;
	int m_instruments;
	bool m_alt8chn;// for FLT8 -- 2x4 channels instead of 8channels
	//long m_magic_offset;// start offset of header in file, default 1080

	ModInfoStruct(int channels, int instruments = 31, bool alt8chn = false)
	{
		m_channels = channels;
		m_instruments = instruments;
		m_alt8chn = alt8chn;
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

	IMPLEMENT_DEFAULT_RIPPER_INTERFACE

	virtual bool checkLocation(unsigned char *pos, const HeaderStruct *header, FoundStruct *found);
};

#endif
