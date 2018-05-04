#pragma once

#include <iostream>
#include <vector>
#include <stack>
#include <algorithm>
#include <cassert>
#include <initializer_list>
#include <fstream>

#include "globals.hpp"
#include "lex.hpp"
#include "table.hpp"
#include "utilfuncs.hpp"

using namespace std;

//~ struct opdType {
//~ // представление типа операнда для проверки соответствия типов 
//~ //   при семантическом анализе
	//~ enum DataType {
		//~ DATA_NULL = 0,
		//~ INT,
		//~ BOOLEAN,
		//~ STRING
	//~ }
	//~ enum StoreType {
		//~ STORE_NULL = 0,
		//~ CONST,
		//~ IDENT
	//~ }

	//~ DataType data = DATA_NULL;
	//~ StoreType store = STORE_NULL;
	
	//~ opdType(DataType a_data = DATA_NULL, StoreType a_store = STORE_NULL):
		//~ data(a_data), store(a_store) {}
	//~ opdType(opdType a_type, StoreType a_store):
		//~ data(a_type.data), store(a_store) {} 
	//~ bool operator==(const opdType& type1, const opdType& type2) {
		//~ return type1.data == type2.data;
	//~ }
	//~ bool operator!=(const opdType& type1, const opdType& type2) {
		//~ return type1.data != type2.data;
	//~ }
//~ }

class Parser {
	Scanner scanner;
	Lex curLex = Lex::LEX_NULL;
	Lex::Type curType = Lex::LEX_NULL;
	int curVal = 0;
	stack<Lex::Type> stType;
	stack<int> stVal;
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
	
	// в проверке совместимости типов операндов,
	//   имена INT, BOOLEAN, STRING означают тип, который может
	//   быть первым операндом операции "=";
	//   имена CONST_INT, CONST_BOOLEAN, CONST_STRING означают
	//   тип, который не может быть первым операндом операции "="
	
	void checkOp();
	void checkNot();
	void checkIdent();
	void checkBoolRes();
	int indentation = -1; //DEBUG
public:
	Parser(istream& stream): scanner(stream) {}
	bool syntaxAnalysis();
	void readLex();
	void assertLex(Lex::Type lexType);
	/* стандартная конструкция, повторяющаяся много раз в
	 * процедурах, соотв. нетерминалам
	 */
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
	catch(const char* str) {
		cout << "error: " << str << endl;
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
	return lexType == Lex::INT || lexType == Lex::BOOLEAN ||
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
	
	Lex::Type identType = curType;
	ntType();
	tid[curVal].type = identType;
	if(tid[curVal].declared)
		throw "declared twice";
	tid[curVal].declared = true;
	ntVar();
	while(curType == Lex::COMMA) {
		readLex();
		tid[curVal].type = identType;
		if(tid[curVal].declared)
			throw "declared twice";
		tid[curVal].declared = true;
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
//процедура возвращает стек таким, каким получила
{
	{
		indentation++; //DEBUG
		cout << "\t"; //DEBUG
		for(int i = 0; i < indentation; i++) cout << "_"; //DEBUG
		cout << "<" << __FUNCTION__ << ">" << endl; //DEBUG
	}
	int ind = curVal;
	assertLex(Lex::IDENT);
	if(curType == Lex::ASSIGN) {
		readLex();
		ntConst();
		if(stType.top() != tid[ind].type)
			throw "initialization types mismatch";
		stType.pop();
		tid[ind].value = stVal.top();
		stVal.pop();
		tid[ind].assigned = true;
	}
	
	{
		cout << "\t"; //DEBUG
		for(int i = 0; i < indentation; i++) cout << "_"; //DEBUG
		cout << "</" << __FUNCTION__ << ">" << endl; //DEBUG
		indentation--; //DEBUG
	}
}

void Parser::ntConst()
//кладет в каждый стек по одному элементу
//(тип и значение константы в соотв. стек)
{
	{
		indentation++; //DEBUG
		cout << "\t"; //DEBUG
		for(int i = 0; i < indentation; i++) cout << "_"; //DEBUG
		cout << "<" << __FUNCTION__ << ">" << endl; //DEBUG
	}
	int num;
	bool plus = false;
	if( (plus = curType == Lex::PLUS) || curType == Lex::MINUS ) {
		readLex();
		if(curType == Lex::CONST_INT) {
			stVal.push(plus ? curVal : -curVal);
			stType.push(Lex::INT);
			readLex();
		} else {
			throw curLex;
		}
	} else switch(curType) {
		case Lex::CONST_INT:
			stVal.push(curVal);
			stType.push(Lex::INT);
			readLex();
			break;
		case Lex::CONST_BOOLEAN:
			stVal.push(curVal);
			stType.push(Lex::BOOLEAN);
			readLex();
			break;
		case Lex::CONST_STRING:
			stVal.push(curVal);
			stType.push(Lex::STRING);
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

void Parser::ntOper()
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
			checkBoolRes();
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
			checkBoolRes();
			assertLex(Lex::CL_PAREN);
			ntOper();
			break;
		case Lex::DO:
			readLex();
			ntOper();
			assertLex(Lex::WHILE);
			assertLex(Lex::OP_PAREN);
			ntExpr();
			checkBoolRes();
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
			if(!tid[curVal].declared)
				throw "variable wasn't declared";
			if(tid[curVal].type == Lex::BOOLEAN)
				throw "trying to read to boolean variable";
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
	while(curType == Lex::ASSIGN) {
		stType.push(curType);
		readLex();
		ntExpr5();
		//~ cout << "ASS" << endl; //DEBUG
		checkOp();
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
	while(curType == Lex::OR) {
		stType.push(curType);
		readLex();
		ntExpr4();
		checkOp();
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
	
	ntExpr3();
	while(curType == Lex::AND) {
		stType.push(curType);
		readLex();
		ntExpr3();
		checkOp();
	}
	
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
		curType == Lex::LE || curType == Lex::GE)
	{
		stType.push(curType);
		readLex();
		ntExpr2();
		checkOp();
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
		stType.push(curType);
		readLex();
		ntExpr1();
		checkOp();
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
		stType.push(curType);
		readLex();
		ntOperand();
		checkOp();
	}
	
	{
		cout << "\t"; //DEBUG
		for(int i = 0; i < indentation; i++) cout << "_"; //DEBUG
		cout << "</" << __FUNCTION__ << ">" << endl; //DEBUG
		indentation--; //DEBUG
	}
}

void Parser::ntOperand()
//заносит в stType тип операнда
{
	{
		indentation++; //DEBUG
		cout << "\t"; //DEBUG
		for(int i = 0; i < indentation; i++) cout << "_"; //DEBUG
		cout << "<" << __FUNCTION__ << ">" << endl; //DEBUG
	}
	
	switch(curType) {
		case Lex::IDENT:
			checkIdent();
			readLex();
			break;
		case Lex::PLUS:
		case Lex::MINUS:
		case Lex::CONST_INT:
		case Lex::CONST_BOOLEAN:
		case Lex::CONST_STRING:
			stType.push(curType);
			ntConst();
			// ntConst выбросило в каждый стек по значению.
			//   они нам сейчас не нужны
			stType.pop();
			stVal.pop();
			
			break;
		case Lex::NOT:
			readLex();
			ntOperand();
			checkNot();
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
