/*****************************************************************************
*
* This source code is distributed under the GPLv3 license, COPYING.
*
* Author: capi_x <capi_x@haibane.org>
* Based in the code of:
*  Harald Schmidl <hjschmidl@acm.org>
*  Fauzan Mirza's <fauzan@dcs.rhbnc.ac.uk>
*
******************************************************************************/

#include "cipher.h"

/*run on all permutations of wheels a, b, c*/
void permute(int a, int b, int c, char *cyph, char *crib, int *ct)
{
  test(a, b, c, cyph, crib, ct);
  test(a, c, b, cyph, crib, ct);
  test(b, a, c, cyph, crib, ct);
  test(b, c, a, cyph, crib, ct);
  test(c, a, b, cyph, crib, ct);
  test(c, b, a, cyph, crib, ct);
}

/*all triples of five possible wheels*/
void permuteAll(char *cyph, char *crib)
{
  int ct = 0;

  permute(1, 2, 3, cyph, crib, &ct);
  permute(1, 2, 4, cyph, crib, &ct);
  permute(1, 2, 5, cyph, crib, &ct);
  permute(1, 3, 4, cyph, crib, &ct);
  permute(1, 3, 5, cyph, crib, &ct);
  permute(1, 4, 5, cyph, crib, &ct);
  permute(2, 3, 4, cyph, crib, &ct);
  permute(2, 3, 5, cyph, crib, &ct);
  permute(2, 4, 5, cyph, crib, &ct);
  permute(3, 4, 5, cyph, crib, &ct);

  printf("\nFound %d solutions.\n", ct);
}

/*helper to read a character*/
char readCh()
{
  char c, ret;

  while((c = getchar()) != '\n')
  ret = c;
  return ret;
}

/*init the starting position*/
void initParams(Params *p)
{
  int i;
  char c;

  printf("d)efault or u)ser: ");
  c = readCh();
  if(c != 'u')
  {
    for(i = 2; i >= 0; i--)
    {
      p->order[i] = 3 - i;
      p->rings[i] = 'A';
      p->pos[i] = 'A';
    }
    strcpy(p->plug, "");
  }
  else
  {
      for(i = 2; i >= 0; i--)
      {
        printf("Wheel %d: ", 3 - i);
        p->order[i] = readCh() - 48;
      }
      for(i = 2; i >= 0; i--)
      {
        printf("Ring  %d: ", 3 - i);
        p->rings[i] = readCh();
      }
      for(i = 2; i >= 0; i--)
      {
        printf("Start %d: ", 3 - i);
        p->pos[i] = readCh();
      }
      printf("Stecker: ");
      i = 0;
      while((c = getchar()) != '\n')
      {
        p->plug[i] = c;
	i++;
      }
      p->plug[i] = '\0';
  }
  printf("Wheels %d %d %d Start %c %c %c Rings %c %c %c Stecker \"%s\"\n",
         p->order[2], p->order[1], p->order[0], 
         p->pos[2], p->pos[1], p->pos[0],
         p->rings[2], p->rings[1], p->rings[0], p->plug);
}


int main(int argc, char *argv[])
{
  Params p;

  if(argc > 2)  /*bombe case*/
  {
    permuteAll(argv[1], argv[2]);
  }
  else
  {
    initParams(&p);
    cypher(p);
  }

  return(0);
}


