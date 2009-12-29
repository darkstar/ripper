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

#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/fcntl.h>
#include "File.h"

// UNIX implementation of File class

File::File(const char *fileName)
{
	m_fileName = strdup(fileName);
	m_isOpen = false;
	m_mapPtr = 0;
	m_error = 0;
	m_private.fd = 0;
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
	if (m_isOpen)
		return m_mapPtr;
	
	// open the file
	m_private.fd = open(m_fileName, O_RDONLY);
	if (m_private.fd < 0)
	{
	  m_error = 1;
	  return 0;
	}

	// Get file size
	m_size = lseek(m_private.fd, 0, SEEK_END);
	lseek(m_private.fd, 0, SEEK_SET);

	// create mapping
	m_mapPtr = mmap(0, m_size, PROT_READ, MAP_PRIVATE, m_private.fd, 0);
	if (m_mapPtr == MAP_FAILED)
	{
	  m_error = 2;
	  close(m_private.fd);
	  return 0;
	}

	m_isOpen = true;
	return m_mapPtr;
}

bool File::Close()
{
	if (!m_isOpen)
		return false;

	munmap(m_mapPtr, m_size);
	close(m_private.fd);

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
        default:
                return "unknown error";
  }
}

