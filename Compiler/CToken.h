#pragma once
#include "CVariant.h"
#include <string>
using namespace std;

/*
* ¬озможный тип токена
*/
enum ETokenType
{
	OPER,
	IDENT,
	VALUE,
	UNDEF
};

/*
*  ласс токена
*/
class CToken {
public:
	ETokenType getType() { return m_T; }
	virtual ~CToken();
protected:
	CToken(ETokenType type);
	ETokenType m_T;
};

/*
*  ласс токена, представл€ющего некое значение
*/
class CValueToken: public CToken{
	using CToken::CToken;
public:
	CVariant getVariant() const { return m_val; }
	CValueToken(CVariant val);
	~CValueToken();
private:
	CVariant m_val;
};

/*
*  ласс токена, представл€ющего некий идентификатор
*/
class CIdentToken: public CToken{
	using CToken::CToken;
public:
	string lexem;
	CIdentToken(string lexem);
	~CIdentToken();
};

/*
*  ласс токена, представл€ющего некий оператор
*/
class COperToken: public CToken {
	using CToken::CToken;
public:
	string lexem;
	COperToken(string lexem);
	~COperToken();
};

/*
*  ласс неопределенного токена
*/
class CUndefToken : public CToken {
	using CToken::CToken;
public:
	string lexem;
	CUndefToken(string lexem);
	~CUndefToken();
};
