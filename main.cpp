#include <iostream>
#include <vector>
#include <cassert>
#include <fstream>

#include "table.hpp"
#include "lex.hpp"
#include "parser.hpp"
#include "rpn.hpp"

using namespace std;

int main(int argc, const char** argv) {
	ifstream filestream;
	bool inputIsFile = false;
	if(argc >= 2) {
		inputIsFile = true;
		filestream.open(argv[1]);
		if(!filestream) throw "failed to open the file";
	}
	istream& input = inputIsFile ? filestream : cin;
	
	//~ Scanner scanner(cin);
	//~ vector<Lex> lexemes;
	//~ bool test;
	//~ test = scanner.lexAnalysis(lexemes);
	//~ if( test ) {
		//~ for(auto& item : lexemes) {
			//~ //cout << item.getType() << "; " << item.getValue() << endl;
			//~ cout << item << endl;
		//~ }
	//~ } else {
		//~ string str;
		//~ while( getline(cin, str) ) {
			//~ str.clear();
		//~ }
	//~ }
	
	Parser parser(input);
	if(parser.syntaxAnalysis()) {
		cout << "SUCCESS" << endl;
	} else {
		//cout << "***" << __LINE__ << endl; //DEBUG
		string str;
		while( getline(input, str) ) {
			str.clear();
		}
		return 1;
	}
	const vector<Ident>& idents = tid.getVectorRef();
	for(auto& item : idents) {
		cout << item.name << ", "
			<< (int) item.type << ", "
			<< (item.declared ? "decl" : "!decl") << ", "
			<< (item.assigned ? "ass" : "!ass") << ", ";
		if(item.type == LexT::STRING)	
			cout << item.str << endl;
		else
			cout << item.value << endl;
	}
	const vector<RpnOp*>& rpn = parser.getRpnRef();
	for(auto &item : rpn) {
		cout << *item << " ";
	}
	cout << endl; 
	for(auto it = rpn.begin(); it != rpn.end(); it++) {
		cout << "rpn[" << (it - rpn.begin()) << "] = ";
		if(*it) {
			cout << **it;
		} else {
			cout << "NULLPTR";
		}
		cout << endl;
	}
	RpnProgram prog(parser.getRpnRef(), tid, tstr);
	cout << "EXECUTE\n\n";
	prog.execute();
	return 0;
}
