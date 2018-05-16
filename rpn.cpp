#include <iostream>
#include <vector>
#include <stack>
#include <cassert>

#include "table.hpp"
#include "lex.hpp"
#include "rpn.hpp"

using namespace std;

RpnT convertType(const Lex& lex)
// создаёт тип INT|BOOLEAN|STRING или VAR_(INT|BOOLEAN|STRING) из
//   лексемы
{
	LexT type = lex.getType();
	switch(type) {
		case LexT::INT:
		case LexT::CONST_INT:
			return RpnT::INT;
		case LexT::BOOLEAN:
		case LexT::CONST_BOOLEAN:
			return RpnT::BOOLEAN;
		case LexT::STRING:
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

void RpnJumpFalse::print(ostream& stream) const
{
	stream << "!f";
}

void RpnJumpTrue::print(ostream& stream) const
{
	stream << "!t";
}

void RpnLabel::print(ostream& stream) const
{
	stream << "label{" << address << "}";
}

void RpnAddress::print(ostream& stream) const
{
	stream << "&" << tid[ind].name;
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
	res = calc(op1, op2);
	context.st.push(res);
}

void RpnPut::execute(RpnContext& context) const
{
	context.st.push(RpnOperand(type, value));
}

void RpnSemicolon::execute(RpnContext& context) const
{
	context.st.pop();
}

RpnOperand RpnAdd::calc(const RpnOperand& op1, const RpnOperand& op2) const
{
	RpnT type = rpnEqTypes(op1.type, RpnT::INT) ? RpnT::INT : RpnT::STRING;
	assert(rpnEqTypes(op2.type, type)); //DEBUG
	int value1, value2;
	value1 = isVarType(op1.type) ? tid[op1.value].value : op1.value;
	value2 = isVarType(op2.type) ? tid[op2.value].value : op2.value;
	return RpnOperand(type, value1 + value2);
}

RpnOperand RpnSub::calc(const RpnOperand& op1, const RpnOperand& op2) const
{
	assert((rpnEqTypes(op1.type, RpnT::INT) && rpnEqTypes(op2.type, RpnT::INT))); //DEBUG
	int value1, value2;
	value1 = isVarType(op1.type) ? tid[op1.value].value : op1.value;
	value2 = isVarType(op2.type) ? tid[op2.value].value : op2.value;
	return RpnOperand(RpnT::INT, value1 - value2);
}

RpnOperand RpnMul::calc(const RpnOperand& op1, const RpnOperand& op2) const
{
	assert((rpnEqTypes(op1.type, RpnT::INT) && rpnEqTypes(op2.type, RpnT::INT))); //DEBUG
	int value1, value2;
	value1 = isVarType(op1.type) ? tid[op1.value].value : op1.value;
	value2 = isVarType(op2.type) ? tid[op2.value].value : op2.value;
	return RpnOperand(RpnT::INT, value1 * value2);
}

RpnOperand RpnDiv::calc(const RpnOperand& op1, const RpnOperand& op2) const
{
	assert((rpnEqTypes(op1.type, RpnT::INT) && rpnEqTypes(op2.type, RpnT::INT))); //DEBUG
	int value1, value2;
	value1 = isVarType(op1.type) ? tid[op1.value].value : op1.value;
	value2 = isVarType(op2.type) ? tid[op2.value].value : op2.value;
	return RpnOperand(RpnT::INT, value1 / value2);
}

RpnOperand RpnMod::calc(const RpnOperand& op1, const RpnOperand& op2) const
{
	assert((rpnEqTypes(op1.type, RpnT::INT) && rpnEqTypes(op2.type, RpnT::INT))); //DEBUG
	int value1, value2;
	value1 = isVarType(op1.type) ? tid[op1.value].value : op1.value;
	value2 = isVarType(op2.type) ? tid[op2.value].value : op2.value;
	return RpnOperand(RpnT::INT, value1 % value2);
}

RpnOperand RpnLogOp::calc(const RpnOperand& op1, const RpnOperand& op2) const
{
	assert((rpnEqTypes(op1.type, RpnT::BOOLEAN) && rpnEqTypes(op2.type, RpnT::BOOLEAN))); //DEBUG
	int value1, value2;
	value1 = isVarType(op1.type) ? tid[op1.value].value : op1.value;
	value2 = isVarType(op2.type) ? tid[op2.value].value : op2.value;
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
	value = isVarType(op.type) ? tid[op.value].value : op.value;
	context.st.push(RpnOperand(RpnT::BOOLEAN, ~value));
}

RpnOperand RpnCmpOp::calc(const RpnOperand& op1, const RpnOperand& op2) const
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
	value1 = isVarType(op1.type) ? tid[op1.value].value : op1.value;
	value2 = isVarType(op2.type) ? tid[op2.value].value : op2.value;
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

RpnOperand RpnAssign::calc(const RpnOperand& op1, const RpnOperand& op2) const
{
}

void RpnJump::execute(RpnContext& context) const
{
}

void RpnJumpFalse::execute(RpnContext& context) const
{
}

void RpnJumpTrue::execute(RpnContext& context) const
{
}

void RpnLabel::execute(RpnContext& context) const
{
}

void RpnAddress::execute(RpnContext& context) const
{
}

void RpnArgc::execute(RpnContext& context) const
{
}

void RpnRead::execute(RpnContext& context) const
{
}

void RpnWrite::execute(RpnContext& context) const
{
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
