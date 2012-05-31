*****************************************************************************
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

#define MSGLEN 80
#define TO 'Z'


/* Rotor wirings */
char rotor[5][26]={
	/* Input "ABCDEFGHIJKLMNOPQRSTUVWXYZ" */
	/* 1: */ "EKMFLGDQVZNTOWYHXUSPAIBRCJ",
	/* 2: */ "AJDKSIRUXBLHWTMCQGZNPYFVOE",
	/* 3: */ "BDFHJLCPRTXVZNYEIWGAKMUSQO",
	/* 4: */ "ESOVPZJAYQUIRHXLNFTGKDCMWB",
	/* 5: */ "VZBRGITYUPSDNHLXAWMJQOFECK" };

char ref[26]="YRUHQSLDPXNGOKMIEBFZCWVJAT";

char refA[26] = "EJMZALYXVBWFCRQUONTSPIKHGD"; // before to WWII
char refB[26] = "YRUHQSLDPXNGOKMIEBFZCWVJAT"; // standard on WWII
char refC[26] = "FVPJIAOYEDRZXWGCTKUQSBNMHL"; // rare on WWII

char notch[5]="QEVJZ";

/* NORWAY ENIGMA (after to WWII)
 *
 * Wheel	ABCDEFGHIJKLMNOPQRSTUVWXYZ	Notch	Turnover	#
 * ----------------------------------------------------------
 * ETW		ABCDEFGHIJKLMNOPQRSTUVWXYZ			 
 * I		WTOKASUYVRBXJHQCPZEFMDINLG	Y		Q			1
 * II		GJLPUBSWEMCTQVHXAOFZDRKYNI	M		E			1
 * III		JWFMHNBPUSDYTIXVZGRQLAOEKC	D		V			1
 * IV		ESOVPZJAYQUIRHXLNFTGKDCMWB	R		J			1
 * V		HEJXQOTZBVFDASCILWPGYNMURK	H		Z			1
 * UKW		MOWJYPUXNDSRAIBFVLKZGQCHET
 *
 */

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
