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
#include <cstdio>
#include "MPEGRipper.h"

/*
 * TODO: Make this aware of LayerI / LayerII files also
 */

const char *MPEGRipper::s_name = "MPEG Layer 3 Ripper v1.0";

const HeaderStruct MPEGRipper::s_headers[] = {
	{"\xff\xfb", 2, 0, (void *)0},  // MPEG I Layer III, no CRC
	{"\xff\xfa", 2, 0, (void *)1},  // MPEG I Layer III, with 16-bit CRC
	{"", 0}
};

static const int MPEG1Layer3Bitrates[] = {
	-1,	32,	40,	48,
	56, 64, 80, 96,
	112, 128, 160, 192,
	224, 256, 320, -1,
};

static const int MPEG1SampleRates[] = {
	44100, 48000, 32000, -1,
};

bool MPEGRipper::isValidMP3Header(unsigned char **pos, const HeaderStruct *header)
{
	unsigned short CRC;
	bool hasCRC;
	unsigned char frameHeader[4];
	int frameLength;
	int bitRate;
	int sampleRate;
	int padding;
	int priv;

	if (*pos + 4 >= m_start + m_length)
		return false;

	hasCRC = (int)(header->user_data) == 1;
	memcpy(frameHeader, *pos, 4);
	*pos += 4;

	bitRate = MPEG1Layer3Bitrates[(frameHeader[2] >> 4) & 0x0f];
	sampleRate = MPEG1SampleRates[(frameHeader[2] >> 2) & 0x03];
	padding = (frameHeader[2] >> 1) & 0x01;
	priv = frameHeader[2] & 0x01;

	// check for correct SYNC bits
	if (frameHeader[0] != 0xff)
		return false;
	if (((frameHeader[1] >> 5) & 0x07) != 7)
		return false;

	// read CRC if needed
	if (hasCRC)
	{
		if (*pos + 2 >= m_start + m_length)
			return false;

		CRC = *((unsigned short *)(*pos));
		*pos += 2;
	}

	if ((bitRate == -1) || (sampleRate == -1))
		return false;

	// calculate frame length (this is for MPEG I layer 3 only!)
	frameLength = 144 * bitRate * 1000 / sampleRate + padding;

	if (*pos + frameLength >= m_start + m_length)
		return false;

	// TODO: Check CRC if present

	// frame length includes the 4 header and two (if present) crc bytes
	*pos += frameLength - 4 - (hasCRC ? 2 : 0);

	//printf("DEBUG: crc=0x%04x, length %d, bitrate= %d, samplerate = %d, priv=%d, pad=%d\n", hasCRC ? CRC : 0, frameLength, bitRate, sampleRate, priv, padding);

	return true;
}

bool MPEGRipper::checkLocation(unsigned char *pos, const HeaderStruct *header, FoundStruct *found)
{
	unsigned char *pos1, *pos2;
	int numFrames = 0;

	pos1 = pos;

	numFrames = 0;

	while (isValidMP3Header(&pos, header))
	{
		numFrames++;
	}

	pos2 = pos;

	// reject MP3s with fewer than 3 valid frame headers (arbitrary decision!)
	if (numFrames < 3)
		return false;

	// if fewer than 20 frames (that's about 1 second best case with 22khz mono, 
	// 44.1khz stereo brings it down to 0.25 seconds)
	if (numFrames < 20)
		found->criterium = CRIT_WEAK;
	else
		found->criterium = CRIT_STRONG;

	found->length = pos2-pos1;
	found->startoffset = pos1;
	strcpy(found->extension, "mp3");
	return true;
}
