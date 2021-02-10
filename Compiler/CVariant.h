#pragma once
#include <string>
using namespace std;

/*
* Возможные типы переменных и значений
*/
enum EVarType {
	INT,
	REAL,
	STRING,
	CHAR
};

/*
* Абстрактный родительский класс, кот наследуюется классами типов переменных
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
* Класс для типа Integer
*/
class CIntVariant : public CVariant {
	using CVariant::CVariant;
public:
	int m_val;
	CIntVariant(int val);
	~CIntVariant();
};

/*
* Класс для типа Real
*/
class CRealVariant : public CVariant {
	using CVariant::CVariant;
public:
	float m_val;
	CRealVariant(float val);
	~CRealVariant();
};

/*
* Класс для типа String
*/
class CStringVariant : public CVariant {
	using CVariant::CVariant;
public:
	string m_val;
	CStringVariant(string val);
	~CStringVariant();
};

/*
* Класс для типа Char
*/
class CCharVariant : public CVariant {
	using CVariant::CVariant;
public:
	char m_val;
	CCharVariant(char val);
	~CCharVariant();
};