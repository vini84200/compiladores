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

all: etapa3

debug: CC += -DDEBUG -g
debug: all

run: all
	@(./etapa3 in)

etapa3: lex.yy.o main.o hash.o lib.o y.tab.o errorHandler.o symbols.o ast.o
	$(CC) -o etapa3 lex.yy.o main.o hash.o lib.o y.tab.o errorHandler.o symbols.o ast.o
main.o: main.c lib.h lex.yy.h y.tab.h errorHandler.h
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

ast.o: ast.c ast.h
	$(CC) -c ast.c


.PHONY: clean
clean:
	rm *.o lex.yy.c etapa3 y.tab.c y.tab.h y.output lex.yy.h
