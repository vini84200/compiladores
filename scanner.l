%{

//
// +--------------------------------------------------------------------------+
// |  Compiladores - 2023/2                                                   |
// |  VINICIUS JOSÉ FRITZEN -- 335519                                         |
// +--------------------------------------------------------------------------+
//

#include "lib.h"
#include "y.tab.h"
#include "hash.h"
%}

%x MULTILINECOMMENT

%option warn nodefault yylineno noyywrap batch

%%

<MULTILINECOMMENT>"\\\\\\"      {BEGIN (INITIAL);}
<MULTILINECOMMENT>.             {}
<MULTILINECOMMENT>\n            {}
"\/\/\/".*              {BEGIN(MULTILINECOMMENT);}

\/\/.*                  {}

char        {return KW_CHAR;}
int         {return KW_INT;}
float       {return KW_FLOAT;}
code        {return KW_CODE;}
if          {return KW_IF;}
else        {return KW_ELSE;}
while       {return KW_WHILE;}
input       {return KW_INPUT;}
print       {return KW_PRINT;}
return      {return KW_RETURN;}


"<="        {return OPERATOR_LE;}
">="        {return OPERATOR_GE;}
"=="        {return OPERATOR_EQ;}
"!="        {return OPERATOR_DIF;}


[-,;()\[\]{}=+*\/%<>&|~] {return yytext[0];}

[0-9]+  {
                hashInsert(getSymbolTable(), LIT_INT, yytext);
                return LIT_INT;
}
[0-9]+\.[0-9]+  {
                hashInsert(getSymbolTable(), LIT_REAL, yytext);
                  return LIT_REAL;
}
\'(\\.|.)\'           {
                hashInsert(getSymbolTable(), LIT_CHAR, yytext);
                  return LIT_CHAR;
}
\"(\\.|[^"])*\"      {
                hashInsert(getSymbolTable(), LIT_STRING, yytext);
                  return LIT_STRING;
}

[a-zA-Z0-9_]+  {
                hashInsert(getSymbolTable(), TK_IDENTIFIER, yytext);
                return TK_IDENTIFIER;
               }

[ \t\n]       {}
.           {printf("Possivel erro: %c", yytext[0]); return TOKEN_ERROR;}

%%

