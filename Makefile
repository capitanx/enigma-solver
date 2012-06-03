# enigma-solver Makefile
# ----------------------
#
# This is the unique Makefile forged by the elves.
# If you drop it to the fire, you can read this:
# << @AloneInTheShell is your GOD and you know it! >>
#
# Once you know that, will be the start of a big travel to the hell.
# You are warned.
#
# [...]
#

CC = gcc
PACKAGE = enigma-solver
VERSION = 0.0
OSTYPE= $(shell uname -s)

ifeq ($(OSTYPE),Linux)
	OPTS = -O3 -Wall -Wextra -pedantic -lbsd
else
	OPTS = -O3 -Wall -Wextra -pedantic
endif

all: enigma

enigma: 
	$(CC) $(OPTS) enigma.c cipher.c -o enigma

enigma-debug:
	$(CC) -g $(OPTS) enigma.c cipher.c -o enigma

clean:
	rm -f enigma
