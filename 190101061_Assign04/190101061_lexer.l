/*
CS348 : Implementation of Programming Languages Lab (6th sem)
ASSIGNMENT-IV
Title : 4-Genrating a parser using YACC and Lex
Submitted on 20 April 2022.
Done by : Sai Ashrritth Patnana - 190101061
Creates tokens for YACC
*/

%{
#include "y.tab.h"
#include <stdio.h>
#include <bits/stdc++.h>
using namespace std;

const std::string red("\033[0;31m");
const std::string reset("\033[0m");



void charError(char c)
{
    // Error function for unrecognised character 
    cout<< endl << red << setw(8) <<"line " + to_string(yylineno) << setw(17) << ": Lexical error" << ": " << reset << "unexpected character "<< c << ".";
    return;
}

%}

%option noyywrap
%option caseless
%option yylineno

DIGIT 				[0-9]
DIGITS  			{DIGIT}+
INTEGER 			{DIGITS}
FLOAT				{INTEGER}(\.{DIGITS}([Ee][+-]?{DIGITS})?)


%%
[ \t]			                    ;
\n				                    {}
"{"[^"}"]*"}"                       ;
"program"                           { return PROGRAM; }
"var"                               { return VAR; }
"begin"                             { return BEGINPROG; }
"("                                 { return LBRACK; }
")"                                 { return RBRACK; }
"end"                               { return END; }
"end."                              {return EOFCODE;}
";"                                 { return SEMICOLON; }   
":"                                 { return COLON; }
","                                 { return COMMA; }
:=					                { return ASSIGNOP; }
"+"                                 { return PLUS; }
"-"                                 { return MINUS; }
"for"                               { return FOR;}
"read"                              { return READ;}
"write"                             { return WRITE;}
"to"                                { return TO; }
"do"                                { return DO; }
"*"                                 { return MULTIPLY; }
"DIV"                               { return DIVIDE; }
"real"                              { return REAL; }
"integer"                           { return INTEGER; }
{INTEGER}                           { return INT; }
{FLOAT}	                            { return FLOAT; }
[a-zA-Z][_a-zA-Z0-9]*		        { yylval.strVal=strdup(yytext);return ID; }

.                                   { charError(yytext[0]); }
%%

