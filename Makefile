all: enigma

enigma:
	gcc -O2 enigma.c -o enigma

clean:
	rm -f enigma
