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
#include "pstring_queue.h"

/*
 *  Name:      score.c
 *  Author:    Herbert Lange
 *  Descriton: Computes scores for all the syllables in file given by first command line parameter
 *  according to the google books frequency list given with second parameter
 *
 */

int read_syllables(pstring_queue *q,int fh)
{
  char *buff;
  struct stat s;
  if (fstat(fh,&s)==-1)
    return errno;
  buff=malloc(s.st_size);
  int len=read(fh,buff,s.st_size);
  if (len!=s.st_size)
    return errno;
  // Start of the token
  int oldpos=0;
  // Current position
  int pos=0;
  pstring token;
  pstring_init(&token);
  for (pos=0;pos<s.st_size;pos++)
    {
      if (buff[pos]=='\n')
	{
	  buff[pos]='\0';
	  pstring_set_value(&token,&buff[oldpos]);
	  oldpos=pos+1;
	  pstring_queue_add(q,token);
	}
    }
  pstring_set_value(&token,&buff[oldpos]);
  pstring_queue_add(q,token);
  return 0;
}

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
      if(ct>1000000)
      	break;
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
pstring_queue *syllables;
hash *google_frequency;
int main(int argc, char **argv)
{
  // Read syllable list
  syllables=create_pstring_queue();
  {
    int fh=open(argv[1],O_RDONLY);
    if (fh==-1)
      {
	perror("Error opening syllable list");
	_exit(errno);
      }
    if (read_syllables(syllables,fh)==-1)
      {
	perror("Error reading syllable list");
	_exit(errno);
      }
    close(fh);
  }
  // Read google list and create simple frequency list
  {
    google_frequency=create_hash();
    FILE *fh=fopen64(argv[2],"r");
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
    fclose(fh);
  }
  // Compute scores 
  hash_dump(google_frequency);
  hash_store(google_frequency,argv[3]);
  return 0;
}
