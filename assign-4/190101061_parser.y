%{

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bits/stdc++.h>
using namespace std;


// Function declarations
void yyerror(const char *s);
int yylex(void);
int yyparse(void);
void insert(string,int);
void error(string x);

// Global variables
vector <string> decl_list; // Temporary variable list
unordered_map <string,pair<int,int>> symbol_table; // Symbol table
int errorFlag=0; // Error flag variable

// Color definitions for pretty printing
const std::string red("\033[0;31m");
const std::string green("\033[1;32m");
const std::string magenta("\033[0;35m");
const std::string reset("\033[0m");


#define YYERROR_VERBOSE

// Extern variables
extern int yylineno;
extern FILE* yyin;
extern void yylex_destroy();


vector <string> types ={"Integer", "Real"}; // Variable types
unordered_set <string> reservedKeywords= {"program","var","begin","end","integer","real","for","read","write","to","do","div"}; // Reserved keywords
%}

%union
{
    int intValue;
    float floatValue;
    char *stringValue;
}
%start Program

%token <intValue> PROGRAM 1
%token <intValue> VAR 2
%token <intValue> BEGINPROG 3
%token <intValue> END 4
%token <intValue> EOFCODE  5
%token <intValue> INTEGER 6
%token <intValue> REAL  7
%token <intValue> FOR 8
%token <intValue> READ 9
%token <intValue> WRITE 10
%token <intValue> TO 11
%token <intValue> DO  12
%token <intValue> SEMICOLON 13
%token <intValue> COLON 14
%token <intValue> COMMA 15
%token <intValue> ASSIGNOP 16
%token <intValue> PLUS 17
%token <intValue> MINUS 18
%token <intValue> MULTIPLY 19
%token <intValue> DIVIDE 20
%token <intValue> LPAREN 21
%token <intValue> RPAREN 22
%token <intValue> IDENTIFIER  23
%token <intValue> INT 24
%token <intValue> FLOAT  25

%type <intValue>  Factor Expression Term Type

%%
Program 				: PROGRAM ProgramName 
						VAR DeclarationsList
						BEGINPROG StatementList EOFCODE
                        ;

ProgramName             : IDENTIFIER
                        ;
                        


DeclarationsList 		: Declaration 
						|  DeclarationsList SEMICOLON Declaration 
						;
Declaration             : IdentifierList COLON Type 
                        {
                            // When declaring variables check if variables isn't a reserved keyword and insert into symbol_table
                            for (auto x: decl_list){
                                if(!reservedKeywords.count(x))
                                    insert(x,$3);
                                else error("Variable name can't be a reserved keyword");
                            }
                            
                            decl_list.clear();
                        }
                        ;

Type					: INTEGER {$$=0;}
						| REAL    {$$=1;}
						;
IdentifierList  		: IDENTIFIER 
                        { decl_list.push_back(yylval.stringValue);}
						
                        | IdentifierList COMMA IDENTIFIER 
                        { decl_list.push_back(yylval.stringValue);}
						;
StatementList           : Statement
                        | StatementList SEMICOLON Statement
                        ;

Statement               : Assign
                        | Read
                        | Write
                        | For
                        ;

Assign                  :  IDENTIFIER ASSIGNOP Expression {
    
                            if(!symbol_table.count(yyval.stringValue)){
                                // Check if variables is declared when it is being referenced
                                   error("Variable "+string(yylval.stringValue)+" referenced before declaration");
                            }else{

                                int type1 = symbol_table[yyval.stringValue].first;

                                // Check if type matches in assignment 
                                if(type1!=$3) error("Type mismatch, Trying to assign "+types[$3] +" to "+types[type1]+" variable : "+yylval.stringValue);
                                else symbol_table[yyval.stringValue].second =1;
                            }
                        }
                        ;
Expression              : Term 
                        | Expression PLUS Term { if($1!=$3) error("Type mismatch, Trying to add "+types[$3] +" to "+types[$1]);}
                        | Expression MINUS Term { if($1!=$3) error("Type mismatch, Trying to subtract "+types[$3] +" to "+types[$1]);}
                        ;

Term                    : Factor {$$=$1;}
                        | Term MULTIPLY Factor { if($1!=$3) error("Type mismatch, Trying to multiply "+types[$3] +" to "+types[$1]);}
                        | Term DIVIDE Factor { if($1!=$3) error("Type mismatch, Trying to divide "+types[$3] +" to "+types[$1]);}
                        ;

Factor                  : IDENTIFIER
                        {
                            // Check if symbol table contains identifier
                            if(symbol_table.count(yyval.stringValue)){
                                // Check if variables is initialised when it is being used
                                if(symbol_table[yyval.stringValue].second==0)
                                    error("Variable "+string(yylval.stringValue)+" used before initialisation"); 
                                $$ = symbol_table[yylval.stringValue].first;
                            }
                            else{
                                // Check if variables is referenced before declaration
                                error("Variable "+string(yylval.stringValue)+" referenced before declaration");
                                $$=0;
                            }
                        }
                        
                        | INT {$$=0;}
                        | FLOAT {$$=1;}
                        | MINUS INT {$$=0;}
                        | MINUS FLOAT {$$=1;}
                        | LPAREN Expression RPAREN {$$=$2;}
                
                        ;

Read                    : READ LPAREN IdentifierList RPAREN
                            {
                                // if variables are read using READ statement then they should be initialised
                                for (auto x : decl_list){
                                    symbol_table[x].second=1;
                                }
                                decl_list.clear();
                            }
                        ;

Write                   : WRITE LPAREN IdentifierList RPAREN
                            {
                                decl_list.clear();
                            }
                        ;


For                     :  FOR IndexExp DO Body
                        ;

IndexExp                :  IDENTIFIER ASSIGNOP Expression TO Expression
                            {
                                if($3!=$5){
                                    error("Type mismatch, Trying to iterate from type "+types[$3] +" to "+types[$5]);
                                }
                                else if(!symbol_table.count(yyval.stringValue)){
                                    // Check if variables is declared when it is being referenced
                                    error("Variable "+string(yylval.stringValue)+" referenced before declaration");
                                }else{
                                    int type1 = symbol_table[yyval.stringValue].first;
                                    // Check if type matches in assignment 
                                    if(type1!=$3) error("Type mismatch, Trying to assign "+types[$3] +" to "+types[type1]+" variable : "+yylval.stringValue);
                                    else symbol_table[yyval.stringValue].second =1;
                                }
                            }
                        ;

Body                    : Statement
                        | BEGINPROG StatementList END
                        ;



%%

// error handlers 
void yyerror(const char *s) {
    // Error function for syntax errors
    errorFlag++;
    cout<<red<<yylineno<<": Syntax error : "<<reset<<s<<endl;
    cout << red << "Parsing terminated" << endl;
    cout << "Total" <<errorFlag<<" error(s) have been detetected till now, please fix and run again."<<endl;
    
    // exit compilation of the program
	exit(EXIT_FAILURE);
}
void error(string x){
    // Error function for semantic errors
    errorFlag++;
    cout <<red<<to_string(yylineno)<<": Semantic error : "<<reset<<x<<endl;
}

void insert(string x,int type1)
{
    // Insert function inserts an identifier into the symbol table

    // Check if symbol table already contains the identifier
    if(symbol_table.count(x)){
        if(type1 == symbol_table[x].first) // Contains same identifier of same type
            error("Redeclaration of variable :  "+x);
        else {
            error("Redefinition of variable : "+x+" from "+types[symbol_table[x].first]+ " to "+types[type1]); // Contains same identifier of different type
        }
    }
    else{
        // Insert into symbol table if all clear
        symbol_table.insert({x,{type1,0}});
    }
}

int main(int argc, char* argv[]) {
    
    if(argc != 2) {
        printf("Program usage: %s <file> \n", argv[0]);
        exit(EXIT_FAILURE);
    }

    yyin = fopen(argv[1], "r"); // Open file passed as argument
    cout <<endl << "File opened succesfully, running parser : "<<endl<<endl;
    int result = yyparse(); // Run yyparse

    if(errorFlag==0) {
        // If error flag is 0 no errors occured and program is parsed succssfully
        cout <<green<<"Parsing completed successfully!!"<<reset<<endl;
        // Print symbol table
        cout<<endl<<endl<<magenta<<"SYMBOL TABLE : "<<reset<<endl;
        cout<<left<<setw(15)<<"Identifier"<<"   "<<setw(15)<<"Type"<<setw(15)<<"Initialised"<<endl;
        for(auto x: symbol_table){
            cout<<left<<setw(15)<<x.first<<"   "<<setw(15)<<types[x.second.first]<<setw(5)<<x.second.second<<endl;
        }
    }
    else{
        // If error flag is not zero errors occurred
        cout << endl<<"Parsing failed "<<"due to "<<errorFlag<<" error(s), please fix and run again."<<endl;
    }
    fclose(yyin); //Close file
    yylex_destroy();
    
    return result;
}
