#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <initializer_list>

using namespace std;

bool isLetter(char c)
{
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || 
		(c == '_');
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
		cout << "PUT: " << '"' << elem << '"' << endl; //DEBUG
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
		cout << "FIND: " << '"' << elem << '"' << endl; //DEBUG
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
	"break",	//12
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
		CONST_STRING,	//6
	};
private:
	Type type;
	int value = 0;
	double realValue = 0.0;
public:
	Lex(Type a_type = LEX_NULL, int a_value = 0, double a_realValue = 0.0): 
		type(a_type), value(a_value), realValue(a_realValue) {}
	Lex(double a_realValue):
		type(CONST_REAL), realValue(a_realValue) {}
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
				stream << lexem.realValue;
				break;
			case CONST_STRING:
				stream << tableStr[lexem.value];
				break;
		}
		return stream;
	}
	Type getType() { return type; }
	int getValue() { return value; }
	double getRealValue() { return realValue; }
};

class Scanner {
	typedef bool (Scanner::*State)(char c);
	istream& input;
	State state = &Scanner::stateInit;
	string buf;
	Lex curLex;
	bool m_ready = true;
	bool m_lexIsRead;
	bool m_eof = false;
	bool stateInit(char c);
	// означает, что автомат прочитал очередную лексему,
	//   либо еще не начал читать очередную, а пока двигается по ' ', '\n' и т.д.
	//   т.е. в любом случае означает, что автомат готов к чтению очередной лексемы,
	//   если она есть в потоке ( т.е. если !eof() )
	bool stateInt(char c);
	bool stateReal(char c);
	bool stateIdent(char c);
	bool stateCmpAss(char c);
	bool stateNE(char c);
	bool stateStr(char c);
	bool stateErr(char c);
	// означает, что автомат встретил лекс. ошибку и больше не будет
	//   читать лексемы
	void prepare() {
		//когда прочитали очередную лексему и присвоили ее curLex
		buf.clear();
		m_ready = true;
		m_lexIsRead = true;
		state = &Scanner::stateInit;
	}
public:
	explicit Scanner(istream& a_input): input(a_input) {}
	bool readLex();
	Lex getCurLex() const { return curLex; }
	bool ready() const { return (state == &Scanner::stateInit); }
	bool lexIsRead() const { return m_lexIsRead; }
	bool eof() const { return m_eof; }
	bool err() const { return (state == &Scanner::stateErr); }	
};

bool Scanner::readLex()
{
	if(m_eof) {
		return false;
	}
	cout << "***" << __LINE__ << endl; //DEBUG
	if( err() ) {
		m_lexIsRead = false;
		return false;
	}
	assert( lexIsRead() || ready() || err() );
	char c;
	bool test;
	m_lexIsRead = false;
	while(1) {
		cout << endl; //DEBUG
		cout << "***" << __LINE__ << endl; //DEBUG
		if( input.get(c) ) {
			cout << "***" << __LINE__ << endl; //DEBUG
		} else {
			cout << "***" << __LINE__ << endl; //DEBUG
			c = '\n';
			m_eof = true;
			// если поток закончился, "скармливаем" автомату
			// '\n', это ничего не изменит
			// это сделано, чтобы автомат мог дочитать некоторые
			// виды лексем до конца
			// но если произошла эта ветвь, автомат при этом не будет
			// делать unget, т.к. символ '\n', данный ему, не из input
		}
		cout << "***" << __LINE__ << endl; //DEBUG
		test = (this->*state) (c);
		cout << "***" << __LINE__ << endl; //DEBUG
		if( err() ) {
			cout << "***" << __LINE__ << endl; //DEBUG
			return false;
		}
		
		cout << "from readlex: lexIsRead = " << m_lexIsRead << endl; //DEBUG
		if(m_lexIsRead) {
			cout << "***" << __LINE__ << endl; //DEBUG
			return true;
		}
		if(m_eof) {
			assert( ready() );
			cout << "***" << __LINE__ << endl; //DEBUG
			return false;
		}
		cout << "***" << __LINE__ << endl; //DEBUG
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
		state = &Scanner::stateInt;
	} else switch(c) {
		case ' ':
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
			prepare();
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
		case '.':
			buf.push_back(c);
			state = &Scanner::stateReal;
			break;
		case '"':
			state = &Scanner::stateStr;
			break;
		default:
			cout << "***" << __LINE__ << endl; //DEBUG
			state = &Scanner::stateErr;
			return false;
	}
	cout << "***" << __LINE__ << endl; //DEBUG
	return true;
}

bool Scanner::stateInt(char c)
//V переименовать состояние и разветвить на два (для инта и рила)
//переход в рил, если увидим точку
{
	cout << "***" << __LINE__ << ", " << c << endl; //DEBUG
	if( isDigit(c) ) {
		buf.push_back(c);
	} else if(c == '.') {
		buf.push_back(c);
		state = &Scanner::stateReal;
	} else {
		if(!m_eof) {
			input.unget();
		}
		int value = stoi(buf);
		curLex = {Lex::CONST_INT, value};
		cout << "***" << __LINE__ << endl; //DEBUG
		prepare();
	}
	return true;
}

bool Scanner::stateReal(char c)
{
	cout << "***" << __LINE__ << ", " << c << endl; //DEBUG
	if( isDigit(c) ) {
		buf.push_back(c);
	} else {
		if(!m_eof) {
			input.unget();
		}
		if(buf.back() == '.') {
			buf.clear();
			state = &Scanner::stateErr;
			return false;
		}
		double value = stod(buf);
		curLex = {value};
		cout << "***" << __LINE__ << endl; //DEBUG
		prepare();
	}
	return true;
}

bool Scanner::stateIdent(char c)
{
	cout << "***" << __LINE__ << ", " << c << endl; //DEBUG
	if( isDigit(c) || isLetter(c) ) {
		buf.push_back(c);
	} else {
		if(!m_eof) {
			input.unget();
		}
		curLex = {buf};
		cout << "***" << __LINE__ << endl; //DEBUG
		prepare();
	}
	return true;
}

bool Scanner::stateCmpAss(char c)
{
	cout << "***" << __LINE__ << ", " << c << endl; //DEBUG
	if(c == '=') {
		buf.push_back(c);
	} else {
		if(!m_eof) {
			input.unget();
		}
	}
	curLex = {Lex::DIVISOR, buf};
	cout << "***" << __LINE__ << endl; //DEBUG
	prepare();
	return true;
}

bool Scanner::stateNE(char c)
{
	cout << "***" << __LINE__ << ", " << c << endl; //DEBUG
	if(c == '=') {
		buf.push_back(c);
		curLex = {Lex::DIVISOR, buf};
		cout << "***" << __LINE__ << endl; //DEBUG
		prepare();
		return true;
	} else {
		buf.clear();
		state = &Scanner::stateErr;
		return false;
	}
}

bool Scanner::stateStr(char c)
{
	switch(c) {
		case '"':
			curLex = {Lex::CONST_STRING, buf};
			cout << "***" << __LINE__ << endl; //DEBUG
			prepare();
			return true;
		case '\n':
			buf.clear();
			state = &Scanner::stateErr;
			return false;
		//V экранирование
		default:
			buf.push_back(c);
			break;
	}
			
}
bool Scanner::stateErr(char c) 
{
	return false;
}

int main() {
	Scanner scanner(cin);
	cout << "***" << __LINE__ << endl; //DEBUG
	vector<Lex> lexemes;
	while(1) {
		cout << endl << endl; //DEBUG
		cout << "***" << __LINE__ << endl; //DEBUG
		scanner.readLex();
		cout << "***" << __LINE__ << endl; //DEBUG
		assert( scanner.ready() ||
			scanner.err() );
		if( scanner.err() ) {
			break;
		}
		if( scanner.lexIsRead() ) {
			lexemes.push_back( scanner.getCurLex() );
		}
		if( scanner.eof() ) {
			break;
		}
	}
	if( !scanner.err() ) {	
		assert( scanner.ready() ||
			scanner.err() );
		for(auto& item : lexemes) {
			//cout << item.getType() << "; " << item.getValue() << endl;
			cout << item << endl;
		}
	} else {
		cout << "lex. error" << endl;
	}
}
