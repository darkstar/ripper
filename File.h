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

#ifndef FILE_H
#define FILE_H

#if defined(_MSC_VER)

#define WINDOWS_LEAN_AND_MEAN
#include <windows.h>

struct FilePrivate
{
	HANDLE hFile;
	HANDLE hMapping;
};

#elif defined(__GCC__)

// do some GCC magic here
struct FilePrivate
{
};

#endif

class File
{
private:
	FilePrivate m_private;
	bool m_isOpen;
	void *m_mapPtr;
	char *m_fileName;
	unsigned long m_size;
	int m_error;

public:
	File(const char *fileName);
	virtual ~File();

	virtual void *Open(); // returns Pointer to mmap()ed file
	virtual bool Close();

	virtual unsigned long getSize() const;
	virtual const char *getError() const;
};


#endif
