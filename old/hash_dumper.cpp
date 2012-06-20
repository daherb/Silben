#include "hash.h"
int main(int argc, char **argv)
{
  hash *h=hash_retrieve(argv[1]);
  hash_dump(h);
  return 0;
}
