#pragma once
#include "CVariant.h"
#include <string>
using namespace std;

/*
* ��������� ��� ������
*/
enum ETokenType
{
	OPER,
	IDENT,
	VALUE,
	UNDEF
};

/*
* ����� ������
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
* ����� ������, ��������������� ����� ��������
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
* ����� ������, ��������������� ����� �������������
*/
class CIdentToken: public CToken{
	using CToken::CToken;
public:
	string lexem;
	CIdentToken(string lexem);
	~CIdentToken();
};

/*
* ����� ������, ��������������� ����� ��������
*/
class COperToken: public CToken {
	using CToken::CToken;
public:
	string lexem;
	COperToken(string lexem);
	~COperToken();
};

/*
* ����� ��������������� ������
*/
class CUndefToken : public CToken {
	using CToken::CToken;
public:
	string lexem;
	CUndefToken(string lexem);
	~CUndefToken();
};
