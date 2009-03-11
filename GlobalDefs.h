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

#ifndef GLOBAL_DEFS_H
#define GLOBAL_DEFS_H

/*
 * Global defines go here, grouped by compiler, or at least by platform
 */


#if(_MSC_VER)
// Visual Studio defines go here
typedef unsigned __int8 uint8;
typedef signed __int8 int8;
typedef unsigned __int16 uint16;
typedef signed __int16 int16;
typedef unsigned __int32 uint32;
typedef signed __int32 int32;
typedef unsigned __int64 uint64;
typedef signed __int64 int64;

#elif(__GCC__)
// GCC defines go here

#endif

#endif
