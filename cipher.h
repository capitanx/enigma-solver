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


#include <stdio.h>
#include <string.h>
#include <ctype.h>

#ifdef __linux__
#include <bsd/string.h>
#endif

#define MSGLEN 80
#define TO 'Z'

/* Encryption parameters follow */

typedef struct P
{
  char order[3];/*={ 1, 2, 3 };*/
  char rings[3];/*={ 'A','A','A' };*/
  char pos[3];/*={ 'A','A','A' };*/
  char plug[10];/*="AMTE";*/
} Params;

char scramble(char c, Params *p);
char out[MSGLEN];
char *enigma(char *in, Params *p);
void cypher(Params p);
char oldcyph1;
char oldcyph2;
int rotate(int a, int b, int c, char *cyph, char *crib, char *plug, int *ct);
void test(int a, int b, int c, char *cyph, char *crib, int *ct);

/*
void permute(int a, int b, int c, char *cyph, char *crib, int *ct)
void permuteAll(char *cyph, char *crib)
char readCh()
void initParams(Params *p)
*/
