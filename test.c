void bla(int **x)
{
  *x++;
}
int main()
{
  char *s="Bla";
  printf("%.*s\n",2,s);
}
