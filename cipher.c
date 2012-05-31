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
void cypher(Params p)
{
  char in[MSGLEN], s[MSGLEN];
  int c, i = 0;

  while((c = getchar()) != '\n')
  {
    in[i] = toupper(c);
    i++;
  }
  in[i] = '\0';
  strlcpy(s, enigma(in, &p), sizeof(s));
  printf("\n%s\n%s\n", s, in);
}

char oldcyph1;
char oldcyph2;
int sattack = 1;

/*given a cipher text, and a crib, test all possible settings of wheel order a, b, c*/
int rotate(int a, int b, int c, char *cyph, char *crib, char *plug, int *ct)
{
  Params p;
  int i = 0;
  int len = strlen(crib);
  char s[MSGLEN];

  p.order[0] = a;
  p.order[1] = b;
  p.order[2] = c;
  p.rings[0] = p.rings[1] = p.rings[2] = 'A';
  strlcpy(p.plug, plug, sizeof(p.plug));

  for(p.pos[0] = 'A'; p.pos[0] <= 'Z'; p.pos[0]++)
  {
    for(p.pos[1] = 'A'; p.pos[1] <= 'Z'; p.pos[1]++)
    {
      for(p.pos[2] = 'A'; p.pos[2] <= 'Z'; p.pos[2]++)
      {

       for(p.rings[0] = 'A'; p.rings[0] <= 'Z'; p.rings[0]++)
       {

		if (sattack == 1) {
			/* First char check if stadistical is on */
			Params cp = p;
			if (cyph[0] != scramble(crib[0], &cp)) break;
		}

        for(p.rings[1] = 'A'; p.rings[1] <= 'Z'; p.rings[1]++)
        {
         for(p.rings[2] = 'A'; p.rings[2] <= 'Z'; p.rings[2]++)
         {

	      Params cp = p;
	      i = 0;

	      while(len > i) {
			if(cyph[i] != scramble(crib[i], &cp)) goto outHackFromHell;
			else i++;
	      }

#ifdef DEBUG
			printf("TESTING: W: %d%d%d S: %c%c%c R: %c%c%c # %s\n ",p.order[2], p.order[1], p.order[0], p.pos[2], p.pos[1], p.pos[0], p.rings[2], p.rings[1], p.rings[0], p.plug);
			printf("LEN: %i I: %i\n",len,i);
#endif

	      if(len == i) {
			(*ct)++;
			cp = p;
			strlcpy(s, enigma(cyph, &cp), sizeof(s));

			if (oldcyph1 != s[9] && oldcyph2 != s[32]) {
				printf("W: %d%d%d S: %c%c%c R: %c%c%c STECKER: \"%s\"\t# %s\n",
				p.order[2], p.order[1], p.order[0],
				p.pos[2],   p.pos[1],   p.pos[0],
				p.rings[2], p.rings[1], p.rings[0], p.plug, s);

				/* Turn to climb the hill only */
				sattack = 0;
			}

			oldcyph1 = s[9];
			oldcyph2 = s[32];
			return(1);
          } else {
			  /* Turn to climb the hill only if 2 chars ok */
			  if (i > 1) sattack = 0;
		  }

			outHackFromHell:
			break;

            }
          }
        }
      }
    }
  }

  return(0);
}

/*do the whole check including steckering of up to two pairs of letters*/
void test(int a, int b, int c, char *cyph, char *crib, int *ct)
{
  char A, B, C, D;
/*  char E, F, G, H, I, J; */
  char s[11];

  strlcpy(s, "", sizeof(s));
  printf("Checking wheels %d %d %d\n",  a, b, c); 
	  rotate(c, b, a, cyph, crib, "", ct);
	  A='A';
      for(B = A+1; A < TO; B++) {
		 if (B == '[') { A++; B=A+1; }
		 if (A == B) { continue; }
		 s[0] = A;
		 s[1] = B;
		 s[2] = '\0';
		 if (rotate(c, b, a, cyph, crib, s, ct)) {
			C='A';
			for(D = C+1; C < TO; D++) {
				if (D == '[') { C++; D=C+1; }
				if (C == D || A == D || B == D || A == C || B == C ) { continue; }
				s[0] = A;
				s[1] = B;
				s[2] = C;
				s[3] = D;
				s[4] = '\0';
				rotate(c, b, a, cyph, crib, s, ct);
				sattack = 1;
			}
		 }
      }
}

