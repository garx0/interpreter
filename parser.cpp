#include <iostream>
#include <vector>
#include <stack>
#include <algorithm>
#include <cassert>
#include <initializer_list>
#include <fstream>

#include "lex.hpp"
#include "parser.hpp"
#include "table.hpp"
#include "utilfuncs.hpp"

#define ntIn { \
	indentation++; \
	cout << "\t"; \
	for(int i = 0; i < indentation; i++) cout << "_"; \
	cout << "<" << __FUNCTION__ << ">" << endl; \
}

#define ntOut { \
	cout << "\t"; \
	for(int i = 0; i < indentation; i++) cout << "_"; \
	cout << "</" << __FUNCTION__ << ">" << endl; \
	indentation--; \
}

using namespace std;

void Parser::checkOp()
{
	//cout << "********" << __FUNCTION__ << endl; //DEBUG
	LexT::Type opn, type1, type2;
	type2 = stType.top();
	//~ cout << "popped " << type2 << endl; //DEBUG
	stType.pop();
	opn = stType.top();
	//~ cout << "popped " << opn << endl; //DEBUG
	stType.pop();
	type1 = stType.top();
	//~ cout << "popped " << type1 << endl; //DEBUG
	stType.pop();
	switch(opn) {
		case LexT::PLUS:
			if( opdTypesEq(type1, type2)
				&& !opdTypesEq(type1, LexT::BOOLEAN) )
				stType.push( opdTypesEq(type1, LexT::INT) ? 
					LexT::CONST_INT : LexT::CONST_STRING );
			else
				throw "types mismatch in operation '+'";
			break;
		case LexT::MINUS:
		case LexT::MUL:
		case LexT::DIV:
		case LexT::MOD:
			if( opdTypesEq(type1, type2)
				&& opdTypesEq(type1, LexT::INT) )
				stType.push(LexT::CONST_INT);
			else
				throw "types mismatch in operation '-' / '*' / '/'";
			break;
		case LexT::AND:
		case LexT::OR:
			if( opdTypesEq(type1, type2)
				&& opdTypesEq(type1, LexT::BOOLEAN) )
				stType.push(LexT::CONST_BOOLEAN);
			else
				throw "types mismatch in operation 'and' / 'or'";
			break;
		case LexT::EQ:
		case LexT::NE:
		case LexT::LE:
		case LexT::GE:
		case LexT::LT:
		case LexT::GT:
			if( opdTypesEq(type1, type2)
				&& !opdTypesEq(type1, LexT::BOOLEAN) )
				stType.push(LexT::CONST_BOOLEAN);
			else
				throw "types mismatch in operation of comparing";
			break;
		case LexT::ASSIGN:
			if( (type1 == LexT::INT || type1 == LexT::BOOLEAN ||
				type1 == LexT::STRING) && opdTypesEq(type1, type2) )
				stType.push(type1);
			else
				throw "types mismatch in operation '='";
			break;
		default:
			type1 = LexT::LEX_NULL;
			assert(type1 == LexT::END); //DEBUG
			break;
	}
}

void Parser::checkNot()
{
	LexT::Type type1 = stType.top();
	//~ cout << "popped " << type1 << endl; //DEBUG
	stType.pop();
	if(opdTypesEq(type1, LexT::BOOLEAN))
		stType.push(LexT::CONST_BOOLEAN);
	else
		throw "types mismatch in operation 'not'";
}

void Parser::checkIdent()
{
	if(tid[curVal].declared)
		stType.push(tid[curVal].type);
	else
		throw "variable wasn't declared";
}

void Parser::checkBoolRes()
{
	LexT::Type type = stType.top();
	//~ cout << "popped " << type << endl; //DEBUG
	stType.pop();
	if(!opdTypesEq(type, LexT::BOOLEAN))
		throw "wrong expression type";
}

void Parser::readLex()
{
	scanner.readLex();
	curLex = scanner.getCurLex();
	cout << "\t\t\t\t\tlexRead: " << curLex << endl; //DEBUG
	curType = curLex.getType();
	curVal = curLex.getValue();
}

void Parser::assertLex(LexT::Type lexType)
/* стандартная конструкция, повторяющаяся много раз в
 * процедурах, соотв. нетерминалам
 */
{
	//cout << curType << " == " << lexType << ": " << (curType == lexType) << endl; //DEBUG
	if(curType == lexType) {
		//cout << "(+) assertLex of type = " << lexType << endl; //DEBUG
		readLex();
	} else {
		//cout << "(-) assertLex" << endl; //DEBUG
		throw curLex;
	}
}
	
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
	ntIn; //DEBUG
	assertLex(LexT::PROGRAM);
	assertLex(LexT::OP_BRACE);
	ntMulDescr();
	ntMulOper();
	assertLex(LexT::CL_BRACE);
	if(curType != LexT::END)
		throw curLex;
	ntOut; //DEBUG
}

//любая процедура nt... проверяет на собственную "конструкцию", и
//после ее выполнения curLex равен первой лексеме после этой конструкции

void Parser::ntMulDescr()
{
	ntIn; //DEBUG
	while(isTypename(curType)) {
		ntDescr();
		assertLex(LexT::SEMICOLON);
	}
	ntOut; //DEBUG
}

void Parser::ntDescr()
{
	ntIn; //DEBUG
	LexT::Type identType = curType;
	ntType();
	tid[curVal].type = identType;
	if(tid[curVal].declared)
		throw "declared twice";
	tid[curVal].declared = true;
	ntVar();
	while(curType == LexT::COMMA) {
		readLex();
		tid[curVal].type = identType;
		if(tid[curVal].declared)
			throw "declared twice";
		tid[curVal].declared = true;
		ntVar();
	}
	ntOut; //DEBUG
}

void Parser::ntType()
{
	ntIn; //DEBUG
	if(isTypename(curType))
		readLex();
	else
		throw curLex;
	ntOut; //DEBUG
}

void Parser::ntVar() 
//исп. только для объявления переменной
//для остальных целей используется лексема-идентификатор
//процедура возвращает стек таким, каким получила
{
	ntIn; //DEBUG
	int ind = curVal;
	assertLex(LexT::IDENT);
	if(curType == LexT::ASSIGN) {
		readLex();
		ntConst();
		if(stType.top() != tid[ind].type)
			throw "initialization types mismatch";
		stType.pop();
		tid[ind].value = stVal.top();
		stVal.pop();
		tid[ind].assigned = true;
	}
	ntOut; //DEBUG
}

void Parser::ntConst()
//кладет в каждый стек по одному элементу
//(тип и значение константы в соотв. стек)
{
	ntIn; //DEBUG
	int num;
	bool plus = false;
	if( (plus = curType == LexT::PLUS) || curType == LexT::MINUS ) {
		readLex();
		if(curType == LexT::CONST_INT) {
			stVal.push(plus ? curVal : -curVal);
			stType.push(LexT::INT);
			readLex();
		} else {
			throw curLex;
		}
	} else switch(curType) {
		case LexT::CONST_INT:
			stVal.push(curVal);
			stType.push(LexT::INT);
			readLex();
			break;
		case LexT::CONST_BOOLEAN:
			stVal.push(curVal);
			stType.push(LexT::BOOLEAN);
			readLex();
			break;
		case LexT::CONST_STRING:
			stVal.push(curVal);
			stType.push(LexT::STRING);
			readLex();
			break;
		default:
			throw curLex;
	}
	ntOut; //DEBUG
}

void Parser::ntMulOper()
{
	ntIn; //DEBUG
	while(curType == LexT::IF || curType == LexT::WHILE || 
		curType == LexT::DO || curType == LexT::BREAK || 
		curType == LexT::READ || curType == LexT::WRITE || 
		curType == LexT::OP_BRACE || curType == LexT::IDENT) {
		ntOper();
	}
	ntOut; //DEBUG
}

void Parser::ntOper()
{
	ntIn; //DEBUG
	switch(curType) {
		case LexT::IF:
			readLex();
			assertLex(LexT::OP_PAREN);
			ntExpr();
			checkBoolRes();
			assertLex(LexT::CL_PAREN);	
			ntOper();
			if(curType == LexT::ELSE) {
				readLex();
				ntOper();
			}
			break;
		case LexT::WHILE:
			readLex();
			assertLex(LexT::OP_PAREN);
			ntExpr();
			checkBoolRes();
			assertLex(LexT::CL_PAREN);
			ntOper();
			break;
		case LexT::DO:
			readLex();
			ntOper();
			assertLex(LexT::WHILE);
			assertLex(LexT::OP_PAREN);
			ntExpr();
			checkBoolRes();
			assertLex(LexT::CL_PAREN);
			assertLex(LexT::SEMICOLON);
			break;
		case LexT::BREAK:
			readLex();
			assertLex(LexT::SEMICOLON);
			break;
		case LexT::READ:
			readLex();
			assertLex(LexT::OP_PAREN);
			if(!tid[curVal].declared)
				throw "variable wasn't declared";
			if(tid[curVal].type == LexT::BOOLEAN)
				throw "trying to read to boolean variable";
			assertLex(LexT::IDENT);
			assertLex(LexT::CL_PAREN);
			assertLex(LexT::SEMICOLON);
			break;
		case LexT::WRITE:
			readLex();
			assertLex(LexT::OP_PAREN);
			ntExpr();
			while(curType == LexT::COMMA) {
				readLex();
				ntExpr();
			}
			assertLex(LexT::CL_PAREN);
			assertLex(LexT::SEMICOLON);
			break;
		case LexT::OP_BRACE:
			ntComplOper();
			break;
		case LexT::IDENT:
			ntExprOper();
			break;
		default:
			throw curLex;
	}
	ntOut; //DEBUG
}

void Parser::ntComplOper()
{
	ntIn; //DEBUG
	assertLex(LexT::OP_BRACE);
	ntMulOper();
	assertLex(LexT::CL_BRACE);
	ntOut; //DEBUG
}

void Parser::ntExprOper()
{
	ntIn; //DEBUG
	ntExpr();
	assertLex(LexT::SEMICOLON);
	ntOut; //DEBUG
}

void Parser::ntExpr()
{
	ntIn; //DEBUG
	ntExpr5();
	while(curType == LexT::ASSIGN) {
		stType.push(curType);
		readLex();
		ntExpr5();
		checkOp();
	}
	ntOut; //DEBUG
}

void Parser::ntExpr5()
{
	ntIn; //DEBUG
	ntExpr4();
	while(curType == LexT::OR) {
		stType.push(curType);
		readLex();
		ntExpr4();
		checkOp();
	}
	ntOut; //DEBUG
}

void Parser::ntExpr4()
{
	ntIn; //DEBUG
	ntExpr3();
	while(curType == LexT::AND) {
		stType.push(curType);
		readLex();
		ntExpr3();
		checkOp();
	}
	ntOut; //DEBUG
}

void Parser::ntExpr3()
{
	ntIn; //DEBUG
	ntExpr2();
	if(curType == LexT::EQ || curType == LexT::NE ||
		curType == LexT::LT || curType == LexT::GT ||
		curType == LexT::LE || curType == LexT::GE)
	{
		stType.push(curType);
		readLex();
		ntExpr2();
		checkOp();
	}
	ntOut; //DEBUG
}

void Parser::ntExpr2()
{
	ntIn; //DEBUG
	ntExpr1();
	while(curType == LexT::PLUS || curType == LexT::MINUS) {
		stType.push(curType);
		readLex();
		ntExpr1();
		checkOp();
	}
	ntOut; //DEBUG
}

void Parser::ntExpr1()
{
	ntIn; //DEBUG
	ntOperand();
	while(curType == LexT::MUL || curType == LexT::DIV ||
		curType == LexT::MOD) {
		stType.push(curType);
		readLex();
		ntOperand();
		checkOp();
	}
	ntOut; //DEBUG
}

void Parser::ntOperand()
//заносит в stType тип операнда
{
	ntIn; //DEBUG
	switch(curType) {
		case LexT::IDENT:
			checkIdent();
			readLex();
			break;
		case LexT::PLUS:
		case LexT::MINUS:
		case LexT::CONST_INT:
		case LexT::CONST_BOOLEAN:
		case LexT::CONST_STRING:
			stType.push(curType);
			ntConst();
			// ntConst выбросило в каждый стек по значению.
			//   они нам сейчас не нужны
			stType.pop();
			stVal.pop();
			
			break;
		case LexT::NOT:
			readLex();
			ntOperand();
			checkNot();
			break;
		case LexT::OP_PAREN:
			readLex();
			ntExpr();
			assertLex(LexT::CL_PAREN);
			break;
	}
	ntOut; //DEBUG
}
