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
CC = gcc -Wall

all: etapa2

debug: CC += -DDEBUG -g -O3
debug: all

run: all
	@(./etapa1 in)

etapa2: lex.yy.o main.o hash.o lib.o y.tab.o errorHandler.o
	$(CC) -o etapa2 lex.yy.o main.o hash.o lib.o y.tab.o errorHandler.o
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

lex.yy.o: lex.yy.c hash.h lib.h y.tab.h
	$(CC) -c lex.yy.c

lex.yy.c: scanner.l
	$(LEX) scanner.l

errorHandler.o: errorHandler.c errorHandler.h
	$(CC) -c errorHandler.c


.PHONY: clean
clean:
	rm *.o lex.yy.c etapa2 lex.yy.h y.tab.c y.tab.h y.output
