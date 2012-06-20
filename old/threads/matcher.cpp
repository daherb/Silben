#include <cstddef>
#include <cstdio>
#include <cstring>
#include <errno.h>
#include <limits.h>
#include <pthread.h>
#include "pair.hpp"
#include "hash.hpp"
#include "matcher.hpp"


#define THREAD_COUNT 4  // Number of Cores of a host
#define SYLLABLE_LIST_FILE (char *) "silbenliste.txt.200"
#define FREQ_LIST_FILE (char *) "frequenzliste.txt.200"
#define RESULT_FILE (char *) "ergebnis.hash"
// Total number of Data Elements
int data_count=freq_count;

// Current possition in the array
int current_data=0;

// Mutex
pthread_mutex_t mutex;

Hash<unsigned long long int> result;

static void *worker(void *args)
{
#ifdef DEBUG
  //  if (current_data % 20000 ==0)
    printf("%d of %d\n",current_data,data_count);
#endif
    vector<int> *map=NULL;
    vector<Pair *> *syllables=NULL;
    init_matcher(SYLLABLE_LIST_FILE,map,syllables,&mutex);
    int mydata=0;
    while (mydata<data_count)
      {
	pthread_mutex_lock(&mutex);
	mydata=current_data;
	current_data++;
	pthread_mutex_unlock(&mutex);
	match(&(freq_list[mydata]->word),freq_list[mydata]->count,map, syllables);
      }
    delete map;
    pthread_exit(syllables);
    return syllables;
}


int main(int argc, char* argv[])
{
  // Read the frequency list
  //  init_matcher(SYLLABLE_LIST_FILE);
  // Read the frequency list
  read_freq_list(FREQ_LIST_FILE);

  
  // Create mutex
  pthread_mutex_init(&mutex,NULL);

  // Create threads
  pthread_t tids[THREAD_COUNT];
  for (int ct=0;ct<THREAD_COUNT;ct++)
    {
      if (pthread_create(&tids[ct],NULL,&worker,NULL)!=0)
	printf("Error creating thread %d\n",ct);
    }
  // wait for all the children
  for (int ct=0;ct<THREAD_COUNT;ct++)
    {
      vector<Pair *> *val=NULL;
      if (pthread_join(tids[ct],(void **)val)!=0)
	printf("Error joining thread %d\n",ct);
      for (unsigned int ct=0; ct<val->size(); ct++)
	result.setValue(val->at(ct)->word,result.getValue(val->at(ct)->word)+val->at(ct)->count);
    }
#ifdef DEBUG
  printf("The Result is:\n");
  result.dump();
#endif
  result.store(RESULT_FILE);
  pthread_mutex_destroy(&mutex);
  return 0;
}
