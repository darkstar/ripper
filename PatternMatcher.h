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

/*
 * Implementation of the Aho-Corasick pattern matching DFA
 */

#ifndef PATTERN_MATCHER_H
#define PATTERN_MATCHER_H

#include "Ripper.h"

class PatternMatcher
{
public:
	// This is one node of the Trie/DFA
	struct ACNode
	{
		struct ACNode *g[256];  // one output for each byte value
		// TODO: Do something with multiple rippers matching the same pattern
		// (we'll handle it when it happens ;-)
		const struct HeaderStruct *header; // or NULL if no header matches
		Ripper *ripper; // or NULL if this is not a terminal node
		struct ACNode *f; // the failure node
	};

private:
	// this is the root of the Trie/DFA
	ACNode *rootNode;
	ACNode *currentNode;

	ACNode *PatternMatcher::GetNode(ACNode *n);
	void DestroyNode(ACNode *node);
	void PatternMatcher::InitNode(ACNode *n);

public:
	PatternMatcher();
	void AddPattern(const HeaderStruct *header, Ripper *ripper);
	void FinalizeMatcher();
	void BeginSearch();
	bool NextByte(unsigned char ch, ACNode **finalState);
	~PatternMatcher();
};

#endif