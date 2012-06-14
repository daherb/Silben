#include <stdlib.h>
#include "stack.hpp"

template<typename T>
StackElement<T>::StackElement(StackElement<T> *below)
{
  this->below=below;
}
// Initializes a empty Stack with default values
template<typename T>
Stack<T>::Stack()
{
  top=NULL;
  count=0;
}

// Puts the element on top of the stack
template<typename T>
void Stack<T>::push(T value)
{
  StackElement<T> *newtop=new StackElement<T>(top);
  newtop->value=value;
  top=newtop;
  count++;
}

// Removes the element on top of the stack and returns the value
template<typename T>
T Stack<T>::pop()
{
  if (count>0)
    {
      StackElement<T> *oldtop=top;
      T value=top->value;
      top=oldtop->below;
      count--;
      delete(top);
      return value;
    }
  else
    {
      return 0;
    }
}

// Removes all elements from the stack
template<typename T>
Stack<T>::~Stack()
{
  StackElement<T> *oldtop;
  while(count>0)
    {
      oldtop=top;
      top=oldtop->below;
      delete(oldtop);
      count--;
    }
}
