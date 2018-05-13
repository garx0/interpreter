#pragma once

#include <iostream>
#include <vector>
#include <cassert>

#include "table.hpp"

using namespace std;

enum class LexT {
	LEX_NULL		= 0,
					
	PROGRAM			= 1,
	INT				= 2,
	BOOLEAN			= 3,
	STRING			= 4,
	NOT				= 5,
	AND				= 6,
	OR				= 7,
	IF				= 8,
	ELSE			= 9,
	DO				= 10,
	WHILE			= 11,
	READ			= 12,
	WRITE			= 13,
	BREAK			= 14,
					
	OP_BRACE		= 32 + 1,	//33
	CL_BRACE		= 32 + 2,	//34
	OP_PAREN		= 32 + 3,	//35
	CL_PAREN		= 32 + 4,	//36
	COMMA			= 32 + 5,	//37
	SEMICOLON		= 32 + 6,	//38
	PLUS			= 32 + 7,	//39
	MINUS			= 32 + 8,	//40
	MUL				= 32 + 9,	//41
	DIV				= 32 + 10,	//42
	MOD				= 32 + 11,	//43
	LT				= 32 + 12,	//44
	GT				= 32 + 13,	//45
	LE				= 32 + 14,	//46
	GE				= 32 + 15,	//47
	EQ				= 32 + 16,	//48
	NE				= 32 + 17,	//49
	ASSIGN			= 32 + 18,	//50
	
	IDENT			= 64,
	CONST_INT		= 65,
	CONST_BOOLEAN	= 66,
	CONST_STRING	= 67,
	END				= 68
};

bool isLetter(char c);

struct Ident {
public:
	string name;
	LexT type = LexT::LEX_NULL;
	int value = 0;
	double realValue = 0.0;
	bool declared = false;
	bool assigned = false;
	
	Ident() = default;
	Ident(string a_name, LexT a_type = LexT::LEX_NULL):
		name(a_name), type(a_type) {}
	bool operator==(const Ident& ident);
};

extern const Table<string> tw;
extern const Table<string> td;

extern Table<Ident> tid;
extern Table<string> tstr;

class Lex {
private:
	LexT type;
	int value = 0;
public:
	Lex(LexT a_type = LexT::LEX_NULL, int a_value = 0, double a_realValue = 0.0): 
		type(a_type), value(a_value) {}
	Lex(string name);
	// создается лексема типа 
	//   "служебное слово", "разделитель", CONST_BOOLEAN или IDENT
	//   в зависимости от содержимого строки
	Lex(LexT a_type, string str);
	// создается лексема типа IDENT или CONST_STRING в зависимости
	//   от типа, указанного в аргументе (в случае другого типа - ошибка)
	friend ostream& operator<<(ostream& stream, Lex lexem);
	bool operator==(const Lex& lex) const;
	bool operator!=(const Lex& lex) const;
	LexT getType() const;	
	int getValue() const;
};

ostream& operator<<(ostream& stream, Lex lexem);
	
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
	void prepare();
	//когда прочитали очередную лексему и присвоили ее curLex
	void unget();
public:
	explicit Scanner(istream& a_input): input(a_input) {}
	bool readLex();
	//возвращаемое значение = прочитал ли лексему
	Lex getCurLex() const { return curLex; }
	bool lexAnalysis(vector<Lex>& lexemes);
	bool ready() const;
	bool lexIsRead() const;
	bool eof() const;
	int getLineNo() const;
};
