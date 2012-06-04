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

float cpercent = 0;
int errora = 1;

/*run on all permutations of wheels a, b, c*/
void permute(int a, int b, int c, char *cyph, char *crib, int *ct)
{
  fprintf(stderr,"[I]  %02i%% Completed - Checking wheels %i%i%i\n",(int)cpercent,a,b,c);
  test(a, b, c, cyph, crib, ct, errora);
  cpercent += 1.66;

  fprintf(stderr,"[I]  %02i%% Completed - Checking wheels %i%i%i\n",(int)cpercent,a,c,b);
  test(a, c, b, cyph, crib, ct, errora);
  cpercent += 1.66;

  fprintf(stderr,"[I]  %02i%% Completed - Checking wheels %i%i%i\n",(int)cpercent,b,a,c);
  test(b, a, c, cyph, crib, ct, errora);
  cpercent += 1.66;

  fprintf(stderr,"[I]  %02i%% Completed - Checking wheels %i%i%i\n",(int)cpercent,b,c,a);
  test(b, c, a, cyph, crib, ct, errora);
  cpercent += 1.66;

  fprintf(stderr,"[I]  %02i%% Completed - Checking wheels %i%i%i\n",(int)cpercent,c,a,b);
  test(c, a, b, cyph, crib, ct, errora);
  cpercent += 1.66;

  fprintf(stderr,"[I]  %02i%% Completed - Checking wheels %i%i%i\n",(int)cpercent,c,b,a);
  test(c, b, a, cyph, crib, ct, errora);
  cpercent += 1.66;
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

  printf("[I] 100%% Completed - Found %d possible solutions.\n", ct);
}

char getCleanChar() {
	char c = 'd';
	char ret = 'd';

	while((c = getchar()) != '\n') ret=c;
	return(ret);
}

/*init the starting position*/
void initParams(Params *p)
{
  int i;
  char c = 'd';

  printf("\nd)efault enigma\nu)ser defined\n\n%s",PROMPT);
  c = getCleanChar();

  if(c == 'd')
  {
    for(i = 2; i >= 0; i--)
    {
      p->order[i] = 3 - i;
      p->rings[i] = 'A';
      p->pos[i] = 'A';
    }
    strlcpy(p->plug, "", sizeof(p->plug));
  } 

  if(c == 'u') {
      for(i = 2; i >= 0; i--)
      {
        printf("Wheel %d: ", 3 - i);
        p->order[i] = getCleanChar() - 48;
      }
      for(i = 2; i >= 0; i--)
      {
        printf("Ring  %d: ", 3 - i);
        p->rings[i] = toupper(getCleanChar());
      }
      for(i = 2; i >= 0; i--)
      {
        printf("Start %d: ", 3 - i);
        p->pos[i] = toupper(getCleanChar());
      }
      printf("Stecker: ");
      i = 0;
      while((c = toupper(getchar())) != '\n')
      {
        p->plug[i] = c;
	i++;
      }
      p->plug[i] = '\0';
  }

  printf("\n[I] Wheels %d%d%d Start %c%c%c Rings %c%c%c Stecker \"%s\"\n\nEnter text:\t",
         p->order[2], p->order[1], p->order[0], 
         p->pos[2], p->pos[1], p->pos[0],
         p->rings[2], p->rings[1], p->rings[0], p->plug);
}

void stringToUpper(char * str) {
	unsigned int i;
	for (i = 0;i < strlen(str); i++)
		str[i] = toupper(str[i]);
}

int main() /*int argc, char *argv[])*/
{
  Params p;
  char outCipherTxt[MSGLEN] = "";
  char inEncryptedTxt[MSGLEN] = "";
  char inCribTxt[MSGLEN] = "";
  char c = 'e';

  printf("    _____  _____________  ______     ________  __ _   _________ \n   / __/ |/ /  _/ ___/  |/  / _ |   / __/ __ \\/ /| | / / __/ _ \\\n  / _//    // // (_ / /|_/ / __ |  _\\ \\/ /_/ / /_| |/ / _// , _/\n /___/_/|_/___/\\___/_/  /_/_/ |_| /___/\\____/____/___/___/_/|_| \n");
  printf("-----------------------------------------------------------------\n\ne)ncrypt\nd)ecrypt\ns)et properties\n\n%s",PROMPT);
  c = getCleanChar();

  if(c == 's') {
	  printf("\nErrors in crib to allow bruteforce (1): ");
	  getCleanChar();
	  errora = 2;
	  printf("\nHackFromTheHell will teletransport you to the \"decrypt\" option :)\n");
	  c = 'd';
  }

  if(c == 'd')  /*bombe case*/
  {
	printf("Encrypted Text: ");
	fgets (inEncryptedTxt,MSGLEN-1,stdin);
	printf("Crib Text:      ");
	fgets (inCribTxt,MSGLEN-1,stdin);
	printf("\n");

	inEncryptedTxt[strlen(inEncryptedTxt) - 1] = '\0';
	inCribTxt[strlen(inCribTxt) - 1] = '\0';

	stringToUpper(inEncryptedTxt);
	stringToUpper(inCribTxt);

    permuteAll(inEncryptedTxt,inCribTxt);
  }

  if (c == 'e')
  {
    initParams(&p);
    printf("Encrypted text:\t%s\n",cypher(p,outCipherTxt));
  }

  return(0);
}


