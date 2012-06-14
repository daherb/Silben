#ifndef _PSTRING_H
#define _PSTRING_H 1

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

class Pstring {
 private:
  // string length
  uint32_t length;
  // string content
  char *value;
 public:
  // Initializes a pstring struct with default values
  Pstring();
  
  // Frees the value field of a pstring struct
  ~Pstring();
    
  // Copy constructor
  Pstring(const Pstring &p);

  // Sets a string value
  Pstring& operator=(const char *c);
  Pstring& operator=(const Pstring &p);
  
  // Returns the string length
  int getLength() const;
  // Returns the value as a char*
  char *getValue() const;
};

// Compares two strings for equality
bool operator==(const Pstring &s1, const Pstring &s2);

// Initializes a pstring struct with default values 
Pstring::Pstring()
{
  value=NULL;
  length=0;
}

// Sets a string value
// Returns the string length
Pstring::Pstring(const Pstring &p)
{
  length=p.length;
  // if value is empty you don't need to copy
  if (length==0)
    return;
  value=(char *)malloc(length);
  memcpy(value,p.value,length);
}

// Frees the value field of a pstring struct
Pstring::~Pstring()
{
  if (value!=NULL)
    {
#ifdef DEBUG
      printf("Destructor %p Value %s Length %d\n",value,value,length); 
#endif
      free(value);
      value=NULL;
    }
#ifdef DEBUG
  else
    {
      printf("Destructor No value\n");
    }
#endif
  length=0;
}



// Sets a string value
// Returns the string length
Pstring& Pstring::operator=(const char *c)
{
  // if it already has a value remove it
  if (value!=NULL)
  {
#ifdef DEBUG
    printf("Assign *char to %p Value %s Length %d\n",value,value,length);
#endif
    free(value);
    value=NULL;
  }
  length=strlen(c);
  // if value is empty you don't need to copy
  if (length==0)
    return *this;
  value=(char *)malloc(length+1);
  memcpy(value,c,length+1);
  return *this;
}

// Sets a string value
// Returns the string length
Pstring& Pstring::operator=(const Pstring &p)
{
  // if it already has a value remove it
  if (value!=NULL)
  {
#ifdef DEBUG
    printf("Assign Pstring to %p Value %s Length %d\n",value,value,length);
#endif
    free(value);
    value=NULL;
  }
  length=p.length;
  // if value is empty you don't need to copy
  if (length==0)
    return *this;
  value=(char *)malloc(length);
  memcpy(value,p.value,length);
  return *this;
}

int Pstring::getLength() const
{
  return length;
}

char* Pstring::getValue() const
{
  return value;
}
// Tests two strings for equalty
bool operator==(const Pstring &s1, const Pstring &s2)
{
#ifdef DEBUG
  int cmp1=(s1.getLength()==s2.getLength());
  int cmp2=(strncmp(s1.getValue(),s2.getValue(),s1.getLength())==0);
  int cmp=cmp1&&cmp2;
  printf("Pstring-Equal %s Length %d and %s Length %d is %d&&%d=%d\n",s1.getValue(),s1.getLength(),s2.getValue(),s2.getLength(),cmp1,cmp2,cmp);
#endif
  return (s1.getLength()==s2.getLength())&&(strncmp(s1.getValue(),s2.getValue(),s1.getLength())==0);
}
#endif
