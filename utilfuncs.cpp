#include "utilfuncs.hpp"
#include "lex.hpp"

bool isLetter(char c)
{
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || 
		(c == '_');
}

bool isDigit(char c)
{
	return c >= '0' && c <= '9';
}

bool opdTypesEq(LexT::Type type1, LexT::Type type2)
{
	if(type1 == LexT::INT || type1 == LexT::CONST_INT)
		return type2 == LexT::INT || type2 == LexT::CONST_INT;
	if(type1 == LexT::BOOLEAN || type1 == LexT::CONST_BOOLEAN)
		return type2 == LexT::BOOLEAN || type2 == LexT::CONST_BOOLEAN;
	if(type1 == LexT::STRING || type1 == LexT::CONST_STRING)
		return type2 == LexT::STRING || type2 == LexT::CONST_STRING;
	throw false;
}

bool isTypename(LexT::Type lexType) {
	return lexType == LexT::INT || lexType == LexT::BOOLEAN ||
		lexType == LexT::STRING;
}

bool isSign(LexT::Type lexType) {
	return lexType == LexT::PLUS || lexType == LexT::MINUS;
}
