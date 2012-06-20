#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>

#ifdef DMALLOC
#include <dmalloc.h>
#endif

#include "hash.hpp"
#include "pstring.hpp"
#include "queue.hpp"

/*
 *  Name:      google_frequncy_list.c
 *  Author:    Herbert Lange
 *  Descriton: Creates a simple frequency list from the google word list given bythe first command line parameter
 *  saves it to the file given by second parameter
 *
 */


int read_frequency(Hash<int> *h,FILE *fh)
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
      //if(ct>1000000)
      //	break;
      ct++;
      //      printf("%s\t%d\t%d\t%d\t%d\n",word,year,match_count,page_count,volume_count);
      // Put word in the frequency list
      // Check if word starts with a letter
      //if (((word[0]>=65)&&(word[0]<=90))||((word[0]>=97)&&(word[0]<=122)))
	{
	  Pstring token;
	  // Set all to lowercase
	  for (int ct = 0; word[ct]; ct++)
	    word[ct] = tolower(word[ct]);
	  token=word;
	  int oldval=h->getValue(token);
#ifdef DEBUG
	  printf("Set Value for %s from %d to %d\n",token.getValue(), oldval, oldval+match_count);
#endif
	  h->setValue(token,oldval+match_count);
	}
    }
  return 0;
}

// Globale Variablen
Hash<int> *google_frequency;
int main(int argc, char **argv)
{
  // Read google list and create simple frequency list
  {
    google_frequency=new Hash<int>();
    FILE *fh=fopen64(argv[1],"r");
    if (fh==NULL)
      {
	perror("Error opening frequency list");
	_exit(errno);
      }
    if (read_frequency(google_frequency,fh)==-1)
      {
	perror("Error reading syllable list");
	_exit(errno);
      }
    fclose(fh);
  }
  // Write hash to disk
  Queue<Pstring> *keys=google_frequency->keys();
  FILE *fh=fopen(argv[2],"w");
  while (keys->isEmpty()==false)
    {
      Pstring key=keys->remove();
      fprintf(fh,"%.*s\t%d\n",key.getLength(),key.getValue(),google_frequency->getValue(key));
    }
  delete(keys);
  google_frequency->statistics();
  delete(google_frequency);
  fclose(fh);
  return 0;
}
