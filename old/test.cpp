#include "stack.hpp"
#include "pstring.hpp"
#include "queue.hpp"
#include "hash.hpp"

int main(int argc, char **argv)
{
  Pstring tmp;
  tmp="hallo";
  Pstring t2;
  t2=tmp;
  tmp="welt";
  Hash<int> h;
  h.setValue(tmp,1);
  h.setValue(t2,2);
  h.store("test.bla");
  /*Queue<Pstring> *keys=h.keys();
  while (keys->isEmpty()==false)
    {
      Pstring key=keys->remove();
      printf("%.*s\t%d\n",key.getLength(),key.getValue(),h.getValue(key));
      if (keys->isEmpty())
	printf("Lööör\n");
    }
    delete(keys);*/
}
