#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <vector>
#include <time.h>

#include "pstring.hpp"
#include "stack.hpp"
#include "pair.hpp"

using namespace std;

/*
 *  Name:      matcher.cpp
 *  Author:    Herbert Lange
 *  Descriton: Matching function to match an input word
 *  with an array of possible substrings
 *
 */

// Approximately 85000 syllables
Pair* syllables[85000];
int map[85000];

// Memorizes the absolute count
int syllables_count;

// Read the syllables from a file into the vector
void read_syllable_list(char *filename)
{
  // Reset counter
  syllables_count=0;
  FILE *f=fopen(filename,"r");
  if (f==NULL)
    {
      printf("Error opening syllable list\n");
      return;
    }
  // Syllables are not longer than eight characters
  char line[8];
  while(fscanf(f,"%s\n",line)>0)
    {
      syllables[syllables_count]=new Pair(Pstring(line));
      syllables_count++;
    }
}

// Sort the mapping so that frequent syllables are preferred to infrequent and longer syllables to shorter
// Returns 1 on success and -1 on error
void sort_map()
{
  int n=syllables_count;
  int swapped;
  clock_t t1=clock();
  do
    {
      swapped=0;
      int ct=0;
      for(ct=0;ct<n-1;ct++)
	{
	  if ((syllables[map[ct]]->count<syllables[map[ct+1]]->count)||((syllables[map[ct]]->count==syllables[map[ct+1]]->count)&&(syllables[map[ct]]->word.getLength()<syllables[map[ct+1]]->word.getLength())))
	    {
	      map[ct]^=map[ct+1];
	      map[ct+1]^=map[ct];
	      map[ct]^=map[ct+1];
	      swapped=1;
	    }	 
	}
      if (n%1000==0)
	{
	  clock_t t2=clock();
#ifdef DEBUG
	  printf("%d elements left to sort, %f\n",n,((float)t2-(float)t1)/CLOCKS_PER_SEC);
#endif
	}
      n=n-1;
    }
  while(swapped==1);
}

int match(Pstring* word, int score)
{
  // Stack for the positions and syllable number
  Stack<int> history;
  int curpos=0;
  int start=0;
  start=0;
  curpos=0;
#ifdef DEBUG
  printf("Trying to match %.*s at %p\n",word->getLength(),word->getValue(),&(word->value));
#endif
    while (curpos!=word->getLength())
    {
      int ct;
      int match=0;
      for (ct=start;ct<syllables_count;ct++)
	{
#ifdef DEBUG
	  printf("Trying syllable %d - %.*s\n",ct, syllables[map[ct]]->word.getLength(), syllables[map[ct]]->word.getValue());
#endif
	  // Syllable is prefix of the substring starting at curpos
	  if (strncmp(&((word->getValue())[curpos]),syllables[map[ct]]->word.getValue(),syllables[map[ct]]->word.getLength())==0)
	    {
	      match=1;
#ifdef DEBUG
	      printf("Matched syllable %.*s at position %d\n",syllables[map[ct]]->word.getLength(),syllables[map[ct]]->word.getValue(),curpos);
#endif
	      history.push(curpos);
	      history.push(ct);
	      curpos=curpos+syllables[map[ct]]->word.getLength();
	      break;
	    }
	}
      // No matching syllable found -> backtracking
      if (match==0)
	{
	  start=history.pop()+1;
	  curpos=history.pop();
#ifdef DEBUG
	  printf("Backtrack to syllable %d at position %d\n",start,curpos);
#endif
	}
      // Start again with first syllable
      else
	start=0;
    }
#ifdef DEBUG
  printf("Clean history\n");
#endif
  while(history.getCount()>0)
    { 
      // get the syllable number
      int pos=history.pop();
      // ignore the saved string position
      history.pop();
      int oldcount=syllables[map[pos]]->count;
      syllables[map[pos]]->count+=score;
      if (oldcount>syllables[map[pos]]->count)
	{
	  return -1;
	}
    }
  sort_map();
  return 1;
}

void init_matcher(char *silbenliste)
{
  int ct;
  //read syllables from file
#if DEBUG
  printf("Reading syllable list\n");
#endif
  read_syllable_list(silbenliste);
  for(ct=0;ct<syllables_count;ct++)
    {
      map[ct]=ct;
    }	     
#if DEBUG
  printf("Sorting map\n");
#endif
  sort_map();
}
