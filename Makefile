#
# +--------------------------------------------------------------------------+
# |  Compiladores - 2023/2                                                   |
# |  VINICIUS JOSÉ FRITZEN -- 335519                                         |
# +--------------------------------------------------------------------------+
#
# Baseado em:
# UFRGS - Compiladores B - Marcelo Johann - 2009/2 - Etapa 1

.PHONY: clean run echo

LEX = lex
YACC = bison -d -v
CC = c99 -Wall -std=c99
NAME = etapa4

src_files 		:= $(wildcard src/*.c)
header_files 	:= $(wildcard src/*.h)
obj_files 		:= $(addprefix build/, $(notdir $(src_files:.c=.o))) generated/lex.yy.o generated/y.tab.o
deps 			:= $(obj_files:.o=.d)

all: $(NAME)

debug: CC += -DDEBUG -g
debug: all

run: all
	@(./$(NAME) inputs/in)

echo:
	@echo "src_files:"
	@echo $(src_files)
	@echo "header_files:"
	@echo $(header_files)
	@echo "obj_files:"
	@echo $(obj_files)

$(NAME): $(obj_files)
	$(CC) -o $(NAME) $(obj_files)

build_folder:
	mkdir -p build generated

clean:
	rm -f build/*.o build/*.d build/*.c build/*.h build/*.output $(NAME) lex.yy.c generated/*

generated/y.tab.o: generated/y.tab.c build_folder
	$(CC) -c generated/y.tab.c -o generated/y.tab.o

generated/y.tab.c generated/y.tab.h: rules/parser.y build_folder
	$(YACC) rules/parser.y -o generated/y.tab.c

generated/lex.yy.o: generated/lex.yy.c   build_folder
	$(CC) -c lex.yy.c -o generated/lex.yy.o

generated/lex.yy.c generated/lex.yy.h: rules/scanner.l build_folder
	$(LEX) --header-file="generated/lex.yy.h" rules/scanner.l

build/%.o: src/$(notdir %.c) src/$(notdir %.h) generated/lex.yy.h generated/y.tab.h build_folder
	$(CC) -c -o $@ -MD -MP -MF $(@:.o=.d) $<

-include $(deps)
