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

#include <bits/stdc++.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
using namespace std;

#define width0 setw(5)
#define width1 setw(8)
#define width2 setw(15)
#define width3 setw(17)
#define width4 setw(40)
#define width5 setw(45)

#define addspace setfill(' ') 
#define adddots setfill('.')
#define addhyphen setfill('-')

typedef string str;

// Function declarations
int yylex(void);
int yyparse(void);
void insert(str,int,int);
void sem_err(str x, int yyl);
void create_symtab();
void print_symtab();
void clear_new_var_list();
bool check_sym(char* val);


// Global variables
vector <char*> new_var_list; // Temporary variable list
map <str,pair<int,int>> symbol_table; // Symbol table
unsigned int errorFlag=0; // Error flag variable

char* input_filename = (char*)"input.p";
bool v = true;
// for lines above and below the errors
bool first_error = true;

// Color definitions to show error messages
const str red("\033[0;31m");
const str green("\033[1;32m");
const str reset("\033[0m");
const std::string magenta("\033[0;35m");


string types[2] ={"Integer", "Real"}; // Variable types
unordered_set <str> reservedKeywords= {"program","var","begin","end","integer","real","for","read","write","to","do","div"}; // Reserved keywords


// semantic error handler
void sem_err(str x, int yyl){
    // Error function for semantic errors
    if(first_error){
        cout << endl << setw(80) << addhyphen << "" << addspace;
        first_error = false;
    }
    errorFlag++;
    cout <<endl << red<< width1 << "line " + to_string(yyl) << width3 << ": Semantic error"<< ": "<<reset<<x;
}

// inserts element into the symbol table
void insert(str x,int type1, int yyl)
{
    // Insert function inserts an identifier into the symbol table

    // Check if symbol table already contains the identifier
    if(!symbol_table.empty() && symbol_table.count(x)){
        if(type1 == symbol_table[x].first) // Contains same identifier of same type
            sem_err("Redeclaration of variable : "+x,yyl);
        else if(type1 != symbol_table[x].first){
            // Contains same identifier of different type
            sem_err("Redefinition of variable : "+x+" from "+types[symbol_table[x].first]+ " to "+types[type1],yyl); 
        }
    }
    else{
        // Insert into symbol table if all clear
        symbol_table.insert({x,{type1,0}});
    }
}

// export symbol table into symbol_table.txt
void create_symtab(){
    ofstream sym("symbol_table.txt");
    if(!sym.is_open() || !sym.good()){
        cout << "Unable to export symbol table." << endl;
    }
    cout << left << width4 << adddots << "Exporting symbol table ";
    cout << addspace;
    sym<<"SYMBOL TABLE : "<<endl;
    sym << width5 << addhyphen << "" << endl << addspace;
    sym<<left<<width2<<"Identifier"<<"   "<<width2<<"Type"<<width2<<"Initialised"<<endl;
    sym << width5 << addhyphen << "" << endl << addspace;
    for(auto x: symbol_table){
        sym<<left<<width2<<x.first<<"   "<<width2<<types[x.second.first]<<width0<<x.second.second<<endl;
    }
    cout << green << " Done" << reset << endl;
    return;
}

// prints incomplete symbol table for failed parsing
void print_symtab(){
    
    cout << "Printing symbol table ... " << endl;
    cout << width5 << addhyphen << "" << endl << addspace;
    cout<<magenta << left<<width2<<"Identifier"<<"   "<<width2<<"Type"<<width2<<"Initialised"<<reset << endl;
    cout << width5 << addhyphen << "" << endl << addspace;
    for(auto x: symbol_table){
        cout<<left<<width2<<x.first<<"   "<<width2<<types[x.second.first]<<width0<<x.second.second<<endl;
    }
    return;
}

// clears declaration in current line
void clear_new_var_list(){
    new_var_list.clear();
    return;
}

// checks if a declaration have been made
bool check_sym(char* val){
    return symbol_table.count(val);
}