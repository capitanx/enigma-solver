/* enigma simulation and bombe, harald schmidl, april 1998
  the encoding scheme uses code from Fauzan Mirza's
  3 rotor German Enigma simulation
  Written by  */

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

char notch[5]="QEVJZ";

/* Encryption parameters follow */

typedef struct P
{
  char order[3];/*={ 1, 2, 3 };*/
  char rings[3];/*={ 'A','A','A' };*/
  char pos[3];/*={ 'A','A','A' };*/
  char plug[10];/*="AMTE";*/
} Params;

/*take a char and return its encoded version according to the 
  encryption params, update params, i.e. advance wheels
  this part uses Fauzan Mirza's code*/
char scramble(char c, Params *p)
{
  int i, j, flag, stop = 0;

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
  int j;
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
  strcpy(s, enigma(in, &p));
  printf("\n%s\n%s\n", s, in);
}

char oldcyph1;
char oldcyph2;

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
  strcpy(p.plug, plug);
  for(p.pos[0] = 'A'; p.pos[0] <= 'Z'; p.pos[0]++)
  {
    for(p.pos[1] = 'A'; p.pos[1] <= 'Z'; p.pos[1]++)
    {
      for(p.pos[2] = 'A'; p.pos[2] <= 'Z'; p.pos[2]++)
      {
/*        for(p.rings[0] = 'A'; p.rings[0] <= 'Z'; p.rings[0]++)
        {
          for(p.rings[1] = 'A'; p.rings[1] <= 'Z'; p.rings[1]++)
          {
            for(p.rings[2] = 'A'; p.rings[2] <= 'Z'; p.rings[2]++)
            {
*/
	      Params cp = p;
	      i = 0;

	      while(len > i) {
			if(cyph[i] != scramble(crib[i], &cp)) break;
			else i++;
	      }

//			printf("TESTING: W: %d%d%d S: %c%c%c R: %c%c%c # %s\n ",p.order[0], p.order[1], p.order[2], p.pos[0], p.pos[1], p.pos[2], p.rings[0], p.rings[1], p.rings[2], p.plug);
//			printf("LEN: %i I: %i\n",len,i);

	      if(len == i) {
			printf("kk\n");
//			char s[MSGLEN];
			(*ct)++;
			cp = p;
			strcpy(s, enigma(cyph, &cp));

			if (oldcyph1 != s[9] && oldcyph2 != s[32]) {
				printf("W: %d%d%d S: %c%c%c R: %c%c%c STECKER: \"%s\"\t# %s\n",
				p.order[0], p.order[1], p.order[2],
				p.pos[0],   p.pos[1],   p.pos[2],
				p.rings[0], p.rings[1], p.rings[2], p.plug, s);
			}

			oldcyph1 = s[9];
			oldcyph2 = s[32];
			return(1);
          }
/*            }
          }
        }
*/      }
    }
  }
  return(0);
}

/*do the whole check including steckering of up to two pairs of letters*/
void test(int a, int b, int c, char *cyph, char *crib, int *ct)
{
  char A, B, C, D, E, F, G, H, I, J;
  int i = 0, cs, fin;
  char s[11];

  strcpy(s, "");
  printf("Checking wheels %d %d %d\n",  a, b, c); 
	  A='A';
      for(B = A+1; A < TO; B++) {
		 if (B == '[') { A++; B=A+1; }
		 if (A == B) { continue; }
		 s[0] = A;
		 s[1] = B;
		 s[2] = '\0';
		 if (rotate(a, b, c, cyph, crib, s, ct)) {
			C='A';
			for(D = C+1; C < TO; D++) {
				if (D == '[') { C++; D=C+1; }
				if (C == D || A == D || B == D || A == C || B == C ) { continue; }
				s[0] = A;
				s[1] = B;
				s[2] = C;
				s[3] = D;
				s[4] = '\0';
				rotate(a, b, c, cyph, crib, s, ct);
			}
		 }
      }
	printf("FIN\n");
}


/*run on all permutations of wheels a, b, c*/
void permute(int a, int b, int c, char *cyph, char *crib, int *ct)
{
  test(a, b, c, cyph, crib, ct);
  test(a, c, b, cyph, crib, ct);
  test(b, a, c, cyph, crib, ct);
  test(b, c, a, cyph, crib, ct);
  test(c, a, b, cyph, crib, ct);
  test(c, b, a, cyph, crib, ct);
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
  printf("\nFound %d solutions.\n", ct);
}

/*helper to read a character*/
char readCh()
{
  char c, ret;

  while((c = getchar()) != '\n')
  ret = c;
  return ret;
}

/*init the starting position*/
void initParams(Params *p)
{
  int i;
  char c;

  printf("d)efault or u)ser: ");
  c = readCh();
  if(c != 'u')
  {
    for(i = 0; i < 3; i++)
    {
      p->order[i] = i + 1;
      p->rings[i] = 'A';
      p->pos[i] = 'A';
    }
    strcpy(p->plug, "");
  }
  else
  {
      for(i = 0; i < 3; i++)
      {
        printf("Wheel %d: ", i + 1);
        p->order[i] = readCh() - 48;
      }
      for(i = 0; i < 3; i++)
      {
/*        printf("Ring  %d: ", i + 1);*/
        p->rings[i] = 'A';/*readCh();*/
      }
      for(i = 0; i < 3; i++)
      {
        printf("Start %d: ", i + 1);
        p->pos[i] = readCh();
      }
      printf("Stecker: ");
      i = 0;
      while((c = getchar()) != '\n')
      {
        p->plug[i] = c;
	i++;
      }
      p->plug[i] = '\0';
  }
  printf("Wheels %d %d %d Start %c %c %c Rings %c %c %c Stecker \"%s\"\n",
         p->order[0], p->order[1], p->order[2], 
         p->pos[0], p->pos[1], p->pos[2],
         p->rings[0], p->rings[1], p->rings[2], p->plug);
}


/********************************************MAIN*********************************************/
int main(int argc, char *argv[])
{
  Params p;

  if(argc > 2)  /*bombe case*/
  {
    permuteAll(argv[1], argv[2]);
  }
  else
  {
    initParams(&p);
    cypher(p);
  }
}














