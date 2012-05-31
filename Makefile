all: enigma

enigma:
	gcc -O3 enigma.c cipher.c -o enigma

clean:
	rm -f enigma
