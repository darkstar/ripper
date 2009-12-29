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

enum ModType
{
	MT_MOD = 0,
	MT_669 = 1,
	MT_S3M = 2,
};

struct ModInfoStruct {
	int m_channels;
	int m_instruments;
	bool m_alt8chn;// for FLT8 -- 2x4 channels instead of 8channels
	ModType m_modType;

	ModInfoStruct(ModType mType, int channels = -1, int instruments = 31, bool alt8chn = false)
	{
		m_modType = mType;
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
	bool IsValid669SampleHeader(unsigned char *pos, unsigned long *totalSampleSize);

public:
	MODRipper() : Ripper()
	{
	}

	virtual ~MODRipper()
	{
	};

	IMPLEMENT_DEFAULT_RIPPER_INTERFACE

	virtual bool checkLocation(unsigned char *pos, const HeaderStruct *header, FoundStruct *found);

	virtual bool checkCompileAssertions();
};

#endif
