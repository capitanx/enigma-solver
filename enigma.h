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

typedef struct P
{
	char order[3];/*={ 1, 2, 3 };*/
	char rings[3];/*={ 'A','A','A' };*/
	char pos[3];/*={ 'A','A','A' };*/
	char plug[10];/*="AMTE";*/
} Params;
