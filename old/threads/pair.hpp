#ifndef _PAIR_HPP
#define _PAIR_HPP 1
#include "pstring.hpp"

class Pair {
public:
  Pstring word;
  unsigned long long int count;

  Pair(Pstring s)
  {
    word=s;
    count=0;
  }
};
#endif
