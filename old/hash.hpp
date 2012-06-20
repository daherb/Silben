#ifndef _HASH_H
#define _HASH_H 1

#include <stdint.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "pstring.hpp"
#include "queue.hpp"

//#define HASH_ARRAY_LENGTH 4096
#define HASH_ARRAY_LENGTH 65535

template <typename T>
class HashElement {
public:
  // Attributes
  // Value
  T value;
  // Index Key
  Pstring key;
  // If Array element already used
  bool used;
  // Chain to store collisions
  HashElement* chain;

  // Methods
  // Creates new HashElement
  HashElement();
};

template <typename T>
class Hash {
private:
  int hashFunction(Pstring key);
  int changeElement(Pstring key, T value);
  int insertElement(Pstring key, T value);
  unsigned char t[256]={  1,  14, 110,  25,  97, 174, 132, 119, 138, 170, 125, 118,  27, 233, 140,  51,
      87, 197, 177, 107, 234, 169,  56,  68,  30,   7, 173,  73, 188,  40,  36,  65,
      49, 213, 104, 190,  57, 211, 148, 223,  48, 115,  15,   2,  67, 186, 210,  28, 
      12, 181, 103,  70,  22,  58,  75,  78, 183, 167, 238, 157, 124, 147, 172, 144,
     176, 161, 141,  86,  60,  66, 128,  83, 156, 241,  79,  46, 168, 198,  41, 254,
     178,  85, 253, 237, 250, 154, 133,  88,  35, 206,  95, 116, 252, 192,  54, 221,
     102, 218, 255, 240,  82, 106, 158, 201,  61,   3,  89,   9,  42, 155, 159,  93,
     166,  80,  50,  34, 175, 195, 100,  99,  26, 150,  16, 145,   4,  33,   8, 189,
     121,  64,  77,  72, 208, 245, 130, 122, 143,  55, 105, 134,  29, 164, 185, 194,
     193, 239, 101, 242,   5, 171, 126,  11,  74,  59, 137, 228, 108, 191, 232, 139,
       6,  24,  81,  20, 127,  17,  91,  92, 251, 151, 225, 207,  21,  98, 113, 112,
      84, 226,  18, 214, 199, 187,  13,  32,  94, 220, 224, 212, 247, 204, 196,  43, 
     249, 236,  45, 244, 111, 182, 153, 136, 129,  90, 217, 202,  19, 165, 231,  71, 
     230, 142,  96, 227,  62, 179, 246, 114, 162,  53, 160, 215, 205, 180,  47, 109,
      44,  38,  31, 149, 135,   0, 216,  52,  63,  23,  37,  69,  39, 117, 146, 184,
     163, 200, 222, 235, 248, 243, 219,  10, 152, 131, 123, 229, 203,  76, 120, 209};
public:
  // Array of Hash Elements
  HashElement<T> elements[HASH_ARRAY_LENGTH];
  // Create empty hash
  Hash();
  // Create Hash by loading stored Hash from file
  Hash(char *filename);
  // Frees all memory used by Hash
  ~Hash();
  // Sets a value in the Hash
  int setValue(Pstring key, T value);
  // Returns a value stored in the Hash
  T getValue(Pstring key);
  // Prints the Values stored in the hash
  void dump();
  // Stores the Hash into a file
  void store(char *filename);
  // Returns list of keys
  Queue<Pstring> *keys();
  // Prints some statistics about the hash usage
  void statistics();
};



template <typename T>
HashElement<T>::HashElement()
{
  used=false;
  chain=NULL;
}

template <typename T>
Hash<T>::Hash()
{
#ifdef DEBUG
  printf("Table Start %p End %p\n",&t[0],&t[255]);
  for (int i=0;i<256;i++)
    {
      printf("Table Cell %d\tValue %d\n",i,t[i]);;
    }
#endif
}

// Create Hash by loading a stored Hash
template <typename T>
Hash<T>::Hash(char *filename)
{  
}

template <typename T>
Hash<T>::~Hash()
{
  for(int ct=0;ct<HASH_ARRAY_LENGTH;ct++)
    {
      if (elements[ct].chain!=NULL)
	{
	  HashElement<T> *last=elements[ct].chain;
	  while(last!=NULL)
	    {
	      HashElement<T> *next=last->chain;
	      delete(last);
	      last=next;
	    }
	}
    }
}
// Simple Hash function to find the right Hash Cell
template <typename T>
int Hash<T>::hashFunction(Pstring key)
{
  // Compate http://burtleburtle.net/bob/hash/pearson.html
  // and "Fast Hashing of Variable-Length Text Strings" by
  // Peter K. Pearson
#ifndef OLD
  unsigned char h1=0;
  unsigned char h2=0;
  char *ckey=key.getValue();
  for(int ct=0;ct<key.getLength();ct++)
    {
      unsigned char tmp=h1^ckey[ct];
#ifdef DEBUG
      printf("TMP %d\n",tmp);
#endif
      h1=t[tmp];
    }
  ckey[0]=(ckey[0]+1)%256;
  for(int ct=0;ct<key.getLength();ct++)
    {
      unsigned char tmp=h2^ckey[ct];
#ifdef DEBUG
      printf("TMP %d\n",tmp);
#endif
      h2=t[tmp];
    }
#ifdef DEBUG
  printf("Hash-Key Key %s H1 %d H2 %d Pos %d\n",key.getValue(),h1,h2,h2+(h1<<8));
#endif
  int ret=(h1<<8)+h2;
  return ret%HASH_ARRAY_LENGTH;
#endif
#ifdef OLD
  int i=0;
  int sum=0;
  char *val=key.getValue();
  for (i=0;i<key.getLength();i++)
    {
      sum+=val[i];
    }
#ifdef DEBUG
  int ret=(sum*key.getLength())%HASH_ARRAY_LENGTH;
  printf("Hash-Key Key %s Pos %d\n",key.getValue(),ret);
  return ret;
#endif
  return (sum*key.getLength())%HASH_ARRAY_LENGTH;
#endif

}

// Changes a Value if already existant
template <typename T>
int Hash<T>::changeElement(Pstring key, T value)
{
  // Position in the Array
  int pos=hashFunction(key);
#ifdef DEBUG2
  printf("Change-Element Key %s Pos %d Value %d\n",key.getValue(),pos,value);
  return 0;
#endif
#ifndef DEBUG2
#ifdef DEBUG
  printf("Change-Element Key %s Pos %d Value %d\n",key.getValue(),pos,value);
#endif
  // Found the right Element
  if ((elements[pos].key.getValue()!=NULL)&&(elements[pos].key==key))
  {
    elements[pos].value=value;
#ifdef DEBUG
    printf("\tChange-Element Successfull\n");
#endif
    return 0;
  }
  else
  {
    // Element to be changed
    HashElement<T>* element=elements[pos].chain;
    while(element!=NULL)
    {
#ifdef DEBUG
      printf("Change-Element Element %p Key %s Child %p\n",element, element->key.getValue(), element->chain);
#endif
      // Found the right Element
      if (element->key.getValue()!=NULL)
      {
	if (element->key==key)
        {
          element->value=value;
#ifdef DEBUG
	  printf("\tChange-Element Successfull\n");
#endif
          return 0;
        }
	else
	{
	  element=element->chain;
	}
      }
    }
  }
  #ifdef DEBUG
  printf("\tChange-Element Failed\n");
  #endif
  return -1;
#endif
}

// Create a new Element
template <typename T>
int Hash<T>::insertElement(Pstring key, T value)
{
  // Position in the Array
  int pos=hashFunction(key);
#ifdef DEBUG
  printf("Insert-Element Key %s Pos %d Value %d\n",key.getValue(),pos,value);
#endif
  // Element to be changed
  HashElement<T>* element;
  //  printf("%d\n",pos);
  // Unused Cell
  if (elements[pos].used==false)
  {
    // Set Used and Value
    element=&(elements[pos]);
  }
  // Used Cell
  else
  {
    // Unused Chain
    if (elements[pos].chain==NULL)
    {
      // Attach Element to Chain
      elements[pos].chain=new HashElement<T>();
      element=elements[pos].chain;
    }
    // Used Chain -> Search first free Place
    else
    {
      // Pointer to the least seen pointer
      HashElement<T>* least=elements[pos].chain;
      // Find last Element in Chain
      while (least!=NULL)
      {
        if (least->chain!=NULL)
        {
          least=least->chain;
        }
        else
        {
          least->chain=new HashElement<T>();
          element=least->chain;
          // End Loop
          break;
        }
      }
    }
  }
  // Change Value;
  element->value=value;
  element->used=1;
  element->chain=NULL;
  element->key=key;
#ifdef DEBUG
  printf("\tInsert-Element Successfull\n");
#endif
  return 0;  
}

// Sets a Value in the Hash
template <typename T>
int Hash<T>::setValue(Pstring key, T value)
{
#ifndef DEBUG
  return (changeElement(key,value)==0)||(insertElement(key,value)==0);
#else
  printf("Set-Value Key %s Value %d\n",key.getValue(),value);
  return (changeElement(key,value)==0)||(insertElement(key,value)==0);
#endif
  //return insert_element(h,key,value);
}

// Gets a Value from the Hash
// Sets errno to -1 if no element found
template <typename T>
T Hash<T>::getValue(Pstring key)
{
  int pos=hashFunction(key);
  // Element directly in Hash Cell
  //if ((h->elements[pos].key!=NULL)&&(strcmp(h->elements[pos].key,key)==0))
  if ((elements[pos].key.getValue()!=NULL)&&(elements[pos].key==key))
  {
    return elements[pos].value;
  }
  // Element in associated Chain
  else
  {
    // Least seen Element
    HashElement<T>* least=elements[pos].chain;
    while(least!=NULL)
    {
      if ((least->key.getValue()!=NULL)&&(least->key==key))
      {
        return least->value;
      }
      else
      {
        if (least->chain!=NULL)
          least=least->chain;
        else
        {
          // Failure case
          errno=-1;
          return 0;
        }
      }
    }
    // Failure case
    errno=-1;
    return 0;
  }
}

// Returns an array of the Hash Keys
// End marked by empty string (length 0)
template<typename T>
Queue<Pstring> *Hash<T>::keys()
{
  // Array to return
  Queue<Pstring> *keys=new Queue<Pstring>;
  int ct=0;
  // Iterate over the Hash Array
  for (ct=0;ct<HASH_ARRAY_LENGTH;ct++)
  {
    // Found used Cell
    if (elements[ct].used==true)
    {
      keys->add(elements[ct].key);
      // Values in the Chain
      if (elements[ct].chain!=NULL)
      {
        HashElement<T>* element=elements[ct].chain;
        // Walk down the List
        while(element!=NULL)
        {
	  keys->add(element->key);
          element=element->chain;
        }
      }
    }
  }
  return keys;
}

// Prints some statistics about the hash usage
template <typename T>
void Hash<T>::statistics()
{
  long double used_array_cells=0;
  long double cells_in_chains=0;
  for (int ct=0;ct<HASH_ARRAY_LENGTH;ct++)
    {
      if (elements[ct].used==true)
	{
	  used_array_cells++;
	  if (elements[ct].chain!=NULL)
	    {
	      HashElement<T> *e=elements[ct].chain;
	      while(e!=NULL)
		{
		  cells_in_chains++;
		  e=e->chain;
		}
	    }
	}
    }
  printf("%ld Hash entries, %ld of %d Cells used, medial %Lf Cells in a chain\n",(long int) (cells_in_chains+used_array_cells),(long int) used_array_cells,HASH_ARRAY_LENGTH,(cells_in_chains+used_array_cells)/used_array_cells);
}

// Prints out the internal structure of the hash
template<typename T>
void Hash<T>::dump()
{
  int ct=0;
  for (ct=0;ct<HASH_ARRAY_LENGTH;ct++)
    {
      if (elements[ct].used==true)
	{
	  printf("%5d - Key: %.*s Value: %ld Used: %d\n",ct,elements[ct].key.getLength(),elements[ct].key.getValue(),elements[ct].value,elements[ct].used);
	  if (elements[ct].chain!=NULL)
	    {
	      HashElement<T> *e=elements[ct].chain;
	      while(e!=NULL)
		{
		  printf("      |__ Key: %.*s Value: %ld Used: %d\n",e->key.getLength(),e->key.getValue(),e->value,e->used);
		  e=e->chain;
		}
	    }
	}
      else
	printf("%5d - (null)\n",ct);
    }
}

// Stores the hash in a file
/* File Structure:
    ________________________________________
   | HASH_ARRAY_LENGTH                      |
   |----------------------------------------|
   | h->elements[0].value: T                |
   | h->elements[0].used: boolean           |
   | h->elements[0].key.length: int         |
   | h->elements[0].key.value: char[length] |
   | h->elements[0].chain                   |
   |----------------------------------------|
   | h->elements[0]->chain.value            |
   | h->elements[0]->chain.used             |
   | h->elements[0]->chain.key.length       |
   | h->elements[0]->chain.key.value        |
   | h->elements[0]->chain.chain            |
   | h->elements[0]->chain->chain.key.value |
   | ...                                    |
   |----------------------------------------|
   | h->elements[0].value: T                |
   | h->elements[0].used: boolean           |
   | h->elements[0].key.length: int         |
   | h->elements[0].key.value: char[length] |
   | h->elements[0].chain                   |
   | ...                                    |
   |----------------------------------------|

There is no chain in the file if the chain pointer
in the element is NULL
There are no more elements in a chain if the pointer
to the next chain element is NULL
*/
template<typename T>
void Hash<T>::store(char *fn)
{
  int fh;
  // Open output file
  fh=open(fn, O_WRONLY | O_CREAT | O_EXCL,0644);
  if (fh==-1)
    {
      perror("Error opening output file for hash");
      _exit(errno);
    }
  else
    {
      // Save the internal array length
      int len=HASH_ARRAY_LENGTH;
      size_t written=write(fh,&(len),sizeof(int));
      if (written!=sizeof(int))
	{
	  perror("Error writing hash to file");
	  _exit(errno);
	}
      // Save all array elements to file
      int ct=0;
      for(ct=0;ct<HASH_ARRAY_LENGTH;ct++)
	{
	  /*// Save the value
	  written=write(fh,&(elements[ct].value),sizeof(elements[ct].value));
	  if (written!=sizeof(elements[ct].value))
	    {
	      perror("Error writing value to file");
	      _exit(errno);
	    }
	  // Save the used-value
	  written=write(fh,&elements[ct].used,sizeof(elements[ct].used));
	  if (written!=sizeof(elements[ct].used))
	    {
	      perror("Error writing used-value to file");
	      _exit(errno);
	    }
	  // Save the key size
	  int size=elements[ct].key.getLength();
	  written=write(fh,&size,sizeof(size));
	  if (written!=sizeof(size))
	    {
	      perror("Error writing key size to file");
	      _exit(errno);
	    }
	  // Save the key for the element
	  char *key=elements[ct].key.getValue();
	  written=write(fh,key,size);
	  if (written!=size)
	    {
	      perror("Error writing key to file");
	      _exit(errno);
	    }
	  
	  // If there is a chain attached save it as well
	  	  HashElement<T> *chain=elements[ct].chain;
	  while(chain!=NULL)
	    {
	      // Save the value
	      written=write(fh,&(chain->value),sizeof(chain->value));
	      if (written!=sizeof(chain->value))
		{
		  perror("Error writing value to file");
		  _exit(errno);
		}
	      // Save the used-value
	      written=write(fh,&(chain->used),sizeof(chain->used));
	      if (written!=sizeof(chain->used))
		{
		  perror("Error writing used-value to file");
		  _exit(errno);
		}
	      // Save the key size
	      int size=chain->key.getLength();
	      written=write(fh,&size,sizeof(size));
	      if (written!=sizeof(size))
		{
		  perror("Error writing key size to file");
		  _exit(errno);
		}
	      // Save the key for the element
	      char *key=elements[ct].key.getValue();
	      written=write(fh,key,size);
	      if (written!=size)
		{
		  perror("Error writing key to file");
		  _exit(errno);
		}
	      chain=chain->chain;
	      }*/
	}
    }
}

/*
// Loads a stored hash from a file
hash *hash_retrieve(char *fn)
{
  // create the new hash
  hash *h=create_hash();
  // Open input file
  int fh=open(fn, O_RDONLY);
  if (fh==-1)
    {
      perror("Error opening output file for hash");
      _exit(errno);
    }
  else 
    {
      int len;
      // Reading internal hash array length
      size_t received=read(fh, &len,sizeof(int));
      if (received!=sizeof(int))
	{
	  perror("Error reading hash from file");
	  _exit(errno);
	}
      // Check if array size matches
      if (len!=HASH_ARRAY_LENGTH)
	{
	  perror("Hash array size mismatch");
	  _exit(errno);
	}
      int ct=0;
      // Read all the array elements
      for (ct=0;ct<HASH_ARRAY_LENGTH;ct++)
	{
	  // Read the element
	  received=read(fh, &(h->elements[ct]),sizeof(hash_element));
	  if (received!=sizeof(hash_element))
	    {
	      perror("Error reading hash from file-1");
	      _exit(errno);
	    }
	  // Read the key for the element
	  h->elements[ct].key.value=malloc(h->elements[ct].key.length);
	  received=read(fh, h->elements[ct].key.value,h->elements[ct].key.length);
	  if (received!=h->elements[ct].key.length)
	    {
	      perror("Error reading hash from file0");
	      _exit(errno);
	    }
	  hash_element *chain=h->elements[ct].chain;
	  while(chain!=NULL)
	    {
	      chain=malloc(sizeof(hash_element));
	      // Read the element
	      received=read(fh, chain,sizeof(hash_element));
	      if (received!=sizeof(hash_element))
		{
		  //		  printf("--> received %d should be %d\n",received,sizeof(hash_element));
		  perror("Error reading hash from file1");
		  _exit(errno);
		}
	      // Read the key for the element
	      chain->key.value=malloc(chain->key.length);
	      received=read(fh, chain->key.value, chain->key.length);
	      if (received!=chain->key.length)
		{
		  perror("Error reading hash from file2");
		  _exit(errno);
		}
	      chain=chain->chain;
	    }
	}
    }
  return h;
}
*/
#endif
