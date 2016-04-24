.PHONY: clean distclean default

CC=gcc
CFLAGS=-Wall -Wno-switch -Wno-unused-function 

default: compiler

parser.h parser.c: parser.y
	bison -dv -o parser.c parser.y

lexer.c: lexer.l
	flex -s -o lexer.c lexer.l


general.o  : general.c general.h error.h
error.o    : error.c general.h error.h

parser.o: parser.c parser.h symbol.h error.h inter.h routines.h 

lexer.o: lexer.c parser.h 

symbol.o   : symbol.c symbol.h general.h error.h

inter.o : inter.c inter.h symbol.h general.h error.h 

routines.o: routines.c routines.h symbol.h 

assembly.o : assembly.c assembly.h symbol.h general.h inter.h error.h

%.o: %.c
	$(CC) $(CFLAGS) -c $<

compiler: lexer.o parser.o symbol.o error.o general.o inter.o routines.o\
assembly.o 
	$(CC) $(CFLAGS) -o compiler $^

clean:
	$(RM) lexer.c parser.c parser.h parser.output *.o *~

distclean: clean
	$(RM) compiler 
