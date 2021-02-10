#pragma once
#include <string>
using namespace std;

/*
* ��������� ���� ���������� � ��������
*/
enum EVarType {
	INT,
	REAL,
	STRING,
	CHAR
};

/*
* ����������� ������������ �����, ��� ������������ �������� ����� ����������
*/
class CVariant {
public:
	EVarType T;
	
	CVariant();
	virtual ~CVariant();
protected:
	CVariant(EVarType type);	
};

/*
* ����� ��� ���� Integer
*/
class CIntVariant : public CVariant {
	using CVariant::CVariant;
public:
	int m_val;
	CIntVariant(int val);
	~CIntVariant();
};

/*
* ����� ��� ���� Real
*/
class CRealVariant : public CVariant {
	using CVariant::CVariant;
public:
	float m_val;
	CRealVariant(float val);
	~CRealVariant();
};

/*
* ����� ��� ���� String
*/
class CStringVariant : public CVariant {
	using CVariant::CVariant;
public:
	string m_val;
	CStringVariant(string val);
	~CStringVariant();
};

/*
* ����� ��� ���� Char
*/
class CCharVariant : public CVariant {
	using CVariant::CVariant;
public:
	char m_val;
	CCharVariant(char val);
	~CCharVariant();
};