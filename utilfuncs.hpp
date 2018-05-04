#include "lex.hpp"

bool isLetter(char c);

bool isDigit(char c);

bool opdTypesEq(Lex::Type type1, Lex::Type type2);
//по этому правилу сравнения:
//любые два типа из {INT, CONST_INT} равны
//любые два типа из {BOOLEAN, CONST_BOOLEAN} равны
//любые два типа из {STRING, CONST_STRING} равны
