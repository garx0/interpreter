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
	//элементы нумеруются с первого (интерфейс)
	vector<T> contents;
public:
	Table() = default;
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
		auto res = std::find(contents.begin() + 1, contents.end(), elem);
		if(res == contents.end() ) {
			contents.push_back(elem);
		}
		return res - contents.begin();
	}
	int find(const T& elem) const
	//возвращает номер элемента в таблице;
	//если не найден, то 0
	{
		auto res = std::find(contents.begin() + 1, contents.end(), elem);
		return res != contents.end() ? res - contents.begin() : 0;
	}
	vector<T> getVector() const {return contents;}
};
	
Table<string> tw {
	"program",
	"int",
	"string",
	"real",
	"not",
	"and",
	"or",
	"if",
	"else",
	"do",
	"while",
	"break"
};

Table<string> td {
	"{",
	"}",
	"(",
	")",
	",",
	";",
	"+",
	"-",
	"*",
	"/",
	"%",
	"<",
	">",
	"<=",
	">=",
	"==",
	"!=",
	"="
};

Table<string> tableIdent;
Table<string> tableStr;
	
class Lex {
public:
	enum Type {
		LEX_NULL = 0,
		KEYWORD,
		DIVISOR,
		IDENT,
		CONST_INT,
		CONST_REAL,
		CONST_STRING
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
				num = tw.find(str);
				if(num > 0) {
					value = num;
				} else {
					throw false;
				}
				break;
			case DIVISOR:
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
	Type getType() { return type; }
	int getValue() { return value; }
};

class Scanner {
	typedef bool (Scanner::*State)(char c);
	istream& input;
	State state = nullptr;
	bool lexIsRead = true;
	//vector<Lex> lexemes;
	string buf;
	Lex curLex;
	bool stateInit(char c);
	bool stateNum(char c);
	bool stateIdent(char c);
	bool stateFin(char c);
public:
	explicit Scanner(istream& a_input): input(a_input) {}
	//bool check(const char* text); //V? string
	bool readLex();
	Lex getCurLex() const { return curLex; }
};

/*
bool Scanner::check(const char* text)
{
	assert(text != nullptr);
	state = &Scanner::stateInit;
	for( ; *text != '\0'; text++) {
		if( !(this->*state) (*text) ) {
			return false;
		}
	}
	return state == &Scanner::stateFin;
}
*/

bool Scanner::readLex()
{
	char c;
	lexIsRead = false;
	bool ok = input.good();
	bool test;
	while(ok) {
		input.get(c);
		test = (this->*state) (c);
		if(!test) {
			lexIsRead = false;
			return false;
		}
		ok = !lexIsRead;
	}
}


bool Scanner::stateInit(char c)
{
	if( isLetter(c) ) {
		state = &Scanner::stateIdent;
	} else if ( isDigit(c) ) {
		buf.push_back(c);
		state = &Scanner::stateNum;
	} else switch(c) {
		case ' ': 
			break;
		case '+': 
			curLex = { Lex::DIVISOR, {c} };
			lexIsRead = true;
			break;
		case '-':
			curLex = { Lex::DIVISOR, {c} };
			lexIsRead = true;
			break;
		case '*':
			curLex = { Lex::DIVISOR, {c} };
			lexIsRead = true;
			break;
		case '/':
			curLex = { Lex::DIVISOR, {c} };
			lexIsRead = true;
			break;
		case '(':
			curLex = { Lex::DIVISOR, {c} };
			lexIsRead = true;
			break;
		case ')':
			curLex = { Lex::DIVISOR, {c} };
			lexIsRead = true;
			break;
		default:
			return false;
	}
	return true;
}

bool Scanner::stateNum(char c)
{
	if( isDigit(c) ) {
		buf.push_back(c);
	} else {
		input.unget();
		//cout << "ungetted\n"; //DEBUG
		int value = stoi(buf);
		curLex = {Lex::CONST_INT, value};
		lexIsRead = true;
		buf.clear();
		state = &Scanner::stateInit;
	}
}

bool Scanner::stateIdent(char c)
{
	if( isDigit(c) || isLetter(c) ) {
		buf.push_back(c);
	} else {
		input.unget();
		//cout << "ungetted\n"; //DEBUG
		curLex = {buf};
		lexIsRead = true;
		buf.clear();
		state = &Scanner::stateInit;
	}
}

int main() {
	Scanner scanner(cin);
	for(int i = 0; i < 10; i++) {
		scanner.readLex();
		int type = scanner.getCurLex().getType();
		int value = scanner.getCurLex().getValue();
		cout << type << "," << value << endl;
	}
}

