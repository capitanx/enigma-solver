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

int getRank(char * cyph) {

	char str[40];
	int  rank = 0;

	FILE * dict = fopen("dict/german.txt", "r");
		if(!dict) return 0; /* cant open file */
	
	while(fgets(str,sizeof(str),dict) != NULL) {
		str[strlen(str)-1] = '\0';
		if(strstr(cyph, str) != NULL) rank++;
	}

	fclose(dict);
	return(rank);
}
