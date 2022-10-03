/*
CS348 : Implementation of Programming Languages Lab (6th sem)
ASSIGNMENT-II
Title : 2-Pass Assmbler and linking loader for SIC/XE machine
code : 2 - Pass Assembler for SIC/XE machine
Submitted on 24 February 2022.
Done by : Sai Ashrritth Patnana - 190101061

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
NOTE : C++ doesn't have a fully established filesystem directories, do not delete folders.
*/
#include <bits/stdc++.h>

using namespace std;

#define width1 setw(6)
#define width2 setw(10)
#define width3 setw(20)
#define width4 setw(34)
#define addspace setfill(' ')
#define addzero setfill('0')
#define adddot setfill('.')

// for easy coding :-)
typedef string st;
// typedf long long ll;

// Utility delcarations
st int2Hex(int x);
int hex2Int(st x);
st hex2Bin(st x);
st bin2Hex(st x);
bool is_number(st &s);
st toUpper(st x);
void word_space_handler(vector<st> &word_list);
void organize_fields(vector<st> inp_fields);
void generatewords(vector <st> &word_list, st input);
void set_fields(st const &input);
vector<st> split(st x,char delim =',');
st pad(st x, int len = 6, char y = '0', bool right = false);
void filename_collision();
void Load_OPTAB(map <st, st> &optab);
void Load_SYMTAB(map <st, st> &symtab);
void Generate_symtable(map <st, st> &symtab);
st init_get(st opcode, int n, int i, int x, int p, int b, int e, int len = 8);
void assign_regs(map <st, st> &symtab);
bool vfind(vector<st> v, st check);

// Variables to debug empty spaces and change values based on processes
int zero = 0, Two = 2, three = 3;
st mt = "", delim = "^";

// assembler functions and variables
void pass1();
void pass2();

// Global variables
bool PASS1=false, PASS2=false; // Assembler state
st inp,         // input line
	label,          // label field
    opcode,         // opcode fields
    operand,        // operand field
    record,         // object record
    recordAddress,  // Object record address
    lineAddress,    // Object line address
    startAddress;   // Program start address

st input_filename = "../Inputs/input.txt";
st output_filename = "../Outputs/objcode.txt";
//Hashtables for addition optable and symtable info
map <st, st> LIT, tempLIT;

map <st,int> sectLen;
set <st> instrt_2 = {"COMPR","CLEAR","TIXR","ADDR"};
vector <st> ext_refs, ext_defs;
vector <st> modified_recs;
/*
Extra opcodes in SIC/XE
LDL,LDB,LDT,STL,COMPR(2),CLEAR(2),J,TIX,TIXR(2),STCH,+JSUB
handle type 1 instructions
load register names symtab
base relative addressing
evaluate expressions buffend - buffer *****
*/

int main(int argc, char* argv[]){
    if(argc >= three){
		output_filename = argv[Two];
		cout<< "Object code destination :- "<< output_filename << endl;
        input_filename = argv[1];
        cout<< "Input source : "<< input_filename << endl;
	}
	if(argc == Two){
        cout<<"Filename of output file not specified" << endl;
        cout<<"Default :: output destination => "<< output_filename <<"\n";

		input_filename = argv[1];
        cout<< "Input source : "<< input_filename<< endl;
    }
    else{
        cout<<"Filename of input file and output file not specified" << endl;
        cout<<"Default :: input and output destination, "<<input_filename<<" and " << output_filename << endl;
    }
    cout << endl;
	filename_collision();

    pass1();
    pass2();
    return 0;
}

// ---------------------------------------
// ----------- FILE HANDLERS -------------
// ---------------------------------------


// checks if the names of the input file are clashing with each other
void filename_collision(){
    if(input_filename == "./Data/intermediate.txt" || output_filename == "./Data/intermediate.txt")
    {
        cout<<"Invalid filenames\n";
        cout<<"Specify a filename other than \"./Data/intermediate.txt\"";
        cout<<"EXITING" << endl;
        exit(0);
    }
	if (input_filename == output_filename){
		cout<<"Invalid filenames\n";
        cout<<"Please specify different filenames for input and output";
        cout<<"EXITING" << endl;
        exit(0);
	}
    // Not a perfect file collision system. Does the basics
}

void Load_OPTAB(map <st, st> &optab){
	// Read the optab file to optable
	cout << left << width4 << adddot << "Loading OPTAB ...";
	ifstream optabfile("../Inputs/optab.txt");
	if(!optabfile){
		cout << "Failed";
		cout << endl << "No optab.txt file found" << endl;
		cout << "Please rename the optable file to optab.txt" << endl;
		cout << "EXITING" << endl;
		exit(0);
	}
	st optab_buffer;
	while (getline(optabfile, optab_buffer))
	{
		set_fields(optab_buffer);
		// As set_fields() function works differently in both cases
		if (PASS1) optab[opcode] = operand;
		if (PASS2) optab[lineAddress] = opcode;
	}
	optabfile.close();
	cout << " completed" << endl;
}

void Load_SYMTAB(map <st,st> &symtab){
	// Read the optab file to optable
	cout << left << width4 << adddot << "Loading SYMTAB ...";
	ifstream symtabfile("./Data/symtab.txt");
	st symtab_buffer;
    if(!symtabfile){
		cout << "Failed";
		cout << endl << "No symtab.txt file found in ./Data" << endl;
		cout << "Please rename the symtable file to symtab.txt" << endl;
		cout << "Failed, EXITING" << endl;
		exit(0);
	}
	while (getline(symtabfile, symtab_buffer))
	{
		set_fields(symtab_buffer);
		symtab[lineAddress] = opcode;
	}
	symtabfile.close();
	cout << " completed" << endl;
}

// loads generated symbol values into a file.
void Generate_symtable(map <st, st> &symtab){
	ofstream symtabfile;
	cout << left << width4 << adddot << "Generating symtab.txt ...";
	symtabfile.open("./Data/symtab.txt");
	for(auto i: symtab){
		symtabfile << i.first << " " << i.second << endl;
	}
	symtabfile.close();
	cout << " completed" << endl;
	return;
}

// --------------------------------------
// ---------- st HANDLERS -----------
// --------------------------------------

// converts to upper letters
st toUpper(st x){
	for (auto &c : x)
		c = toupper(c);
	return x;
}

// This function takes care splitting when there are spaces in the operand field of BYTE
// Ex: C'E OF'
void word_space_handler(vector<st> &word_list){
    long long sz = word_list.size();
    bool specialCASE = false;
    long long idx;
    for(long long i = zero; i < sz; i++){
        if(word_list[i].length() > Two){
            if(word_list[i].substr(zero, Two) == "C'" && word_list[i].back() != '\''){
                    specialCASE = true;
                    idx = i;
            }
        }
    }
    if(specialCASE){
        vector<st> new_word_list;

        for(long long i = zero; i < idx; i++){
            new_word_list.push_back(word_list[i]);
        }

        st new_word = mt;
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
void organize_fields(vector<st> inp_fields){
	if (PASS2){
		lineAddress = (inp_fields.size() >= Two) ? inp_fields[0] : mt;
		label = (inp_fields.size() == 4) ? inp_fields[1] : mt;
		opcode = inp_fields[inp_fields.size()/Two];
		operand = (inp_fields.size()>Two) ? inp_fields[inp_fields.size()-1] : mt;
	}
	else if (PASS1){
		label = (inp_fields.size() == three) ? inp_fields[0] : (inp_fields.size() > 1 && inp_fields[1]== "CSECT") ? inp_fields[0] : mt;
		opcode = (inp_fields.size() == three) ? inp_fields[1] : (inp_fields.size() > 1 && inp_fields[1]== "CSECT") ? inp_fields[1] : inp_fields[0];
		operand = (inp_fields.size() > 1) ? ( (inp_fields[1] == "CSECT") ? mt : inp_fields[inp_fields.size()-1] ) : mt;
	}
    return;
}

//generates list of words present
//in the line read from input file
void generatewords(vector <st> &inp_fields, st input1){
    inp_fields.clear();
    stringstream ss(input1);
    st word;
    while(ss>>word){
        inp_fields.push_back(word);
    }
    word_space_handler(inp_fields);
	organize_fields(inp_fields); // organizes inp_fields into its respective fields
    if (input1[0]== '.'){
        opcode = ".";
        label  = ".";
        operand = "comment line";
    }
}

// intermediate is true if the line is read from intermediate file and false if read from input.asm
// set_fields the st based on whitespace and store it in lineAddress label opcode and operand
// Genarating wordlist
void set_fields(st const &input1){
    vector<st> inp_fields;
    generatewords(inp_fields, input1);
}

// splits a st into words using the given char
vector<st> split(st x, char delim){
	vector<st> vect;
	stringstream ss(x);
	st str;
	while (getline(ss, str, delim))
	{
		vect.push_back(str);
	}
	return vect;
}

// Adds or removes certain charecters to st
// Pad a st to length len with char y on the right if right = true
st pad(st x, int len , char y, bool right ){
	if(len <= x.length()) {
		return x.substr(x.length()-len);
	}
	if (right)
		return x + st(len - x.length(), y);
	return st(len - x.length(), y) + x;
}

// ---------------------------------------------
// --------- BASE CONVERSION HANDLERS ----------
// ---------------------------------------------

// Manage convertions inbetween hex and dec
// st(hex) <-> Int(dec)
st int2Hex(int x){
	stringstream hexS;
	hexS << hex << x;
	st res(hexS.str());
	hexS.clear();
	return toUpper(res);
}
int hex2Int(st x){
	int res;
	stringstream intS;
	intS << x;
	intS >> hex >> res;
	intS.clear();
	return res;
}

// checks whether is a number
bool is_number(st &s){
	return !s.empty() && all_of(s.begin(), s.end(), ::isdigit);
}

// manage convertion in between hex and bin
// st(hex) <-> st(binary)
st hex2Bin(st x){
	stringstream ss;
	ss << hex << x;
	unsigned n;
	ss >> n;
	bitset<8> b(n);
	return b.to_string();
}
st bin2Hex(st x){
	stringstream ss;
	ss << hex << stoll(x, NULL, 2);
	return ss.str();
}

// ---------------------------------------------
// ----------- ASSEMBLER FUNCTIONS -------------
// ---------------------------------------------

void pass1(){
    PASS1=true;
    cout << "Initiating <PASS-1>" << endl;
    ifstream input;
	ofstream output;
	input.open(input_filename);
    if(!input){
        cout << "input file not present, please provide the input file or rename the file as \"input.txt\" and place it in the Inputs Folder." << endl;
		cout << "EXITING" << endl;
		exit(0);
    }
	output.open("./Data/intermediate.txt");

    map <st, st> optab, // stores opcode table info
                    symtab;     // stores symbol values
    Load_OPTAB(optab); // Read optable from file
    bool illegal = true;
    int currentAddress = zero,progLength = zero;
	st progName; //progName is name of the program and startAddress is the starting address of first instruction

    cout << width4 << adddot << "Generating intermediate.txt ";
    getline(input,inp); // Get the first line from input file
	set_fields(inp); // Splits the line into label opcode operand
	if(opcode != "START") cout << endl << "START statement not found";
	else{
        illegal = false;
        currentAddress = hex2Int(operand);
		startAddress = operand;
		progName = label;
		output << left << addspace << width1 << int2Hex(currentAddress);
        output << width2 << addspace << label;
        output << width2 << addspace << opcode;
        output << width3 << addspace << operand << endl;
    }

    // Loop over every line in the file
    while(getline(input,inp)){
		if (inp.size()>0){
			set_fields(inp);

            // Line is a comment
			if(inp[0] == '.' ||opcode[0] == '.'){
                output << inp << endl;
                continue;
            }

            if (opcode=="CSECT"){
				sectLen.insert({progName,currentAddress - hex2Int(startAddress)});
				progName = label;
				currentAddress = 0;
				output << left << width1 << addspace << pad(int2Hex(currentAddress), 4);
                output << width2 << addspace << label;
                output << width2 << addspace << opcode;
                output << width3 << addspace << mt << endl;
				continue;
			}

            if(operand[0]=='='){
				if(tempLIT.count(operand.substr(1))) ;
				else tempLIT[operand.substr(1)] = mt;
			}

			if (opcode == "LTORG" || opcode =="END")
			{
				for (auto elem : tempLIT)
				{
					LIT[elem.first] = int2Hex(currentAddress);

					output << left << width1 << addspace << pad(int2Hex(currentAddress), 4);
                    output << width2 << addspace << mt;
                    output << width2 << addspace << "*";
                    output << width3 << addspace << elem.first << endl;

					if (elem.first[0] == 'C'){
						int len = elem.first.length() - three;
						currentAddress += len;
					}
					else if (elem.first[0] == 'X'){
						int len = elem.first.length() - three;
						currentAddress += len / 2;
					}
				}
				tempLIT.clear();
				if(opcode=="LTORG") continue;
			}
			else if (opcode == "EQU"){
				if (operand == "*"){
					operand = int2Hex(currentAddress);
					symtab[label] =operand;
				}
				else if(is_number(operand)) symtab[label] = operand;
				continue;
			}
			output << left << width1 << addspace << pad(int2Hex(currentAddress),4);
            output << width2 << addspace << label;
            output << width2 << addspace << opcode;
            output << width3 << addspace << operand << endl;

            if (label != mt){
				if(symtab.count(label)){
                    illegal = true;
                    cout<< endl << "Duplicate label";
                }
				else symtab[label] = int2Hex(currentAddress);
			}

			// Check what opcode is and increment the currentAddress accordingly
			if(opcode=="COMPR" || opcode=="CLEAR" || opcode =="TIXR") currentAddress+=2;
			else if (optab.count(opcode) || opcode == "WORD") currentAddress+=three;
			else if(opcode =="START"){
                illegal = true;
                cout << endl << "Illegal START statement";
            }
			else if (opcode == "BYTE"){
				if(operand[0] == 'X') currentAddress+=(operand.length()-three)/2;
				else if (operand[0] == 'C') currentAddress +=operand.length()-three;
			}
			else if (opcode == "RESW") currentAddress +=(three*stoi(operand));
			else if (opcode == "RESB") currentAddress+=stoi(operand);
			else if(opcode[0]=='+'){
				currentAddress+=4;
			}
			else if(opcode == "END" || opcode == "EXTREF" || opcode =="EXTDEF") ;
			else{
                illegal = true;
                cout << endl << "Illegal opcode : " << opcode;
            }
		}
	}
	sectLen.insert({progName,currentAddress - hex2Int(startAddress)});
	input.close();
	output.close();
    if(illegal) cout << endl << "Intermediate file completed" << endl;
    else cout << " completed" << endl;
    if (!symtab.empty()) cout << "Symtab data collected" << endl;
	assign_regs(symtab);
    Generate_symtable(symtab);
	cout << "PASS-1 completed";
    if(!illegal) cout << " successfully";
    cout << endl << endl;
    PASS1 = false;
    return;
}

void pass2(){
    PASS2 = true;
    cout << "Initiating <PASS-2>" << endl;
    ifstream input;
	ofstream output;
	input.open("./Data/intermediate.txt");
	output.open(output_filename);

    map <st, st> optab, // stores opcode table info
                    symtab;     // stores symbol values
    Load_OPTAB(optab);
    Load_SYMTAB(symtab);

    bool firstSect = true;
	int recordLen=zero;
	int progCounter = zero;
    bool illegal = false;
    bool check = true;

    cout << left << width4 << adddot << "Generating object code ...";
    while (getline(input, inp)) // Read the intermediate file
	{
		set_fields(inp);
        if(inp[0] == '.') continue; // Comment line
		if(recordAddress==mt) recordAddress=lineAddress;
		if(opcode == "START"){
			// Output the header record to output file
			output << "H" + delim << left << width1 << addspace << label;
            output << delim << right << width1 << addzero << startAddress;
            output << delim << width1 << addzero << int2Hex(sectLen[label]) <<endl;
		}
		else if(opcode == "RESW" || opcode =="RESB"){
			// Incase of RESW or RESB a new record needs to be started
			if(recordLen>zero){
				output << "T" + delim << right << width1 << addzero << recordAddress;
                output << delim << right << setw(2) << addzero << int2Hex(recordLen);
                output << record << endl;
				record = mt;
				recordLen = zero;
				continue;
			}
			recordAddress = mt;
			if(opcode == "RESB") progCounter += stoi(operand);
			else progCounter += three*stoi(operand);
		}
		else if (opcode == "EXTREF"){
			ext_refs.clear();
			ext_refs = split(operand);
			output << "R";
			for(auto elem:ext_refs){
				output<< delim << left << width1 << addspace << elem;
			}
			output<<endl;
		}
		else if (opcode == "EXTDEF"){
			ext_defs.clear();
			ext_defs = split(operand);
			output << "D";
			for (auto elem : ext_defs){
				output << delim << left << width1 << addspace << elem;
                output << delim << right << width1 << addzero << symtab[elem];
			}
			output << endl;
		}
		else{
			// Handle the case of WORD and BYTE instructions
			if(opcode == "WORD"){
				if(is_number(operand)) record+= delim + pad(int2Hex(stoi(operand)));
				else{
					record += delim + pad("0");
					// put in function
					if (operand.find('-') != std::string::npos){
						vector<st> operands = split(operand,'-');
						modified_recs.push_back(pad(int2Hex(progCounter+1),6) + delim + "06" + delim + "+" + operands[0]);
						modified_recs.push_back(pad(int2Hex(progCounter+1),6) + delim + "06" + delim + "-" + operands[1]);
					}
					else if (operand.find('+') != std::string::npos){
						vector<st> operands = split(operand,'+');
						modified_recs.push_back(pad(int2Hex(progCounter+1),6) + delim + "06" + delim + "+" + operands[0]);
						modified_recs.push_back(pad(int2Hex(progCounter+1),6) + delim + "06" + delim + "+" + operands[1]);
					}
					else{
                        illegal = true;
						cout<< endl << "Unsupported symbol found in operand : " << operand;
					}
				}
				recordLen+=three;
			}
			else if (opcode =="BYTE"){
				if(operand[0]=='C'){
					record+=delim;
					for(int i=2;i<operand.length()-1;i++){
						record+=int2Hex((int)operand[i]);
						recordLen+=1;
					}
				}
				else if(operand[0]=='X'){
					record+= delim + operand.substr(2,operand.length() - three);
					recordLen+=(operand.length()-three)/2;
				}
			}
			else{
				// Handle various cases of opcode and operand
				int type = 3, indirect = 1, immediate = 1, index = 0, base = 0, pc_relative = 1,extended = 0,literal= 0;
				int padding = type;

                if (operand.length() > 2 && operand.substr(operand.length() - 2, 2) == ",X"){
                //indexed addressing
					index = 1;
					operand = operand.substr(0, operand.length() - 2);
				}
				if(opcode[0] == '+'){
					padding = 5;
					pc_relative = zero;
					extended = 1;
					type = 4;
					opcode = opcode.substr(1);
					modified_recs.push_back(pad(int2Hex(progCounter+1),6)+delim+"05"+delim+"+"+operand);
				}

				if(operand[0] == '#'){
					pc_relative = zero;
					indirect = zero;
					operand = operand.substr(1);
				}
				else if(operand[0]=='='){
					operand = operand.substr(1);
					literal = 1;
				}
				else if (operand[0]=='@'){
					immediate = zero;
					operand = operand.substr(1);
				}
				if(instrt_2.count(opcode)){
					type = 2;
					pc_relative = zero;
					padding = type;
				}

				st target;
				if (opcode == "CLEAR" ||opcode == "TIXR"){
					pc_relative = zero;
					type = 2;
					progCounter +=type;
					record += (delim + optab[opcode] + pad(symtab[operand],2,'0',true));
					recordLen+=2;
				}
				else if(opcode == "COMPR"){
					type = 2;
					progCounter+=type;
					record += (delim + optab[opcode] + symtab[operand.substr(0,1)]+symtab[operand.substr(2)]);
					recordLen += 2;
				}
				else if (optab.count(opcode) && (indirect == 0 || index || literal || symtab.count(operand) ||vfind(ext_refs, operand)))
				{
					progCounter += type;
					if (vfind(ext_refs, operand))
						target = pad(mt, padding);
					else if(index){
						target = pad("0", 5); //Change this
					}
					else if (literal){
						target = pad(int2Hex((hex2Int(LIT[operand]) - progCounter)), type);
					}
					else if (symtab.count(operand))
					{
						if (hex2Int(symtab[operand]) > progCounter)
							target = pad(int2Hex((hex2Int(symtab[operand]) - progCounter)), padding);
						else
						{

							int diff = hex2Int(symtab[operand]) - progCounter;
							target = pad(int2Hex(diff), padding);
						}
					}
					else if (indirect == 0)
					{
						if (is_number(operand))
							target = pad(operand, type);
						else
							target = pad(symtab[operand], type);
					}

					// cout << init_get(optab[opcode], indirect, immediate, index, base, pc_relative, extended) << " "+opcode << target << endl;
					record += delim + init_get(optab[opcode], indirect, immediate, index, base, pc_relative, extended) + target;
					recordLen += type;
				}
				else if (operand.length()>2 && operand.substr(operand.length() - 2, 2) == ",X")  //indexed addressing
				{
					index = 1;
					progCounter += type;
					operand = operand.substr(0, operand.length() - 2);
					int shifted_address = 32768+hex2Int(symtab[operand]);
					target = pad("0",5); //Change this
					record += (delim + init_get(optab[opcode], indirect, immediate, index, base, pc_relative, extended)) + target;
					recordLen += type;
				}
				else if(opcode =="RSUB"){
					progCounter += type;
					pc_relative = zero;
					record += delim + init_get(optab[opcode], indirect, immediate, index, base, pc_relative, extended) + pad("0", 3);
					// should change the  given padding correctly
					recordLen += 3;
				}
				else if(opcode == "END"){
					if (recordLen > 0)
					{
						output << "T^" + pad(recordAddress) + delim + pad(int2Hex(recordLen), 2)  + record << endl;
						record = mt;
						recordLen = 0;
					}
					for (auto elem : modified_recs)
					{
						output << "M^" + elem << endl;
					}
					modified_recs.clear();
					recordAddress = mt;
					output << "E";
					if (firstSect)
						output << delim << right << width1 << addzero << startAddress;
						output  << endl << endl;
				}
				else if(opcode =="*"){
					if (operand[0] == 'C')
					{
						record += delim;
						for (int i = 2; i < operand.length() - 1; i++)
						{
							record += int2Hex((int)operand[i]);
							recordLen += 1;

						}
						progCounter += (operand.length() - 3);
					}
					else if (operand[0] == 'X')
					{
						record += delim + operand.substr(2, operand.length() - 3);
						recordLen += (operand.length() - 3) / 2;
						progCounter += (operand.length() - 3) / 2;
					}
				}
				else if(operand=="CSECT"){

					if (recordLen > 0){
						output << "T" << delim + pad(recordAddress);
                        output << delim + pad(int2Hex(recordLen), 2);
                        output << record << endl;
						record = mt;
						recordLen = 0;
					}
					for(auto elem:modified_recs){
						output<<"M"+delim << elem << endl;
					}
					modified_recs.clear();
					recordAddress = mt;
					output << "E" ;
					if(firstSect) output << delim << right << width1 << addzero << startAddress;
					output <<endl<<endl;
					progCounter = zero;
					firstSect = false;
					output << "H" << delim << left << width1 << addspace << opcode << delim;
                    output << right << width1 << addzero << startAddress;
                    output << delim << right << width1 << addzero << int2Hex(sectLen[opcode]) << endl;
				}
				else{
                    illegal = true;
                    cout<< endl << "Opcode or operand not found =>" << " opcode : "<<opcode<<" operand : "<<operand;
                }
            }
			if(recordLen>27){ // If recordLen has exceeded 27 a new record has to be started
				output<<"T" + delim;
                output << right << width1 << addzero << recordAddress;
                output << delim << right << setw(2) << addzero << int2Hex(recordLen);
                output << record << endl;
				record = mt;
				recordAddress=mt;
				recordLen=0;
			}
		}
	}
    if(illegal) cout << endl << "Object code generated" << endl;
    else cout << " completed" << endl;
    PASS2 = false;
	cout << "PASS-2 completed";
    if(!illegal) cout << " successfully" << endl;
	cout << "Object code file location => " << output_filename << endl;
}

st init_get(st opcode, int n , int i, int x, int p, int b, int e, int len){
	st opcodeBin = hex2Bin(opcode);
	opcodeBin[len - 2] = (char)(n + '0');
	opcodeBin[len - 1] = (char)(i + '0');
	stringstream ss;
	ss << x << p << b << e;
	opcodeBin = toUpper(bin2Hex(opcodeBin) + bin2Hex(ss.str()));
	return pad(opcodeBin,3);
}

void assign_regs(map <st, st> &symtab){
	symtab["A"] = "0";
	symtab["X"] = "1";
	symtab["L"] = "2";
	symtab["B"] = "3";
	symtab["S"] = "4";
	symtab["T"] = "5";
	symtab["F"] = "6";
}

bool vfind(vector<st> v, st check){
    if(find(v.begin(), v.end(), check)!= v.end()) return true;
    return false;
}
