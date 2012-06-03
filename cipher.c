/*****************************************************************************
 *
 * This source code is distributed under the GPLv3 license, COPYING.
 *
 * Author: capi_x <capi_x@haibane.org>
 * Based in the code of:
 *	Harald Schmidl <hjschmidl@acm.org>
 *	Fauzan Mirza's <fauzan@dcs.rhbnc.ac.uk>
 *
 ******************************************************************************/

#include "cipher.h"
#include "enigma.h"

int flag = 0;

char scramble(char c, Params *p)
{
  int i, j;

		c=toupper(c);
		if (!isalpha(c))
			return -1;
		
		/* Step up first rotor */
		p->pos[0]++;
		if (p->pos[0]>'Z')
			p->pos[0] -= 26;

		/* Check if second rotor reached notch last time */
		if (flag)
		{
			/* Step up both second and third rotors */
			p->pos[1]++;
			if (p->pos[1]>'Z')
				p->pos[1] -= 26;
			p->pos[2]++;
			if (p->pos[2]>'Z')
				p->pos[2] -= 26;
			flag=0;
		}

		/*  Step up second rotor if first rotor reached notch */
		if (p->pos[0]==notch[p->order[0]-1])
		{
			p->pos[1]++;
			if (p->pos[1]>'Z')
				p->pos[1] -= 26;
			/* Set flag if second rotor reached notch */
			if (p->pos[1]==notch[p->order[1]-1])
				flag=1;
		}

		/*  Swap pairs of letters on the plugboard */
		for (i=0; p->plug[i]; i+=2)
		{
			if (c==p->plug[i])
				c=p->plug[i+1];
			else if (c==p->plug[i+1])
				c=p->plug[i];
		}

		/*  Rotors (forward) */
		for (i=0; i<3; i++)
		{
			c += p->pos[i]-'A';
			if (c>'Z')
				c -= 26;

			c -= p->rings[i]-'A';
			if (c<'A')
				c += 26;

			c=rotor[p->order[i]-1][c-'A'];

			c += p->rings[i]-'A';
			if (c>'Z')
				c -= 26;

			c -= p->pos[i]-'A';
			if (c<'A')
				c += 26;
		}

		/*  Reflecting rotor */
		c=ref[c-'A'];

		/*  Rotors (reverse) */
		for (i=3; i; i--)
		{
			c += p->pos[i-1]-'A';
			if (c>'Z')
				c -= 26;

			c -= p->rings[i-1]-'A';
			if (c<'A')
				c += 26;

			for (j=0; j<26; j++)
				if (rotor[p->order[i-1]-1][j]==c)
					break;
			c=j+'A';

			c += p->rings[i-1]-'A';
			if (c>'Z')
				c -= 26;

			c -= p->pos[i-1]-'A';
			if (c<'A')
				c += 26;
		}
		
		/*  Plugboard  */
		for (i=0; p->plug[i]; i+=2) {
			if (c==p->plug[i]) c=p->plug[i+1];
			else if (c==p->plug[i+1]) c=p->plug[i];
		}

  return c;
}

/* Global variable for performance on enigma function */
char out[MSGLEN];

/*take a string, return encoded string*/
char *enigma(char *in, Params *p)
{
  unsigned int j;
  for(j = 0; j < strlen(in); j++)
  out[j] = scramble(in[j], p);
  out[j] = '\0';
  return out;
}

/*read in a string, and pass it through enigma*/
char * cypher(Params p, char * s)
{
  char in[MSGLEN];
  int c, i = 0;

  while((c = getchar()) != '\n')
  {
    in[i] = toupper(c);
    i++;
  }
  in[i] = '\0';
  strlcpy(s, enigma(in, &p), sizeof(in));

#ifdef DEBUG
  printf("[D] s: %s in: %s\n", s, in);
#endif

  return(s);
}

char oldcyph[MSGLEN] = "";

/*given a cipher text, and a crib, test all possible settings of wheel order a, b, c*/
int rotate(int a, int b, int c, char *cyph, char *crib, char *plug, int *ct, int errora)
{
  Params p;
  int i = 0;
  int fail = 0;
  int len = strlen(crib);
  int pluglen = strlen(plug);
  char s[MSGLEN];

  p.order[0] = a;
  p.order[1] = b;
  p.order[2] = c;
  p.rings[0] = p.rings[1] = p.rings[2] = 'A';
  strlcpy(p.plug, plug, sizeof(p.plug));

  for(p.pos[2] = 'A'; p.pos[2] <= 'Z'; p.pos[2]++)
  {
    for(p.pos[1] = 'A'; p.pos[1] <= 'Z'; p.pos[1]++)
    {
      for(p.pos[0] = 'A'; p.pos[0] <= 'Z'; p.pos[0]++)
      {
       for(p.rings[2] = 'A'; p.rings[2] <= 'Z'; p.rings[2]++)
       {
        for(p.rings[1] = 'A'; p.rings[1] <= 'Z'; p.rings[1]++)
        {
         for(p.rings[0] = 'A'; p.rings[0] <= 'Z'; p.rings[0]++)
         {

	      Params cp = p;
	      i = 0;
		  fail = 0;
	      while(len > i) {
			if(cyph[i] != scramble(crib[i], &cp)) {
				/*
				if (i == 0) {
					fail = 666;
					break;
				}
				*/
				fail++;
			}
			/* if we are bruteforcing plug dont allow fails */
			if ( (pluglen == 0 && fail > errora) || (pluglen > 0 && fail > 0) ) {
/*				fail += i; */
				fail = 666;
				break;
			}
			else i++;
	      }

#ifdef DEBUG
		  printf("LEN: %i I: %i\n",len,i);
#endif

		if (fail == 0 || ( pluglen == 0 && fail < errora + 1) ) {
			/* Mark as good coincidence */
			(*ct)++;

			/* Calculate all cypher */
			cp = p;
			strlcpy(s, enigma(cyph, &cp), sizeof(s));

			/* XXX: Show result if is different to the last one */
			if( strcmp( oldcyph, s ) != 0 ) {
				printf("[R] Wheel: %d%d%d Start: %c%c%c Rotor: %c%c%c Stecker: \"%s\"\tDecoded String: %s\n",
				p.order[2], p.order[1], p.order[0],
				p.pos[2],   p.pos[1],   p.pos[0],
				p.rings[2], p.rings[1], p.rings[0], p.plug, s);
		    }

			/* Copy the actual cyph to a external variable for next itinerance check */
			strlcpy(oldcyph,s,sizeof(s));

			if (strlen(plug) == 0)
				printf("[I] Has been found a coincidence, bruteforcing enigma plug.\n[I] This process can take a very long time.\n");

			return(1);

          } else continue;

			/* end if fail < 2 */
         }
       }
     }
   }
  }
 }

  /* Exit without results */
  return(0);
}

/*do the whole check including steckering of up to two pairs of letters*/
void test(int a, int b, int c, char *cyph, char *crib, int *ct, int errora)
{
  char A, B, C, D;
/*  char E, F, G, H, I, J; */
  char s[11];

  strlcpy(s, "", sizeof(s));
#ifdef DEBUG
  printf("Checking wheels %d %d %d\n",  a, b, c);
#endif
	  if (rotate(c, b, a, cyph, crib, "", ct, errora) == 0) return;
	  A='A';
      for(B = A+1; A < TO; B++) {
		 if (B == '[') { A++; B=A+1; }
		 if (A == B) { continue; }
		 s[0] = A;
		 s[1] = B;
		 s[2] = '\0';
		 printf ("[I] Bruteforce plug: \"%s\"\n",s);
		 if (rotate(c, b, a, cyph, crib, s, ct, errora)) {
			C='A';
			for(D = C+1; C < TO; D++) {
				if (D == '[') { C++; D=C+1; }
				if (C == D || A == D || B == D || A == C || B == C ) { continue; }
				s[0] = A;
				s[1] = B;
				s[2] = C;
				s[3] = D;
				s[4] = '\0';
				printf ("[I] Bruteforce plug: \"%s\"\n",s);
				rotate(c, b, a, cyph, crib, s, ct, errora);
			}
		 }
      }
	  /* TODO: Movida con los corchetes [D] STAT: 1 W: 214 S: NQP R: AAA # KWZ[ */
}

