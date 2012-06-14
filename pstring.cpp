#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "pstring.hpp"

// Initializes a pstring struct with default values 
Pstring::Pstring()
{
  value=NULL;
  length=0;
}

// Frees the value field of a pstring struct
Pstring::~Pstring()
{
  if (value!=NULL)
    free(value);
  Pstring();
}

// Tests two strings for equalty
// Returns 1 if s1 equals s2 and 0 else
int Pstring::equal(Pstring s1, Pstring s2)
{
#ifdef DEBUG
  int cmp1=(s1.length==s2.length);
  int cmp2=(strncmp(s1.value,s2.value,s1.length)==0);
  int cmp=cmp1&&cmp2;
  printf("Pstring-Equal %s Length %d and %s Length %d is %d&&%d=%d\n",s1.value,s1.length,s2.value,s2.length,cmp1,cmp2,cmp);
#endif
  return (s1.length==s2.length)&&(strncmp(s1.value,s2.value,s1.length)==0);
}

// Sets a string value
// Returns the string length
Pstring& Pstring::operator=(char* const &c)
{
  length=strlen(c);
  // if it already has a value remove it
  if (value!=NULL)
  {
    free(value);
    value=NULL;
  }
  // if value is empty you don't need to copy
  if (length==0)
    return *this;
  value=(char *)malloc(length+1);
  memcpy(value,c,length+1);
  return *this;
}

// Sets a string value
// Returns the string length
Pstring& Pstring::operator=(Pstring const &p)
{
  length=p.length;
  // if it already has a value remove it
  if (value!=NULL)
  {
    free(value);
    value=NULL;
  }
  // if value is empty you don't need to copy
  if (length==0)
    return *this;
  value=(char *)malloc(length);
  memcpy(value,p.value,length);
  return *this;
}

int Pstring::getLength()
{
  return length;
}
