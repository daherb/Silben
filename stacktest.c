#include "stack.hpp"
#include "pvm/pair.hpp"
int main()
{
  Stack<Pair *> s;
  Pair *p=new Pair(Pstring("eins"));
  p->count=1;
  s.push(p);
  /*  p=new Pair(Pstring("zwei"));
  p->count=2;
  s.push(p);*/
  p=s.pop();
  p=s.pop();
  return p->count;
}
