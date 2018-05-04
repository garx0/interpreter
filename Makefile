all:
	g++ -std=c++11 -o run main.cpp globals.cpp lex.cpp parser.cpp table.cpp utilfuncs.cpp
