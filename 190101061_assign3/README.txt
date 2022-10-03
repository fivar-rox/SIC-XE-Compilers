CS348 : Implementation of Programming Languages Lab (6th sem)
ASSIGNMENT-III
Submitted on 21 March 2022.
Done by : Sai Ashrritth Patnana - 190101061

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