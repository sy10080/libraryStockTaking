#include <iostream>
#include <string>
#include <stack>
#include <vector>
#include <tuple>
#include <fstream>
#include <algorithm>
#include <time.h>

// iostream
using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::ios_base;

// string
using std::string;

// stack
using std::stack;

// vector
using std::vector;

// utility
using std::pair;

// tuple
using std::tuple;
using std::get;

// fstream
using std::ifstream;
using std::ofstream;

// algorithm
using std::sort;

// front format
string format;
// back format
string format_back;
// track the filename
string tracking = "PLEASE_TYPE_A_FILENAME_BEFORE_YOUR_FIRST_SAVE";
// book code buffer
vector<string> buffer;
// user-defined booklist
vector<pair<string, int>> booklist;
// buffer limiting for protecting your work
int limit = 50;

std::ifstream fin;
std::ofstream fout;

// main input string
string commandInput;
// undo stack
stack<std::tuple<string, int, string>> undoStack; // operation, position, value

string defaultFileLocation = "booklist"; // load from config later

int basicLine = 49; // controlling the output amount of /help
int timeLimit = 2;
int showAllBooksPosition = 2;

bool cmdError = 0;
bool tle = 0;
bool preload = 1;

void systemMessage(){
	fin.open("systemMessage.txt");
	do{
		getline(fin, commandInput);
		cout << commandInput << endl;
	}while (!fin.eof());
	fin.close();
}

void commandSyntaxError(){
	cout << "Syntax Error: Please retype the command again." << endl;
	cmdError = 0;
}

void timeLimitExceedError(){
	cout << "Time Limit Exceeded: Please double check if your parameters are correctly typed." << endl;
	tle = 0;
}

string ordinal(int n){
	if (n % 100 >= 11 && n % 100 <= 13) return "th";
	else if (n % 10 == 1) return "st";
	else if (n % 10 == 2) return "nd";
	else if (n % 10 == 3) return "rd";
	return "th";
}

string toUpper(string str){
	int ss = commandInput.size();
	for (int i = 0; i < ss; i++) if (str[i] >= 'a' && str[i] <= 'z') str[i] -= 32;
	return str;
}

int bs(int left, int right, vector<pair<string, int>> v, string val){
  int mid = (left + right) / 2;
  int result = v[mid].first.compare(val);
  if (left > right) return -1;
  else if (result == 0) return v[mid].second;
  else if (result < 0) return bs(mid + 1, right, v, val);
  else return bs(left, mid - 1, v, val);
}

vector<pair<string, int>> loadBooklist(string file){
  	fin.open(file + ".txt");
  	time_t originalTime;
	time_t currentTime;
	time(&originalTime);
	time(&currentTime);
	vector<pair<string, int>> internalBooklist;
	int it = 1;
	string _;
	getline(fin, _);
	while (!fin.eof()){
		if (currentTime - originalTime > timeLimit){
			tle = 1;
			break;
		}
		//read until the '|' character
			getline(fin, _);
		int p = 0;
		string barcode;
		while (_[p] != '|'){
			barcode += _[p++];
		}
		internalBooklist.push_back(make_pair(barcode, it++));
		cerr << barcode << '\n';
	}
	if (tle){
		timeLimitExceedError();
	}else{
		sort(internalBooklist.begin(), internalBooklist.end());
	}
	fin.close();
	return internalBooklist;
}

void save(){
	if (commandInput.size() > 6){
		string filename = commandInput.substr(6, commandInput.size() - 5);
		tracking = filename;
	}

	do{
		cout << "Confirm saving into \"" << tracking << ".txt\" this file? [Y|N] ";
		getline(cin, commandInput);
	}while (commandInput != "Y" && commandInput != "N");

	if (commandInput == "N"){
		cout << "Save operation stopped." << endl;
		return;
	}


	fout.open(tracking + ".txt", ios_base::app);
	for (int i = 0; i < int(buffer.size()); i++){
		fout << buffer[i] << endl;
	}
    buffer.resize(0);
	fout.flush();
	fout.close();

	cout << "Saved to " << tracking << ".txt." << endl;
}


void undo(){
  if (undoStack.empty()){
    cout << "There is nothing for you to undo." << endl;
    return;
  }
	if (get<0>(undoStack.top()) == "del"){
		buffer.insert(buffer.begin() + get<1>(undoStack.top()), get<2>(undoStack.top()));
		undoStack.pop();

		cout << "Undo last delete operation." << endl;
	}
}

void help(){
	if (commandInput.size() < 7){
		fin.open("help.txt");
		while (!fin.eof()){
			string str;
			getline(fin, str);
			cout << str << endl;
		}
	}else{
		string subs = toUpper(commandInput.substr(6, commandInput.size() - 6));
		if (subs == "BASIC"){
			int line = 1;
			fin.open("help.txt");
			while (line <= basicLine){
				string str;
				getline(fin, str);
				cout << str << endl;
				line++;
			}
		}else if (subs == "ADVANCED"){
			fin.open("help.txt");
			int line = 1;
			while (!fin.eof()){
				if (line <= 12 || line >= basicLine + 2){
					string str;
					getline(fin, str);
					cout << str << endl;
				}
				line++;
			}
		}else{
			fin.open("help.txt");
			while (!fin.eof()){
				string str;
				getline(fin, str);
				cout << str << endl;
			}
		}
	}
	fin.close();
}

void del(){
	if (commandInput.size() < 6){
		//undo
		undoStack.push(make_tuple("del", buffer.size() - 1, buffer[buffer.size() - 1]));

		buffer.pop_back();
		cout << "Deleted last input." << endl;
	}else{
		string sub = commandInput.substr(5, commandInput.size() - 5);
		int subs = sub.size();
		for (int i = 0; i < subs; i++) if(!((sub[i] >= '0' && sub[i] <= '9') || sub[i] == '-')) cmdError = 1;

		if (!cmdError){
			int pos = stoi(sub);
			if (pos < 0){
				//undo
				undoStack.push(make_tuple("del", buffer.size() + pos, buffer[buffer.size() + pos]));

				int ord = buffer.size() + pos + 1; 
				buffer.erase(buffer.end() + pos);

				cout << "Deleted " << ord << ordinal(ord) << " input." << endl;

			}else{
				//undo
				undoStack.push(make_tuple("del", pos, buffer[pos]));

				int ord = pos + 1;
				buffer.erase(buffer.begin() + pos);

				cout << "Deleted " << ord << ordinal(ord) << " input." << endl;
			}
		}
	}
}

void changeLimit(){
	if (commandInput.size() < 8) cmdError = 1;
	else{
		string sub = commandInput.substr(7, commandInput.size() - 7);
		int subs = sub.size();
		for (int i = 0; i < subs; i++) if (!(sub[i] >= '0' && sub[i] <= '9')) cmdError = 1;

		if (!cmdError){
			limit = stoi(sub);
			cout << "Changed limit to " << limit << "." << endl;
		}
	}
}

void clear(){
	cout << "Are you sure to clear the undo stack? Type \"Y\" to confirm. ";
	char c;
	cin >> c;
	if (c == 'Y'){
		int sts = undoStack.size() - 1;
		while (sts-- >= 0){
			undoStack.pop();
		}
		cout << "Undo stack cleared." << endl;
	}else{
		cout << "Clear command failed to execute." << endl;
	}
}

void count(){
    if (commandInput.size() < 8) cmdError = 1;
    else{
        time_t originalTime, currentTime;
        time(&originalTime);
        time(&currentTime);
        string sub = commandInput.substr(7, commandInput.size() - 7);
        fin.open(sub + ".txt");
        int cnt = 0;
        while (!fin.eof()){
            if (currentTime - originalTime > timeLimit){
                tle = 1;
                break;
            }
            string _;
            getline(fin, _);
            if (_ != "") cnt++;
            time(&currentTime);
        }
        if (!tle) cout << cnt << endl;
        else timeLimitExceedError();
        fin.close();
    }
}
  
void config(){
	if (commandInput.size() < 9) cmdError = 1;
	else{
		string subs = commandInput.substr(8, commandInput.size() - 8);
	}
}


void smallCheck(string file, vector<pair<string, int>> v){
	bool ok = 0;
  	fin.open(file + ".txt");
  	time_t originalTime, currentTime;
  	time(&originalTime);
  	time(&currentTime);
  	while (!fin.eof()){
		ok = 1;
		if (currentTime - originalTime > 3 * timeLimit){
			tle = 1;
			break;
		}
		string _;
		getline(fin, _);
		int pos = bs(0, v.size() - 1, v, _);
		if (_ != ""){
			if (pos != -1){
				if (showAllBooksPosition == 2){
					cout << "Found Book Code " << _ << " in booklist position of " << pos << " .\n";
				}else if (showAllBooksPosition == 1){
					cout << "Found Book Code " << _ << " .\n";
				}
			}else{
				cout << "BOOK CODE " << _ << " NOT FOUND IN THE BOOKLIST.\n";
			}
		}
	}
	fin.close();
	if (tle){
		cout << "Failed to check all the books in targeted file.\n";
		timeLimitExceedError();
	}else if (!ok){
		cout << "You have checked a blank file. Please make sure that you have typed the file location correctly.\n";
	}else{
		cout << "Sucessfully check all the books in targeted file.\n";
	}
}
void check(){
	if (booklist.size() == 0){
		vector<pair<string, int>> tempBooklist = loadBooklist(defaultFileLocation);

		if (commandInput.size() < 8){
			//check last saved file
			cout << "Checking file " << tracking << ".txt.\n";
			smallCheck(tracking, tempBooklist);
		}else{
			string sub = commandInput.substr(8, commandInput.size() - 8);
			cout << "Checking file " << sub << ".txt.\n";
			smallCheck(sub, tempBooklist);
		}
	}else{
		if (commandInput.size() < 8){
			cout << "Checking file " << tracking << ".txt.\n";
			smallCheck(tracking, booklist);
		}else{
			string sub = commandInput.substr(7, commandInput.size() - 7);\
			cout << "Checking file " << sub << ".txt.\n";
			smallCheck(sub, booklist);
		}
	}
}

void reload(){
	if (commandInput.size() < 9){
		commandSyntaxError();
	}else{
		booklist.clear();
		string sub = commandInput.substr(8, commandInput.size() - 8);
		booklist = loadBooklist(sub);
		cout << "Reloaded.\n";
	}
}

void commandHandling(){
	string cmd;

	//delete 
	cmd = toUpper(commandInput.substr(1, 3));
	if (cmd == "DEL") del();

	//save
	cmd = toUpper(commandInput.substr(1, 4));
	if (cmd == "SAVE") save();

	//undo
	if (cmd == "UNDO") undo();

  	//redo
  	if (cmd == "REDO") ;

	//help
	if (cmd == "HELP") help();

	//limit
	cmd = toUpper(commandInput.substr(1, 5));
	if (cmd == "LIMIT") changeLimit();

	//clear
	if (cmd == "CLEAR") clear();

	//count
  	if (cmd == "COUNT") count();

	//check
  	if (cmd == "CHECK") check();

	cmd = toUpper(commandInput.substr(1, 6));
	//reload
	if (cmd == "RELOAD") reload();

	//config
	if (cmd == "CONFIG") ;

  

	if (cmdError){
		commandSyntaxError();
	}
}

void formatChange(){
	int pos = commandInput.find('*');
	if (pos != -1){
		format = commandInput.substr(1, pos - 1);
		format_back = commandInput.substr(pos + 1, commandInput.size() - pos - 1);
	}else{
		format = commandInput.substr(1, commandInput.size() - 1);
		format_back = "";
	}
}