#pragma once
#include <string>
#include <vector>

using namespace std;

enum EType {
	eINT, eREAL, eSTRING, eCHAR, eBOOLEAN, eARRAY, eSUBRANGE, eENUM, eNONE
};

class CType {
protected:
	EType type;
	CType(EType type);

public:
	~CType();
};
class CNoneType : public CType {
	using CType::CType;
public:
	CNoneType();
	~CNoneType();
};

/// <summary>
/// Класс соответствующий простым (базовым) типам
/// </summary>
class CBaseType : public CType {
	using CType::CType;
public:
	CBaseType();
	~CBaseType();
	// const int MAXINT = 2147483647;		// TODO(создание модуля проверки соответствия условиями)
	// const int MININT = -2147483648;
};
class CEnumType : public CType {
	using CType::CType;
private:
	vector<string> constants;			// список констант
public:
	CEnumType(vector<string> constants);
	~CEnumType();
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
	~CArrayType();
};
/// <summary>
/// Ограниченный тип
/// </summary>
class CSubrangeType : public CType {
	using CType::CType;
private:
	CType* elType;
public:
	const CType* getElType() { return elType; }
	CSubrangeType(CType* elType);
	~CSubrangeType();

};
