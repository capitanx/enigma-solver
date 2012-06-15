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


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#ifdef __linux__
#define strlcpy strncpy
#endif

/* Defines */
#define MSGLEN	256
#define TO		'Z'
#define PROMPT	"ENIGMA> "

/* Structs */
typedef struct P
{
  char order[3];/*={ 1, 2, 3 };*/
  char rings[3];/*={ 'A','A','A' };*/
  char pos[3];/*={ 'A','A','A' };*/
  char plug[11];/*="AMTE";*/
} Params;

/* Variables */
char out[MSGLEN];

/* Functions */
char scramble(char c, Params *p);
char *enigma(char *in, Params *p);
char *cypher(Params p, char * out);
Params rotate(int a, int b, int c, char *cyph, char *crib, char *plug, int *ct, int errora);
int decrypt(Params p, char *cyph, char *crib, char *plug, int *ct, int errora);
void test(int a, int b, int c, char *cyph, char *crib, int *ct, int errora);

