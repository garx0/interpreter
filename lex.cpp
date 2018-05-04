#pragma once

#include <iostream>
#include <vector>
#include <stack>
#include <algorithm>
#include <cassert>
#include <initializer_list>
#include <fstream>

#include "lex.hpp"
#include "globals.hpp"
#include "utilfuncs.hpp"

using namespace std;

Lex::Lex(string name)
{
	if(isDigit(name[0])) {
		throw false;
	}
	if(name == "true") {
		type = Lex::CONST_BOOLEAN;
		value = 1;
		return;
	}
	if(name == "false") {
		type = Lex::CONST_BOOLEAN;
		value = 0;
		return;
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

Lex::Lex(Type a_type, string str): type(a_type)
{
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

bool Lex::operator==(const Lex& lex) const
{
	if(type != lex.type)
		return false;
	switch(type) {
		case Lex::CONST_INT:
		case Lex::CONST_STRING:
		case Lex::CONST_BOOLEAN:
		case Lex::IDENT:
			return value == lex.value;
		default:
			return true;
	}
}
	
bool Lex::operator!=(const Lex& lex) const
{
	return !(*this == lex);
}
	
Lex::Type Lex::getType() const
{
	return type;
}
	
int Lex::getValue() const
{
	return value;
}

ostream& operator<<(ostream& stream, Lex lexem)
{
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
		case Lex::CONST_BOOLEAN:
			stream << (lexem.value ? "true" : "false");
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
	stream << " {" << (int) lexem.type << ", " << lexem.value << "}";
	return stream;
}

void Scanner::prepare()
{
	//когда прочитали очередную лексему и присвоили ее curLex
	buf.clear();
	m_lexIsRead = true;
	state = &Scanner::stateInit;
}

void Scanner::unget()
{
	if(!m_eof) {
		ungetted = 1;
		input.unget();
	}
}

bool Scanner::readLex()
{
	if(!ready()) {
		if(m_eof) {
			return false;
		} else {
			throw false;
		}
	}
	char c;
	m_lexIsRead = false;
	while(true) {
		if( input.get(c) ) {
			if(ungetted) {
				ungetted = false;
			} else if(c == '\n') {
				needIncLineNo = true;
			}
		} else {
			c = '\n';
			m_eof = true;
			// если поток закончился, "скармливаем" автомату
			// '\n', это ничего не изменит
			// это сделано, чтобы автомат мог дочитать некоторые
			// виды лексем до конца
			// но если произошла эта ветвь, автомат при этом не будет
			// делать unget, т.к. символ '\n', данный ему, не из input
		}
		(this->*state) (c);
		if(needIncLineNo) {
			lineNo++;
			needIncLineNo = false;
		}
		if(m_lexIsRead) {
			return true;
		}
		if(m_eof) {
			return false;
		}
	}
}

bool Scanner::lexAnalysis(vector<Lex>& lexemes)
{
	while(true) {
		try {
			readLex();
		}
		catch(const char& c) {
			cout << "lex. error at symbol '" << c << "', line "
				<< lineNo << endl;
			return false;
		}
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
	//~ cout << "***" << __FUNCTION__ << "(" << c << ")" << endl; //DEBUG	
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
		case '"':
			state = &Scanner::stateStr;
			break;
		case '/':
			state = &Scanner::stateSlash;
			break;
		default:
			throw c;
	}
	//~ cout << "***/" << __FUNCTION__ << endl; //DEBUG
}

void Scanner::stateInt(char c)
{
	//~ cout << "***" << __FUNCTION__ << "(" << c << ")" << endl; //DEBUG
	if( isDigit(c) ) {
		buf.push_back(c);
	} else {
		unget();
		int value = stoi(buf);
		curLex = {Lex::CONST_INT, value};
		prepare();
	}
	//~ cout << "***/" << __FUNCTION__ << endl; //DEBUG
}

void Scanner::stateIdent(char c)
{
	//~ cout << "***" << __FUNCTION__ << "(" << c << ")" << endl; //DEBUG
	if( isDigit(c) || isLetter(c) ) {
		buf.push_back(c);
	} else {
		unget();
		curLex = {buf};
		prepare();
	}
	//~ cout << "***/" << __FUNCTION__ << endl; //DEBUG	
}

void Scanner::stateCmpAss(char c)
{
	//~ cout << "***" << __FUNCTION__ << "(" << c << ")" << endl; //DEBUG
	if(c == '=') {
		buf.push_back(c);
	} else {
		if(!m_eof) {
			input.unget();
		}
	}
	curLex = {buf};
	prepare();
	//~ cout << "***/" << __FUNCTION__ << endl; //DEBUG	
}

void Scanner::stateNE(char c)
{
	//~ cout << "***" << __FUNCTION__ << "(" << c << ")" << endl; //DEBUG
	if(c == '=') {
		buf.push_back(c);
		curLex = {Lex::NE};
		prepare();
	} else {
		throw c;
	}
	//~ cout << "***/" << __FUNCTION__ << endl; //DEBUG	
}

void Scanner::stateStr(char c)
{
	//~ cout << "***" << __FUNCTION__ << "(" << c << ")" << endl; //DEBUG
	switch(c) {
		case '"':
			curLex = {Lex::CONST_STRING, buf};
			prepare();
			break;
		case '\n':
			buf.clear();
			throw c;
		case '\\':
			state = &Scanner::stateStrEsc;
			break;
		default:
			buf.push_back(c);
			break;
	}
	//~ cout << "***/" << __FUNCTION__ << endl; //DEBUG			
}

void Scanner::stateStrEsc(char c)
{
	//~ cout << "***" << __FUNCTION__ << "(" << c << ")" << endl; //DEBUG	
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
	//~ cout << "***/" << __FUNCTION__ << endl; //DEBUG
}

void Scanner::stateSlash(char c)
{
	//~ cout << "***" << __FUNCTION__ << "(" << c << ")" << endl; //DEBUG	
	if(c == '*') {
		state = &Scanner::stateComment;
	} else {
		unget();
		curLex = {Lex::DIV};
		prepare();
	}
	//~ cout << "***/" << __FUNCTION__ << endl; //DEBUG	
}

void Scanner::stateComment(char c)
{
	//~ cout << "***" << __FUNCTION__ << "(" << c << ")" << endl; //DEBUG	
	if(m_eof) {
		throw c;
	}
	if(c == '*') {
		state = &Scanner::stateCommentAst;
	}
	//~ cout << "***/" << __FUNCTION__ << endl; //DEBUG	
}

void Scanner::stateCommentAst(char c)
{
	//~ cout << "***" << __FUNCTION__ << "(" << c << ")" << endl; //DEBUG	
	if(m_eof) {
		throw c;
	}
	if(c == '/') {
		state = &Scanner::stateInit;
	} else {
		state = &Scanner::stateComment;
	}
	//~ cout << "***/" << __FUNCTION__ << endl; //DEBUG	
}

void stateErr(char c)
{
	//~ cout << "***" << __FUNCTION__ << "(" << c << ")" << endl; //DEBUG	
	throw c;
	//~ cout << "***/" << __FUNCTION__ << endl; //DEBUG
}

bool Scanner::ready() const
{
	return (state == &Scanner::stateInit && !m_eof);
}
	
bool Scanner::lexIsRead() const
{
	return m_lexIsRead;
}

bool Scanner::eof() const
{
	return m_eof;
}

int Scanner::getLineNo() const
{
	return lineNo;
}