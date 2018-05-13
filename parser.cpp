#include <iostream>
#include <vector>
#include <stack>
#include <cassert>

#include "table.hpp"
#include "lex.hpp"
#include "parser.hpp"
#include "rpn.hpp"


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
	LexT opn, type1, type2;
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
			assert(false); //DEBUG
	}
	switch(opn) {
		case LexT::PLUS: rpn.push_back(new RpnAdd); break;
		case LexT::MINUS: rpn.push_back(new RpnSub); break;
		case LexT::MUL: rpn.push_back(new RpnMul); break;
		case LexT::DIV: rpn.push_back(new RpnDiv); break;
		case LexT::MOD: rpn.push_back(new RpnMod); break;
		case LexT::AND: rpn.push_back(new RpnAnd); break;
		case LexT::OR: rpn.push_back(new RpnOr); break;
		case LexT::EQ: rpn.push_back(new RpnEq); break;
		case LexT::NE: rpn.push_back(new RpnNe); break;
		case LexT::LE: rpn.push_back(new RpnLe); break;
		case LexT::GE: rpn.push_back(new RpnGe); break;
		case LexT::LT: rpn.push_back(new RpnLt); break;
		case LexT::GT: rpn.push_back(new RpnGt); break;
		case LexT::ASSIGN: rpn.push_back(new RpnAssign); break;
		default: assert(false); //DEBUG
	}
}

void Parser::checkNot()
{
	LexT type1 = stType.top();
	//~ cout << "popped " << type1 << endl; //DEBUG
	stType.pop();
	if(opdTypesEq(type1, LexT::BOOLEAN))
		stType.push(LexT::CONST_BOOLEAN);
	else
		throw "types mismatch in operation 'not'";
	rpn.push_back(new RpnNot);
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
	LexT type = stType.top();
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

void Parser::assertLex(LexT lexType)
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
		for(auto &item : rpn) {
			cout << *item << " ";
		}
		cout << endl; 
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
	LexT identType = curType;
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
		curType == LexT::OP_BRACE || curType == LexT::IDENT ||
		curType == LexT::PLUS || curType == LexT::MINUS ||
		curType == LexT::CONST_INT || curType == LexT::CONST_BOOLEAN ||
		curType == LexT::CONST_STRING) {
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
			rpn.push_back(new RpnSemicolon);
			break;
		case LexT::BREAK:
			readLex();
			assertLex(LexT::SEMICOLON);
			rpn.push_back(new RpnSemicolon);
			break;
		case LexT::READ:
			rpn.push_back(new RpnRead);
			readLex();
			assertLex(LexT::OP_PAREN);
			if(curType == LexT::IDENT) {
				if(!tid[curVal].declared)
					throw "variable wasn't declared";
				if(tid[curVal].type == LexT::BOOLEAN)
					throw "trying to read to boolean variable";
				rpn.push_back(new RpnPut(curType, curVal));
				readLex();
			} else
				throw curLex;
			assertLex(LexT::CL_PAREN);
			assertLex(LexT::SEMICOLON);
			rpn.push_back(new RpnSemicolon);
			break;
		case LexT::WRITE:
		{
			rpn.push_back(new RpnWrite);
			readLex();
			assertLex(LexT::OP_PAREN);
			rpn.push_back(nullptr);
			int ind = rpn.size() - 1;
			ntExpr();
			int argc = 1;
			while(curType == LexT::COMMA) {
				readLex();
				ntExpr();
				argc++;
			}
			rpn[ind] = new RpnArgc(argc);
			assertLex(LexT::CL_PAREN);
			assertLex(LexT::SEMICOLON);
			rpn.push_back(new RpnSemicolon);
			break;
		}
		case LexT::OP_BRACE:
			ntComplOper();
			break;
		case LexT::IDENT:
		case LexT::PLUS:
		case LexT::MINUS:
		case LexT::CONST_INT:
		case LexT::CONST_BOOLEAN:
		case LexT::CONST_STRING:
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
	rpn.push_back(new RpnSemicolon);
	ntOut; //DEBUG
}

void Parser::ntExpr()
{
	ntIn; //DEBUG
	ntExpr5();
	if(curType == LexT::ASSIGN) {
		stType.push(curType);
		readLex();
		ntExpr();
		checkOp();
	}
	ntOut; //DEBUG
}

/*
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
*/

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
			rpn.push_back(new RpnPut(LexT::IDENT, curVal));
			readLex();
			break;
		case LexT::PLUS:
		case LexT::MINUS:
		case LexT::CONST_INT:
		case LexT::CONST_BOOLEAN:
		case LexT::CONST_STRING:
			stType.push(curType);
			ntConst();
			// ntConst выбросило в каждый стек по значению
			rpn.push_back(new RpnPut(stType.top(), stVal.top()));
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
		default:
			throw curLex;
	}
	ntOut; //DEBUG
}

bool opdTypesEq(LexT type1, LexT type2)
{
	if(type1 == LexT::INT || type1 == LexT::CONST_INT)
		return type2 == LexT::INT || type2 == LexT::CONST_INT;
	if(type1 == LexT::BOOLEAN || type1 == LexT::CONST_BOOLEAN)
		return type2 == LexT::BOOLEAN || type2 == LexT::CONST_BOOLEAN;
	if(type1 == LexT::STRING || type1 == LexT::CONST_STRING)
		return type2 == LexT::STRING || type2 == LexT::CONST_STRING;
	throw false;
}

bool isTypename(LexT lexType) {
	return lexType == LexT::INT || lexType == LexT::BOOLEAN ||
		lexType == LexT::STRING;
}

bool isSign(LexT lexType) {
	return lexType == LexT::PLUS || lexType == LexT::MINUS;
}
