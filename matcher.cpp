#include <cstddef>
#include <cstdio>
#include <cstring>
#include <errno.h>
#include <limits.h>
#include <pthread.h>
#include <unordered_map>
#include <string>
#include <vector>
#include <utility>
#include <stack>
#include <iostream>
#include <fstream>
#include <algorithm>

#define THREAD_COUNT 1  // Number of Cores of a host
#define SYLLABLE_LIST_FILE (char *) "silbenliste.txt"
#define FREQ_LIST_FILE (char *) "frequenzliste.txt"
//#define RESULT_FILE (char *) "ergebnis.txt"

using namespace std;

// Criteria for std::sort: Sort the mapping so that frequent syllables are preferred to infrequent and longer syllables to shorter
static bool sort_criteria(const pair<string, unsigned long long int>  *a, const pair<string, unsigned long long int>  *b)
{
#ifdef LONG_FIRST
  return ((a->second<b->second)||((a->second==b->second)&&(a->first.length()>b->first.length())));
#endif
#ifdef SHORT_FIRST
  return ((a->second<b->second)||((a->second==b->second)&&(a->first.length()<b->first.length())));
#endif
  return 1;
}

class matcher
{
private:
  // The result
  unordered_map<string,unsigned long long int> *result;
  // Array of Word-Count-Pairs
  vector<pair<string,unsigned long long int> *> freq_list;
  // Current possition in the array
  int current_data;
  // Thread ids
  pthread_t tids[THREAD_COUNT];
  // Mutex for thread access to data
  pthread_mutex_t mutex;
  void read_syllable_list(char *filename, vector<pair<string,unsigned long long int> *> *syllables);
  void read_freq_list(char *filename);
  // Create variable for syllable list and return it
  void init_syllables(char *syllable_list, vector<pair<string,unsigned long long int> *> **syllables);
  void sort_syllables(vector<pair<string,unsigned long long int> *> *syllables);
  // Find syllables in word
  int match(string word, int score, vector<pair<string,unsigned long long int> *> *syllables);
  // Criteria for std::sort
  //  bool sort_criteria(pair<string, unsigned long long int>  *a, pair<string, unsigned long long int>  *b);
  void *thread_work();
  static void *start_thread(void *obj);
public:
  matcher();
  ~matcher();
  void wait_for_threads();
  unordered_map<string,unsigned long long int> *get_result();
  void store_result(ostream *out);
  void store_result(char *filename);
  void dump_result();
};

matcher::matcher()
{
  current_data=0;
  result=NULL;
  // Initialize vector for result
  result=new unordered_map<string, unsigned long long int>();
  // Read the frequency list
  read_freq_list(FREQ_LIST_FILE);
   // Create mutex
  pthread_mutex_init(&mutex,NULL); 
  // Create threads
  for (int ct=0;ct<THREAD_COUNT;ct++)
    {
#ifdef DEBUG
      cerr << "Starting thread " << ct << endl;
#endif
      if (pthread_create(&tids[ct],NULL,&matcher::start_thread, this)!=0)
	cerr << "Error creating thread " << ct << endl;
    }
}

void matcher::wait_for_threads()
{
  // wait for all the children
#ifdef STATUS
  cerr << "Waiting for threads" << endl;
#endif
  for (int ct=0;ct<THREAD_COUNT;ct++)
    {
      vector<pair<string,unsigned long long int> *> *val=NULL;
      if (pthread_join(tids[ct],(void **)&val)!=0)
	cerr << "Error joining thread" << ct << endl;
      for (unsigned int ct=0; ct<val->size(); ct++)
	(*result)[val->at(ct)->first]+=val->at(ct)->second;
    }
}

matcher ::~matcher()
{
  // Free mutex ... and tibet ;)
  pthread_mutex_destroy(&mutex);
}


void matcher::read_syllable_list(char *filename, vector<pair<string,unsigned long long int> *> *syllables)
{
#ifdef STATUS
  cerr << "Reading syllable frequency list" << endl;
#endif
  ifstream in;
  in.open(filename);
  string word;
  while (!in.eof())
    {
      in >> word;
#ifdef DEBUG2
      cerr << "Added " << word << endl;
#endif
      syllables->push_back(new pair<string, unsigned long long int>(word, 0));
    }
  in.close();
}

// Read the syllables from a file into the vector
void matcher::read_freq_list(char *filename)
{
#ifdef STATUS
  cerr << "Reading word frequency list" << endl;
#endif
  ifstream in;
  in.open(filename);
  string word;
  unsigned long long int count;
  while (!in.eof())
  {
    in >> word >> count;
#ifdef DEBUG2
    cerr << "Added " << word << " Count " << count << endl;
#endif
    freq_list.push_back(new pair<string, unsigned long long int>(word,count));
  }
  in.close();
}

// Returns 1 on success and -1 on error
void matcher::sort_syllables(vector<pair<string, unsigned long long int>  *> *syllables)
{
  sort(syllables->begin(),syllables->end(),sort_criteria);
}

// Updates syllable score by word and word score
int matcher::match(string word, int score, vector<pair<string, unsigned long long int> *> *syllables)
{
#ifdef DUMB
  for (unsigned int ct=0;ct<syllables->size();ct++)
    {
      size_t pos=word.find((*syllables)[ct]->first);
      if (pos>0)
	{
	  unsigned long long int oldscore=(*syllables)[ct]->second;
	  (*syllables)[ct]->second+=score;
	  if ((*syllables)[ct]->second<oldscore)
	    return -1;
#ifdef DEBUG2
  cerr << "Got syllable " << ct <<" - " << (*syllables)[ct]->first << " with new count " << (*syllables)[ct]->second <<endl;
#endif
	}
    }
#else
  // Stack for the positions and syllable number
  vector<int> history;
  unsigned int curpos=0;
  int start=0;
  start=0;
  curpos=0;
#ifdef DEBUG
  cerr << "Trying to match " << word << endl;
#endif
    while (curpos<word.length())
    {
      unsigned int ct;
      int match=0;
      for (ct=start;ct<syllables->size();ct++)
	{
#ifdef DEBUG2
  cerr << "Trying syllable " << ct <<" - " << (*syllables)[ct]->first << " at Position " << curpos <<endl;
#endif
	  // Syllable is prefix of the substring starting at curpos
	  if (word.substr(curpos,(*syllables)[ct]->first.length())==(*syllables)[ct]->first)
	    {
	      match=1;
#ifdef DEBUG2
	      cerr << "Matched syllable " << (*syllables)[ct]->first << " at position " << curpos << endl;
#endif
	      history.push_back(curpos);
#ifdef DEBUG2
	      cerr << "Pushing " << curpos << " onto stack with size " << history.size() << endl;
#endif
	      history.push_back(ct);
	      curpos=curpos+(*syllables)[ct]->first.length();
	      break;
	      }
	}
      // No matching syllable found -> backtracking
      if (match==0 && !history.empty())
	{
	  start=history.back()+1;
	  history.pop_back();
	  curpos=history.back();
	  history.pop_back();
#ifdef DEBUG2
	  cerr << "Backtrack to syllable "<< start <<" at position " << curpos << endl;
#endif
	}
      // Start again with first syllable
      else if (curpos>0)
	{
	  start=0;
	}
      // Loop -> break
      else
	curpos=word.length()+1;
    }
    if (curpos>word.length())
      cerr << "Unable to match word " << word << endl;
#ifdef DEBUG
    cerr <<"Clean history" << endl;
#endif
  while(!history.empty())
    { 
      // get the syllable number
      int pos=history.back();
#ifdef DEBUG2
      cerr << "Pop-ing " << pos << " from stack with size " << history.size() << endl;
#endif
      history.pop_back();
      // ignore the saved string position
      history.pop_back();
      unsigned long long int oldcount=(*syllables)[pos]->second;
      (*syllables)[pos]->second+=score;
      if (oldcount>(*syllables)[pos]->second)
	{
	  return -1;
	}
    }
  sort_syllables(syllables);
#endif
  return 1;
}

void matcher::init_syllables(char *syllable_list, vector<pair<string, unsigned long long int> *> **syllables)
{
  // All the syllables are belong to us
  *syllables=new vector<pair<string, unsigned long long int> *>();
  if (syllables==NULL)
    {
  cerr << "Unable to alloc syllable list" << endl; 
}
#if DEBUG
  cerr << "Reading syllable list" << endl;
#endif
  // No deadlock on file
  pthread_mutex_lock(&mutex);
  //read syllables from file
  read_syllable_list(syllable_list,*syllables);
  // Free again
  pthread_mutex_unlock(&mutex);
#if DEBUG
  cerr << "Sorting map" << endl;
#endif
#ifndef DUMB
  sort_syllables(*syllables);
#endif
}

// Worker for Threads
void *matcher::thread_work()
{
#ifdef STATUS
  if (current_data % 1000 ==0)
    cerr << current_data << " of " << freq_list.size() << endl;
#endif
    vector<pair<string,unsigned long long int> *> *syllables=NULL;
    init_syllables(SYLLABLE_LIST_FILE,&syllables);
    if (syllables==NULL)
      {
	cerr << "No syllable list loaded"<< endl;
      }
    unsigned int mydata=0;
    while (mydata<freq_list.size())
      {
	pthread_mutex_lock(&mutex);
	mydata=current_data;
	current_data++;
	pthread_mutex_unlock(&mutex);
	if (mydata<freq_list.size())
	  {
	    if (match(freq_list[mydata]->first,freq_list[mydata]->second, syllables)==-1)
	      cerr << "Overflow in syllable counter" << endl;
	  }
      }
    return syllables;
}

void *matcher::start_thread(void *obj)
{
  //All we do here is call the do_work() function
  void *ret=reinterpret_cast<matcher *>(obj)->thread_work();
  pthread_exit(ret);
  return ret;
}

unordered_map<string,unsigned long long int> *matcher::get_result()
{
  return result;
}

// Prints out the internal structure of the hash
void matcher::dump_result()
{
  store_result(&cout);
}

// Store result in file with filename
void matcher::store_result(char *filename)
{
  ofstream out;
  out.open(filename);
  store_result(&out);
}

// Store result in file with outstream
void matcher::store_result(ostream *out)
{
  for ( unsigned i = 0; i < (*result).bucket_count(); ++i) {
    for ( auto local_it = (*result).begin(i); local_it!= (*result).end(i); ++local_it )
      *out << i <<":" << local_it->first << ":" << local_it->second <<  std::endl;
  }
}

int main(int argc, char* argv[])
{
  
  matcher m;
  m.wait_for_threads();
#ifdef DEBUG
  cerr << "The Result is:" << endl;
  m.store_result(&cerr);
#endif
  m.store_result(RESULT_FILE);
  return 0;
}
