#pragma once
#include <string>
#include "CToken.h"
#include <set>
#include <regex>

using namespace std;

/*
*  ласс определ€ющий тип токена по лексеме и создающий объект определенного класса
*/
class CTokenFactory {
private:
	set<string> sOperators = { "and","array","asm","begin","break","case","const","constructor","continue","destructor",
		"div","do","downto","else","end", "end.","false","file","for","function","goto","if","implementation","in",
		"inline","interface","label","mod","nil","not","object","of","on","operator","or","packed","procedure",
		"program","record","repeat","set","shl","shr","string","then","to","true","type","unit","until","uses",
		"var","while","with","xor","as","class","constref","dispose","except","exit","exports","finalization",
		"finally","inherited","initialization","is","library","new","on","out","property","raise","self","threadvar",
		"try", "*", "/", "=", ",", ";", ":", ".", "^", "(",")", "[", "]", "{", "}", "<", ">", "<=",">=",
		"<>", "+", "-", ":=",
		"float", "integer", "string", "char", "return"
	};
public:
	CTokenFactory();
	CToken* createToken(string lexem);		// создание токена определенного класса
	~CTokenFactory();
};
