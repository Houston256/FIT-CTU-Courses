CC=gcc
CFLAGS= -Wall -Wextra -O3 -funroll-loops
#CFLAGS=  -pg -ggdb -Wall -lm   -Wno-missing-braces

all: probSAT

probSAT:	probSAT.c
			$(CC) $(CFLAGS)  probSAT.c -lm -o probSAT
clean:	
		rm -f probSAT
