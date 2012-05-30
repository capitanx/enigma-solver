/*
  Program to simulate Enigma cipher 
  
  Consistent with the excellent flash Enigma simulator
  at http://www.enigmaco.de/
  
  As with the flash simulator, here we do not
  allow the rings to be adjusted.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void getInversePerm(int invPerm[], int perm[], int n);


// "natural" positions of the notches,
// that is, permutations are given relative to
// these notch positions, where the the notch causes
// rotor to step after indicated position
// Q
#define NOTCH_0 16
// E
#define NOTCH_1 4
// V
#define NOTCH_2 21
// J
#define NOTCH_3 9
// Z
#define NOTCH_4 25

#define ROTOR_0 "EKMFLGDQVZNTOWYHXUSPAIBRCJ"
#define ROTOR_1 "AJDKSIRUXBLHWTMCQGZNPYFVOE"
#define ROTOR_2 "BDFHJLCPRTXVZNYEIWGAKMUSQO"
#define ROTOR_3 "ESOVPZJAYQUIRHXLNFTGKDCMWB"
#define ROTOR_4 "VZBRGITYUPSDNHLXAWMJQOFECK"

#define REFLECTOR_B "YRUHQSLDPXNGOKMIEBFZCWVJAT"
#define REFLECTOR_C "FVPJIAOYEDRZXWGCTKUQSBNMHL"

// no stecker cables (i.e., stecker == identity perm)
#define STECKER "ABCDEFGHIJKLMNOPQRSTUVWXYZ"

// stecker with 10 cables connecting: (0,10) (1,25) (2,6) (3,11) (5,20)
// (7,9) (12,23) (13,17) (14,24) (15,22): 4,8,16,18,19,21 are un-steckered
//#define STECKER "KZGLEUCJIHADXRYWQNSTFVPMOB"


// Print flags
// print rotors, reflector, and stecker permutations
//#define PR_PERMS
// print output for each step of encryption/decryption
//#define PR_STEPS
// print the key
#define PR_KEY


// function prototypes
void getInversePerm(int invPerm[], int perm[], int n);
int stepRotor(int a, int b);


int main(int argc, const char *argv[])
{
	FILE *in;

	int i,
		j,
		n,
		temp,
		reflect,
		cur_L,
		cur_M,
		cur_R,
		init_L,
		init_M,
		init_R,
		notch_M,
		notch_R,
		numL,
		numM,
		numR;
	
	int Lrotor[26],
		Mrotor[26],
		Rrotor[26],
		reflector[26],
		stecker[26];

	int L[26][26],
		M[26][26],
		R[26][26],
		L_inv[26][26],
		M_inv[26][26],
		R_inv[26][26];
		
	char letter[26] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ",
		 rot[5][26] = {ROTOR_0, ROTOR_1, ROTOR_2, ROTOR_3, ROTOR_4},
		 ref[2][26] = {REFLECTOR_B, REFLECTOR_C},
		 steck[26] = STECKER,
		 notch[5] = {NOTCH_0, NOTCH_1, NOTCH_2, NOTCH_3, NOTCH_4},
		 infname[100];
		 
	unsigned char inChar,
				  outChar;

	if(argc != 5)
	{
oops:   //fprintf(stderr, "\n\nUsage: %s rotors reflector init infile\n\n", 
	//			argv[0]);
	//	fprintf(stderr, "where rotors == LMR rotors (0 thru 4, no space, no repeats)\n");
	//	fprintf(stderr, "      reflector == B or C\n");
	//	fprintf(stderr, "      init == initial position for LMR rotors (A thru Z, no space)\n");
	//	fprintf(stderr, "      infile == input file name\n\n");
	//	fprintf(stderr, "For example: %s 214 B XWA plain.txt\n\n", argv[0]);
	//	fprintf(stderr, "Note 1: Input file must contain only upper case A thru Z\n");
	//	fprintf(stderr, "Note 2: Adjustable rings on rotors is not implemented\n\n");
		exit(0);
	}

	if(strlen(argv[1]) != 3)
	{
		fprintf(stderr, "\nError --- must specify 3 rotors\n");
		goto oops;
	}
	numL = argv[1][0] - 48;
	if(numL < 0 || numL > 4)
	{
		fprintf(stderr, "\nError --- left rotor must be 0 thru 4\n");
		goto oops;
	}
	numM = argv[1][1] - 48;
	if(numM < 0 || numM > 4)
	{
		fprintf(stderr, "\nError --- middle rotor must be 0 thru 4\n");
		goto oops;
	}
	numR = argv[1][2] - 48;
	if(numR < 0 || numR > 4)
	{
		fprintf(stderr, "\nError --- right rotor must be 0 thru 4\n");
		goto oops;
	}
	if((numL == numM) || (numL == numR) || (numM == numR)) 
	{
		//fprintf(stderr, "\nError --- rotors must be distinct\n");
		exit(0);
		//goto oops;
	}
	notch_M = notch[numM];
	notch_R = notch[numR];
		
//	printf("numL = %d, numM = %d, numR = %d\n", numL, numM, numR);
	
	reflect = argv[2][0] - 66;	
	if(strlen(argv[2]) > 1 || reflect < 0 || reflect > 1)
	{
		fprintf(stderr, "\nError --- reflector must be a single character upper case B or C\n");
		goto oops;
	}

	if(strlen(argv[3]) != 3)
	{
		fprintf(stderr, "\nError --- init must be a three characters, each upper case A thru Z\n");
		goto oops;
	}
	
	init_L = argv[3][0] - 65;	
	if(init_L < 0 || init_L > 25)
	{
		fprintf(stderr, "\nError --- init must be three characters, each upper case A thru Z\n");
		goto oops;
	}
	
	init_M = argv[3][1] - 65;
	if(init_M < 0 || init_M > 25)
	{
		fprintf(stderr, "\nError --- init must be three characters, each upper case A thru Z\n");
		goto oops;
	}
	
	init_R = argv[3][2] - 65;
	if(init_R < 0 || init_R > 25)
	{
		fprintf(stderr, "\nError --- init must be three characters, each upper case A thru Z\n");
		goto oops;
	}
	
/***
	// rings allow the inner permutation wirings to rotate
	// notch and labeling on rotor are not affected
	// note that this is comparable to rotating the rim
	// of a car wheel, while holding the tire fixed
	// ********** NOT IMPLEMENTED *************
	if(strlen(argv[4]) != 3)
	{
		fprintf(stderr, "\nError --- rings must be a three characters, each upper case A thru Z\n");
		goto oops;
	}

	ring_L = argv[4][0] - 65;
	if(ring_L < 0 || ring_L > 25)
	{
		fprintf(stderr, "\nError --- rings must be three characters, each upper case A thru Z\n");
		goto oops;
	}

	ring_M = argv[4][1] - 65;
	if(ring_M < 0 || ring_M > 25)
	{
		fprintf(stderr, "\nError --- rings must be three characters, each upper case A thru Z\n");
		goto oops;
	}

	ring_R = argv[4][2] - 65;
	if(ring_R < 0 || ring_R > 25)
	{
		fprintf(stderr, "\nError --- rings must be three characters, each upper case A thru Z\n");
		goto oops;
	}
***/	
    sprintf(infname, "%s", argv[4]);
    in = fopen(infname, "r");
    if(in == NULL)
    {
        fprintf(stderr, "\n\nError opening file %s\nTry again\n\n", infname);
        goto oops;
    }

	// initialize rotor and reflector arrays
	for(i = 0; i < 26; ++i)
	{
		Lrotor[i] = (int)rot[numL][i] - 65;
		Mrotor[i] = (int)rot[numM][i] - 65;
		Rrotor[i] = (int)rot[numR][i] - 65;
		reflector[i] = (int)ref[reflect][i] - 65;
		stecker[i] = (int)steck[i] - 65;
		
	}// next i

	for(i = 0; i < 26; ++i)
	{
		L[0][i] = Lrotor[i];
		M[0][i] = Mrotor[i];
		R[0][i] = Rrotor[i];

	}// next i
	
	for(j = 0; j < 26; ++j)
	{
		L[0][j] = Lrotor[j];
		M[0][j] = Mrotor[j];
		R[0][j] = Rrotor[j];
	
	}// next j
	
	for(i = 1; i < 26; ++i)
	{
		for(j = 0; j < 26; ++j)
		{
			L[i][j] = (Lrotor[(i + j) % 26] + 26 - i) % 26;
			M[i][j] = (Mrotor[(i + j) % 26] + 26 - i) % 26;
			R[i][j] = (Rrotor[(i + j) % 26] + 26 - i) % 26;

		}// next j

	}// next i

	for(i = 0; i < 26; ++i)
	{
		getInversePerm(L_inv[i], L[i], 26);
		getInversePerm(M_inv[i], M[i], 26);
		getInversePerm(R_inv[i], R[i], 26);
		
	}// next i

#ifdef PR_PERMS
	printf("L perms\n");
	for(i = 0; i < 26; ++i)
	{
		printf("L[%2d] = ", i);
		for(j = 0; j < 26; ++j)
		{
			printf("%c", letter[L[i][j]]);
		}
		printf("\n");
	}
	printf("\n");
	printf("M perms\n");
	for(i = 0; i < 26; ++i)
	{
		printf("M[%2d] = ", i);
		for(j = 0; j < 26; ++j)
		{
			printf("%c", letter[M[i][j]]);
		}
		printf("\n");
	}
	printf("\n");
	printf("R perms\n");
	for(i = 0; i < 26; ++i)
	{
		printf("R[%2d] = ", i);
		for(j = 0; j < 26; ++j)
		{
			printf("%c", letter[R[i][j]]);
		}
		printf("\n");
	}
	printf("\n");
	printf("reflector\n");
	for(i = 0; i < 26; ++i)
	{
		printf("%c", letter[reflector[i]]);
	}
	printf("\n\n");
	printf("stecker\n");
	for(i = 0; i < 26; ++i)
	{
		printf("%c", letter[stecker[i]]);
	}
	printf("\n\n");
#endif

	cur_L = init_L;
	cur_M = init_M;
	cur_R = init_R;

	while(1)
	{
		temp = fgetc(in);
		if(temp == EOF)
		{
			break;
		}
		temp -= 65;
		if(temp < 0 || temp > 25)
		{
			fprintf(stderr, "\nError --- all input characters must be upper case A thru Z\n");
			exit(0);
		}
		
		inChar = (unsigned char)temp;
		
		// Note that Enigma stepping is not exactly odometer-like
		// R is fast rotor
		// M is medium rotor
		// L is slow rotor
		// rotors step _before_ encryption/decryption
		if(cur_M == notch_M)// all 3 step (so step left and middle here)
		{
			cur_L = stepRotor(cur_L, 26);
			cur_M = stepRotor(cur_M, 26);
		}
		else
		{
			if(cur_R == notch_R)// M and R both step (so step middle here)
			{
				cur_M = stepRotor(cur_M, 26);
				
			}// end if
		
		}// end if
		
		// step right (fast) rotor --- always steps
		cur_R = stepRotor(cur_R, 26);
		
		// Enigma transformation
#ifdef PR_STEPS
		printf("\ninChar = %c\n", letter[inChar]);
		temp = stecker[inChar];
		printf("stecker[%d] = %c\n", inChar, letter[temp]);
		printf("R[%d][%d] = ", cur_R, temp);
		temp = R[cur_R][temp];
		printf("%c\n", letter[temp]);
		printf("M[%d][%d] = ", cur_M, temp);
		temp = M[cur_M][temp];
		printf("%c\n", letter[temp]);
		printf("L[%d][%d] = ", cur_L, temp);
		temp = L[cur_L][temp];
		printf("%c\n", letter[temp]);
		printf("reflector[%d] = ", temp);
		temp = reflector[temp];
		printf("%c\n", letter[temp]);
		printf("L_inv[%d][%d] = ", cur_L, temp);
		temp = L_inv[cur_L][temp];
		printf("%c\n", letter[temp]);
		printf("M_inv[%d][%d] = ", cur_M, temp);
		temp = M_inv[cur_M][temp];
		printf("%c\n", letter[temp]);
		printf("R_inv[%d][%d] = ", cur_R, temp);
		temp = R_inv[cur_R][temp];
		printf("%c\n", letter[temp]);
		printf("stecker[%d] = ", temp);
		temp = stecker[temp];
		printf("%c\n", letter[temp]);
#endif

		outChar = stecker[R_inv[cur_R][M_inv[cur_M][L_inv[cur_L][
					reflector[L[cur_L][M[cur_M][R[cur_R][stecker[inChar]]]]]]]]];

#ifdef PR_STEPS
		printf("letter[temp] = %c, letter[outChar] = %c\n", letter[temp], letter[outChar]);
#endif

		printf("%c", letter[outChar]);
		

/***
		// enigma cannot encrypt any letter to itself
		if(outChar == inChar)
		{
			printf("\n\nError!!!\n\n");
		}
***/		
	
	}// end while
#ifdef PR_KEY
	printf(" Key:");
	printf(" (%d,%d,%d)", numL, numM, numR);
	printf(" %c", (char)(reflect + 66));
	printf(" (%c,%c,%c)\n", 
		(char)(init_L + 65), (char)(init_M + 65), (char)(init_R + 65));
	//printf("Output:");

#endif

	printf("\n");

}// end main


void getInversePerm(int invPerm[], int perm[], int n)
{
	int i;
  
	for(i = 0; i < n; ++i)
	{
		invPerm[perm[i]] = i;
  
	}// next i

}// end getInversePerm


int stepRotor(int a, int b)
{
	int t;
	t = a + 1;
	if(t >= b)
	{
		t = 0;
	}
	return(t);

}// end stepRotor

