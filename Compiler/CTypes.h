#pragma once
#include <string>
#include <list>

using namespace std;

enum EType {
	eINT, eREAL, eSTRING, eCHAR, eBOOLEAN, eNONE
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
