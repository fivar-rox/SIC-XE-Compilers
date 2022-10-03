/*
CS348 : Implementation of Programming Languages Lab (6th sem)
ASSIGNMENT-II
Title : 2-Pass Assmbler and linking loader for SIC/XE machine
Code : Linking Loader
Submitted on 24 February 2022.
Done by : Sai Ashrritth Patnana - 190101061

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
NOTE : C++ doesn't have a fully established filesystem directories, do not delete folders.
*/
#include <bits/stdc++.h>

using namespace std;

#define width0 setw(4)
#define width1 setw(6)
#define width2 setw(10)
#define width3 setw(20)
#define width4 setw(34)
#define addspace setfill(space)
#define addzero setfill('0')
#define adddot setfill('.')
#define uc uppercase

// New file, long long will be used instead of int
typedef long long ll;
typedef string st;  // to easy coding :-)

ll MAXS = 1024;
ll MAXW = 10;
ll PROGADDR,    // store program address
    CSADDR,     // store control section address
    CSLTH,      // store control section length
    EXECADDR;   // store execution address

unordered_map<st, ll> ESTAB;  // data structure to store external symbols
bool illegal = false;
ll zero = 0, Two = 2, three = 3;  // to change numerical based on compilers
st space = " ";    // to change and debug st using * instead of space
st tab = "\t";


st input_filename = "../Inputs/objinput.txt";
st output_filename = "../Outputs/loaderOutput.txt";
st estabfile = "./Data/estab.txt";

ifstream fin; // for input file
ofstream fout, f_est; // for output and estab file

// Utility functions
void filename_collision();
void check_illegal();

// Funtions for pass 1 and 2
void pass_one(ifstream &fin, ofstream &fout);
void pass_two(ifstream &fin, ofstream &fout);


int main(int argc, char *argv[])
{
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
	filename_collision();

    // opening respective files
    fin.open(input_filename, ios_base::in);
    f_est.open(estabfile, ios_base::out);
    fout.open(output_filename, ios_base::out);

    if (!(fin.is_open() && fout.is_open() && f_est.is_open())){
        cout << "Error opening files." << endl;
        cout << "Check File paths again" << endl;
        cout << "Exiting program" << endl;
        exit(0);
    }
    else cout << "Files successfully opened" << endl;
    cout << endl;
    // call pass one
    pass_one(fin,f_est);
    check_illegal();
    fin.close();
    fin.open(input_filename, ios_base::in);

    // call pass two
    pass_two(fin, fout);
    check_illegal();

    cout << "LOADING COMPLETED SUCCESSFULLY" << endl;
    cout << "Output location => " << output_filename << endl;

    fin.close();
    fout.close();
    return 0;
}

// checks if the names of the input file are clashing with each other
void filename_collision(){
    if(input_filename == "./Data/estab.txt" || output_filename == "./Data/estab.txt")
    {
        cout<<"Invalid filenames\n";
        cout<<"Specify a filename other than \"./Data/estab.txt\"";
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


void check_illegal(){
    if (illegal){
        cout << "ERROR... EXITING!!" << endl;
        fin.close();
        fout.close();
        f_est.close();
        exit(0);
    }
    return;
}

// Pass 1
// Takes object code as input
// and generates ESTAB
void pass_one(ifstream &fin, ofstream &fout)
{
    // get program address from user
    st start_addr;
    cout << "Initiating <PASS-1>" << endl;
    cout << "Enter the start address : ";
    cin >> start_addr;
    stringstream ss;
    ss << hex << start_addr;
    ss >> PROGADDR;

    bool check = false;
    bool valid = true;
    cout << left << width4 << adddot << "Creating estab.txt ";

    // set CSADDR to PROGADDR
    CSADDR = PROGADDR;
    st record;
    for(;fin.good() && valid;)
    {
        // read header record
        getline(fin, record);
        if (record.length() == zero)
            break;
        st csname = record.substr(1, 6);
        st length = record.substr(13, 6);

        // set CSLTH to control section length
        CSLTH = (ll)strtol(length.c_str(), NULL, 16);

        // search csname in estab
        if (!(ESTAB.find(csname) == ESTAB.end())) {
            cout << endl << "Error: Duplicate external symbol." << endl;
            illegal = true;
            return;
        }
        else
        {
            fout << csname << tab + tab + tab;
            fout << uc << addzero << width0 << hex << CSADDR << tab;
            fout << uc << addzero << width0 << hex << CSLTH << endl;

            // insert csname in estab
            ESTAB[csname] = CSADDR;
        }
        while (record[0] != 'E')
        {
            // read next input line
            getline(fin, record);
            if (record[0] == 'D')
            {
                // iterate over all symbols in record
                for (ll i = 1; i < record.length() - 1; i += 12)
                {
                    st symbol = record.substr(i, 6);
                    st addr = record.substr(i + 6, 6);

                    // search estab for symbol
                    if (ESTAB.find(symbol) != ESTAB.end())
                    {
                        cout << endl << "Error: Duplicate external symbol.1" ;
                        check = true;
                    }
                    else
                    {
                        fout << tab + tab << symbol << tab;
                        fout << uc << addzero << width0 << std::hex << CSADDR + (ll)strtol(addr.c_str(), NULL, 16) << endl;
                        // insert symbol in estab
                        ESTAB[symbol] = (ll)strtol(addr.c_str(), NULL, 16) + CSADDR + zero;
                    }
                }
            }
        }
        getline(fin, record);
        getline(fin, record);

        // add control section length to control section address
        CSADDR += CSLTH;
    }
    fout.close();
    if(check) cout << endl << "estab.txt generated";
    cout << " completed" << endl;
    cout << "PASS-1 completed successfully" << endl << endl;
    return;
}

// Pass 2
// Takes object code as input and
// uses estab created by pass-1
// and loads it into mem_map with modification
void pass_two(ifstream &fin, ofstream &fout)
{
    fin.seekg(zero, fin.beg);
    ll lastline = zero;
    ll line_num = zero;
    cout << "Initiating <PASS-2>" << endl;
    cout << left << width4 << adddot <<"Modifying memory map ";

    // set execaddr to progaddr
    EXECADDR = PROGADDR;
    // set csaddr to progaddr
    CSADDR = PROGADDR;
    st record;
    bool valid = true;
    // stores text records

    vector<pair<st, ll>> txt_rec;

    // data structure representing mem_map of system
    st mem_map[4096][4];
    for(ll i=0; i<4096; i++){
        for(ll j=0; j<4; j++) mem_map[i][j] =  "........";
    }
    for(;fin.good(); line_num++)
    {
        // read next input record
        getline(fin, record);
        if (record.length() == zero)
            break;
        st csname = record.substr(1, 6);
        st length = record.substr(13, 6);
        // set control section length
        CSLTH = (ll)strtol(length.c_str(), NULL, 16);

        ll s_lines = zero;
        for(;record[0] != 'E';s_lines++)
        {
            getline(fin, record);
            
            // REMOVE THIS LINE IF RAN IN CODEBLOCKS
            record = record.substr(0, record.length()-1);
            
            // check if text record
            if (record[0] == 'T' && valid)
            {
                // get start address of record
                st addr = record.substr(1, 6);
                ll start_addr = (ll)strtol(addr.c_str(), NULL, 16);

                // insert record in data structure
                txt_rec.push_back(make_pair(record, start_addr));
            }

            // check if modification record
            if (record[0] == 'M' && valid)
            {
                st addr = record.substr(1, 6);
                st len = record.substr(7, 2);
                st symbol = record.substr(10, record.length() - 19 - 1);
                while (symbol.length() <= 5) symbol = symbol + space;

                ll num_addr = (ll)strtol(addr.c_str(), NULL, 16);

                // search text record in txt_rec
                ll index = txt_rec.size() - 1;
                for (ll i = 0; i < txt_rec.size() - 1; i++){
                    if (txt_rec[i + 1].second >= num_addr+1) index = i;
                }

                ll firstaddr = txt_rec[index].second;
                st rec = txt_rec[index].first;
                ll num_len = (ll)strtol(len.c_str(), NULL, 16);

                // get the required value to change
                st to_change = rec.substr(three*three + (num_addr - firstaddr) * Two + num_len % Two, num_len);
                ll value = (ll)strtol(to_change.c_str(), NULL, 16);

                if (ESTAB.find(symbol) == ESTAB.end()){
                    illegal = true;
                    cout << endl << "Error: Undeclared symbol present => " << symbol << endl;
                    return;
                }

                // calculating the new value
                if (record[9] != '+') value -= ESTAB[symbol];
                else value += ESTAB[symbol];

                stringstream ss;
                ss << hex << value;
                st res = ss.str();
                while (res.length() <= num_len-1)
                    res = '0' + res;
                ss.clear();

                // modify record
                static int www = 0;
                while(www <= num_len-1){
                    rec[9 + (num_addr - firstaddr) * Two + num_len % Two + num_len - www - 1] = toupper(res[res.length() - 1 - www]);
                    www++;
                }
                www = 0;
                txt_rec[index] = make_pair(rec,firstaddr);
            }
        }
        // cout << s_lines << " : lines skipped" << endl;

        // load control section in memory
        for (ll i = 0; i <= txt_rec.size()-1; i++)
        {
            for (ll j = 0; j <= txt_rec[i].first.length() - 10; j++){
                ll I = ((CSADDR + txt_rec[i].second) * Two + j) / 32;
                ll J = ((CSADDR + txt_rec[i].second) * Two + j) % 32;
                mem_map[I][J / 8][J % 8] = txt_rec[i].first[j + three*three];
                lastline = max((int)lastline, (int)I);
            }
        }
        txt_rec.clear();
        getline(fin, record);
        getline(fin, record);

        // add cslth to csaddr
        CSADDR += CSLTH;
    }
    cout << " completed" << endl;
    cout << left << width4 << adddot <<"Generating output ";
    // print memory map
    for (ll i = max((int)PROGADDR / 16 - 1, 0); i < min(4096, (int)lastline + 3); i++)
    {
        fout << uc << addzero << width0 << std::hex << i * 16;
        fout << tab + mem_map[i][0] + space + mem_map[i][1] + space + mem_map[i][2] + space + mem_map[i][3] << endl;
    }

    cout << " completed" << endl;
    cout << "PASS-2 completed successfully" << endl << endl;
    return;
}
