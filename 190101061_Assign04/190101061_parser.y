/*
CS348 : Implementation of Programming Languages Lab (6th sem)
ASSIGNMENT-IV
Title : 4-Genrating a parser using YACC and Lex
Submitted on 20 April 2022.
Done by : Sai Ashrritth Patnana - 190101061

INSTRUCTIONS TO EXECUTE THE PARSER
-----------------------------------

1. The environment required for running the code is on LINUX g++.
2. To install lex and flex use command >> sudo apt-get install bison flex
3. For compilation, go into the assembler folder and then:
				>> make
4. To run the parser, type
				>> ./parser <inputfile>
                NOTE: PATHS SHOULD BE RELATIVE TO parser (../<path>) or <ABSOLUTE_PATH>
   Providing the <inputfile> is optional.
   If no command line argument is provided, the default inputfile  "input.p" will be used.

5. The symbol table will be created upon successful completion of the parser and will be stored on symbol_table.txt.

6. A make clean option is also provided.
*/

%{
#include "utility.h"

void yyerror(const char *s);

#define YYERROR_VERBOSE

// Extern variables
extern int yylineno;
extern FILE* yyin;
extern void yylex_destroy();
%}

%union
{
    unsigned int int_val;
    double real_val;
    char *strVal;
}
%start Program

%token <int_val> PROGRAM 1
%token <int_val> VAR 2
%token <int_val> BEGINPROG 3
%token <int_val> END 4
%token <int_val> EOFCODE  5
%token <int_val> INTEGER 6
%token <int_val> REAL  7
%token <int_val> FOR 8
%token <int_val> READ 9
%token <int_val> WRITE 10
%token <int_val> TO 11
%token <int_val> DO  12
%token <int_val> SEMICOLON 13
%token <int_val> COLON 14
%token <int_val> COMMA 15
%token <int_val> ASSIGNOP 16
%token <int_val> PLUS 17
%token <int_val> MINUS 18
%token <int_val> MULTIPLY 19
%token <int_val> DIVIDE 20
%token <int_val> LBRACK 21
%token <int_val> RBRACK 22
%token <int_val> ID  23
%token <int_val> INT 24
%token <int_val> FLOAT  25

%type <int_val>  Factor Expression Term Type


// declaring all the rulers and factors
%%
Program 				: PROGRAM ProgramName VAR DECLsList BEGINPROG STMTList EOFCODE
                        ;

ProgramName             : ID
                        ;
                        


DECLsList 		        : DECL 
						| DECLsList SEMICOLON DECL 
						;
DECL                    : IDList COLON Type 
                        {
                            for (auto x: new_var_list) insert((str)x,$3, yylineno);                            
                            clear_new_var_list();
                        }
                        ;

Type					: REAL    {$$=1;}
                        | INTEGER {$$=0;}
						;
IDList  		        : ID 
                        { 
                            // When declaring variables check if variables 
                            // isn't a reserved keyword and insert into symbol_table
                            if(!reservedKeywords.count(yylval.strVal) && v){
                                new_var_list.push_back(yylval.strVal);
                            }
                            else sem_err("Variable name can't be a reserved keyword", yylineno);

                        }
						
                        | IDList COMMA ID 
                        { 
                            // When declaring variables check if variables 
                            // isn't a reserved keyword and insert into symbol_table
                            if(!reservedKeywords.count(yylval.strVal) && v){
                                new_var_list.push_back(yylval.strVal);
                            }
                            else sem_err("Variable name can't be a reserved keyword", yylineno);
                        }
						;
STMTList                : STMTList SEMICOLON STMT
                        | STMT
                        ;

STMT                    : Assign
                        | Write
                        | For
                        | Read
                        ;

Assign                  :  ID ASSIGNOP Expression {
                            if(check_sym(yyval.strVal)){

                                int type1 = symbol_table[yyval.strVal].first;
                                // Check if type matches in assignment 
                                if(type1!=$3) sem_err("Type mismatch, Trying to assign "+types[$3] +" to "+types[type1]+" variable : "+yylval.strVal,yylineno);
                                else symbol_table[yyval.strVal].second =1;
                            }
                            else{
                                // Check if variables is declared when it is being referenced
                                sem_err("Reference of an unknown variable : "+string(yylval.strVal),yylineno);
                            }
                        }
                        ;
Expression              : Term 
                        | Expression MINUS Term 
                        { if(!($1==$3)) sem_err("Type mismatch, Trying to subtract "+types[$3] +" to "+types[$1],yylineno);}
                        | Expression PLUS Term 
                        { if(!($1==$3)) sem_err("Type mismatch, Trying to add "+types[$3] +" to "+types[$1],yylineno);}
                        ;

Term                    : Factor {$$=$1;}
                        | Term DIVIDE Factor { if($1!=$3) sem_err("Type mismatch, Trying to divide "+types[$3] +" to "+types[$1],yylineno);}
                        | Term MULTIPLY Factor { if($1!=$3) sem_err("Type mismatch, Trying to multiply "+types[$3] +" to "+types[$1],yylineno);}
                        ;

Factor                  : ID
                        {
                            // Check if symbol table contains ID
                            if(check_sym(yyval.strVal)){
                                // Check if variables is initialised when it is being used
                                if(symbol_table[yyval.strVal].second==0)
                                    sem_err("Variable "+string(yylval.strVal)+" used before initialisation",yylineno); 
                                $$ = symbol_table[yylval.strVal].first;
                            }
                            else{
                                // Check if variables is referenced before declaration
                                sem_err("Reference of an unknown variable : "+string(yylval.strVal),yylineno);
                                $$=0;
                            }
                        }
                        
                        | LBRACK Expression RBRACK {$$=$2;}
                        | FLOAT {$$=1;}
                        | INT {$$=0;}
                        | MINUS FLOAT {$$=1;}
                        | MINUS INT {$$=0;}
                
                        ;

Read                    : READ LBRACK IDList RBRACK
                            {
                                // if variables are read using READ STMT then they should be initialised
                                for (auto x : new_var_list){
                                    if(!check_sym(x)){
                                        sem_err("Reference of an unknown variable : "+(str)x, yylineno);
                                    }
                                    symbol_table[(str)x].second=1;
                                }
                                clear_new_var_list();
                            }
                        ;

Write                   : WRITE LBRACK IDList RBRACK
                            {
                                clear_new_var_list();
                            }
                        ;

For                     :  FOR IndexExp DO Body
                        ;

IndexExp                :  ID ASSIGNOP Expression TO Expression
                            {
                                if(!($3==$5)){
                                    sem_err("Type mismatch, Trying to iterate from type "+types[$3] +" to "+types[$5],yylineno);
                                }
                                else if(!check_sym(yyval.strVal)){
                                    // Check if variables is declared when it is being referenced
                                    sem_err("Reference of an unknown variable : " + string(yylval.strVal),yylineno);
                                }else{
                                    int type1 = symbol_table[yyval.strVal].first;
                                    // Check if type matches in assignment 
                                    if(type1 == $3) symbol_table[yyval.strVal].second =1;
                                    else sem_err("Type mismatch, Trying to assign "+types[$3] +" to "+types[type1]+" variable : "+yylval.strVal,yylineno);
                                }
                            }
                        ;

Body                    : BEGINPROG STMTList END
                        | STMT
                        ;

%%


// error handlers 
void yyerror(const char *s) {
    // Error function for syntax errors
    if(first_error){
        cout << endl << setw(80) << addhyphen << "" << addspace;
        first_error = false;
    }
    int yyl = yylineno;
    errorFlag++;
    cout<<endl<<red<< width1 << "line " + to_string(yyl) << width3 << ": Syntax error" << ": "<<reset<<s;
    cout << endl<< "Parsing TERMINATED ---> due to a syntax error.";
    cout << endl << setw(80) << addhyphen << "" << addspace;
    cout << endl<<"Total " <<errorFlag<<" error(s) have been detetected till now, please fix and run again."<<endl;
    print_symtab();
    // exit compilation of the program
	exit(EXIT_FAILURE);
}

int main(int argc, char* argv[]) {
    
    if(argc >= 2) {
        input_filename = argv[1];
    }
    else{
        cout << "Using default input filename => \'input.p\'." << endl;
    }
    
    yyin = fopen(input_filename, "r"); // Open file passed as argument
    if(yyin == NULL) {
        cout << "Unable to open " << argv[1] <<  " ... Exiting" << endl;
        cout << "Check the filename and try again." << endl; 
        exit(EXIT_FAILURE);
    }
    cout << "File \'" << input_filename <<  "\'" << " opened succesfully. Starting parser:-" << endl;
    cout << left << width4 << adddots << "Running parser ";
    cout << addspace;
    
    int start_parse = yyparse(); // Run yyparse

    if(errorFlag>0) {
        // If error flag is not zero errors occurred
        cout << setw(80) << addhyphen << "" << endl << addspace;
        cout << endl << errorFlag <<" error(s) have been detetected, Parsing failed. please fix and run again."<<endl;
    }
    else{
        // If error flag is 0 no errors occured and program is parsed succssfully
        cout <<green<<" Done"<<reset<<endl;
        cout << errorFlag << " errors(s) have been detetected. Parsing succuessful." << endl;
        // Print symbol table
        create_symtab();
    }
    fclose(yyin); //Close file
    yylex_destroy();
    
    return start_parse;
}
