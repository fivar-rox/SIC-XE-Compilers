CS348 Assignment 1 of Implementation of Programming Languages Lab (6th sem)
Title : Two Pass Assembler submitted on 25 January 2022.
Done by : Sai Ashrritth Patnana - 190101061

INSTRUCTIONS TO EXECUTE THE ASSEMBLER
--------------------------------------

1. The environment required for running the code is Linux g++, or u can also use codeblocks mingw.
2. For compilation, type
				g++ 190101061_Assign01.cpp
3. To run the file, type
				./a.out <name_of_input_file> <name_of_the_output_file> 
   Providing the <filenames> are optional.
   If no command line argument is provided, the default filenames are "input.txt" and "output.txt".
   If only one argument is provided then the code treats it as the inputfile name and uses default name for output.
   Example:
			1: ./a.out input_file.txt output_file.txt
			2: ./a.out input_file.txt
			3: ./a.out
			There are the three possible ways to run the file.
4. The code will need a optab.txt file for optab values.