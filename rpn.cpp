#include <iostream>
#include <vector>
#include <stack>
#include <cassert>

#include "table.hpp"
#include "lex.hpp"
#include "rpn.hpp"

using namespace std;

RpnT addrType(RpnT valType)
{
	switch(valType) {
		case RpnT::INT: return RpnT::ADDR_INT;
		case RpnT::BOOLEAN: return RpnT::ADDR_BOOLEAN;
		case RpnT::STRING: return RpnT::ADDR_STRING;
		default: assert(false);
	}
}

RpnT valType(RpnT addrType)
{
	switch(addrType) {
		case RpnT::ADDR_INT: return RpnT::INT;
		case RpnT::ADDR_BOOLEAN: return RpnT::BOOLEAN;
		case RpnT::ADDR_STRING: return RpnT::STRING;
		default: assert(false);
	}
}

RpnT addrType(LexT type)
{
	switch(type) {
		case LexT::INT:
		case LexT::CONST_INT:return RpnT::ADDR_INT;
		case LexT::BOOLEAN: 
		case LexT::CONST_BOOLEAN: return RpnT::ADDR_BOOLEAN;
		case LexT::STRING: 
		case LexT::CONST_STRING: return RpnT::ADDR_STRING;
		default: assert(false);
	}
}

RpnT valType(LexT type)
{
	switch(type) {
		case LexT::INT: return RpnT::INT;
		case LexT::CONST_INT: return RpnT::INT;
		case LexT::BOOLEAN: return RpnT::BOOLEAN;
		case LexT::CONST_BOOLEAN: return RpnT::BOOLEAN;
		case LexT::STRING: return RpnT::STRING;
		case LexT::CONST_STRING: return RpnT::STRING;
		default: assert(false);
	}
}

bool rpnEqTypes(RpnT type1, RpnT type2)
{
	if(type1 == type2)
		return true;
	else if(type1 == RpnT::INT || type1 == RpnT::ADDR_INT)
		return type2 == RpnT::INT || type2 == RpnT::ADDR_INT;
	else if(type1 == RpnT::BOOLEAN || type1 == RpnT::ADDR_BOOLEAN)
		return type2 == RpnT::BOOLEAN || type2 == RpnT::ADDR_BOOLEAN;
	else if(type1 == RpnT::STRING || type1 == RpnT::ADDR_STRING)
		return type2 == RpnT::STRING || type2 == RpnT::ADDR_STRING;
}
	
ostream& operator<<(ostream& stream, const RpnOp& op)
{
	op.print(stream);
	return stream;
}

void RpnPut::print(ostream& stream) const
{
	switch(type) {
		case LexT::IDENT:
			stream << tid[value].name;
			break;
		case LexT::CONST_INT:
			stream << value;
			break;
		case LexT::CONST_BOOLEAN:
			stream << (value ? "true" : "false");
			break;
		case LexT::CONST_STRING:
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
	RpnOperand res;
	if(type == LexT::IDENT) {
		Ident id = context.tid[value];
		if(!id.assigned) {
			throw ("runtime error: variable" + id.name + "not assigned");
		}
		if(id.type == LexT::STRING) {
			res = {RpnT::STRING, id.str};
		} else {
			res = {valType(id.type), id.value};
		}
	} else if(type == LexT::CONST_STRING) {
		res = {RpnT::STRING, context.tstr[value]};
	} else {
		res = {valType(type), value};
	}
	context.st.push(res);
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
	RpnT type = op1.type;
	assert(op2.type == type); //DEBUG
	if(type == RpnT::INT) {
		return RpnOperand(type, op1.value + op2.value);
	} else {
		return RpnOperand(type, op1.str + op2.str);
	}
}

RpnOperand RpnSub::calc(const RpnOperand& op1, 
	const RpnOperand& op2, RpnContext& context) const
{
	assert(op1.type == op2.type && op1.type == RpnT::INT); //DEBUG
	return RpnOperand(RpnT::INT, op1.value - op2.value);
}

RpnOperand RpnMul::calc(const RpnOperand& op1, 
	const RpnOperand& op2, RpnContext& context) const
{
	assert(op1.type == op2.type && op1.type == RpnT::INT); //DEBUG
	return RpnOperand(RpnT::INT, op1.value * op2.value);
}

RpnOperand RpnDiv::calc(const RpnOperand& op1, 
	const RpnOperand& op2, RpnContext& context) const
{
	assert(op1.type == op2.type && op1.type == RpnT::INT); //DEBUG
	return RpnOperand(RpnT::INT, op1.value / op2.value);
}

RpnOperand RpnMod::calc(const RpnOperand& op1, 
	const RpnOperand& op2, RpnContext& context) const
{
	assert(op1.type == op2.type && op1.type == RpnT::INT); //DEBUG
	return RpnOperand(RpnT::INT, op1.value % op2.value);
}

RpnOperand RpnAnd::calc(const RpnOperand& op1, 
	const RpnOperand& op2, RpnContext& context) const
{
	assert(op1.type == op2.type && op1.type == RpnT::BOOLEAN); //DEBUG
	return RpnOperand(RpnT::BOOLEAN, op1.value & op2.value);
}

RpnOperand RpnOr::calc(const RpnOperand& op1, 
	const RpnOperand& op2, RpnContext& context) const
{
	assert(op1.type == op2.type && op1.type == RpnT::BOOLEAN); //DEBUG
	return RpnOperand(RpnT::BOOLEAN, op1.value | op2.value);
}

void RpnNot::execute(RpnContext& context) const
{
	RpnOperand op, res;
	op = context.st.top();
	context.st.pop();
	assert(op.type == RpnT::BOOLEAN); //DEBUG
	context.st.push(RpnOperand(RpnT::BOOLEAN, ~op.value));
	context.eip++;
}

RpnOperand RpnEq::calc(const RpnOperand& op1, 
	const RpnOperand& op2, RpnContext& context) const
{
	assert(op1.type == op2.type); //DEBUG
	if(op1.type == RpnT::STRING)
		return RpnOperand(RpnT::BOOLEAN, op1.str == op2.str);
	else
		return RpnOperand(RpnT::BOOLEAN, op1.value == op2.value);
}

RpnOperand RpnNe::calc(const RpnOperand& op1, 
	const RpnOperand& op2, RpnContext& context) const
{
	assert(op1.type == op2.type); //DEBUG
	if(op1.type == RpnT::STRING)
		return RpnOperand(RpnT::BOOLEAN, op1.str != op2.str);
	else
		return RpnOperand(RpnT::BOOLEAN, op1.value != op2.value);
}

RpnOperand RpnLe::calc(const RpnOperand& op1, 
	const RpnOperand& op2, RpnContext& context) const
{
	assert(op1.type == op2.type); //DEBUG
	if(op1.type == RpnT::STRING)
		return RpnOperand(RpnT::BOOLEAN, op1.str <= op2.str);
	else
		return RpnOperand(RpnT::BOOLEAN, op1.value <= op2.value);
}

RpnOperand RpnGe::calc(const RpnOperand& op1, 
	const RpnOperand& op2, RpnContext& context) const
{
	assert(op1.type == op2.type); //DEBUG
	if(op1.type == RpnT::STRING)
		return RpnOperand(RpnT::BOOLEAN, op1.str >= op2.str);
	else
		return RpnOperand(RpnT::BOOLEAN, op1.value >= op2.value);
}

RpnOperand RpnLt::calc(const RpnOperand& op1, 
	const RpnOperand& op2, RpnContext& context) const
{
	assert(op1.type == op2.type); //DEBUG
	if(op1.type == RpnT::STRING)
		return RpnOperand(RpnT::BOOLEAN, op1.str < op2.str);
	else
		return RpnOperand(RpnT::BOOLEAN, op1.value < op2.value);
}

RpnOperand RpnGt::calc(const RpnOperand& op1, 
	const RpnOperand& op2, RpnContext& context) const
{
	assert(op1.type == op2.type); //DEBUG
	if(op1.type == RpnT::STRING)
		return RpnOperand(RpnT::BOOLEAN, op1.str > op2.str);
	else
		return RpnOperand(RpnT::BOOLEAN, op1.value > op2.value);
}

RpnOperand RpnAssign::calc(const RpnOperand& op1, 
	const RpnOperand& op2, RpnContext& context) const
{
	valType(op1.type); //DEBUG
	assert(addrType(op2.type) == op1.type); //DEBUG
	context.tid[op1.value].assigned = true;
	RpnT type = valType(op1.type);
	if(op1.type == RpnT::ADDR_STRING) {
		context.tid[op1.value].str = op2.str;
		return RpnOperand(type, op2.str);
	} else {
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
	context.st.push(RpnOperand(addrType(context.tid[value].type), value));
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
	valType(op.type); //DEBUG
	assert(op.type != ADDR_BOOLEAN);
	if(op.type == RpnT::ADDR_INT) {
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
	stack<RpnOperand> argv;
	for(int i = 0; i < argc; i++) {
		argv.push(context.st.top());
		context.st.pop();
	}
	while(!argv.empty()) {
		RpnOperand op = argv.top();
		argv.pop();
		switch(op.type) {
			case INT:
				cout << op.value;
				break;
			case BOOLEAN:
				cout << (op.value ? "true" : "false");
				break;
			case STRING:
				cout << "writing string:"; //DEBUG
				cout << op.str;
				break;
			default:
				assert(false);
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
		//cout << "eip=" << context.eip << endl; //DEBUG
		rpn[context.eip]->execute(context);
	}
}

//~ RpnProgram::RpnProgram(const Parser& parser,
	//~ const Table<Ident>& a_tid, const Table<string>& a_tstr):
//~ context(a_tid, a_tstr)
//~ {
	//~ rpn = parser.getRpnRef();
//~ }
