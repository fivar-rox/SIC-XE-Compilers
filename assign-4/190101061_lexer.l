%{
#include "y.tab.h"
#include <stdio.h>
#include <bits/stdc++.h>
using namespace std;



void charError(char c)
{
    // Error function for unrecognised character 
    cout<< "Lexical error, unexpected character "<<c<<" on line "<<yylineno<<endl;
    
}

%}

%option noyywrap
%option caseless
%option yylineno

DIGIT 				[0-9]
DIGITS  			{DIGIT}+
INTEGER 			{DIGITS}
FLOAT				{INTEGER}(\.{DIGITS}([Ee][+-]?{DIGITS})?)
NUMBER 				{DIGITS}

%%
[ \t]			                    ;
\n				                    {}
"{"[^"}"]*"}"                       ;
"program"                           { return PROGRAM; }
"var"                               { return VAR; }
"begin"                             { return BEGINPROG; }
"end"                               { return END; }
"end."                              {return EOFCODE;}
"integer"                           { return INTEGER; }
"real"                              { return REAL; }
"for"                               { return FOR;}
"read"                              { return READ;}
"write"                             { return WRITE;}
"to"                                { return TO; }
"do"                                { return DO; }
";"                                 { return SEMICOLON; }   
":"                                 { return COLON; }
","                                 { return COMMA; }
:=					                { return ASSIGNOP; }
"+"                                 { return PLUS; }
"-"                                 { return MINUS; }
"*"                                 { return MULTIPLY; }
"DIV"                               { return DIVIDE; }
"("                                 { return LPAREN; }
")"                                 { return RPAREN; }
{INTEGER}                           { return INT; }
{FLOAT}	                            { return FLOAT; }
[a-zA-Z][_a-zA-Z0-9]*		        { yylval.stringValue=strdup(yytext);return IDENTIFIER; }

.                                   { charError(yytext[0]); }
%%

