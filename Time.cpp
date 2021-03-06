/***********************************************************************
* Copyright 2007-2010 Michael Drueing <michael@drueing.de>
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License as
* published by the Free Software Foundation; either version 2 of
* the License or (at your option) version 3 or any later version
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
***********************************************************************/

#include "Config.h"
#include "Time.h"

#ifdef _MSC_VER
#include <windows.h>
#endif

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#ifdef _MSC_VER
uint64 GetTicks()
{
  return 1000*GetTickCount();
};
#endif

#ifdef linux
uint64 GetTicks()
{
  struct timeval tv;

  gettimeofday(&tv, 0);
  return tv.tv_sec*1000000 + tv.tv_usec;
};
#endif

