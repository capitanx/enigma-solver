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


/* Rotor wirings */
char rotor[5][26]={
	/* Input "ABCDEFGHIJKLMNOPQRSTUVWXYZ" */
	/* 1: */ "EKMFLGDQVZNTOWYHXUSPAIBRCJ",
	/* 2: */ "AJDKSIRUXBLHWTMCQGZNPYFVOE",
	/* 3: */ "BDFHJLCPRTXVZNYEIWGAKMUSQO",
	/* 4: */ "ESOVPZJAYQUIRHXLNFTGKDCMWB",
	/* 5: */ "VZBRGITYUPSDNHLXAWMJQOFECK" };

char ref[26]="YRUHQSLDPXNGOKMIEBFZCWVJAT";

char refA[26] = "EJMZALYXVBWFCRQUONTSPIKHGD"; /* before to WWII   */
char refB[26] = "YRUHQSLDPXNGOKMIEBFZCWVJAT"; /* standard on WWII */
char refC[26] = "FVPJIAOYEDRZXWGCTKUQSBNMHL"; /* rare on WWII     */

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

