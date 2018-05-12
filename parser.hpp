#pragma once

#include <iostream>
#include <vector>
#include <stack>
#include <algorithm>
#include <cassert>
#include <initializer_list>
#include <fstream>

#include "lex.hpp"
#include "table.hpp"
#include "utilfuncs.hpp"
#include "rpn.hpp"

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
	Lex curLex = LexT::LEX_NULL;
	LexT curType = LexT::LEX_NULL;
	int curVal = 0;
	stack<LexT> stType;
	stack<int> stVal;
	//RpnContext context;
	vector<RpnOp*> rpn;
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
	void assertLex(LexT lexType);
	/* стандартная конструкция, повторяющаяся много раз в
	 * процедурах, соотв. нетерминалам
	 */
};
