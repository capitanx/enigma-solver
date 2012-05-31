all: enigma

enigma:
	gcc -Wall -O3 enigma.c cipher.c -o enigma

clean:
	rm -f enigma
