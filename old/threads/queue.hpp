#ifndef _QUEUE_H
#define _QUEUE_H 1

#include<stdlib.h>
#include<stdio.h>

template <typename T> 
class QueueElement {
public:
  // Next element in the queue
  struct QueueElement *next;
  // Tha value for the queue element
  T value;
  // Creates a new queue element
  QueueElement();
};

template <typename T> 
class Queue {
public:
  // First element in the queue
  QueueElement<T> *first;
  // Last element in the queue
  QueueElement<T> *last;
  // Creates a new queue
  Queue();
  // Frees all memory in the queue
  ~Queue();
  // Adds a new element at the end of the queue
  void add(const T &value);
  // Removes an element from the start of the queue
  T remove();
  // Checks if Queue is empty
  bool isEmpty();

  //  void dump();
};

// Creates a new queue element
template <typename T> 
QueueElement<T>::QueueElement()
{
  next=NULL;
}

// Creates a new queue
template <typename T> 
Queue<T>::Queue()
{
  first=NULL;
  last=NULL;
}

// Frees all memory in the queue
template <typename T> 
Queue<T>::~Queue()
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
template <typename T> 
void Queue<T>::add(const T &val)
{
  // Empty queue -> create first element
  if(last==NULL)
    {
      first=new QueueElement<T>();
      first->value=val;
      last=first;
    }
  // Append element at the end
  else
    {
      last->next=new QueueElement<T>();
      last->next->value=val;
      last=last->next;
    }
}

// Removes an element from the start of the queue
template <typename T> 
T Queue<T>::remove()
{
  if (first==NULL)
    {
      throw -1;
    }
  else
    {
      QueueElement<T> *old=first;
      T oldval=first->value;
      first=first->next;
      delete old;
      return oldval;
    }
}

template <typename T>
bool Queue<T>::isEmpty()
{
  if (first==NULL)
    {
      return true;
    }
  else
    {
      return false;
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

#endif
