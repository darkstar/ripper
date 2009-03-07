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

#include <memory.h>
#include <cstdio>
#include <queue>

#include "PatternMatcher.h"

// stupid straightforwars implementation of Aho-Corasick parallel pattern matching algorithm

void PatternMatcher::InitNode(ACNode *n)
{
	memset(n->g, 0, 256 * sizeof(ACNode *));

	n->f = rootNode;
	n->header = 0;
	n->ripper = 0;
}

PatternMatcher::PatternMatcher()
{
	// initialize the root node
	rootNode = new ACNode();
	InitNode(rootNode);
}

PatternMatcher::~PatternMatcher()
{
	DestroyNode(rootNode);
}

void PatternMatcher::BeginSearch()
{
	currentNode = rootNode;
}

void PatternMatcher::DestroyNode(ACNode *node)
{
	for (int i = 0; i < 256; i++)
	{
		if ((node->g[i] != 0) && (node->g[i] != rootNode))
		{
			DestroyNode(node->g[i]);
		}
	}
	delete node;
}

bool PatternMatcher::NextByte(unsigned char ch, ACNode **finalState)
{
	while (currentNode->g[ch] == 0)
		currentNode = currentNode->f; // follow a FAIL

	currentNode = currentNode->g[ch]; // follow a GOTO

	*finalState = currentNode;
	return (currentNode->ripper != 0);
}

PatternMatcher::ACNode *PatternMatcher::GetNode(ACNode *n)
{
	if (n == 0)
		return rootNode;

	return n;
}

void PatternMatcher::AddPattern(const HeaderStruct *header, Ripper *ripper)
{
	int i;
	ACNode *n = rootNode;

	// parse the Trie and add the pattern
	for (i = 0; i < header->length; i++)
	{
		if (n->g[(unsigned char)(header->header[i])] == 0)
		{
			// no node yet. Create it
			ACNode *newNode = new ACNode();
			InitNode(newNode);
			n->g[(unsigned char)(header->header[i])] = newNode;
		}
		// skip to next node
		n = n->g[(unsigned char)(header->header[i])];
	}

	// now, n points to the last node, which will become an output node
	if (n->header != 0)
	{
		fprintf(stderr, "INTERNAL WARNING: there's already a ripper registered here!\n");
	}
	n->header = header;
	n->ripper = ripper;
}

void PatternMatcher::FinalizeMatcher()
{
	unsigned int a;
	std::queue<ACNode *> queue;
	ACNode *q, *r, *u, *v;

	// Phase II of the Aho-Corasick algorithm
	for (a = 0; a < 256; a++)
	{
		q = rootNode->g[a];
		if (q != 0)
		{
			q->f = rootNode;
			queue.push(q);
		}
		else
		{
			rootNode->g[a] = rootNode;
		}
	}

	while (!queue.empty())
	{
		r = queue.front();
		queue.pop();

		for (a = 0; a < 256; a++)
		{
			u = r->g[a];
			if (u != 0)
			{
				queue.push(u);
				v = r->f;
				while (v->g[a] == 0)
					v = v->f;

				u->f = GetNode(v->g[a]);
				// TODO: merge OUT patterns here
			}
		}
	}
}
