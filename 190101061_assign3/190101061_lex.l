/*
-- 190101061 - Patnana Sai Ashrritth -- 3rd Year Btech CSE -
-- CS348 - Implementation of programming languages -

Instructions to compile and run :-
-----------------------------------
-- Use Linux g++ environment to compile the code
-- Use "sudo apt-get install flex" command for lexical analyzer
-- Open the terminal and go to the assignment folder(directory).
-- To create the lex file
		-> lex -o 190101061.yy.c 190101061.l
-- To compile the code
		-> g++ 190101061.yy.c
-- To run the 
		-> ./a.out < input.p [or] ./a.out < "input_filename" 
-- OR u can simple use this one 
        -> lex -o 190101061.yy.c 190101061.l; g++ 190101061.yy.c; ./a.out < input.p

-- Check the "output.txt" file which is created in the same directory.

Note : Avoid using regualar expression in commands while compiling the code, leads some warnings.
*/
%{
// Token coding scheme
int PROGRAM = 1;
int VAR = 2;
int BEGINPROGRAM = 3;
int END = 4;
int EOFCODE = 5;
int INTEGER = 6;
int FOR = 7;
int READ = 8;
int WRITE = 9;
int TO = 10;
int DO = 11;
int SEMI = 12;
int COLON = 13;
int COMMA = 14;
int COLEQ = 15;
int PLUS = 16;
int MINUS = 17;
int TIMES = 18;
int DIV = 19;
int LPAREN = 20;
int RPAREN = 21;
int ID = 22;
int INT = 23;

#include <bits/stdc++.h>
using namespace std;

// for writability
typedef string str;
int o = 1, z = 0; // buttons dont work properly thats why i am using this.
#define deli "^"
#define hash "#"
#define width1 setw(4)
#define width3 setw(10)
#define addspace setfill(' ')


// Lex variables and functions
int yylval, tabInd = z, yyline = o, yycolumn[2] = {o,z};
vector <str> symtab(278);
void handle_incorrect(str s);
%}

%option noyywrap

digit  [0-9]
number {digit}+
letter [a-zA-Z]
id     {letter}({letter}|{digit})*

%%

\n						{yyline++; yycolumn[0] = o;}
[ \t]+					{yycolumn[0] += yyleng;}

(PROGRAM|program)		return PROGRAM;
(BEGIN|begin)			return BEGINPROGRAM;
(END|end)\.				return EOFCODE;
(VAR|var)				return VAR;
(INTEGER|integer)		return INTEGER;
(END|end)				return END;
(FOR|for)				return FOR;
(WRITE|write)			return WRITE;
(DIV|div)				return DIV;
(READ|read)				return READ;
(DO|do)					return DO;
(TO|to)					return TO;

{number}				{str s(yytext); symtab[(yylval = tabInd++)]=s; return INT;}
{id}					{str s(yytext); symtab[(yylval = tabInd++)]=s; return ID;}

":="					return COLEQ;
"-"						return MINUS;
"*"						return TIMES;
"+"						return PLUS;

","						return COMMA;
":"						return COLON;
"."						return EOFCODE;
";"						return SEMI;
"("						return LPAREN;
")"						return RPAREN;
.						{str s(yytext); handle_incorrect(s);}

%%

void handle_incorrect(str s){
	cout << "Incorrect character \'" + s + "\'" << ", line=" << yyline << " column=" << yycolumn[0] << endl; yycolumn[0] += yyleng;
}

int main()
{	
	int word;
	int prev = z;
	ofstream output;
	output.open("output.txt");

	output << width1 << addspace << "LINE";
	output << width3 << addspace << "TYPE";
	output << width3 << addspace << "S" << "PECIFIER" << endl;
	output << width1 << addspace << "----";
	output << width3 << addspace << "----";
	output << width3 << addspace << "-" << "--------" << endl;
	
	for(;(word=yylex()) != EOFCODE && output.good();){
		// Read line by line and print output in format
		str lineNo = (prev != yyline) ? to_string(yyline) : ""; //Change line number
		prev=yyline;
		
		if(word!=ID && word <=23 && word>=1 ) yycolumn[0] += yyleng;

		if (word==INT && output.is_open()){
			output << width1 << addspace << lineNo;
			output << width3 << addspace << word;
			output << width3 << addspace << hash << symtab[yylval] << endl; 
		}
		else if(word==ID && output.is_open()){
			output << width1 << addspace << lineNo;
			output << width3 << addspace << word;
			output << width3 << addspace << deli << symtab[yylval] << endl;
		}
		else if (output.is_open()){
			output << width1 << addspace << lineNo;
			output << width3 << addspace << word << endl;
		}
		else if (!output.is_open()) {
			cout << "ERROR : Output stream interrupted : EXITING" << endl;
			exit(z);
		}
	}

	if(!output.good()){
		cout << "ERROR : Output stream interrupted : EXITING" << endl;
		exit(z);
	}

	output << width1 << addspace << yyline;
	output << width3 << addspace << EOFCODE << endl; // Print EOFCODE
	
	cout << "Completed Successfully!" << endl << "Output file generated in ./output.txt" << endl;
	return z;
}



