#include <fstream>
#include <iostream>
#include <unordered_map>

using namespace std;

/*
 *  Name:      google_frequncy_list.c
 *  Author:    Herbert Lange
 *  Descriton: Creates a simple frequency list from the google word list given bythe first command line parameter
 *  saves it to the file given by second parameter
 *
 */

int main(int argc, char **argv)
{
  unordered_map<string,unsigned long long int> freq_list;
  string word;
  int year;
  int match_count;
  int page_count;
  int volume_count;
  unsigned long long int ct=0;
  ifstream in;
  for (int ct=0; ct<10; ct++)
    {
      string fn="googlebooks-ger-all-1gram-20090715-"+to_string(ct)+string(".csv");
      in.open(fn.c_str());
      while(!in.eof())
	{
	  in >> word >> year >> match_count >> page_count >> volume_count;
	  if(ct%100000==0)
	    cerr << ct << endl;
	  ct++;
	  // Set all to lowercase
	  for (int ct2=0; ct2<word.length(); ct2++)
	    word[ct2]=tolower(word[ct2]);
	  if (word[0]>='a'&&word[0]<='z')
	    {
#ifdef DEBUG
	      cerr << word << endl;
#endif
	      freq_list[word]+=match_count;
	    }
	}
      in.close();
    }
  for ( auto it = freq_list.begin(); it != freq_list.end(); ++it )
    std::cout << it->first << " " << it->second << std::endl;
  return 0;
}
