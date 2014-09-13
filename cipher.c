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
#include "dict.h"

int flag = 0;

char scramble(char c, Params *p)
{
    int i, j;

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

int oldrank = IN_RANK;

/*given a cipher text, and a crib, test all possible settings of wheel order a, b, c*/
Params rotate(int a, int b, int c, char *cyph, char *crib, char plug[11], int *ct, int errora)
{
    Params p;
    int i = 0;
    int fail = 0;
    int rank = IN_RANK;
    int len = strlen(crib);
    char s[MSGLEN];

    p.order[0] = a;
    p.order[1] = b;
    p.order[2] = c;
    p.rings[0] = 'A';
    p.rings[1] = 'A';
    p.rings[2] = 'A';
    strlcpy(p.plug, plug, sizeof(p.plug));

    for(p.pos[2] = 'A'; p.pos[2] <= 'Z'; p.pos[2]++)
     for(p.pos[1] = 'A'; p.pos[1] <= 'Z'; p.pos[1]++)
      for(p.pos[0] = 'A'; p.pos[0] <= 'Z'; p.pos[0]++)
       for(p.rings[2] = 'A'; p.rings[2] <= 'Z'; p.rings[2]++)
        for(p.rings[1] = 'A'; p.rings[1] <= 'Z'; p.rings[1]++)
         for(p.rings[0] = 'A'; p.rings[0] <= 'Z'; p.rings[0]++) {

            Params cp = p;
            i = 0;
            fail = 0;

            #ifdef DEBUG
            printf("TESTING: W: %d%d%d S: %c%c%c R: %c%c%c ST: \"%s\"\n", p.order[2], p.order[1], p.order[0],p.pos[2],   p.pos[1],   p.pos[0],p.rings[2], p.rings[1], p.rings[0], p.plug);
            #endif

            while(len > i) {
                if(cyph[i] != scramble(crib[i], &cp)) fail++;

                /* if we are bruteforcing plug dont allow fails */
                if ( ( fail > errora) || (i == 0 && fail > 0 ) || (i == 1 && fail > 0) ) { fail = 666; break; }
                else i++;
            }

            if (fail == 666) continue;

            #ifdef DEBUG
            printf("LEN: %i I: %i F: %i\n",len,i,fail);
            #endif

            if ( fail <= errora ) {
                /* Calculate all cypher */
                cp = p;

                strlcpy(s, enigma(cyph, &cp), sizeof(s));
                rank = getRank(s);

                /* XXX: Show result if is different to the last one */
                if( rank >= oldrank ) {
                    printf("[R] Wheel: %d%d%d Start: %c%c%c Rotor: %c%c%c Stecker: \"%s\"\tDecoded String: %s Rank: %i\n",
                    p.order[2], p.order[1], p.order[0],
                    p.pos[2],   p.pos[1],   p.pos[0],
                    p.rings[2], p.rings[1], p.rings[0], p.plug, s, rank);

                    /* Mark as good coincidence */
                    (*ct)++;
                    oldrank = rank;
                }
                return(p);
            } else continue;
            /* end if fail < 2 */
         }
    /* Exit without results */
    return(p);
}

int decrypt(Params p, char *cyph, char *crib, char plug[11], int *ct, int errora) {

	char s[MSGLEN];
	Params cp;
	int i = 0;
	int fail = 0;
	int rank = 0;
	int len = strlen(crib);

	strlcpy(p.plug,  plug, sizeof( p.plug));
	cp = p;

#ifdef DEBUG
	printf("DECRYPT: W: %d%d%d S: %c%c%c R: %c%c%c ST: \"%s\"\n",cp.order[2], cp.order[1], cp.order[0],cp.pos[2], cp.pos[1], cp.pos[0],cp.rings[2], cp.rings[1], cp.rings[0], cp.plug);
#endif

	while(len > i) {
		if(cyph[i] != scramble(crib[i], &cp)) fail++;

		/* if we are bruteforcing plug dont allow fails */
		if ( fail > errora ) break;
		else i++;
	}


#ifdef DEBUG
	printf("\nEA: %i I: %i F: %i\n",errora,i,fail);
#endif

	if (fail <= errora) {

		/* Calculate all cypher */
		cp = p;
		strlcpy(s, enigma(cyph, &cp), sizeof(s));
		rank = getRank(s);

#ifdef DEBUG
		printf("EA: %i I: %i F: %i R: %i\n",errora,i,fail,rank);
#endif

		/* XXX: Show result if is different to the last one */
		if( rank >= oldrank || errora != 0 ) {
			printf("[R] Wheel: %d%d%d Start: %c%c%c Rotor: %c%c%c Stecker: \"%s\"\tDecoded String: %s Rank: %i\n",
				p.order[2], p.order[1], p.order[0],
				p.pos[2],   p.pos[1],   p.pos[0],
				p.rings[2], p.rings[1], p.rings[0], p.plug, s, rank);

			/* Mark as good coincidence */
			(*ct)++;

			oldrank = rank;

			return(1);
		}
	}

	return(0);
}

/*do the whole check including steckering of up to two pairs of letters*/
void test(int a, int b, int c, char *cyph, char *crib, int *ct, int errora)
{
    char A, B, C, D;
    char E, F, G, H, I, J;
    char s[11];
    Params p;
    int brute = 1;
    int olderrora = errora;

    strlcpy(s, "", sizeof(s));

    p = rotate(c, b, a, cyph, crib, "", ct, errora);
    /*  if (*ct == 0) return; */

    A='A';
    for(B = A+1; A < TO; B++) {
     if (errora != olderrora) errora = olderrora;
     if (B == '[') { A++; B=A+1; }
     if (A == B || A == 'Z') { continue; }
     s[0] = A;
     s[1] = B;
     s[2] = '\0';
     if (*ct == 0 && brute == 1) { /*printf("[I] Bruteforce stecker \"%s\"\n",s);*/ p = rotate(c, b, a, cyph, crib, s, ct, errora); }
     else (decrypt(p, cyph, crib, s, ct, errora));
     if (*ct != 0) {
        C='A';
        for(D = C+1; C < TO; D++) {
            if (D == '[') { C++; D=C+1; }
            if (C == D || A == D || B == D || A == C || B == C || C == 'Z' ) { continue; }
            s[0] = A;
            s[1] = B;
            s[2] = C;
            s[3] = D;
            s[4] = '\0';
            if (errora > 1) errora = 1;
            if (decrypt(p, cyph, crib, s, ct, errora)) {
                E='A';
                for (F = E+1; E < TO; F++) {
                    if (F == '[') { E++; F=E+1; }
                    if (C == D || A == D || B == D || A == C || B == C || F == A || F == B || F == C || F == D || F == E || E == A || E == B || E == C || E == D || E == 'Z') { continue; }
                    s[0] = A;
                    s[1] = B;
                    s[2] = C;
                    s[3] = D;
                    s[4] = E;
                    s[5] = F;
                    s[6] = '\0';
                    if (decrypt(p, cyph, crib, s, ct, errora)) {
                        if (errora > 0) errora = 0;
                        G='A';
                        for (H = G+1; G < TO; H++) {
                            if (H == '[') { G++; H=G+1; }
                            if (C == D || A == D || B == D || A == C || B == C || F == A || F == B || F == C || F == D || F == E || E == A || E == B || E == C || E == D || H == A || H == B || H == C || H ==     D || H == E || H == F || H == G || G == A || G == B || G == C || G == D || G == E || G == F || G == 'Z') { continue; }
                            s[0] = A;
                            s[1] = B;
                            s[2] = C;
                            s[3] = D;
                            s[4] = E;
                            s[5] = F;
                            s[6] = G;
                            s[7] = H;
                            s[8] = '\0';
                            if (decrypt(p, cyph, crib, s, ct, errora)) {
                                I='A';
                                for (J = I+1; I < TO; J++) {
                                    if (J == '[') { I++; J=I+1; }
                                    if (C == D || A == D || B == D || A == C || B == C || F == A || F == B || F == C || F == D || F == E || E == A || E == B || E == C || E == D || H == A || H == B || H == C || H ==     D || H == E || H == F || H == G || G == A || G == B || G == C || G == D || G == E || G == F || J == A || J == B || J == C || J == D || J == E || J == F || J == G || J == H || J == I || I == A || I == B || I == C || I == D || I == E || I == F || I == G || I == H || I == 'Z') { continue; }
                                    s[0] = A;
                                    s[1] = B;
                                    s[2] = C;
                                    s[3] = D;
                                    s[4] = E;
                                    s[5] = F;
                                    s[6] = G;
                                    s[7] = H;
                                    s[8] = I;
                                    s[9] = J;
                                    s[10] = '\0';
                                    decrypt(p, cyph, crib, s, ct, errora);
                                }
                            }
                        }
                    }
                }
            } else {
                if (brute == 1) { *ct = 0; brute = 0; }
                continue;
    /*				break;  */
            }
        }
     }
    }
}

