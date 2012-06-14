#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "hash.hpp"
#include "pstring.hpp"

template <typename T>
HashElement<T>::HashElement()
{
  used=false;
  chain=NULL;
}

// Create Hash by loading a stored Hash
template <typename T>
Hash<T>::Hash(char *filename)
{  
}

// Simple Hash function to find the right Hash Cell
template <typename T>
int Hash<T>::hashFunction(Pstring key)
{
  int i=0;
  int sum=0;
  for (i=0;i<key.length;i++)
    {
      sum+=key.value[i];
    }
#ifdef DEBUG
  int ret=(sum*key.length)%HASH_ARRAY_LENGTH;
  printf("Hash-Key Key %s Pos %d\n",key.value,ret);
  return ret;
#endif
  return (sum*key.length)%HASH_ARRAY_LENGTH;
}

// Changes a Value if already existant
template <typename T>
int Hash<T>::changeElement(Pstring key, T value)
{
  // Position in the Array
  int pos=hashFunction(key);
#ifdef DEBUG
  printf("Change-Element Key %s Pos %d Value %d\n",key.value,pos,value);
#endif
  // Found the right Element
  if ((elements[pos].key.value!=NULL)&&pstring_equal(elements[pos].key,key))
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
      printf("Change-Element Element %p Key %s Child %p\n",element, element->key.value, element->chain);
#endif
      // Found the right Element
      if (element->key.value!=NULL)
      {
	if (element->key.equal(key))
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
}

// Create a new Element
template <typename T>
int Hash<T>::insertElement(Pstring key, T value)
{
  // Position in the Array
  int pos=hashFunction(key);
#ifdef DEBUG
  printf("Insert-Element Key %s Pos %d Value %d\n",key.value,pos,value);
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
  printf("Set-Value Key %s Value %d\n",key.value,value);
  return (changeElement(h,key,value)==0)||(insertElement(key,value)==0);
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
  if ((elements[pos].key.value!=NULL)&&(elements[pos].key.equal(key)))
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
      if ((least->key.value!=NULL)&&(least->key.equal(key)))
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
Pstring* Hash<T>::keys()
{
  /*
  // Array to return
  Pstring* keys=NULL;
  int keys_length;
  // Counter of found Keys
  int counter=0;
  int ct=0;
  // Initialize Array with LENGTH Elements
  keys=malloc(HASH_ARRAY_LENGTH*sizeof(Pstring));
  // Error
  if (keys==NULL)
  { 
    printf("Error malloc\n");
    exit(-1);
  }
  keys_length=HASH_ARRAY_LENGTH;
  // Iterate over the Hash Array
  for (ct=0;ct<HASH_ARRAY_LENGTH;ct++)
  {
    // Found used Cell
    if (h->elements[ct].used==1)
    {
      pstring_copy(&keys[counter],h->elements[ct].key);
      //printf("%d %s ### %s\n",counter, keys[counter],h->elements[ct].key);
      counter++;
      // Array to small for new values -> realloc
      if (counter==keys_length)
      {
        keys=realloc(keys,(keys_length+HASH_ARRAY_LENGTH)*sizeof(pstring));
        // Error
        if (keys==NULL)
        { 
          printf("Error realloc\n");
          exit(-1);
        }
        keys_length+=HASH_ARRAY_LENGTH;
      }
      // Values in the List
      if (h->elements[ct].chain!=NULL)
      {
        hash_element* element=h->elements[ct].chain;
        // Walt down the List
        while(element!=NULL)
        {
          pstring_copy(&keys[counter],element->key);
          //printf("%s\n",keys[counter]);
          counter++;
          // Array to small for new values -> realloc
          if (counter==keys_length)
          {
            keys=realloc(keys,(keys_length+HASH_ARRAY_LENGTH)*sizeof(pstring));
            // Error
            if (keys==NULL)
            { 
              printf("Error realloc\n");
              exit(-1);
            }
            keys_length+=HASH_ARRAY_LENGTH;
          }
          element=element->chain;
        }
      }
    }
  }
  // Really no Keys found
  if (counter==0)
  {
    free(keys);
    keys=NULL;
  }
  // Set empty string as a End-Marker
  else if (counter<keys_length)
  {
    keys[counter+1].length=0;
  }
  // Resize and set End-Marker
  else
  {
    keys=realloc(keys,(keys_length+1)*sizeof(char*));
    // Error
    if (keys==NULL)
    { 
      printf("Error realloc\n");
      exit(-1);
    }
    keys[counter+1].length=0;
  }
  return keys; */
  return NULL;
}

// Prints out the internal structure of the hash
template<typename T>
void Hash<T>::dump()
{
  /*
  int ct=0;
  for (ct=0;ct<HASH_ARRAY_LENGTH;ct++)
    {
      if (h->elements[ct].key.value!=NULL)
	{
	  printf("%5d - Key: %.*s Value: %ld Used: %d\n",ct,h->elements[ct].key.length,h->elements[ct].key.value,h->elements[ct].value,h->elements[ct].used);
	  if (h->elements[ct].chain!=NULL)
	    {
	      hash_element *e=h->elements[ct].chain;
	      while(e!=NULL)
		{
		  printf("      |__ Key: %.*s Value: %ld Used: %d\n",e->key.length,e->key.value,e->value,e->used);
		  e=e->chain;
		}
	    }
	}
      else
	printf("%5d - (null)\n",ct);
    }
  */
}

// Stores the hash in a file
/* File Structure:
    ________________________________________
   | HASH_ARRAY_LENGTH                      |
   |----------------------------------------|
   | h->elements[0]                         |
   | h->elements[0].key.value               |
   |----------------------------------------|
   | h->elements[0]->chain                  |
   | h->elements[0]->chain.key.value        |
   | h->elements[0]->chain->chain           |
   | h->elements[0]->chain->chain.key.value |
   | ...                                    |
   |----------------------------------------|
   | h->elements[1]                         |
   | h->elements[1].key.value               |
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
  /*  int fh;
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
	  
	  // Save the element
	  written=write(fh,&(h->elements[ct]),sizeof(hash_element));
	  if (written!=sizeof(hash_element))
	    {
	      perror("Error writing hash to file");
	      _exit(errno);
	    }
	  // Save the key for the element
	  written=write(fh,h->elements[ct].key.value,h->elements[ct].key.length);
	  if (written!=h->elements[ct].key.length)
	    {
	      perror("Error writing hash to file");
	      _exit(errno);
	    }
	  // If there is a chain attached save it as well
	  hash_element *chain=h->elements[ct].chain;
	  while(chain!=NULL)
	    {
	      // Save the element
	      written=write(fh,chain,sizeof(hash_element));
	      if (written!=sizeof(hash_element))
		{
		  perror("Error writing hash to file");
		  _exit(errno);
		}
	      // Save the key for the element
	      written=write(fh,chain->key.value,chain->key.length);
	      if (written!=chain->key.length)
		{
		  perror("Error writing hash to file");
		  _exit(errno);
		}
	      chain=chain->chain;
	    }
	}
    }
  */
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
