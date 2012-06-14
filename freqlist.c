#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "hash.h"
#include "pstring.h"

/*
 *  Name:      freqlist.c
 *  Author:    Herbert Lange
 *  Descriton: Generates the frequency list from the google 1-gram-list and saves it as a hash dump
 *
 */

int read_frequency(hash *h,FILE *fh)
{
  char word[255];
  int year;
  int match_count;
  int page_count;
  int volume_count;
  long long int ct=0;
  while(fscanf(fh,"%s\t%d\t%d\t%d\t%d",word,&year,&match_count,&page_count,&volume_count)>0)
    {
      if(ct%100000==0)
      	printf("%lld\n",ct);
      //      if(ct>1000000)
      //      	break;
      ct++;
      //      printf("%s\t%d\t%d\t%d\t%d\n",word,year,match_count,page_count,volume_count);
      // Put word in the frequency list
      // Check if word starts with a letter
      if (((word[0]>=65)&&(word[0]<=90))||((word[0]>=97)&&(word[0]<=122)))
	{
	  pstring *token=create_pstring();
	  pstring_set_value(token,word);
	  hash_set_value(h,*token,match_count+hash_get_value(h,*token));
	}
    }
  return 0;
}

// Globale Variablen
hash *google_frequency;
int main(int argc, char **argv)
{

  // Read google list and create simple frequency list
  {
    google_frequency=create_hash();
    FILE *fh=fopen(argv[1],"r");
    if (fh==NULL)
      {
	perror("Error opening frequency list list");
	_exit(errno);
      }
    if (read_frequency(google_frequency,fh)==-1)
      {
	perror("Error reading syllable list");
	_exit(errno);
      }
  }
  // Compute scores 
  hash_dump(google_frequency);
  hash_store(google_frequency,argv[2]);
  return 0;
}
