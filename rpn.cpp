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
			stream << tstr[value];
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

void RpnGo::print(ostream& stream) const
{
	stream << "!";
}

void RpnFgo::print(ostream& stream) const
{
	stream << "!f";
}

void RpnLabel::print(ostream& stream) const
{
	stream << "label{" << address << "}";
}

void RpnAddress::print(ostream& stream) const
{
}

void RpnArgc::print(ostream& stream) const
{
	stream << num;
}

void RpnRead::print(ostream& stream) const
{
	stream << "read";
}

void RpnWrite::print(ostream& stream) const
{
	stream << "write";
}

void RpnBinOp::calc(RpnContext& context) const
{
}

void RpnPut::calc(RpnContext& context) const
{
}

void RpnSemicolon::calc(RpnContext& context) const
{
}

void RpnAdd::doCalc(RpnContext& context) const
{
}

void RpnSub::doCalc(RpnContext& context) const
{
}

void RpnMul::doCalc(RpnContext& context) const
{
}

void RpnDiv::doCalc(RpnContext& context) const
{
}

void RpnMod::doCalc(RpnContext& context) const
{
}

void RpnAnd::doCalc(RpnContext& context) const
{
}

void RpnOr::doCalc(RpnContext& context) const
{
}

void RpnNot::calc(RpnContext& context) const
{
}

void RpnEq::doCalc(RpnContext& context) const
{
}

void RpnNe::doCalc(RpnContext& context) const
{
}

void RpnLe::doCalc(RpnContext& context) const
{
}

void RpnGe::doCalc(RpnContext& context) const
{
}

void RpnLt::doCalc(RpnContext& context) const
{
}

void RpnAssign::doCalc(RpnContext& context) const
{
}

void RpnGt::doCalc(RpnContext& context) const
{
}

void RpnGo::calc(RpnContext& context) const
{
}

void RpnFgo::calc(RpnContext& context) const
{
}

void RpnLabel::calc(RpnContext& context) const
{
}

void RpnAddress::calc(RpnContext& context) const
{
}

void RpnArgc::calc(RpnContext& context) const
{
}

void RpnRead::calc(RpnContext& context) const
{
}

void RpnWrite::calc(RpnContext& context) const
{
}


