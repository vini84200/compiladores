%{
//
// +--------------------------------------------------------------------------+
// |  Compiladores - 2023/2                                                   |
// |  VINICIUS JOSÉ FRITZEN -- 335519                                         |
// +--------------------------------------------------------------------------+
//
  #include <stdio.h>
  #include "hash.h"
  int yylex (void);
  void yyerror (char const *);
%}

%union
{
  HashEntry *symbol;
}


%token KW_CHAR
%token KW_INT
%token KW_FLOAT

%token KW_CODE

%token KW_IF
%token KW_ELSE
%token KW_WHILE
%token KW_INPUT
%token KW_PRINT
%token KW_RETURN

%token OPERATOR_LE
%token OPERATOR_GE
%token OPERATOR_EQ
%token OPERATOR_DIF

%token <symbol> TK_IDENTIFIER

%token <symbol>LIT_INT
%token<symbol>LIT_REAL
%token<symbol>LIT_CHAR
%token<symbol>LIT_STRING

%token TOKEN_ERROR

%left '+' '-'
%left '*' '/'
%left '<' '>' OPERATOR_LE OPERATOR_GE OPERATOR_EQ OPERATOR_DIF
%left '&' '|' '~'

%nonassoc "then"
%nonassoc KW_ELSE


%%
program: list_declare list_implementation
    ;

list_declare: decl list_declare
    | /* empty */
    ;
decl:   decl_var
    | decl_array
    | decl_func
    ;

type:   KW_INT
    | KW_FLOAT
    | KW_CHAR
    ;
value:  LIT_INT
    | LIT_REAL
    | LIT_CHAR
    ;
decl_var: type TK_IDENTIFIER '=' value ';'
decl_array: type TK_IDENTIFIER '[' LIT_INT ']' array_init_list ';'
array_init_list: value array_init_list
    | /* empty */
    ;

decl_func: type TK_IDENTIFIER '(' list_param_def ')' ';';
list_param_def: param_def param_def_tail
    | /* empty */
    ;
param_def:  type TK_IDENTIFIER;
param_def_tail: ',' param_def param_def_tail
    | /* empty */
    ;

list_implementation: implementation list_implementation
    | /* empty */
    ;
implementation: func_implementation;
func_implementation: KW_CODE TK_IDENTIFIER cmd;

cmd: cmd_block
    | cmd_attr
    | cmd_attr_array
    | cmd_print
    | cmd_if
    | cmd_while
    | KW_RETURN expr ';'
    | ';'
;

cmd_block: '{' list_cmd '}';
list_cmd: cmd list_cmd
    | /* empty */
    ;

cmd_attr: TK_IDENTIFIER '=' expr ';'
cmd_attr_array: TK_IDENTIFIER '[' expr ']' '=' expr ';'

cmd_print: KW_PRINT expr ';'
        | KW_PRINT LIT_STRING ';'

expr:   TK_IDENTIFIER
    | TK_IDENTIFIER '[' expr ']'
    | TK_IDENTIFIER '(' list_expr ')'
    | LIT_INT
    | LIT_REAL
    | LIT_CHAR
    | expr '+' expr
    | expr '-' expr
    | expr '*' expr
    | expr '/' expr
    | expr '<' expr
    | expr '>' expr
    | expr OPERATOR_LE expr
    | expr OPERATOR_GE expr
    | expr OPERATOR_EQ expr
    | expr OPERATOR_DIF expr
    | expr '&' expr
    | '~' expr
    | '(' expr ')'
    | '-' expr
    | KW_INPUT '(' type ')'
    ;

list_expr: expr expr_tail
    | /* empty */
    ;

expr_tail: ',' expr expr_tail
    | /* empty */
    ;

cmd_if: KW_IF '(' expr ')' cmd cmd_if_tail
    ;
cmd_if_tail: KW_ELSE cmd
    | /* empty */   %prec "then"
    ;

cmd_while: KW_WHILE '(' expr ')' cmd
%%
