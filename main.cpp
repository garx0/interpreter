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

Table<string> tableIdent;
Table<string> tableStr;
	
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
		
	Lex(string name) {
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
				num = tableIdent.put(name);
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
	
	Lex(Type a_type, string str): type(a_type) {
		/* создается лексема типа IDENT или CONST_STRING в зависимости
		 * от типа, указанного в аргументе (в случае другого типа - ошибка)
		 */
		int num;
		switch(a_type) {
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
		Lex::Type t = lexem.type;
		switch(t) {
			case LEX_NULL:
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
			default:
				if(t >= 1 && t <= 14) {
					stream << tw[t];
				} else if(t >= 32 + 1 && t <= 32 + 18) {
					stream << td[t - 32];
				}
		}
		stream << " {" << (int) lexem.type << ", " << lexem.value <<
			", " << lexem.realValue << "}" << endl;
		return stream;
	}
	
	bool operator==(const Lex& lex) const {
		if(type != lex.type)
			return false;
		if(type == Lex::END)
			return true;
		if(type == Lex::CONST_REAL)
			return realValue == lex.realValue;
		else
			return value == lex.value;
	}
	
	bool operator!=(const Lex& lex) const {
		return !(*this == lex);
	}
	
	Type getType() const { return type; }
	
	int getValue() const { return value; }
	
	double getRealValue() const { return realValue; }
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
	bool stateStrEsc(char c);
	bool stateSlash(char c);
	bool stateComment(char c);
	bool stateCommentAst(char c);
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
	void unget() {
		if(!m_eof) {
			input.unget();
		}
	}
public:
	explicit Scanner(istream& a_input): input(a_input) {}
	bool readLex();
	Lex getCurLex() const { return curLex; }
	bool lexAnalysis(vector<Lex>& lexemes);
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

bool Scanner::lexAnalysis(vector<Lex>& lexemes) {
	cout << "***" << __LINE__ << endl; //DEBUG
	while(1) {
		cout << endl << endl; //DEBUG
		cout << "***" << __LINE__ << endl; //DEBUG
		readLex();
		cout << "***" << __LINE__ << endl; //DEBUG
		assert( ready() || err() );
		if( err() ) {
			break;
		}
		if( lexIsRead() ) {
			lexemes.push_back( getCurLex() );
		}
		if( eof() ) {
			break;
		}
	}
	return ( !err() );
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
		case '\t':
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
		unget();
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
		unget();
		if(buf.back() == '.') {
			buf.clear();
			state = &Scanner::stateErr;
			return false;
		}
		double value = stod(buf);
		curLex = {Lex::CONST_REAL, 0, value};
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
		unget();
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
	curLex = {buf};
	cout << "***" << __LINE__ << endl; //DEBUG
	prepare();
	return true;
}

bool Scanner::stateNE(char c)
{
	cout << "***" << __LINE__ << ", " << c << endl; //DEBUG
	if(c == '=') {
		buf.push_back(c);
		curLex = {Lex::NE};
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
		case '\\':
			state = &Scanner::stateStrEsc;
			return true;
		default:
			buf.push_back(c);
			break;
	}
			
}

bool Scanner::stateStrEsc(char c)
{
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
			state = &Scanner::stateErr;
			return false;
	}
	return true;
}

bool Scanner::stateSlash(char c)
{
	if(c == '*') {
		state = &Scanner::stateComment;
	} else {
		unget();
		curLex = {Lex::DIV};
		prepare();
	}
}

bool Scanner::stateComment(char c)
{
	if(m_eof) {
		state = &Scanner::stateErr;
		return false;
	}
	if(c == '*') {
		state = &Scanner::stateCommentAst;
	}
	return true;
}

bool Scanner::stateCommentAst(char c)
{
	if(m_eof) {
		state = &Scanner::stateErr;
		return false;
	}
	if(c == '/') {
		state = &Scanner::stateInit;
	} else {
		state = &Scanner::stateComment;
	}
	return true;
}

bool Scanner::stateErr(char c) 
{
	return false;
}


//~ class Parser {
	//~ Scanner scanner;
	//~ Lex curLex;
	//~ //bool analyse();
	//~ void ntProgram();
	//~ void ntMulDescr();
	//~ void ntDescr();
	//~ void ntType();
	//~ void ntVar();
	//~ void ntConst();
	//~ void ntInt();
	//~ void ntReal();
	//~ void ntSign();
	//~ void ntString();
	//~ void ntMulOper();
	//~ void ntOper();
	//~ void ntComplOper();
	//~ void ntExprOper();
	//~ void ntExpr();
//~ public:
	//~ void readLex() {
		//~ bool test = scanner.readLex();
		//~ if( scanner.err() )
			//~ throw scanner.getCurLex();
		//~ if( scanner.lexIsRead() )
			//~ curLex = scanner.getCurLex();
		//~ else
			//~ curLex = Lex::END;
	//~ }
	//~ void assertLex(bool statement) {
	//~ /* стандартная конструкция, повторяющаяся много раз в
	 //~ * процедурах, соотв. нетерминалам
	 //~ */
		//~ if(statement)
			//~ readLex();
		//~ else
			//~ throw curLex;
	//~ }
//~ };

//~ void Parser::ntProgram()
//~ {
	//~ assertLex(curLex == Lex(Lex::KEYWORD, twNames::PROGRAM));
	//~ assertLex(curLex == Lex(Lex::DIVISOR, tdNames::OP_BRACE));
	//~ ntMulDescr();
	//~ ntMulOper();
	//~ assertLex(curLex == Lex(Lex::DIVISOR, tdNames::CL_BRACE));
	//~ if(curLex != Lex::END)
		//~ throw curLex;
//~ }

//~ //любая процедура nt... проверяет на собственную "конструкцию", и
//~ //после ее выполнения curLex равен первой лексеме после этой конструкции

//~ bool isTypename(const Lex& lex) {
	//~ if(lex.getType() != Lex::KEYWORD)
		//~ return false;
	//~ switch(lex.getValue()) {
		//~ case twNames::INT: 
		//~ case twNames::REAL:
		//~ case twNames::STRING:
			//~ return true;
		//~ default:
			//~ return false;
	//~ }
//~ }

//~ bool isConst(const Lex& lex) {
	//~ switch(lex.getType()) {
		//~ case Lex::CONST_INT:
		//~ case Lex::CONST_REAL:
		//~ case Lex::CONST_STRING:
			//~ return true;
		//~ default:
			//~ return false;
	//~ }
//~ }

//~ bool isSign(const Lex& lex) {
	//~ if(lex.getType() != Lex::DIVISOR)
		//~ return false;
	//~ switch(lex.getValue()) {
		//~ case tdNames::PLUS:
		//~ case tdNames::MINUS:
			//~ return true;
		//~ default:
			//~ return false;
	//~ }
//~ }

//~ void Parser::ntMulDescr()
//~ {
	//~ while(isTypename(curLex)) {
		//~ ntDescr();
		//~ assertLex(curLex == Lex(Lex::DIVISOR, tdNames::SEMICOLON));
	//~ }
//~ }

//~ void Parser::ntDescr()
//~ {
	//~ ntType();
	//~ ntVar();
	//~ Lex semicolon(Lex::DIVISOR, tdNames::SEMICOLON);
	//~ while(curLex == semicolon) {
		//~ readLex();
		//~ ntVar();
	//~ }
//~ }

//~ void Parser::ntType()
//~ {
	//~ assertLex(isTypename(curLex));
//~ }

//~ void Parser::ntVar() 
//~ //исп. только для объявления переменной
//~ //для остальных целей используется лексема-идентификатор
//~ {
	//~ assertLex(curLex.getType() == Lex::IDENT);
	//~ if(curLex == Lex(Lex::DIVISOR, tdNames::ASSIGN)) {
		//~ readLex();
		//~ ntConst();
	//~ }
//~ }

//~ void Parser::ntConst()
//~ {
	//~ if(isSign(curLex)) {
		//~ readLex();
		//~ switch(curLex.getType()) {
			//~ case Lex::CONST_INT:
				//~ readLex();
				//~ break;
			//~ case Lex::CONST_REAL:
				//~ readLex();
				//~ break;
			//~ default:
				//~ throw curLex;
		//~ }
	//~ } else if(curLex.getType() == Lex::CONST_STRING)
		//~ readLex();
	//~ else
		//~ throw curLex;
		
//~ }

//~ void Parser::ntMulOper()
//~ {
	//~ //потом
//~ }

//~ void Parser::ntOper() //V do-while и if без else реализовать!
//~ {
	//~ switch(curLex) {
		//~ case Lex(Lex::KEYWORD, twNames::IF):
			//~ readLex();
			//~ assertLex(curLex == Lex(Lex::DIVISOR, tdNames::OP_PAREN));
			//~ ntExpr();
			//~ assertLex(curLex == Lex(Lex::DIVISOR, tdNames::CL_PAREN));	
			//~ ntOper();
			//~ assertLex(curLex == Lex(Lex::KEYWORD, twNames::ELSE));
			//~ ntOper();
			//~ break;
		//~ case Lex(Lex::KEYWORD, twNames::WHILE):
			//~ readLex();
			//~ assertLex(curLex == Lex(Lex::DIVISOR, tdNames::OP_PAREN));
			//~ ntExpr();
			//~ assertLex(curLex == Lex(Lex::DIVISOR, tdNames::CL_PAREN));
			//~ ntOper();
			//~ break;
		//~ case Lex(Lex::KEYWORD, twNames::READ):
			//~ readLex();
			//~ assertLex(curLex == Lex(Lex::DIVISOR, tdNames::OP_PAREN));
			//~ assertLex(curLex.getType() == Lex::IDENT));
			//~ assertLex(curLex == Lex(Lex::DIVISOR, tdNames::CL_PAREN));
			//~ assertLex(curLex == Lex(Lex::DIVISOR, tdNames::SEMICOLON));
			//~ break;
		//~ case Lex(Lex::KEYWORD, twNames::WRITE):
			//~ readLex;
			//~ assertLex(curLex == Lex(Lex::DIVISOR, tdNames::OP_PAREN));
			//~ ntExpr();
			//~ while(curLex == Lex(Lex::DIVISOR, tdNames::COMMA)) {
				//~ readLex();
				//~ ntExpr();
			//~ }
			//~ assertLex(curLex == Lex(Lex::DIVISOR, tdNames::CL_PAREN));
			//~ assertLex(curLex == Lex(Lex::DIVISOR, tdNames::SEMICOLON));
			//~ break;
		//~ case Lex(Lex::DIVISOR, tdNames::OP_BRACE):
			//~ ntComplOper();
			//~ break;
		//~ default:
			//~ if(curLex.getType() == Lex::IDENT)
				//~ ntExprOper();
			//~ else
				//~ throw curLex;
			//~ break;
	//~ }
//~ }

//~ void Parser::ntComplOper()
//~ {
	//~ assertLex(curLex == Lex(Lex::DIVISOR, tdNames::OP_BRACE));
	//~ ntMulOper();
	//~ assertLex(curLex == Lex(Lex::DIVISOR, tdNames::CL_BRACE));
//~ }

//~ void Parser::ntExprOper()
//~ {
	//~ ntExpr();
//~ }

//~ void Parser::ntExpr()
//~ {
	
//~ }

int main() {
	Scanner scanner(cin);
	cout << "***" << __LINE__ << endl; //DEBUG
	vector<Lex> lexemes;
	bool test;
	test = scanner.lexAnalysis(lexemes);
	if( test ) {	
		assert( scanner.ready() ||
			scanner.err() );
		for(auto& item : lexemes) {
			//cout << item.getType() << "; " << item.getValue() << endl;
			cout << item << endl;
		}
	} else {
		string str;
		while( getline(cin, str) ) {
			str.clear();
		}
		cout << "lex. error" << endl;
	}
}
