#include <iostream>
#include <vector>
#include <stack>
#include <algorithm>
#include <cassert>
#include <initializer_list>
#include <fstream>

#include "table.hpp"
#include "lex.hpp"
#include "parser.hpp"
#include "utilfuncs.hpp"

using namespace std;

int main(int argc, const char** argv) {
	ifstream filestream;
	bool inputIsFile = false;
	if(argc >= 2) {
		inputIsFile = true;
		filestream.open(argv[1]);
		if(!filestream) throw false;
	}
	
	istream& input = inputIsFile ? filestream : cin; 

	// SOMETHING
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
	}
	/*
	vector<Ident> idents = tid.getVector();
	for(auto& item : idents) {
		cout << item.name << ", "
			<< (int) item.type << ", "
			<< (item.declared ? "decl" : "!decl") << ", "
			<< (item.assigned ? "ass" : "!ass") << ", "
			<< item.value << endl;
	}
	*/
}
