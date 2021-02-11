#include "CToken.h"


CToken::~CToken() {}


CToken::CToken(ETokenType type)
{
	this->m_T = type;
}


CValueToken::CValueToken(CVariant val): CToken(VALUE) {
	this->m_val = val;
}
CValueToken::~CValueToken() {}


CIdentToken::CIdentToken(string lexem) : CToken(IDENT) {
	this->lexem = lexem;
}
CIdentToken::~CIdentToken(){}


COperToken::COperToken(string lexem) : CToken(OPER) {
	this->lexem = lexem;
}
COperToken::~COperToken(){}

CUndefToken::CUndefToken(string lexem) : CToken(UNDEF) {
	this->lexem = lexem;
}
CUndefToken::~CUndefToken(){}

