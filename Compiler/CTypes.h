#pragma once
#include <string>
#include <vector>

using namespace std;

enum EType {
	INT, REAL, STRING, CHAR, BOOLEAN, ARRAY, SUBRANGE, ENUM
};

class CType {
protected:
	EType type;
	CType(EType type);

public:
	~CType();
};

/// <summary>
/// Класс соответствующий простым (базовым) типам
/// </summary>
class CScalarsType : public CType {
public:
	// const int MAXINT = 2147483647;		// TODO(создание модуля проверки соответствия условиями)
	// const int MININT = -2147483648;


};
class CEnumType : public CType {
private:
	vector<string> enumEls;
public:
	CEnumType(vector<string> enumEls);
};
class CArrayType : public CType {
private:
	CType* elType;
	CType* indexType;
public:
	const CType* getElType() { return elType; }
	const CType* getIndexType() { return indexType; }
	CArrayType(CType* elType, CType* indexType);
};
/// <summary>
/// Ограниченный тип
/// </summary>
class CSubrangeType : public CType {
private:
	CType* elType;
public:
	const CType* getElType() { return elType; }
	CSubrangeType(CType* elType);

};
