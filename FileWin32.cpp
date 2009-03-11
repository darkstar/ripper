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

#include "File.h"

File::File(const char *fileName)
{
	m_fileName = strdup(fileName);
	m_isOpen = false;
	m_mapPtr = 0;
	m_private.hFile = m_private.hMapping = INVALID_HANDLE_VALUE;
	m_error = 0;
}

File::~File()
{
	if (m_isOpen)
		Close();

	if (m_fileName)
		free(m_fileName);
}

void *File::Open()
{
	// return pointer if file is already open
	if (m_isOpen)
		return m_mapPtr;

	// open the file
	m_private.hFile = CreateFile(m_fileName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (m_private.hFile ==  INVALID_HANDLE_VALUE)
	{
		m_error = 1;
		return 0;
	}

	// get file size
	m_size = SetFilePointer(m_private.hFile, 0, 0, FILE_END);
	SetFilePointer(m_private.hFile, 0, 0, FILE_BEGIN);

	// create file mapping
	m_private.hMapping = CreateFileMapping(m_private.hFile, 0, PAGE_READONLY, 0, 0, 0);
	if (m_private.hMapping == INVALID_HANDLE_VALUE)
	{
		m_error = 2;
		CloseHandle(m_private.hFile);
		return 0;
	}
	m_mapPtr = MapViewOfFile(m_private.hMapping, FILE_MAP_READ, 0, 0, 0);
	if (!m_mapPtr)
	{
		m_error = 3;
		CloseHandle(m_private.hFile);
		CloseHandle(m_private.hMapping);
		return 0;
	}

	m_error = 0;
	m_isOpen = true;
	return m_mapPtr;
}

bool File::Close()
{
	if (!m_isOpen)
		return false;

	UnmapViewOfFile(m_mapPtr);
	CloseHandle(m_private.hMapping);
	CloseHandle(m_private.hFile);

	m_isOpen = false;

	return true;
}

unsigned long File::getSize() const
{
	return m_size;
}

const char *File::getError() const
{
	switch (m_error)
	{
	case 0:
		return "no error";
	case 1:
		return "file not found";
	case 2:
		return "cannot create mapping";
	case 3:
		return "cannot map file (too big?)";
	default:
		return "unknown error";
  }
}
