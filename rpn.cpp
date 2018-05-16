#include <iostream>
#include <vector>
#include <stack>
#include <cassert>

#include "table.hpp"
#include "lex.hpp"
#include "rpn.hpp"

using namespace std;

RpnT rpnConvertType(const Lex& lex, const RpnContext& context)
// используется на этапе выполнения программы.
// создаёт тип INT|BOOLEAN|STRING или VAR_(INT|BOOLEAN|STRING) из
//   лексемы.
{
	LexT type = lex.getType();
	switch(type) {
		case LexT::CONST_INT:
			return RpnT::INT;
		case LexT::CONST_BOOLEAN:
			return RpnT::BOOLEAN;
		case LexT::CONST_STRING:
			return RpnT::STRING;
		case LexT::IDENT:
		{
			LexT idType = context.tid[lex.getValue()].type;
			switch(idType) {
				case LexT::INT:
					return RpnT::VAR_INT;
				case LexT::BOOLEAN:
					return RpnT::VAR_BOOLEAN;
				case LexT::STRING:
					return RpnT::VAR_STRING;
				default:
					assert(false);
			}
		}	
		default:
			assert(false);
	}
}

RpnT rpnConvertType(const Lex& lex)
// используется на этапе генерации массива rpn-лексем.
// создаёт тип INT|BOOLEAN|STRING или VAR_(INT|BOOLEAN|STRING) из
//   лексемы.
{
	LexT type = lex.getType();
	cout << "TYPE = " << (int) type << endl; //DEBUG
	switch(type) {
		case LexT::CONST_INT:
			return RpnT::INT;
		case LexT::CONST_BOOLEAN:
			return RpnT::BOOLEAN;
		case LexT::CONST_STRING:
			return RpnT::STRING;
		case LexT::IDENT:
		{
			LexT idType = tid[lex.getValue()].type;
			switch(idType) {
				case LexT::INT:
					return RpnT::VAR_INT;
				case LexT::BOOLEAN:
					return RpnT::VAR_BOOLEAN;
				case LexT::STRING:
					return RpnT::VAR_STRING;
				default:
					assert(false);
			}
		}	
		default:
			assert(false);
	}
}

bool rpnEqTypes(RpnT type1, RpnT type2)
{
	if(type1 == type2)
		return true;
	else if(type1 == RpnT::INT || type1 == RpnT::VAR_INT)
		return type2 == RpnT::INT || type2 == RpnT::VAR_INT;
	else if(type1 == RpnT::BOOLEAN || type1 == RpnT::VAR_BOOLEAN)
		return type2 == RpnT::BOOLEAN || type2 == RpnT::VAR_BOOLEAN;
	else if(type1 == RpnT::STRING || type1 == RpnT::VAR_STRING)
		return type2 == RpnT::STRING || type2 == RpnT::VAR_STRING;
}

bool isVarType(RpnT type)
{
	switch(type) {
		case RpnT::VAR_INT:
		case RpnT::VAR_BOOLEAN:
		case RpnT::VAR_STRING:
			return true;
		case RpnT::INT:
		case RpnT::BOOLEAN:
		case RpnT::STRING:
			return false;
		default:
			assert(false);
	}
}
	
ostream& operator<<(ostream& stream, const RpnOp& op)
{
	op.print(stream);
	return stream;
}

void RpnPut::print(ostream& stream) const
{
	switch(type) {
		case RpnT::VAR_INT:
		case RpnT::VAR_BOOLEAN:
		case RpnT::VAR_STRING:
			stream << tid[value].name;
			break;
		case RpnT::INT:
			stream << value;
			break;
		case RpnT::BOOLEAN:
			stream << (value ? "true" : "false");
			break;
		case RpnT::STRING:
			stream << "\"" << tstr[value] << "\"";
			break;
		default:
			assert(false);
	}
}

void RpnSemicolon::print(ostream& stream) const
{
	stream << ";";
}

void RpnAdd::print(ostream& stream) const
{
	stream << "+";
}

void RpnSub::print(ostream& stream) const
{
	stream << "-";
}

void RpnMul::print(ostream& stream) const
{
	stream << "*";
}

void RpnDiv::print(ostream& stream) const
{
	stream << "/";
}

void RpnMod::print(ostream& stream) const
{
	stream << "%";
}

void RpnAnd::print(ostream& stream) const
{
	stream << "and";
}

void RpnOr::print(ostream& stream) const
{
	stream << "or";
}

void RpnNot::print(ostream& stream) const
{
	stream << "not";
}

void RpnEq::print(ostream& stream) const
{
	stream << "==";
}

void RpnNe::print(ostream& stream) const
{
	stream << "!=";
}

void RpnLe::print(ostream& stream) const
{
	stream << "<=";
}

void RpnGe::print(ostream& stream) const
{
	stream << ">=";
}

void RpnLt::print(ostream& stream) const
{
	stream << "<";
}

void RpnAssign::print(ostream& stream) const
{
	stream << "=";
}

void RpnGt::print(ostream& stream) const
{
	stream << ">";
}

void RpnJump::print(ostream& stream) const
{
	stream << "!";
}

void RpnJumpTrue::print(ostream& stream) const
{
	stream << "!t";
}

void RpnJumpFalse::print(ostream& stream) const
{
	stream << "!f";
}

void RpnLabel::print(ostream& stream) const
{
	stream << "label{" << value << "}";
}

void RpnAddress::print(ostream& stream) const
{
	stream << "&" << tid[value].name;
}

void RpnArgc::print(ostream& stream) const
{
	stream << value;
}

void RpnRead::print(ostream& stream) const
{
	stream << "read";
}

void RpnWrite::print(ostream& stream) const
{
	stream << "write";
}

void RpnBinOp::execute(RpnContext& context) const
{
	RpnOperand op1, op2, res;
	op2 = context.st.top();
	context.st.pop();
	op1 = context.st.top();
	context.st.pop();
	res = calc(op1, op2, context);
	context.st.push(res);
	context.eip++;
}

void RpnPut::execute(RpnContext& context) const
{
	cout << "RPNPUT:TYPE=" << (int) type << endl; //DEBUG
	context.st.push(RpnOperand(type, value));
	context.eip++;
}

void RpnSemicolon::execute(RpnContext& context) const
{
	if(!context.st.empty())
		context.st.pop();
	else
		cout << "stack is empty\n"; //DEBUG
	context.eip++;
}

RpnOperand RpnAdd::calc(const RpnOperand& op1, 
	const RpnOperand& op2, RpnContext& context) const
{
	RpnT type = rpnEqTypes(op1.type, RpnT::INT) ? RpnT::INT : RpnT::STRING;
	assert(rpnEqTypes(op2.type, type)); //DEBUG
	if(type == RpnT::INT) {
		int value1, value2;
		value1 = isVarType(op1.type) ? context.tid[op1.value].value : op1.value;
		value2 = isVarType(op2.type) ? context.tid[op2.value].value : op2.value;
		return RpnOperand(type, value1 + value2);
	} else {
		string str1, str2;
		str1 = isVarType(op1.type) ? context.tid[op1.value].str : op1.str;
		str2 = isVarType(op2.type) ? context.tid[op2.value].str : op2.str;
		return RpnOperand(type, str1 + str2);
	}
}

RpnOperand RpnSub::calc(const RpnOperand& op1, 
	const RpnOperand& op2, RpnContext& context) const
{
	assert((rpnEqTypes(op1.type, RpnT::INT) && rpnEqTypes(op2.type, RpnT::INT))); //DEBUG
	int value1, value2;
	value1 = isVarType(op1.type) ? context.tid[op1.value].value : op1.value;
	value2 = isVarType(op2.type) ? context.tid[op2.value].value : op2.value;
	return RpnOperand(RpnT::INT, value1 - value2);
}

RpnOperand RpnMul::calc(const RpnOperand& op1, 
	const RpnOperand& op2, RpnContext& context) const
{
	assert((rpnEqTypes(op1.type, RpnT::INT) && rpnEqTypes(op2.type, RpnT::INT))); //DEBUG
	int value1, value2;
	value1 = isVarType(op1.type) ? context.tid[op1.value].value : op1.value;
	value2 = isVarType(op2.type) ? context.tid[op2.value].value : op2.value;
	return RpnOperand(RpnT::INT, value1 * value2);
}

RpnOperand RpnDiv::calc(const RpnOperand& op1, 
	const RpnOperand& op2, RpnContext& context) const
{
	assert((rpnEqTypes(op1.type, RpnT::INT) && rpnEqTypes(op2.type, RpnT::INT))); //DEBUG
	int value1, value2;
	value1 = isVarType(op1.type) ? context.tid[op1.value].value : op1.value;
	value2 = isVarType(op2.type) ? context.tid[op2.value].value : op2.value;
	return RpnOperand(RpnT::INT, value1 / value2);
}

RpnOperand RpnMod::calc(const RpnOperand& op1, 
	const RpnOperand& op2, RpnContext& context) const
{
	assert((rpnEqTypes(op1.type, RpnT::INT) && rpnEqTypes(op2.type, RpnT::INT))); //DEBUG
	int value1, value2;
	value1 = isVarType(op1.type) ? context.tid[op1.value].value : op1.value;
	value2 = isVarType(op2.type) ? context.tid[op2.value].value : op2.value;
	return RpnOperand(RpnT::INT, value1 % value2);
}

RpnOperand RpnLogOp::calc(const RpnOperand& op1, 
	const RpnOperand& op2, RpnContext& context) const
{
	assert((rpnEqTypes(op1.type, RpnT::BOOLEAN) && rpnEqTypes(op2.type, RpnT::BOOLEAN))); //DEBUG
	int value1, value2;
	value1 = isVarType(op1.type) ? context.tid[op1.value].value : op1.value;
	value2 = isVarType(op2.type) ? context.tid[op2.value].value : op2.value;
	return RpnOperand(RpnT::BOOLEAN, doCalc(value1, value2));
}

int RpnAnd::doCalc(int a, int b) const
{
	return a & b;
}

int RpnOr::doCalc(int a, int b) const
{
	return a | b;
}

void RpnNot::execute(RpnContext& context) const
{
	RpnOperand op, res;
	op = context.st.top();
	context.st.pop();
	assert(rpnEqTypes(op.type, RpnT::BOOLEAN)); //DEBUG
	int value;
	value = isVarType(op.type) ? context.tid[op.value].value : op.value;
	context.st.push(RpnOperand(RpnT::BOOLEAN, ~value));
	context.eip++;
}

RpnOperand RpnCmpOp::calc(const RpnOperand& op1, 
	const RpnOperand& op2, RpnContext& context) const
{
	RpnT type;
	if(rpnEqTypes(op1.type, RpnT::INT))
		type = RpnT::INT;
	else if(rpnEqTypes(op1.type, RpnT::BOOLEAN))
		type = RpnT::BOOLEAN;
	else {
		assert(rpnEqTypes(op1.type, RpnT::STRING));
		type = RpnT::STRING;
	}
	assert(rpnEqTypes(op2.type, type)); //DEBUG
	int value1, value2;
	value1 = isVarType(op1.type) ? context.tid[op1.value].value : op1.value;
	value2 = isVarType(op2.type) ? context.tid[op2.value].value : op2.value;
	return RpnOperand(RpnT::BOOLEAN, doCalc(value1, value2));
}

int RpnEq::doCalc(int a, int b) const
{
	return a == b;
}

int RpnNe::doCalc(int a, int b) const
{
	return a != b;
}

int RpnLe::doCalc(int a, int b) const
{
	return a <= b;
}

int RpnGe::doCalc(int a, int b) const
{
	return a >= b;
}

int RpnLt::doCalc(int a, int b) const
{
	return a < b;
}

int RpnGt::doCalc(int a, int b) const
{
	return a > b;
}

RpnOperand RpnAssign::calc(const RpnOperand& op1, 
	const RpnOperand& op2, RpnContext& context) const
{
	assert(op1.type == RpnT::ADDRESS);
	RpnT varType = rpnConvertType(Lex(LexT::IDENT, op1.value), context);
	//varType == VAR_(INT|BOOLEAN|STRING)
	assert(rpnEqTypes(varType, op2.type));
	context.tid[op1.value].assigned = true;
	RpnT type;
	if(varType == RpnT::VAR_STRING) {
		type = RpnT::STRING;
		context.tid[op1.value].str = op2.str;
		return RpnOperand(type, op2.str);
	} else {
		type = (varType == RpnT::VAR_INT) ? RpnT::INT : RpnT::BOOLEAN;
		context.tid[op1.value].value = op2.value;
		return RpnOperand(type, op2.value);
	}
}

void RpnJump::execute(RpnContext& context) const
{
	RpnOperand op = context.st.top();
	context.st.pop();
	context.eip = op.value;
	context.st.push(RpnOperand());
}


RpnOperand RpnJumpTrue::calc(const RpnOperand& op1,
	const RpnOperand& op2, RpnContext& context) const
{
	if(op1.value)
		context.eip = op2.value - 1;
		//т.к. RpnBinOp::execute() сделает eip++ после этого
	return RpnOperand();		
}

RpnOperand RpnJumpFalse::calc(const RpnOperand& op1, 
	const RpnOperand& op2, RpnContext& context) const
{
	if(!op1.value)
		context.eip = op2.value - 1;
		//т.к. RpnBinOp::execute() сделает eip++ после этого
	return RpnOperand();
}

void RpnLabel::execute(RpnContext& context) const
{
	context.st.push(RpnOperand(RpnT::LABEL, value));
	context.eip++;
}

void RpnAddress::execute(RpnContext& context) const
{
	context.st.push(RpnOperand(RpnT::ADDRESS, value));
	context.eip++;
}

void RpnArgc::execute(RpnContext& context) const
{
	context.st.push(RpnOperand(RpnT::ARGC, value));
	context.eip++;
}

void RpnRead::execute(RpnContext& context) const
{
	RpnOperand op = context.st.top();
	context.st.pop();
	assert(op.type == RpnT::ADDRESS);
	RpnT varType = rpnConvertType(Lex(LexT::IDENT, op.value), context);
	assert(varType != VAR_BOOLEAN);
	if(varType == RpnT::VAR_INT) {
		int res;
		cin >> res;
		context.tid[op.value].value = res;
	} else {
		string res;
		cin >> res;
		context.tid[op.value].str = res;
	}
	context.st.push(RpnOperand());
	context.eip++;
}

void RpnWrite::execute(RpnContext& context) const
{
	assert(context.st.top().type == RpnT::ARGC);
	int argc = context.st.top().value;
	context.st.pop();
	for(int i = 0; i < argc; i++) {
		RpnOperand op = context.st.top();
		context.st.pop();
		switch(op.type) {
			case INT:
				cout << op.value;
				break;
			case BOOLEAN:
				cout << op.value ? "true" : "false";
				break;
			case STRING:
				cout << op.str;
				break;
			case VAR_INT:
				cout << context.tid[op.value].value;
				break;
			case VAR_BOOLEAN:
				cout << context.tid[op.value].value ? "true" : "false";
				break;
			case VAR_STRING:
				cout << context.tid[op.value].str;
				break;
		}
	}
	context.st.push(RpnOperand());
	context.eip++;
}

void idToAddr(RpnOp*& abstrPtr)
// заменяет rpn-операцию put(значение идентификатора) на
// операцию-адрес идентификатора
// (*abstrPtr) должен быть типа RpnPut
// (*abstrPtr) должен быть порожден при помощи new
{
	RpnPut* ptr = dynamic_cast<RpnPut*>(abstrPtr);
	int value = ptr->getValue();
	delete abstrPtr;
	abstrPtr = new RpnAddress(value);
}

RpnProgram::RpnProgram(const vector<RpnOp*>& a_rpn,
	const Table<Ident>& a_tid, const Table<string>& a_tstr):
context(a_tid, a_tstr)
{
	rpn = a_rpn;
}

void RpnProgram::execute()
{
	int progSize = rpn.size();
	while(context.eip < progSize) {
		cout << "eip=" << context.eip << endl; //DEBUG
		rpn[context.eip]->execute(context);
	}
}

//~ RpnProgram::RpnProgram(const Parser& parser,
	//~ const Table<Ident>& a_tid, const Table<string>& a_tstr):
//~ context(a_tid, a_tstr)
//~ {
	//~ rpn = parser.getRpnRef();
//~ }
