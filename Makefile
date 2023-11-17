#
# +--------------------------------------------------------------------------+
# |  Compiladores - 2023/2                                                   |
# |  VINICIUS JOSÉ FRITZEN -- 335519                                         |
# +--------------------------------------------------------------------------+
#
# Baseado em:
# UFRGS - Compiladores B - Marcelo Johann - 2009/2 - Etapa 1

LEX = lex
YACC = yacc -d -v
CC = clang -Wall -std=c99

all: etapa2

debug: CC += -DDEBUG -g -O3
debug: all

run: all
	@(./etapa2 in)

etapa2: lex.yy.o main.o hash.o lib.o y.tab.o errorHandler.o symbols.o
	$(CC) -o etapa2 lex.yy.o main.o hash.o lib.o y.tab.o errorHandler.o symbols.o
main.o: main.c
	$(CC) -c main.c
hash.o: hash.c
	$(CC) -c hash.c
lib.o: lib.c lib.h
	$(CC) -c lib.c

y.tab.o: y.tab.c
	$(CC) -c y.tab.c

y.tab.c y.tab.h: parser.y
	$(YACC) parser.y

lex.yy.o: lex.yy.c hash.h lib.h y.tab.h errorHandler.h symbols.h
	$(CC) -c lex.yy.c

lex.yy.c lex.yy.h: scanner.l
	$(LEX) --header-file="lex.yy.h" scanner.l

errorHandler.o: errorHandler.c errorHandler.h
	$(CC) -c errorHandler.c

symbols.o: symbols.c symbols.h
	$(CC) -c symbols.c


.PHONY: clean
clean:
	rm *.o lex.yy.c etapa2 y.tab.c y.tab.h y.output lex.yy.h
