#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "pstring.h"
#include "pstring_queue.h"
#include "int_stack.h"
/*
 *  Name:      matcher.c
 *  Author:    Herbert Lange
 *  Descriton: Matching function to match an input word
 *  with an array of possible substrings
 *
 */

#define MAX_VAL 9
int map[MAX_VAL];
struct pair {
  pstring word;
  int count;
} syllables[MAX_VAL];

// Sort the mapping so that frequent syllables are preferred to infrequent and longer syllables to shorer
void sort_map()
{
  int ct;
  for (ct=0;ct<MAX_VAL;ct++)
    {
      map[ct]=ct;
    }
  int n=MAX_VAL;
  int swapped;
  do
    {
      swapped=0;
      int ct=0;
      for(ct=0;ct<n-1;ct++)
	{
	  if ((syllables[map[ct]].count<syllables[map[ct+1]].count)||((syllables[map[ct]].count==syllables[map[ct+1]].count)&&(syllables[map[ct]].word.length<syllables[map[ct+1]].word.length)))
	    {
	      map[ct]^=map[ct+1];
	      map[ct+1]^=map[ct];
	      map[ct]^=map[ct+1];
	      swapped=1;
	    }	 
	}
      n=n-1;
    }
  while(swapped==1);
}

void match(pstring word, int score)
{
  // Stack for the positions and syllable number
  int_stack history;
  int curpos=0;
  int start=0;
  int_stack_init(&history);
  start=0;
  curpos=0;
#ifdef DEBUG
  printf("Trying to match %.*s\n",word.length,word.value);
#endif
  while (curpos!=word.length)
    {
      int ct;
      int match=0;
      for (ct=start;ct<MAX_VAL;ct++)    
	{
#ifdef DEBUG
	  printf("Trying syllable %d\n",ct);
#endif
	  // Syllable is prefix of the substring starting at curpos
	  if (strncmp(&word.value[curpos],syllables[map[ct]].word.value,syllables[map[ct]].word.length)==0)
	    {
	      match=1;
#ifdef DEBUG
	      printf("Matched syllable %.*s at position %d\n",syllables[map[ct]].word.length,syllables[map[ct]].word.value,curpos);
#endif
	      int_stack_push(&history,curpos);
	      int_stack_push(&history,ct);
	      curpos=curpos+syllables[map[ct]].word.length;
	      break;
	    }
	}
      // No matching syllable found -> backtracking
      if (match==0)
	{
	  start=int_stack_pop(&history)+1;
	  curpos=int_stack_pop(&history);
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
  while(history.count>0)
    { 
      // get the syllable number
      int pos=int_stack_pop(&history);
      // ignore the saved string position
      int_stack_pop(&history);
      int oldcount=syllables[map[pos]].count;
      syllables[map[pos]].count+=score;
      if (oldcount>syllables[map[pos]].count)
	{
	  perror("Feuer Feuer Feuer\n");
	}
    }
  sort_map();
  int_stack_free(&history);
}

int main(int argc,char **argv)
{
  int ct;
  for(ct=0;ct<MAX_VAL;ct++)
    {
      map[ct]=ct;
      pstring_init(&syllables[ct].word);
      syllables[ct].count=0;
    }
  pstring_set_value(&syllables[0].word,"a");
  pstring_set_value(&syllables[1].word,"aa");
  pstring_set_value(&syllables[2].word,"aaa");
  pstring_set_value(&syllables[3].word,"b");
  pstring_set_value(&syllables[4].word,"bb");
  pstring_set_value(&syllables[5].word,"bbb");
  pstring_set_value(&syllables[6].word,"c");
  pstring_set_value(&syllables[7].word,"cc");
  pstring_set_value(&syllables[8].word,"ccc");
  char *line=malloc(255);
  pstring pline;
  pstring_init(&pline);
  ct=0;
  sort_map();
  while(1)
    {
      scanf("%s",line);
      pstring_set_value(&pline,line);
      match(pline,ct);
      ct++;
      int ct2;
      for (ct2=0;ct2<MAX_VAL;ct2++)
	{
	  printf("%d\t%.*s\t%d\n",ct2,syllables[map[ct2]].word.length,syllables[map[ct2]].word.value,syllables[map[ct2]].count);
	}
    }
  free(line);
  pstring_free(&pline);
  return 0;
}
