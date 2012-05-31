all: enigma

enigma:
	gcc -O3 enigma.c -o enigma

clean:
	rm -f enigma
