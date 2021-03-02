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
	BOOLEAN,
	CHAR
};

/*
* Абстрактный родительский класс, кот наследуюется классами типов переменных
*/
class CVariant {
public:
	//R m_val;
	CVariant();
	virtual ~CVariant();
	EVarType getType();
protected:
	EVarType T;
	CVariant(EVarType type);
};

/*
* Класс для типа Integer
*/
class CIntVariant : public CVariant {
	using CVariant::CVariant;
public:
	CIntVariant(int val);
	~CIntVariant();
	int getVal() { return m_val; }
private:
	int m_val;
};

/*
* Класс для типа Real
*/
class CRealVariant : public CVariant {
	using CVariant::CVariant;
public:
	CRealVariant(float val);
	~CRealVariant();
	float getVal() { return m_val; }
private:
	float m_val;
};

/*
* Класс для типа String
*/
class CStringVariant : public CVariant {
	using CVariant::CVariant;
public:
	CStringVariant(string val);
	~CStringVariant();
	string getVal() { return m_val; }
private:
	string m_val;
};

/*
* Класс для типа Char
*/
class CCharVariant : public CVariant {
	using CVariant::CVariant;
public:
	CCharVariant(char val);
	~CCharVariant();
	char getVal() { return m_val; }
private:
	char m_val;
};

/*
* Класс для типа Boolean
*/
class CBooleanVariant : public CVariant {
	using CVariant::CVariant;
public:
	CBooleanVariant(bool val);
	~CBooleanVariant();
	char getVal() { return m_val; }
private:
	bool m_val;
};