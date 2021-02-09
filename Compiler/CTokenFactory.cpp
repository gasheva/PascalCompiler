#include "CTokenFactory.h"
#include <iostream>
#include <stdlib.h>

using namespace std;


CTokenFactory::CTokenFactory() {}

string toLower(string lexem) {
	transform(lexem.begin(), lexem.end(), lexem.begin(), ::tolower);
	return lexem;
}
CToken* CTokenFactory::createToken(string lexem)
{
	// является ли char (тип value)
	if (lexem.length() == 3 && lexem[0] == '\'' && lexem[lexem.length() - 1] == '\'')
		return new CValueToken(new CCharVariant(lexem[1]));

	// является ли string (тип value)
	if (lexem[0] == '\'' && lexem[lexem.length() - 1] == '\'')
		return new CValueToken(new CStringVariant(lexem));

	// является ли лексема оператором
	if (sOperators.find(toLower(lexem)) != sOperators.end())
		return new COperToken(lexem);

	// можно ли сконвертировать в число (тип value)		
	if (regex_match(lexem, regex(R"(\d+)"))) {
		int temp = std::stoi(lexem);
		return new CValueToken(new CIntVariant(temp));
	}
	if (regex_match(lexem, regex(R"(\d+.\d+)"))) {
		float temp = std::stof(lexem);
		return new CValueToken(new CRealVariant(temp));
	}
	

	// может ли она быть идентификатором
	const regex pattern(R"(^[a-zA-Z_]+[a-zA-Z_0-9]*$)");
	if (regex_match(lexem, pattern))
		return new CIdentToken(lexem);

	return new CUndefToken(lexem);
}


CTokenFactory::~CTokenFactory(){}
