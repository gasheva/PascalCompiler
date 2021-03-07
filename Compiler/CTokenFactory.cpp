#include "CTokenFactory.h"
#include "FStringFunc.h"

using namespace std;


CTokenFactory::CTokenFactory() {}

CToken* CTokenFactory::createToken(string lexem)
{
	string lowerLexem = toLower(lexem);
	// является ли char (тип value)
	if (lexem.length() == 3 && lexem[0] == '\'' && lexem[lexem.length() - 1] == '\'')
		return new CValueToken(CCharVariant(lexem[1]));

	// является ли string (тип value)
	if (lexem[0] == '\'' && lexem[lexem.length() - 1] == '\'')
		return new CValueToken(CStringVariant(lexem));

	// является ли лексема оператором
	if (sOperators.find(lowerLexem) != sOperators.end())
		return new COperToken(lowerLexem);

	// можно ли сконвертировать в число (тип value)		
	if (regex_match(lexem, regex(R"(\d+)"))) {
		int temp = std::stoi(lexem);
		return new CValueToken(CIntVariant(temp));
	}
	
	if (regex_match(lexem, regex(R"(\d+\.\d+)"))) {
		float temp = std::stof(lexem);
		return new CValueToken(CRealVariant(temp));
	}
	// может ли она быть идентификатором
	const regex pattern(R"(^[a-zA-Z_]+[a-zA-Z_0-9]*$)");
	if (regex_match(lexem, pattern))
		return new CIdentToken(lexem);

	return new CUndefToken(lexem);
}


CTokenFactory::~CTokenFactory(){}
