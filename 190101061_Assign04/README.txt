CS348 : Implementation of Programming Languages Lab (6th sem)
ASSIGNMENT-IV
Title : 4-Genrating a parser using YACC and Lex
Submitted on 20 April 2022.
Done by : Sai Ashrritth Patnana - 190101061

INSTRUCTIONS TO EXECUTE THE PARSER
-----------------------------------

>>> One command to run the whole thing :
    -->> make; echo -e "\n"; ./parser input.p; cat "symbol_table.txt"; echo -e "\n"; ./parser input_error.p; echo -e "\n"; ./parser input_error2.p; make clean

1. The environment required for running the code is on LINUX g++.
2. To install lex and flex use command >> sudo apt-get install bison flex
3. For compilation, go into the assembler folder and then:
				-> make

4. To run the parser, type
				-> ./parser <inputfile>

                NOTE: PATHS SHOULD BE RELATIVE TO parser (../<path>) or <ABSOLUTE_PATH>
   Providing the <inputfile> is optional.
   If no command line argument is provided, the default inputfile  "input.p" will be used.
   3 sample inputs are given,
        > input.p
        > input_error.p
        > input_error2.p

5. The symbol table will be created upon successful completion of the parser and will be stored on symbol_table.txt.

6. A 'make clean' option is also provided.

NOTE: The parser is designed to stop parsing upon encountering a syntax error.
