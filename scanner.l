%{

//
// +--------------------------------------------------------------------------+
// |  Compiladores - 2023/2                                                   |
// |  VINICIUS JOSÉ FRITZEN -- 335519                                         |
// +--------------------------------------------------------------------------+
//

#include "scannerDefinitions.c"
%}


%x MULTILINECOMMENT

%option warn yylineno noyywrap batch nounput never-interactive noinput

%%

<MULTILINECOMMENT>\\\\\\        {BEGIN (INITIAL);}
<MULTILINECOMMENT>.             {}
<MULTILINECOMMENT>\n            {resetCollumn();}
"\/\/\/"                      {BEGIN(MULTILINECOMMENT);}

\/\/[^\n\/].*                  {resetCollumn();}
\/\/                          {resetCollumn();}


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
[0-9]+          {
                    yylval.symbol = hashInsert(getSymbolTable(), SYMBOL_LIT_INT, yytext);
                    return LIT_INT;
                }
[0-9]+\.[0-9]+  {
                    yylval.symbol = hashInsert(getSymbolTable(), SYMBOL_LIT_FLOAT, yytext);
                    return LIT_REAL;
                }
\'(\\.|.)\'     {
                    yylval.symbol = hashInsert(getSymbolTable(), SYMBOL_LIT_CHAR, yytext);
                    return LIT_CHAR;
                }
\"(\\.|[^"])*\"      {
                        yylval.symbol = hashInsert(getSymbolTable(), SYMBOL_LIT_STRING, yytext);
                        return LIT_STRING;
                     }

[a-zA-Z0-9_]+  {
                   yylval.symbol = hashInsert(getSymbolTable(), SYMBOL_IDENTIFIER, yytext);
                   return TK_IDENTIFIER;
               }

[ \t]       {}
\n          {resetCollumn();}
.           {
            handleUnrecognizedToken(yytext[0], yylineno);
            return TOKEN_ERROR;
}

%%

