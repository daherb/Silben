#include<stdlib.h>
#include<stdio.h>
#include "queue.hpp"

// Creates a new queue element
template <class T> QueueElement<T>::QueueElement()
{
  next=NULL;
}

// Creates a new queue
template <class T> Queue<T>::Queue()
{
  first=NULL;
  last=NULL;
}

// Frees all memory in the queue
template <class T> Queue<T>::~Queue()
{
  QueueElement<T> *cur=first;
  while(cur!=NULL)
    {
      QueueElement<T> *old=cur;
      cur=cur->next;
      delete old;
    }
}

// Adds a new element at the end of the queue
template <class T> void Queue<T>::add(T val)
{
  // Empty queue -> create first element
  if(last=NULL)
    {
      first=new QueueElement<T>;
      first->value=val;
      last=first;
    }
  // Append element at the end
  else
    {
      last->next=new QueueElement<T>;
      last->next->value=val;
      last=last->next;
    }
}

// Removes an element from the start of the queue
template <class T> T Queue<T>::remove()
{
  if (first==NULL)
    {
      return NULL;
    }
  else
    {
      QueueElement<T> *old=first;
      T oldval=first->value;
      first=first->next;
      delete first;
      return oldval;
    }
}


/*template <class T> void Queue<T>::dump()
{
    pstring_queue_element *addr=q->first;
  printf("%p\n",addr);
  while(addr!=NULL)
    {
      printf("Current Address: %p\nFirst Address: %p\nLast Address: %p\nNext Adress: %p\nValue: %.*s\nSize: %d\n--------------------\n",addr,q->first,q->last,addr->next,addr->value->length,addr->value->value,sizeof(pstring_queue));
      addr=addr->next;
      }
}*/
