#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <initializer_list>

using namespace std;

bool isLetter(char c)
{
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool isDigit(char c)
{
	return c >= '0' && c <= '9';
}

template<class T>
class Table {
	//элементы нумеруются с 1 в интерфейсе
	//в векторе нулевой элемент - "пустой"
	//при инициализации таблицы этот пустой элемент указывать не нужно,
	//	начинать сразу с первого.
	vector<T> contents;
public:
	Table(): contents{ T() } {}
	Table(initializer_list<T> list)
	{
		contents.push_back( T() );
        for(auto& item : list) {
			contents.push_back(item);
		}
	}
	int put(const T& elem)
	//если такого элемента нет в таблице, он заносится в таблицу;
	//в любом случае, возвращается номер элемента в таблице
	{
		cout << "PUT: " << elem << endl; //DEBUG
		typename vector<T>::const_iterator res = 
			std::find(contents.begin() + 1, contents.end(), elem);
		int num = res - contents.begin();
		//cout << "res == end(): " << (res == contents.end()) << endl; //DEBUG
		//cout << "num = " << num << endl; //DEBUG
		if(res == contents.end() ) {
			contents.push_back(elem);
		}
		return num;
	}
	int find(const T& elem) const
	//возвращает номер элемента в таблице;
	//если не найден, то 0
	{
		cout << "FIND: " << elem << endl; //DEBUG
		typename vector<T>::const_iterator res = std::find(contents.begin() + 1, contents.end(), elem);
		//cout << "res == " << (res - contents.begin()) << endl; //DEBUG
		//cout << "res == end(): " << (res == contents.end()) << endl; //DEBUG
		int num = res != contents.end() ? res - contents.begin() : 0;
		//cout << "num = " << num << endl; //DEBUG
		return num;
	}
	const T& operator[](int ind) const {
		return contents[ind];
	}	
	vector<T> getVector() const {return contents;}
};
	
Table<string> tw {
	"program",	//1
	"int",		//2
	"string",	//3
	"real",		//4
	"not",		//5
	"and",		//6
	"or",		//7
	"if",		//8
	"else",		//9
	"do",		//10
	"while",	//11
	"break"		//12
};

Table<string> td {
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

Table<string> tableIdent;
Table<string> tableStr;
	
class Lex {
public:
	enum Type {
		LEX_NULL = 0,
		
		KEYWORD,		//1
		DIVISOR,		//2
		IDENT,			//3
		CONST_INT,		//4
		CONST_REAL,		//5
		CONST_STRING	//6
	};
private:
	Type type;
	int value;
public:
	Lex(Type a_type = LEX_NULL, int a_value = 0): 
		type(a_type), value(a_value) {}
	Lex(string name) {
		//создается лексема типа KEYWORD или IDENT
		//  в зависимости от содержимого строки
		int num = tw.find(name);
		if(num > 0) {
			type = Lex::KEYWORD;
			value = num;
		} else {
			num = tableIdent.put(name);
			type = Lex::IDENT;
			value = num;
		}
	}
	Lex(Type a_type, string str): type(a_type) {
		int num;
		switch(a_type) {
			case KEYWORD:
				cout << "***" << __LINE__ << endl; //DEBUG
				num = tw.find(str);
				if(num > 0) {
					value = num;
				} else {
					throw false;
				}
				break;
			case DIVISOR:
				cout << "***" << __LINE__ << endl; //DEBUG
				num = td.find(str);
				if(num > 0) {
					value = num;
				} else {
					throw false;
				}
				break;
			case IDENT:
				value = tableIdent.put(str);
				break;
			case CONST_STRING:
				value = tableStr.put(str);
				break;
			default:
				throw false;
		}
	}
	friend ostream& operator<<(ostream& stream, Lex lexem) {
		switch(lexem.type) {
			case LEX_NULL:
				break;
			case KEYWORD:
				stream << tw[lexem.value];
				break;
			case DIVISOR:
				stream << td[lexem.value];
				break;
			case IDENT:
				stream << tableIdent[lexem.value];
				break;
			case CONST_INT:
				stream << lexem.value;
				break;
			case CONST_REAL:
				//позже
				break;
			case CONST_STRING:
				//позже
				break;
		}
		return stream;
	}
	Type getType() { return type; }
	int getValue() { return value; }
};

class Scanner {
	typedef bool (Scanner::*State)(char c);
	istream& input;
	State state = &Scanner::stateInit;
	bool lexIsRead = true;
	//vector<Lex> lexemes;
	string buf;
	Lex curLex;
	bool stateInit(char c);
	bool stateNum(char c);
	bool stateIdent(char c);
	bool stateCmpAss(char c);
	bool stateNE(char c);
	bool stateFin(char c);
public:
	explicit Scanner(istream& a_input): input(a_input) {}
	bool readLex();
	Lex getCurLex() const { return curLex; }
	bool lexIsRead_() const { return lexIsRead; } //DEBUG
};

bool Scanner::readLex()
{
	cout << "***" << __LINE__ << endl; //DEBUG
	if(!lexIsRead) {
		cout << "***" << __LINE__ << endl; //DEBUG
		return false;
	}
	char c;
	lexIsRead = false;
	bool ok = !lexIsRead;
	bool test;
	while(ok) {
		cout << endl;
		cout << "***" << __LINE__ << endl; //DEBUG
		if(!input.good()) {
			cout << "***" << __LINE__ << endl; //DEBUG
			return false;
		}
		input.get(c);
		cout << "***" << __LINE__ << endl; //DEBUG
		test = (this->*state) (c);
		cout << "***" << __LINE__ << endl; //DEBUG
		if(!test) {
			cout << "***" << __LINE__ << endl; //DEBUG
			lexIsRead = false;
			return false;
		}
		cout << "***" << __LINE__ << endl; //DEBUG
		ok = !lexIsRead;
	}
}


bool Scanner::stateInit(char c)
{
	cout << "***" << __LINE__ << ", " << c << endl; //DEBUG
	if( isLetter(c) ) {
		buf.push_back(c);
		state = &Scanner::stateIdent;
	} else if ( isDigit(c) ) {
		buf.push_back(c);
		state = &Scanner::stateNum;
	} else switch(c) {
		case ' ': 
			break;
		case '\n':
			break;
		case '{':
		case '}':
		case '(':
		case ')':
		case ',':
		case ';':
		case '+':
		case '-':
		case '*':
		case '/':
		case '%':
			curLex = { Lex::DIVISOR, string{c} };
			lexIsRead = true;
			break;
		case '<':
		case '>':
		case '=':
			buf.push_back(c);
			state = &Scanner::stateCmpAss;
			break;
		case '!':
			buf.push_back(c);
			state = &Scanner::stateNE;
			break;
		default:
			cout << "***" << __LINE__ << endl; //DEBUG
			return false;
	}
	cout << "***" << __LINE__ << endl; //DEBUG
	return true;
}

bool Scanner::stateNum(char c)
//V переименовать состояние и разветвить на два (для инта и рила)
//переход в рил, если увидим точку
{
	cout << "***" << __LINE__ << ", " << c << endl; //DEBUG
	if( isDigit(c) ) {
		buf.push_back(c);
	} else {
		input.unget();
		int value = stoi(buf);
		curLex = {Lex::CONST_INT, value};
		lexIsRead = true;
		buf.clear();
		state = &Scanner::stateInit;
	}
	return true;
}

bool Scanner::stateIdent(char c)
{
	cout << "***" << __LINE__ << ", " << c << endl; //DEBUG
	if( isDigit(c) || isLetter(c) ) {
		buf.push_back(c);
	} else {
		input.unget();
		curLex = {buf};
		lexIsRead = true;
		buf.clear();
		state = &Scanner::stateInit;
	}
	return true;
}

bool Scanner::stateCmpAss(char c)
{
	if(c == '=') {
		buf.push_back(c);
	} else {
		input.unget();
	}
	curLex = {Lex::DIVISOR, buf};
	buf.clear();
	state = &Scanner::stateInit;
	lexIsRead = true;
	return true;
}

bool Scanner::stateNE(char c)
{
	if(c == '=') {
		buf.push_back(c);
		curLex = {Lex::DIVISOR, buf};
		buf.clear();
		state = &Scanner::stateInit;
		lexIsRead = true;
		return true;
	} else {
		buf.clear();
		return false;
	}
}

int main() {
	Scanner scanner(cin);
	cout << "***" << __LINE__ << endl; //DEBUG
	vector<Lex> lexemes;
	while(1) {
		cout << endl << endl;
		cout << "***" << __LINE__ << endl; //DEBUG
		scanner.readLex();
		cout << "***" << __LINE__ << endl; //DEBUG
		if(!scanner.lexIsRead_()) {
			break;
		}
		lexemes.push_back( scanner.getCurLex() );
	}
	if(scanner.lexIsRead_()) {	
		for(auto& item : lexemes) {
			//cout << item.getType() << "; " << item.getValue() << endl;
			cout << item << endl;
		}
	} else {
		cout << "lexical error" << endl;
	}
}

