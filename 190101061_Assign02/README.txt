CS348 : Implementation of Programming Languages Lab (6th sem)
ASSIGNMENT-II
Title : 2-Pass Assmbler and linking loader for SIC/XE machine
Submitted on 24 February 2022.
Done by : Sai Ashrritth Patnana - 190101061

NOTE : C++ doesn't have a fully established filesystem directories, DO NOT delete folders (even empty folders).

INSTRUCTIONS TO EXECUTE THE ASSEMBLER
--------------------------------------

1. The environment required for running the code is <LINUX g++>, or it also runs <Codeblocks GNU GCC> compiler.
2. For compilation, go into the assembler folder and then:
				g++ -o assembler assembler.cpp
3. To run the file, type
				./assembler <path_of_input_file> <path_of_the_output_file>
                NOTE: PATHS SHOULD BE RELATIVE TO assembler.cpp (../<path>) or <ABSOLUTE_PATH>
   Providing the <paths> are optional.
   If no command line argument is provided, the default filenames are "../Inputs/input.txt" and "../Outputs/objcode.txt".
   If only one argument is provided then the code treats it as the inputfile name.
   Example:
			1: ./a.out input_file_path output_file_path
			2: ./a.out input_file_path
			3: ./a.out
			There are the three possible ways to run the file.
4. The code will need a optab.txt file in Inputs with the same name format.
5. Intermediate and symtab files are located in "Assembler/Data/" folder.

INSTRUCTIONS TO EXECUTE THE LINKING LOADER:
--------------------------------------------

1. The environment required for running the code is <LINUX g++>, or it also runs <Codeblocks GNU GCC> compiler.
   NOTE: If codeblocks evironment is used, comment or remove line 293. (EOL char is also taken as input in Linux g++)
2. For compilation, go into the assembler folder and then:
				g++ -o loader link_loader.cpp
3. To run the file, type
				./loader <path_of_input_file> <path_of_the_output_file>
                NOTE: PATHS SHOULD BE RELATIVE TO link_loader.cpp (../<path>) or <ABSOLUTE_PATH>
   Providing the <paths> are optional.
   If no command line argument is provided, the default filenames are "../Inputs/onjinput.txt" and "../Outputs/loaderOutput.txt".
   If only one argument is provided then the code treats it as the inputfile name.
   Example:
			1: ./a.out input_file_path output_file_path
			2: ./a.out input_file_path
			3: ./a.out
			There are the three possible ways to run the file.
    Provide the starting address => Preferably 4000.
4. ESTAB file will be located in "Loader/Data/" folder.

NOTE : Error in link_loader.txt => try removing line 293
NOTE : In case of different input files, if the link loader produces wrong results/errors try adding an empty line at the end of the objinput.txt 
