%{
//
// +--------------------------------------------------------------------------+
// |  Compiladores - 2023/2                                                   |
// |  VINICIUS JOSÉ FRITZEN -- 335519                                         |
// +--------------------------------------------------------------------------+
//
  #include <stdio.h>
  #include "hash.h"
  #include "ast.h"
  int yylex (void);
  void yyerror (char const *);

%}

%union
{
  HashEntry *symbol;
  AST *ast;
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


%left '&' '|' '~'
%left '<' '>' OPERATOR_LE OPERATOR_GE OPERATOR_EQ OPERATOR_DIF
%left '+' '-'
%left '*' '/'
%left UMINUS

%nonassoc "then"
%nonassoc KW_ELSE

%type <ast> program list_declare decl decl_var type value decl_array array_init_list decl_func list_param_def param_def param_def_tail list_implementation implementation func_implementation cmd cmd_block list_cmd cmd_attr cmd_attr_array cmd_print cmd_if cmd_if_tail cmd_while cmd_return expr list_expr expr_tail

%%
program: list_declare list_implementation       { $$ = createASTProgram($1, $2); printAST($$, 0);}
    ;

list_declare: decl list_declare                 { $$ = createASTDeclList($1, $2); }
    | /* empty */                               { $$ = NULL; }
    ;
decl:   decl_var                                { $$ = $1; }
    | decl_array                                { $$ = $1; }
    | decl_func                                 { $$ = $1; }
    ;

type:   KW_INT                                  { $$ = createASTType(TYPE_INT); }
    | KW_FLOAT                                  { $$ = createASTType(TYPE_FLOAT); }
    | KW_CHAR                                   { $$ = createASTType(TYPE_CHAR); }
    ;
value:  LIT_INT                                 { $$ = createASTValue($1); }
    | LIT_REAL                                  { $$ = createASTValue($1); }
    | LIT_CHAR                                  { $$ = createASTValue($1); }
    ;

decl_var: type TK_IDENTIFIER '=' value ';'      { $$ = createASTVarDecl($1, $2, $4); }
    ;

decl_array: type TK_IDENTIFIER '[' LIT_INT ']' array_init_list ';'      { $$ = createASTArrayDecl($1, $2, $4, $6); }
    ;

array_init_list: value array_init_list      { $$ = createASTArrayValues($1, $2); }
    | /* empty */                           { $$ = NULL; }
    ;

decl_func: type TK_IDENTIFIER '(' list_param_def ')' ';'                   { $$ = createASTFuncDecl($1, $2, $4); }
    ;
list_param_def: param_def param_def_tail                                    { $$ = createASTParamList($1, $2); }
    | /* empty */                                                           { $$ = NULL; }
    ;
param_def:  type TK_IDENTIFIER                         { $$ = createASTParam($1, $2); }
    ;
param_def_tail: ',' param_def param_def_tail            { $$ = createASTParamList($2, $3); }
    | ','                                               { $$ = NULL; }
    | /* empty */                                   { $$ = NULL; }
    ;

list_implementation: implementation list_implementation    { $$ = createASTCodeList($1, $2); }
    |                                                     { $$ = NULL; }
    ;
implementation: func_implementation                     { $$ = $1; }
    ;
func_implementation: KW_CODE TK_IDENTIFIER cmd          { $$ = createASTImplFunc($2, $3); }
    ;

cmd: cmd_block                          { $$ = $1; }
    | cmd_attr                          { $$ = $1; }
    | cmd_attr_array                    { $$ = $1; }
    | cmd_print                         { $$ = $1; }
    | cmd_if                        { $$ = $1; }
    | cmd_while                     { $$ = $1; }
    | cmd_return                    { $$ = $1; }
    | ';'                           { $$ = createASTEmptyCommand(); }
;

cmd_return: KW_RETURN expr ';'     { $$ = createASTCommandReturn($2); }
    ;
cmd_block: '{' list_cmd '}'        { $$ = createASTCommandBlock($2); }
    ;
list_cmd: cmd list_cmd          { $$ = createASTCommandList($1, $2); }
    | /* empty */               { $$ = NULL; }
    ;

cmd_attr: TK_IDENTIFIER '=' expr ';'    { $$ = createASTCommandAssign($1, $3); }
    ;
cmd_attr_array: TK_IDENTIFIER '[' expr ']' '=' expr ';'     { $$ = createASTCommandAssignArray($1, $3, $6); }
    ;

cmd_print: KW_PRINT expr ';'                                { $$ = createASTCommandPrintExpr($2); }
        | KW_PRINT LIT_STRING ';'                           { $$ = createASTCommandPrintString($2); }

expr:   TK_IDENTIFIER                                   { $$ = createASTExprVar($1); }
    | TK_IDENTIFIER '[' expr ']'                    { $$ = createASTExprArrayGet($1, $3); }
    | TK_IDENTIFIER '(' list_expr ')'               { $$ = createASTExprFuncCall($1, $3); }
    | LIT_INT                                       { $$ = createASTExprLitInt($1); }
    | LIT_REAL                                      { $$ = createASTExprLitFloat($1); }
    | LIT_CHAR                                      { $$ = createASTExprLitChar($1); }
    | expr '+' expr                                 { $$ = createASTExprAdd($1, $3); }
    | expr '-' expr                                 { $$ = createASTExprSub($1, $3); }
    | expr '*' expr                                 { $$ = createASTExprMul($1, $3); }
    | expr '/' expr                                 { $$ = createASTExprDiv($1, $3); }
    | expr '<' expr                                 { $$ = createASTExprLess($1, $3); }
    | expr '>' expr                                 { $$ = createASTExprGreater($1, $3); }
    | expr OPERATOR_LE expr                         { $$ = createASTExprLE($1, $3); }
    | expr OPERATOR_GE expr                         { $$ = createASTExprGE($1, $3); }
    | expr OPERATOR_EQ expr                         { $$ = createASTExprEQ($1, $3); }
    | expr OPERATOR_DIF expr                        { $$ = createASTExprNE($1, $3); }
    | expr '&' expr                                 { $$ = createASTExprAnd($1, $3); }
    | expr '|' expr                                 { $$ = createASTExprOr($1, $3); }
    | '~' expr                                      { $$ = createASTExprNot($2); }
    | '(' expr ')'                                  { $$ = $2; }
    | '-' expr                     %prec UMINUS     { $$ = createASTExprMinus($2); }
    | KW_INPUT '(' type ')'                         { $$ = createASTExprRead($3); }
    ;

list_expr: expr expr_tail                       { $$ = createASTExprList($1, $2); }
    | /* empty */                               { $$ = NULL; }
    ;

expr_tail: ',' expr expr_tail                   { $$ = createASTExprList($2, $3); }
    | /* empty */                               { $$ = NULL; }
    ;

cmd_if: KW_IF '(' expr ')' cmd cmd_if_tail      { $$ = createASTCommandIf($3, $5, $6); }
    ;
cmd_if_tail: KW_ELSE cmd            { $$ = $2; }
    | /* empty */   %prec "then"    { $$ = createASTEmptyCommand(); }
    ;

cmd_while: KW_WHILE '(' expr ')' cmd        { $$ = createASTCommandWhile($3, $5); }
    ;
%%
