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