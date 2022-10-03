/*
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
   If only one argument is provided then the code treats it as the inputfile name.
   Example:
			1: ./a.out input_file.txt output_file.txt
			2: ./a.out input_file.txt
			3: ./a.out
			There are the three possible ways to run the file.
4. The code will need a optab.txt file with the same name format.
*/

#include <bits/stdc++.h>

using namespace std;

// macros to be used in intermediate file and console log
#define width1 setw(6)
#define width2 setw(10)
#define width3 setw(20)
#define width4 setw(30)
#define addspace setfill(' ')

string word = "WORD";
string end1 = "END";
string resw = "RESW";
string resb = "RESB";
string byte = "BYTE";
string start = "START";

//Utility Functions
string intToHex(long long x);
long long hexToInt(string x);
void word_space_handler(vector<string> &word_list);
void organize_fields(vector<string> inp_fields);
void generatewords(vector <string> &word_list, string input);
void set_fields(string const &input);
string padding(string x, long long len = 6, char y = '0', bool right = false);
int Two = 2, three = 3;
void Load_OPTAB(map <string,string> &optab);
void Load_SYMTAB(map <string,string> &symtab);
void Generate_symtable(map <string,string> &symtable);
void filename_collision();

// Assembler function declarations
void pass1();
void pass2();

// Keeps track of whether pass1 is running or pass Two is running
bool Pass1 = false, Pass2 = false;

// Declarations of variables
string inp, 		// takes input
    label, 			// stores label field of the line
    opcode, 		// stores opcode field of the line
    operand, 		// stores operand field of the line
    lineAddress;	// stores address field from the intermediate file.

long long progLength = 0;
string progName, start_addr; //progName is name of the program and startAddress is the starting address of first instruction

// Names of the input file and output files
string input_filename = "input.txt", output_filename = "object_code.txt";

int main(int argc, char* argv[]){
	if(argc >= three){
		output_filename = argv[Two];
		cout<< "Object code destination :- "<< output_filename << endl;
        input_filename = argv[1];
        cout<< "Input source : "<< input_filename << endl;
	}
	if(argc == Two){
        cout<<"Filename of output file not specified" << endl;
        cout<<"Default :: writing output in "<< output_filename <<"\n";

		input_filename = argv[1];
        cout<< "Input source : "<< input_filename<< endl;
    }
    else{
        cout<<"Filename of input file and output file not specified" << endl;
        cout<<"Default :: taking input from and writing output to, "<<input_filename<<" and " << output_filename << endl;
    }
	filename_collision();
	
	// assembler process start
	pass1();
	pass2();
	return 0;
}

string intToHex(long long n){
	//Takes an integer and return Hex string corresponding to it
	char h[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
    if(n == 0) return "0";
    string hex = "";
    while(n) {
        hex.push_back(h[n%16]);
        n /= 16;
    }
    reverse(hex.begin(), hex.end());
    return hex;
}

long long hexToInt(string hex) {
	// Takes a hex string and converts and returns its decimal value.
    map <char, long long> hextable =
		{{'0', 0}, {'1', 1}, {'2', 2}, {'3', 3}, {'4', 4},
		{'5', 5}, {'6', 6}, {'7', 7}, {'8', 8}, {'9', 9},
		{'A', 10}, {'B', 11},{'C', 12}, {'D', 13}, {'E', 14},
		{'F', 15}};

    long long len = hex.length() - 1;
    long long n = 0;
    long long c = 0;
    while(len>=0){
        n += hextable[hex[len]] * ((long long)pow(16, c++));
        len--;
    }
    return n;
}

// checks if the names of the input file are clashing with each other
void filename_collision(){
    if(input_filename == "intermediate.txt" || output_filename == "intermediate.txt")
    {
        cout<<"Invalid filenames\n";
        cout<<"Specify a filename other than \"intermediate.txt\"";
        cout<<"EXITING" << endl;
        exit(0);
    }
	if (input_filename == output_filename){
		cout<<"Invalid filenames\n";
        cout<<"Please specify different filenames for input and output";
        cout<<"EXITING" << endl;
        exit(0);
	}
}

// This function takes care splitting when there are spaces in the operand field of BYTE
// Ex: C'E OF'
void word_space_handler(vector<string> &word_list){
    long long sz = word_list.size();
    bool specialCASE = false;
    long long idx;
    for(long long i = 0; i < sz; i++){
        if(word_list[i].length() > Two){
            if(word_list[i].substr(0, Two) == "C'" && word_list[i].back() != '\''){
                    specialCASE = true;
                    idx = i;
            }
        }
    }
    if(specialCASE){
        vector<string> new_word_list;

        for(long long i = 0; i < idx; i++){
            new_word_list.push_back(word_list[i]);
        }

        string new_word = "";
        long long last;
        for(long long i = idx; i < sz; i++){
            last = i;
            new_word.append(word_list[i]);
            if(word_list[i].back() == '\'')
                break;
            else{
                new_word.push_back(' ');
            }
        }
        new_word_list.push_back(new_word);

        for(long long i = last + 1; i < sz; i++){
            new_word_list.push_back(word_list[i]);
        }

        word_list = new_word_list;
    }
    return;
}

// organizes sliptted input into its respective fields
void organize_fields(vector<string> inp_fields){
	if (Pass2){
		lineAddress = (inp_fields.size() >= Two) ? inp_fields[0] : "";
		label = (inp_fields.size() == 4) ? inp_fields[1] : "";
		opcode = inp_fields[inp_fields.size()/Two];
		operand = (inp_fields.size()>Two) ? inp_fields[inp_fields.size()-1] : "";
	}
	else if (Pass1){
		label = (inp_fields.size() == three) ? inp_fields[0] : "";
		opcode = (inp_fields.size() == three) ? inp_fields[1] : inp_fields[0];
		operand = (inp_fields.size() > 1) ? inp_fields[inp_fields.size()-1] : "";
	}
    return;
}
//generates list of words present
//in the line read from input file
void generatewords(vector <string> &inp_fields, string input1){
    inp_fields.clear();
    stringstream ss(input1);
    string word;
    while(ss>>word){
        inp_fields.push_back(word);
    }
    word_space_handler(inp_fields);
	organize_fields(inp_fields); // organizes inp_fields into its respective fields
}

void set_fields(string const &input1){
	// intermediate is true if the line is read from intermediate file and false if read from input.asm
	// Split the string based on whitespace and store it in lineAddress label opcode and operand
	// Genarating wordlist
    vector<string> inp_fields;
    generatewords(inp_fields, input1);
}
string padding(string x, long long len , char y, bool right ){
	// Padding a string to length len with char y on the right if right = true
	string res = "";
    if((long long)x.length() == len)
        res = x;
    else if((long long)x.length() > len)
        res = x.substr(0, len);
    else{
        res = x;
        if(right == 0){
            reverse(res.begin(), res.end());
            while((long long)res.length() < len){
                res.push_back(y);
            }
            reverse(res.begin(), res.end());
        }
        else{
            while((long long)res.length() < len){
                res.push_back(y);
            }
        }
    }
    return res;
}


void Load_OPTAB(map <string,string> &optab){
	// Read the optab file to optable
	cout << left << width4 << "Loading OPTAB ... ";
	ifstream optabfile("optab.txt");
	if(!optabfile){
		cout << "Failed";
		cout << endl << "No optab.txt file found" << endl;
		cout << "Please rename the optable file to optab.txt" << endl;
		cout << "EXITING" << endl;
		exit(0);
	}
	string optab_buffer;
	while (getline(optabfile, optab_buffer))
	{
		set_fields(optab_buffer);
		// As set_fields() function works differently in both cases
		if (pass1) optab[opcode] = operand;
		if (pass2) optab[lineAddress] = opcode;
	}
	optabfile.close();
	cout << "completed" << endl;
}

void Load_SYMTAB(map <string,string> &symtab){
	// Read the optab file to optable
	cout << left << width4 << "Loading SYMTAB ... ";
	ifstream symtabfile("symtab.txt");
	string symtab_buffer;
	while (getline(symtabfile, symtab_buffer))
	{
		set_fields(symtab_buffer);
		symtab[lineAddress] = opcode;
	}
	symtabfile.close();
	cout << "completed" << endl;
}

// loads generated symbol values into a file.
void Generate_symtable(map <string, string> &symtab){
	ofstream symtabfile;
	cout << left << width4 << "Generating Symbol table ... ";
	symtabfile.open("symtab.txt");
	for(auto i: symtab){
		symtabfile << i.first << " " << i.second << endl;
	}
	symtabfile.close();
	cout << "completed" << endl;
	return;
}

// PASS - 1
void pass1(){
	cout<< endl << "Initiating PASS-1 ..." << endl;
	Pass1 = true;

	ifstream input;
	ofstream output;
	input.open(input_filename);
	if(!input){
		cout << "input file not present, please provide the input file or rename the file as \"input.txt\"." << endl;
		cout << "EXITING" << endl;
		exit(0);
	}
	output.open("intermediate.txt");
	long long locctr = 0; // locate counter

	map <string, string> optab,symtab;
	Load_OPTAB(optab); // Read optable from file

	getline(input,inp); 	// Get the first line from input file
	set_fields(inp); 		// Splits the line into label opcode operand
	if(opcode == start){
		locctr = hexToInt(operand);
		start_addr = operand;
		progName = label;
		output << left<< width1 << addspace << intToHex(locctr); 	// setting width as 6 and writing the current address
		output << width2 << addspace << label; 					// writing label with width 10
		output << width2 << addspace << opcode; 					// writing opcode
		output << width3 << addspace << operand << endl; 			// writing operand at the end
	}
	else cout<<"START statement not found"<<endl;

    while(getline(input,inp)){
        // Loop over every line in the file
		if (inp.size()>0){
			set_fields(inp);
			// cout << opcode << " " << operand << endl; // used for debugging
			if(opcode[0] == '.'){
				output << ".  .  .  ."  << "\tComment" << endl;
			}
			else{
				output << left<< width1 << addspace << intToHex(locctr); 	// setting width as 6 and writing the current address
				output << width2 << addspace << label; 					// writing label with width 10
				output << width2 << addspace << opcode; 					// writing opcode
				output << width3 << addspace << operand << endl; 			// writing operand at the end
			}
			if (label != ""){
				if(!symtab.count(label)) symtab[label] = intToHex(locctr);
				else{
					cout<<"Duplicate label is present"<<endl;
				}
			}
			// Increasing LOCCTR w.r.t opcode
			if(optab.count(opcode) || opcode == word ) locctr+=three;
			else if(opcode == "." || label == "." || operand == ".") continue;
			else if(opcode ==start) cout << "FAULTY START statement" << endl;
			else if(opcode == end1) break;
			else if (opcode == resw) locctr += ( three*stoi(operand) );
			else if (opcode == resb) locctr += stoi(operand);
			else if (opcode == byte){
				if(operand[0] == 'X')
					locctr+=(operand.length()-three)/Two; // -three to remove X' and ', and /Two as Two hex values occupy one byte
				else if (operand[0] == 'C')
					locctr += operand.length()-three; // -three to remove C' and '
			}
            else cout << "Illegal opcode " << opcode << " " << endl;

		}
	}
	progLength = locctr-hexToInt(start_addr);
    input.close();
	output.close();
	Generate_symtable(symtab); // load symtab values into a txt file.
	Pass1 = false;
	cout << "PASS-1 complete" << endl;
}

// PASS - Two
void pass2(){
	cout<< endl << "Initiating PASS-2 ..." << endl;
	Pass2 = true;

	ifstream input;
	ofstream output;
	input.open("intermediate.txt");
	output.open(output_filename);

	map<string, string > optab,symtab;
	Load_OPTAB(optab);
	Load_SYMTAB(symtab);
	string record_addr, // starting address of a record
			record; // the record itself
	long long record_size=0; // length of the record in bytes
	bool error_encountered = false;
	string caret = "^";
	long long buff_const = 32768;
	cout << left << width4 << "Generating object code ... ";
	while (getline(input, inp)) // Read the intermediate file
	{
		set_fields(inp);
		if(opcode[0] == '.') continue; // if it is a comment line continue
		if(record_addr=="") record_addr=lineAddress;
		if(opcode == start){
			// Output the header record to output file
			output<<"H"<<caret<<padding(label,6,' ',true)<<caret<<padding(start_addr)<<caret<<padding(intToHex(progLength))<<endl;
		}
		else if(opcode == resw || opcode ==resb){
			// Incase of RESW or RESB a new record needs to be started
			if(record_size >= 1 ){
				output << "T" << caret << padding(record_addr) << caret << padding(intToHex(record_size), Two)  << record << endl;
				record = "";
				record_size = 0;
				continue;
			}
			record_addr = "";
		}
		else if (opcode ==byte){
			if(operand[0]=='C'){
				record+=caret;
				for(long long i=Two;i<operand.length()-1;i++){
					record+=intToHex((long long)operand[i]);
					record_size+=1;
				}
			}
			else if(operand[0]=='X'){
				record+= caret + operand.substr(Two,operand.length() - three);
				record_size+=(operand.length()-three)/Two;
			}
		}
		else if(opcode == word){
		// Handle the case of WORD and BYTE instructions
			record+= caret + padding(intToHex(stoi(operand)));
			record_size+=three;
		}
		else if(opcode =="RSUB"){
			record += caret + padding(optab[opcode], Two) + padding("0", 4);
			record_size += three;
		}
		else if (optab.count(opcode) && symtab.count(operand)){
			// Handle various cases of opcode and operand
			record += (caret + padding(optab[opcode], Two) + padding(symtab[operand], 4));
			record_size+=three;
		}
		else if (operand.substr(operand.length() - Two, Two) == ",X"){
			operand = operand.substr(0, operand.length() - Two);
			long long shifted_address = hexToInt(symtab[operand]);
			record += caret + padding(optab[opcode], Two) + padding(intToHex(shifted_address += buff_const), 4);
			record_size += three;
		}
		else if(opcode == end1){
			if (record_size > 0){
				output << "T" << caret << padding(record_addr) << caret << padding(intToHex(record_size), Two)  << record << endl;
				record = "";
				record_size = 0;
			}
			record_addr = "";
			output<< "E" << caret << padding(start_addr);
		}
		else {
			cout<< endl << "Opcode or operand not found "<<"opcode : "<<opcode<<" operand : "<<operand;
			error_encountered = true;
		}
		if (record_size >= 28){
			// If lenght of the record has exceeded 27
			// then start a new record
			output<<"T" + caret + padding(record_addr) + caret + padding(intToHex(record_size),Two) + record <<endl;
			record = "";
			record_addr="";
			record_size=0;
		}
	}
	if (error_encountered) cout << endl;
	cout << "completed" << endl;
	Pass2 = false;
	cout<<"PASS-2 complete"<<endl;
}
