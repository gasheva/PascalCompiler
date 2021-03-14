#pragma once
#include <string>
#include <list>

using namespace std;

enum EType {
	eINT, eREAL, eSTRING, eCHAR, eARRAY, eSUBRANGE, eENUM, eENUMEL, eNONE
};

class CType {
protected:
	EType type;
	CType(EType type);

public:
	const EType getType() { return type; }
	~CType();
};
class CNoneType : public CType {
	using CType::CType;
public:
	CNoneType();
	~CNoneType();
};

class CEnumType;

class CEnumElType : public CType {
	using CType::CType;
private:
	CEnumType* parent;
public:
	CEnumElType();
	CEnumElType(CEnumType* parent);
	~CEnumElType();
	void setParent(CEnumType* parent);
	CEnumType* getParent() { return parent; };
};

/// <summary>
/// Класс соответствующий простым (базовым) типам
/// </summary>
class CBaseType : public CType {
	using CType::CType;
public:
	CBaseType(EType type);
	~CBaseType();
	// const int MAXINT = 2147483647;		// TODO(создание модуля проверки соответствия условиями)
	// const int MININT = -2147483648;
};
class CEnumType : public CType {
	using CType::CType;
private:
	list<CEnumElType*> els;			// список констант
public:
	CEnumType();
	~CEnumType();
	void addEl(CEnumElType* el);
};
class CArrayType : public CType {
	using CType::CType;
private:
	CType* elType;
	CType* indexType;
	int dimension;				// размерность массива
public:
	const CType* getElType() { return elType; }
	const CType* getIndexType() { return indexType; }
	int getDimension() const { return dimension; }
	CArrayType(CType* elType, CType* indexType);
	CArrayType();
	~CArrayType();
};
/// <summary>
/// Ограниченный тип
/// </summary>
class CSubrangeType : public CType {
	using CType::CType;
private:
	CType* elType;
	list<string> childEls;
public:
	CSubrangeType(CType* elType);
	CSubrangeType();
	~CSubrangeType();

	void addEl(string el);					// хранит цифры, строки, имена в string
	const CType* getElType() { return elType; }
	bool setElType(CType* type);			// кидает ошибку, если тип не совпал
	
	string getStart();
	string getEnd();
};
