#pragma once

#include <iostream>
#include <vector>
#include <stack>
#include <algorithm>
#include <cassert>
#include <initializer_list>
#include <fstream>

#include "table.hpp"
#include "lex.hpp"

using namespace std;

extern const Table<string> tw;

extern const Table<string> td {
	"{",	//1
	"}",	//2
	"(",	//3
	")",	//4
	",",	//5
	";",	//6
	"+",	//7
	"-",	//8
	"*",	//9
	"/",	//10
	"%",	//11
	"<",	//12
	">",	//13
	"<=",	//14
	">=",	//15
	"==",	//16
	"!=",	//17
	"="		//18
};

extern Table<Ident> tid;
extern Table<string> tstr;
