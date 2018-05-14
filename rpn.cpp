#include <iostream>
#include <vector>
#include <stack>
#include <cassert>

#include "table.hpp"
#include "lex.hpp"
#include "rpn.hpp"

using namespace std;

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
		case LexT::INT:
			stream << value;
			break;
		case LexT::BOOLEAN:
			stream << (value ? "true" : "false");
			break;
		case LexT::STRING:
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
}

void RpnPut::execute(RpnContext& context) const
{
}

void RpnSemicolon::execute(RpnContext& context) const
{
}

void RpnAdd::calc(RpnContext& context) const
{
}

void RpnSub::calc(RpnContext& context) const
{
}

void RpnMul::calc(RpnContext& context) const
{
}

void RpnDiv::calc(RpnContext& context) const
{
}

void RpnMod::calc(RpnContext& context) const
{
}

void RpnAnd::calc(RpnContext& context) const
{
}

void RpnOr::calc(RpnContext& context) const
{
}

void RpnNot::execute(RpnContext& context) const
{
}

void RpnEq::calc(RpnContext& context) const
{
}

void RpnNe::calc(RpnContext& context) const
{
}

void RpnLe::calc(RpnContext& context) const
{
}

void RpnGe::calc(RpnContext& context) const
{
}

void RpnLt::calc(RpnContext& context) const
{
}

void RpnAssign::calc(RpnContext& context) const
{
}

void RpnGt::calc(RpnContext& context) const
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
