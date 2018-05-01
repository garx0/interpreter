#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <initializer_list>
#include <fstream>

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
	int size() const {
		return contents.size() - 1;
	}
	int put(const T& elem)
	//если такого элемента нет в таблице, он заносится в таблицу;
	//в любом случае, возвращается номер элемента в таблице
	{
		//~ cout << "PUT: " << '"' << elem << '"' << endl; //DEBUG
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
		//~ cout << "FIND: " << '"' << elem << '"' << endl; //DEBUG
		typename vector<T>::const_iterator res = std::find(contents.begin() + 1, contents.end(), elem);
		//cout << "res == " << (res - contents.begin()) << endl; //DEBUG
		//cout << "res == end(): " << (res == contents.end()) << endl; //DEBUG
		int num = res != contents.end() ? res - contents.begin() : 0;
		//cout << "num = " << num << endl; //DEBUG
		return num;
	}
	int push_back(const T& elem) {
		contents.push_back(elem);
		return size();
	}

	const T& operator[](int ind) const {
		return contents[ind];
	}	
	vector<T> getVector() const {return contents;}
};

const Table<string> tw {
	"program",	//1
	"int",		//2
	"real",		//3
	"string",   //4
	"not",		//5
	"and",		//6
	"or",		//7
	"if",		//8
	"else",		//9
	"do",		//10
	"while",	//11
	"read",		//12
	"write",	//13
	"break"		//14
};

const Table<string> td {
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

class Lex {
public:
	enum Type {
		LEX_NULL	= 0,
		
		PROGRAM		= 1,
		INT			= 2,
		REAL		= 3,
		STRING		= 4,
		NOT			= 5,
		AND			= 6,
		OR			= 7,
		IF			= 8,
		ELSE		= 9,
		DO			= 10,
		WHILE		= 11,
		READ		= 12,
		WRITE		= 13,
		BREAK		= 14,
		
		OP_BRACE	= 32 + 1,	//33
		CL_BRACE	= 32 + 2,	//34
		OP_PAREN	= 32 + 3,	//35
		CL_PAREN	= 32 + 4,	//36
		COMMA		= 32 + 5,	//37
		SEMICOLON	= 32 + 6,	//38
		PLUS		= 32 + 7,	//39
		MINUS		= 32 + 8,	//40
		MUL			= 32 + 9,	//41
		DIV			= 32 + 10,	//42
		MOD			= 32 + 11,	//43
		LT			= 32 + 12,	//44
		GT			= 32 + 13,	//45
		LE			= 32 + 14,	//46
		GE			= 32 + 15,	//47
		EQ			= 32 + 16,	//48
		NE			= 32 + 17,	//49
		ASSIGN		= 32 + 18,	//50
		
		IDENT		= 64,
		CONST_INT	= 65,
		CONST_REAL	= 66,
		CONST_STRING= 67,
		END			= 68
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
	Lex(string name);
	//создается лексема типа "служебное слово", "разделитель" или IDENT
	//  в зависимости от содержимого строки
	Lex(Type a_type, string str);
	/* создается лексема типа IDENT или CONST_STRING в зависимости
	 * от типа, указанного в аргументе (в случае другого типа - ошибка)
	 */
	friend ostream& operator<<(ostream& stream, Lex lexem);
	bool operator==(const Lex& lex) const {
		if(type != lex.type)
			return false;
		switch(type) {
			case Lex::CONST_INT:
			case Lex::CONST_STRING:
			case Lex::IDENT:
				return value == lex.value;
			case Lex::CONST_REAL:
				return realValue == lex.realValue;
			default:
				return true;
		}
	}
	
	bool operator!=(const Lex& lex) const {
		return !(*this == lex);
	}
	
	Type getType() const { return type; }
	
	int getValue() const { return value; }
	
	double getRealValue() const { return realValue; }
};

struct Ident {
public:
	string name;
	Lex::Type type = Lex::LEX_NULL;
	int value = 0;
	double realValue = 0.0;
	bool declared = false;
	bool assigned = false;
	
	Ident() = default;
	Ident(string a_name, Lex::Type a_type = Lex::LEX_NULL):
		name(a_name), type(a_type) {}
	bool operator==(const Ident& ident) {
		return name == ident.name;
	}
};

Table<Ident> tid;
Table<string> tstr;

Lex::Lex(string name) {
	//создается лексема типа "служебное слово", "разделитель" или IDENT
	//  в зависимости от содержимого строки
	if(isDigit(name[0])) {
		throw false;
	}
	int num;
	if(isLetter(name[0])) {
		num = tw.find(name);
		if(num > 0) {
			type = (Lex::Type) num;
		} else {
			num = tid.put(name);
			type = Lex::IDENT;
			value = num;
		}
	} else {
		num = td.find(name);
		if(num > 0) {
			type = (Lex::Type) (32 + num);
		} else {
			throw false;
		}
	}
}

Lex::Lex(Type a_type, string str): type(a_type) {
	/* создается лексема типа IDENT или CONST_STRING в зависимости
	 * от типа, указанного в аргументе (в случае другого типа - ошибка)
	 */
	int num;
	switch(a_type) {
		case Lex::IDENT:
			value = tid.put(str);
			break;
		case Lex::CONST_STRING:
			value = tstr.put(str);
			break;
		default:
			throw false;
	}
}

ostream& operator<<(ostream& stream, Lex lexem) {
	Lex::Type t = lexem.type;
	switch(t) {
		case Lex::LEX_NULL:
			break;
		case Lex::END:
			stream << "END";
		case Lex::IDENT:
			stream << tid[lexem.value].name;
			break;
		case Lex::CONST_INT:
			stream << lexem.value;
			break;
		case Lex::CONST_REAL:
			stream << lexem.realValue;
			break;
		case Lex::CONST_STRING:
			stream << tstr[lexem.value];
			break;
		default:
			if(t >= 1 && t <= 14) {
				stream << tw[t];
			} else if(t >= 32 + 1 && t <= 32 + 18) {
				stream << td[t - 32];
			}
			break;
	}
	stream << " {" << (int) lexem.type << ", " << lexem.value <<
		", " << lexem.realValue << "}";
	return stream;
}

class Scanner {
	typedef void (Scanner::*State)(char c);
	istream& input;
	State state = &Scanner::stateInit;
	string buf;
	Lex curLex;
	int lineNo = 1;
	bool ungetted = 0;
	bool needIncLineNo = 0;
	bool m_lexIsRead;
	bool m_eof = false;
	void stateInit(char c);
	// означает, что автомат прочитал очередную лексему,
	//   либо еще не начал читать очередную, а пока двигается по ' ', '\n' и т.д.
	//   т.е. в любом случае означает, что автомат готов к чтению очередной лексемы,
	//   если она есть в потоке ( т.е. если !eof() )
	void stateInt(char c);
	void stateReal(char c);
	void stateIdent(char c);
	void stateCmpAss(char c);
	void stateNE(char c);
	void stateStr(char c);
	void stateStrEsc(char c);
	void stateSlash(char c);
	void stateComment(char c);
	void stateCommentAst(char c);
	void stateErr(char c);
	// означает, что автомат встретил лекс. ошибку и больше не будет
	//   читать лексемы
	void prepare() {
		//когда прочитали очередную лексему и присвоили ее curLex
		buf.clear();
		m_lexIsRead = true;
		state = &Scanner::stateInit;
	}
	void unget() {
		if(!m_eof) {
			ungetted = 1;
			input.unget();
		}
	}
public:
	explicit Scanner(istream& a_input): input(a_input) {}
	bool readLex();
	Lex getCurLex() const { return curLex; }
	bool lexAnalysis(vector<Lex>& lexemes);
	bool ready() const {
		return (state == &Scanner::stateInit && !m_eof);
	}
	bool lexIsRead() const { return m_lexIsRead; }
	bool eof() const { return m_eof; }
	int getLineNo() const { return lineNo; }
};

bool Scanner::readLex()
//возвращаемое значение = прочитал ли лексему
{
	cout << "state == Init: " << (state == &Scanner::stateInit) << endl; //DEBUG
	//cout << "state == Err: " << (state == &Scanner::stateErr) << endl; //DEBUG
	if(!ready()) {
		cout << "***" << __LINE__ << endl; //DEBUG
		if(m_eof) {
			cout << "***" << __LINE__ << endl; //DEBUG
			return false;
		} else {
			cout << "***" << __LINE__ << endl; //DEBUG
			throw false;
		}
	}
	//~ cout << "***" << __LINE__ << endl; //DEBUG
	char c;
	m_lexIsRead = false;
	while(true) {
		//~ cout << endl; //DEBUG
		//~ cout << "***" << __LINE__ << endl; //DEBUG
		if( input.get(c) ) {
			if(ungetted) {
				ungetted = false;
			} else if(c == '\n') {
				needIncLineNo = true;
			}
			cout << "***" << __LINE__ << ", got '" << c << "'" << endl; //DEBUG
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
		//~ cout << "***" << __LINE__ << endl; //DEBUG
		(this->*state) (c);
		//~ cout << "***" << __LINE__ << endl; //DEBUG
		//~ cout << "from readlex: lexIsRead = " << m_lexIsRead << endl; //DEBUG
		if(needIncLineNo) {
			lineNo++;
			needIncLineNo = false;
		}
		if(m_lexIsRead) {
			//~ cout << "***" << __LINE__ << endl; //DEBUG
			return true;
		}
		if(m_eof) {
			//~ cout << "***" << __LINE__ << endl; //DEBUG
			return false;
		}
		//~ cout << "***" << __LINE__ << endl; //DEBUG
	}
}

bool Scanner::lexAnalysis(vector<Lex>& lexemes) {
	//~ cout << "***" << __LINE__ << endl; //DEBUG
	while(1) {
		//cout << endl << endl; //DEBUG
		//~ cout << "***" << __LINE__ << endl; //DEBUG
		try {
			readLex();
		}
		catch(const char& c) {
			cout << "lex. error at symbol '" << c << "', line "
				<< lineNo << endl;
			return false;
		}
		//~ cout << "***" << __LINE__ << endl; //DEBUG
		if( lexIsRead() ) {
			lexemes.push_back( getCurLex() );
		}
		if( eof() ) {
			break;
		}
	}
	return true;
}

void Scanner::stateInit(char c)
{
	cout << "***" << __FUNCTION__ << "(" << c << ")" << endl; //DEBUG	
	if( isLetter(c) ) {
		buf.push_back(c);
		state = &Scanner::stateIdent;
	} else if ( isDigit(c) ) {
		buf.push_back(c);
		state = &Scanner::stateInt;
	} else switch(c) {
		case ' ':
		case '\t':
			break;
		case '\n':
			if(m_eof) {
				curLex = Lex::END;
				prepare();
			}
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
		case '%':
			curLex = string{c};
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
		case '/':
			state = &Scanner::stateSlash;
			break;
		default:
			//~ cout << "***" << __LINE__ << endl; //DEBUG
			throw c;
	}
	cout << "***/" << __FUNCTION__ << endl; //DEBUG
}

void Scanner::stateInt(char c)
//V переименовать состояние и разветвить на два (для инта и рила)
//переход в рил, если увидим точку
{
	cout << "***" << __FUNCTION__ << "(" << c << ")" << endl; //DEBUG
	if( isDigit(c) ) {
		buf.push_back(c);
	} else if(c == '.') {
		buf.push_back(c);
		state = &Scanner::stateReal;
	} else {
		unget();
		int value = stoi(buf);
		curLex = {Lex::CONST_INT, value};
		//~ cout << "***" << __LINE__ << endl; //DEBUG
		prepare();
	}
	cout << "***/" << __FUNCTION__ << endl; //DEBUG
}

void Scanner::stateReal(char c)
{
	cout << "***" << __FUNCTION__ << "(" << c << ")" << endl; //DEBUG
	if( isDigit(c) ) {
		buf.push_back(c);
	} else {
		unget();
		if(buf.back() == '.') {
			buf.clear();
			throw c;
		}
		double value = stod(buf);
		curLex = {Lex::CONST_REAL, 0, value};
		//~ cout << "***" << __LINE__ << endl; //DEBUG
		prepare();
	}
	cout << "***/" << __FUNCTION__ << endl; //DEBUG	
}

void Scanner::stateIdent(char c)
{
	cout << "***" << __FUNCTION__ << "(" << c << ")" << endl; //DEBUG
	if( isDigit(c) || isLetter(c) ) {
		buf.push_back(c);
	} else {
		unget();
		curLex = {buf};
		//~ cout << "***" << __LINE__ << endl; //DEBUG
		prepare();
	}
	cout << "***/" << __FUNCTION__ << endl; //DEBUG	
}

void Scanner::stateCmpAss(char c)
{
	cout << "***" << __FUNCTION__ << "(" << c << ")" << endl; //DEBUG
	if(c == '=') {
		buf.push_back(c);
	} else {
		if(!m_eof) {
			input.unget();
		}
	}
	curLex = {buf};
	//~ cout << "***" << __LINE__ << endl; //DEBUG
	prepare();
	cout << "***/" << __FUNCTION__ << endl; //DEBUG	
}

void Scanner::stateNE(char c)
{
	cout << "***" << __FUNCTION__ << "(" << c << ")" << endl; //DEBUG
	if(c == '=') {
		buf.push_back(c);
		curLex = {Lex::NE};
		//~ cout << "***" << __LINE__ << endl; //DEBUG
		prepare();
	} else {
		throw c;
	}
	cout << "***/" << __FUNCTION__ << endl; //DEBUG	
}

void Scanner::stateStr(char c)
{
	cout << "***" << __FUNCTION__ << "(" << c << ")" << endl; //DEBUG
	switch(c) {
		case '"':
			curLex = {Lex::CONST_STRING, buf};
			//~ cout << "***" << __LINE__ << endl; //DEBUG
			prepare();
			break;
		case '\n':
			buf.clear();
			throw c;
		//V экранирование
		case '\\':
			state = &Scanner::stateStrEsc;
			break;
		default:
			buf.push_back(c);
			break;
	}
	cout << "***/" << __FUNCTION__ << endl; //DEBUG			
}

void Scanner::stateStrEsc(char c)
{
	cout << "***" << __FUNCTION__ << "(" << c << ")" << endl; //DEBUG	
	switch(c) {
		case '\"':
		case '\\':
			buf.push_back(c);
			state = &Scanner::stateStr;
			break;
		case 'n':
			buf.push_back('\n');
			state = &Scanner::stateStr;
			break;
		case 't':
			buf.push_back('\t');
			state = &Scanner::stateStr;
			break;
		case 'b':
			buf.push_back('\b');
			//V проверить так ли работает
			state = &Scanner::stateStr;
			break;
		case '0':
			buf.push_back('\0');
			//V проверить так ли работает
			state = &Scanner::stateStr;
			break;
		default:
			buf.clear();
			throw c;
	}
	cout << "***/" << __FUNCTION__ << endl; //DEBUG
}

void Scanner::stateSlash(char c)
{
	cout << "***" << __FUNCTION__ << "(" << c << ")" << endl; //DEBUG	
	if(c == '*') {
		state = &Scanner::stateComment;
	} else {
		unget();
		curLex = {Lex::DIV};
		prepare();
	}
	cout << "***/" << __FUNCTION__ << endl; //DEBUG	
}

void Scanner::stateComment(char c)
{
	cout << "***" << __FUNCTION__ << "(" << c << ")" << endl; //DEBUG	
	if(m_eof) {
		throw c;
	}
	if(c == '*') {
		state = &Scanner::stateCommentAst;
	}
	cout << "***/" << __FUNCTION__ << endl; //DEBUG	
}

void Scanner::stateCommentAst(char c)
{
	cout << "***" << __FUNCTION__ << "(" << c << ")" << endl; //DEBUG	
	if(m_eof) {
		throw c;
	}
	if(c == '/') {
		state = &Scanner::stateInit;
	} else {
		state = &Scanner::stateComment;
	}
	cout << "***/" << __FUNCTION__ << endl; //DEBUG	
}

void stateErr(char c)
{
	cout << "***" << __FUNCTION__ << "(" << c << ")" << endl; //DEBUG	
	throw c;
	cout << "***/" << __FUNCTION__ << endl; //DEBUG
}

class Parser {
	Scanner scanner;
	Lex curLex = Lex::LEX_NULL;
	Lex::Type curType = Lex::LEX_NULL;
	void ntProgram();
	void ntMulDescr();
	void ntDescr();
	void ntType();
	void ntVar();
	void ntConst();
	void ntMulOper();
	void ntOper();
	void ntComplOper();
	void ntExprOper();
	void ntExpr();
	void ntExpr5();
	void ntExpr4();
	void ntExpr3();
	void ntExpr2();
	void ntExpr1();
	void ntOperand();
	int indentation = -1; //DEBUG
public:
	Parser(istream& stream): scanner(stream) {}
	bool syntaxAnalysis();
	void readLex() {
		scanner.readLex();
		curLex = scanner.getCurLex();
		cout << "\t\t\t\t\tlexRead: " << curLex << endl; //DEBUG
		curType = curLex.getType();
	}
	void assertLex(Lex::Type lexType) {
	/* стандартная конструкция, повторяющаяся много раз в
	 * процедурах, соотв. нетерминалам
	 */
		//~ cout << curType << " == " << lexType << ": " << (curType == lexType) << endl; //DEBUG
		if(curType == lexType) {
			//~ cout << "(+) assertLex of type = " << lexType << endl; //DEBUG
			readLex();
		} else {
			//~ cout << "(-) assertLex" << endl; //DEBUG
			throw curLex;
		}
	}
};

bool Parser::syntaxAnalysis()
{
	try {
		readLex();
		ntProgram();
		return true;
	}
	catch(const Lex& lex) {
		cout << "syntax error: lexem " << lex << ", line "
			<< scanner.getLineNo() << endl;
		return false;
	}
	catch(const char& c) {
		cout << "lexical error: symbol '" << c << "', line "
			<< scanner.getLineNo() << endl;
		return false;
	}
}

void Parser::ntProgram()
{
	{
		indentation++; //DEBUG
		cout << "\t"; //DEBUG
		for(int i = 0; i < indentation; i++) cout << "_"; //DEBUG
		cout << "<" << __FUNCTION__ << ">" << endl; //DEBUG
	}
	
	assertLex(Lex::PROGRAM);
	assertLex(Lex::OP_BRACE);
	ntMulDescr();
	ntMulOper();
	assertLex(Lex::CL_BRACE);
	if(curType != Lex::END)
		throw curLex;
	
	{
		cout << "\t"; //DEBUG
		for(int i = 0; i < indentation; i++) cout << "_"; //DEBUG
		cout << "</" << __FUNCTION__ << ">" << endl; //DEBUG
		indentation--; //DEBUG
	}
}

//любая процедура nt... проверяет на собственную "конструкцию", и
//после ее выполнения curLex равен первой лексеме после этой конструкции

bool isTypename(Lex::Type lexType) {
	return lexType == Lex::INT || lexType == Lex::REAL ||
		lexType == Lex::STRING;
}

bool isSign(Lex::Type lexType) {
	return lexType == Lex::PLUS || lexType == Lex::MINUS;
}

void Parser::ntMulDescr()
{
	{
		indentation++; //DEBUG
		cout << "\t"; //DEBUG
		for(int i = 0; i < indentation; i++) cout << "_"; //DEBUG
		cout << "<" << __FUNCTION__ << ">" << endl; //DEBUG
	}
	
	while(isTypename(curType)) {
		ntDescr();
		assertLex(Lex::SEMICOLON);
	}
	
	{
		cout << "\t"; //DEBUG
		for(int i = 0; i < indentation; i++) cout << "_"; //DEBUG
		cout << "</" << __FUNCTION__ << ">" << endl; //DEBUG
		indentation--; //DEBUG
	}
}

void Parser::ntDescr()
{
	{
		indentation++; //DEBUG
		cout << "\t"; //DEBUG
		for(int i = 0; i < indentation; i++) cout << "_"; //DEBUG
		cout << "<" << __FUNCTION__ << ">" << endl; //DEBUG
	}
	
	ntType();
	ntVar();
	while(curType == Lex::COMMA) {
		readLex();
		ntVar();
	}
	
	{
		cout << "\t"; //DEBUG
		for(int i = 0; i < indentation; i++) cout << "_"; //DEBUG
		cout << "</" << __FUNCTION__ << ">" << endl; //DEBUG
		indentation--; //DEBUG
	}
}

void Parser::ntType()
{
	{
		indentation++; //DEBUG
		cout << "\t"; //DEBUG
		for(int i = 0; i < indentation; i++) cout << "_"; //DEBUG
		cout << "<" << __FUNCTION__ << ">" << endl; //DEBUG
	}
	
	if(isTypename(curType))
		readLex();
	else
		throw curLex;
	
	{
		cout << "\t"; //DEBUG
		for(int i = 0; i < indentation; i++) cout << "_"; //DEBUG
		cout << "</" << __FUNCTION__ << ">" << endl; //DEBUG
		indentation--; //DEBUG
	}
}

void Parser::ntVar() 
//исп. только для объявления переменной
//для остальных целей используется лексема-идентификатор
{
	{
		indentation++; //DEBUG
		cout << "\t"; //DEBUG
		for(int i = 0; i < indentation; i++) cout << "_"; //DEBUG
		cout << "<" << __FUNCTION__ << ">" << endl; //DEBUG
	}
	
	assertLex(Lex::IDENT);
	if(curType == Lex::ASSIGN) {
		readLex();
		ntConst();
	}
	
	{
		cout << "\t"; //DEBUG
		for(int i = 0; i < indentation; i++) cout << "_"; //DEBUG
		cout << "</" << __FUNCTION__ << ">" << endl; //DEBUG
		indentation--; //DEBUG
	}
}

void Parser::ntConst()
{
	{
		indentation++; //DEBUG
		cout << "\t"; //DEBUG
		for(int i = 0; i < indentation; i++) cout << "_"; //DEBUG
		cout << "<" << __FUNCTION__ << ">" << endl; //DEBUG
	}
	
	if(curType == Lex::PLUS || curType == Lex::MINUS) {
		readLex();
		switch(curType) {
			case Lex::CONST_INT:
				readLex();
				break;
			case Lex::CONST_REAL:
				readLex();
				break;
			default:
				throw curLex;
		}
	} else switch(curType) {
		case Lex::CONST_INT:
			readLex();
			break;
		case Lex::CONST_REAL:
			readLex();
			break;
		case Lex::CONST_STRING:
			readLex();
			break;
		default:
			throw curLex;
	}
	
	{
		cout << "\t"; //DEBUG
		for(int i = 0; i < indentation; i++) cout << "_"; //DEBUG
		cout << "</" << __FUNCTION__ << ">" << endl; //DEBUG
		indentation--; //DEBUG
	}
}

void Parser::ntMulOper()
{
	{
		indentation++; //DEBUG
		cout << "\t"; //DEBUG
		for(int i = 0; i < indentation; i++) cout << "_"; //DEBUG
		cout << "<" << __FUNCTION__ << ">" << endl; //DEBUG
	}
	
	while(curType == Lex::IF || curType == Lex::WHILE || 
		curType == Lex::DO || curType == Lex::BREAK || 
		curType == Lex::READ || curType == Lex::WRITE || 
		curType == Lex::OP_BRACE || curType == Lex::IDENT) {
		ntOper();
	}
	
	{
		cout << "\t"; //DEBUG
		for(int i = 0; i < indentation; i++) cout << "_"; //DEBUG
		cout << "</" << __FUNCTION__ << ">" << endl; //DEBUG
		indentation--; //DEBUG
	}
}

void Parser::ntOper() //V do-while и if без else реализовать!
{
	{
		indentation++; //DEBUG
		cout << "\t"; //DEBUG
		for(int i = 0; i < indentation; i++) cout << "_"; //DEBUG
		cout << "<" << __FUNCTION__ << ">" << endl; //DEBUG
	}
	
	switch(curType) {
		case Lex::IF:
			readLex();
			assertLex(Lex::OP_PAREN);
			ntExpr();
			assertLex(Lex::CL_PAREN);	
			ntOper();
			if(curType == Lex::ELSE) {
				readLex();
				ntOper();
			}
			break;
		case Lex::WHILE:
			readLex();
			assertLex(Lex::OP_PAREN);
			ntExpr();
			assertLex(Lex::CL_PAREN);
			ntOper();
			break;
		case Lex::DO:
			readLex();
			ntOper();
			assertLex(Lex::WHILE);
			assertLex(Lex::OP_PAREN);
			ntExpr();
			assertLex(Lex::CL_PAREN);
			assertLex(Lex::SEMICOLON);
			break;
		case Lex::BREAK:
			readLex();
			assertLex(Lex::SEMICOLON);
			break;
		case Lex::READ:
			readLex();
			assertLex(Lex::OP_PAREN);
			assertLex(Lex::IDENT);
			assertLex(Lex::CL_PAREN);
			assertLex(Lex::SEMICOLON);
			break;
		case Lex::WRITE:
			readLex();
			assertLex(Lex::OP_PAREN);
			ntExpr();
			while(curType == Lex::COMMA) {
				readLex();
				ntExpr();
			}
			assertLex(Lex::CL_PAREN);
			assertLex(Lex::SEMICOLON);
			break;
		case Lex::OP_BRACE:
			ntComplOper();
			break;
		case Lex::IDENT:
			ntExprOper();
			break;
		default:
			throw curLex;
	}
	
	{
		cout << "\t"; //DEBUG
		for(int i = 0; i < indentation; i++) cout << "_"; //DEBUG
		cout << "</" << __FUNCTION__ << ">" << endl; //DEBUG
		indentation--; //DEBUG
	}
}

void Parser::ntComplOper()
{
	{
		indentation++; //DEBUG
		cout << "\t"; //DEBUG
		for(int i = 0; i < indentation; i++) cout << "_"; //DEBUG
		cout << "<" << __FUNCTION__ << ">" << endl; //DEBUG
	}
	
	assertLex(Lex::OP_BRACE);
	ntMulOper();
	assertLex(Lex::CL_BRACE);
	
	{
		cout << "\t"; //DEBUG
		for(int i = 0; i < indentation; i++) cout << "_"; //DEBUG
		cout << "</" << __FUNCTION__ << ">" << endl; //DEBUG
		indentation--; //DEBUG
	}
}

void Parser::ntExprOper()
{
	{
		indentation++; //DEBUG
		cout << "\t"; //DEBUG
		for(int i = 0; i < indentation; i++) cout << "_"; //DEBUG
		cout << "<" << __FUNCTION__ << ">" << endl; //DEBUG
	}
	
	ntExpr();
	assertLex(Lex::SEMICOLON);
	
	{
		cout << "\t"; //DEBUG
		for(int i = 0; i < indentation; i++) cout << "_"; //DEBUG
		cout << "</" << __FUNCTION__ << ">" << endl; //DEBUG
		indentation--; //DEBUG
	}
}

void Parser::ntExpr()
{
	{
		indentation++; //DEBUG
		cout << "\t"; //DEBUG
		for(int i = 0; i < indentation; i++) cout << "_"; //DEBUG
		cout << "<" << __FUNCTION__ << ">" << endl; //DEBUG
	}
	
	ntExpr5();
	while(curType == Lex::OR) {
		readLex();
		ntExpr5();
	}
	
	{
		cout << "\t"; //DEBUG
		for(int i = 0; i < indentation; i++) cout << "_"; //DEBUG
		cout << "</" << __FUNCTION__ << ">" << endl; //DEBUG
		indentation--; //DEBUG
	}
}

void Parser::ntExpr5()
{
	{
		indentation++; //DEBUG
		cout << "\t"; //DEBUG
		for(int i = 0; i < indentation; i++) cout << "_"; //DEBUG
		cout << "<" << __FUNCTION__ << ">" << endl; //DEBUG
	}
	
	ntExpr4();
	while(curType == Lex::AND) {
		readLex();
		ntExpr4();
	}
	
	{
		cout << "\t"; //DEBUG
		for(int i = 0; i < indentation; i++) cout << "_"; //DEBUG
		cout << "</" << __FUNCTION__ << ">" << endl; //DEBUG
		indentation--; //DEBUG
	}
}

void Parser::ntExpr4()
{
	{
		indentation++; //DEBUG
		cout << "\t"; //DEBUG
		for(int i = 0; i < indentation; i++) cout << "_"; //DEBUG
		cout << "<" << __FUNCTION__ << ">" << endl; //DEBUG
	}
	
	while(curType == Lex::NOT)
		readLex();
	ntExpr3();
	
	{
		cout << "\t"; //DEBUG
		for(int i = 0; i < indentation; i++) cout << "_"; //DEBUG
		cout << "</" << __FUNCTION__ << ">" << endl; //DEBUG
		indentation--; //DEBUG
	}
}

void Parser::ntExpr3()
{
	{
		indentation++; //DEBUG
		cout << "\t"; //DEBUG
		for(int i = 0; i < indentation; i++) cout << "_"; //DEBUG
		cout << "<" << __FUNCTION__ << ">" << endl; //DEBUG
	}
	
	ntExpr2();
	if(curType == Lex::EQ || curType == Lex::NE ||
		curType == Lex::LT || curType == Lex::GT ||
		curType == Lex::LE || curType == Lex::GE ||
		curType == Lex::ASSIGN)
	{
		readLex();
		ntExpr2();
	}
	
	{
		cout << "\t"; //DEBUG
		for(int i = 0; i < indentation; i++) cout << "_"; //DEBUG
		cout << "</" << __FUNCTION__ << ">" << endl; //DEBUG
		indentation--; //DEBUG
	}
}

void Parser::ntExpr2()
{
	{
		indentation++; //DEBUG
		cout << "\t"; //DEBUG
		for(int i = 0; i < indentation; i++) cout << "_"; //DEBUG
		cout << "<" << __FUNCTION__ << ">" << endl; //DEBUG
	}
	
	ntExpr1();
	while(curType == Lex::PLUS || curType == Lex::MINUS) {
		readLex();
		ntExpr1();
	}
	
	{
		cout << "\t"; //DEBUG
		for(int i = 0; i < indentation; i++) cout << "_"; //DEBUG
		cout << "</" << __FUNCTION__ << ">" << endl; //DEBUG
		indentation--; //DEBUG
	}
}

void Parser::ntExpr1()
{
	{
		indentation++; //DEBUG
		cout << "\t"; //DEBUG
		for(int i = 0; i < indentation; i++) cout << "_"; //DEBUG
		cout << "<" << __FUNCTION__ << ">" << endl; //DEBUG
	}
	
	ntOperand();
	while(curType == Lex::MUL || curType == Lex::DIV ||
		curType == Lex::MOD) {
		readLex();
		ntOperand();
	}
	
	{
		cout << "\t"; //DEBUG
		for(int i = 0; i < indentation; i++) cout << "_"; //DEBUG
		cout << "</" << __FUNCTION__ << ">" << endl; //DEBUG
		indentation--; //DEBUG
	}
}

void Parser::ntOperand()
{
	{
		indentation++; //DEBUG
		cout << "\t"; //DEBUG
		for(int i = 0; i < indentation; i++) cout << "_"; //DEBUG
		cout << "<" << __FUNCTION__ << ">" << endl; //DEBUG
	}
	
	switch(curType) {
		case Lex::IDENT:
			readLex();
			break;
		case Lex::CONST_INT:
		case Lex::CONST_REAL:
		case Lex::CONST_STRING:
			ntConst();
			break;
		case Lex::OP_PAREN:
			readLex();
			ntExpr();
			assertLex(Lex::CL_PAREN);
			break;
	}
	
	{
		cout << "\t"; //DEBUG
		for(int i = 0; i < indentation; i++) cout << "_"; //DEBUG
		cout << "</" << __FUNCTION__ << ">" << endl; //DEBUG
		indentation--; //DEBUG
	}
}

int main(int argc, const char** argv) {
	ifstream filestream;
	bool inputIsFile = false;
	if(argc >= 2) {
		inputIsFile = true;
		filestream.open(argv[1]);
	}
	if(!filestream) throw false;
	istream& input = inputIsFile ? filestream : cin; 
	/*
	Scanner scanner(cin);
	vector<Lex> lexemes;
	bool test;
	test = scanner.lexAnalysis(lexemes);
	if( test ) {
		for(auto& item : lexemes) {
			//cout << item.getType() << "; " << item.getValue() << endl;
			cout << item << endl;
		}
	} else {
		string str;
		while( getline(cin, str) ) {
			str.clear();
		}
	}
	*/
	Parser parser(input);
	if(parser.syntaxAnalysis()) {
		cout << "SUCCESS" << endl;
	} else {
		cout << "***" << __LINE__ << endl; //DEBUG
		string str;
		while( getline(input, str) ) {
			str.clear();
		}
	}
}
