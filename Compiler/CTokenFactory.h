#pragma once
#include "CToken.h"
#include <string>
#include <set>
#include <regex>
#include <iostream>

using namespace std;

/*
* ����� ������������ ��� ������ �� ������� � ��������� ������ ������������� ������
*/
class CTokenFactory {
private:
	set<string> sOperators = { "and","array","asm","begin","break","case","const","constructor","continue","destructor",
		"div","do","downto","else","end","file","for","function","goto","if","implementation","in",
		"inline","interface","label","mod","nil","not","object","of","on","operator","or","packed","procedure",
		"program","record","repeat","set","shl","shr","then","to","type","unit","until","uses",
		"var","while","with","xor","as","class","constref","dispose","except","exit","exports","finalization",
		"finally","inherited","initialization","is","library","new","on","out","property","raise","self","threadvar",
		"try", "*", "/", "=", ",", ";", ":", ".", "^", "(",")", "[", "]", "{", "}", "<", ">", "<=",">=",
		"<>", "+", "-", ":=", "return", ".."
	};
public:
	CTokenFactory();
	CToken* createToken(string lexem);		// �������� ������ ������������� ������
	~CTokenFactory();
};
