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

bool opdTypesEq(Lex::Type type1, Lex::Type type2)
{
	if(type1 == Lex::INT || type1 == Lex::CONST_INT)
		return type2 == Lex::INT || type2 == Lex::CONST_INT;
	if(type1 == Lex::BOOLEAN || type1 == Lex::CONST_BOOLEAN)
		return type2 == Lex::BOOLEAN || type2 == Lex::CONST_BOOLEAN;
	if(type1 == Lex::STRING || type1 == Lex::CONST_STRING)
		return type2 == Lex::STRING || type2 == Lex::CONST_STRING;
	throw false;
}
