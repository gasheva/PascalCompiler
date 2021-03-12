#pragma once
#include <string>
#include <list>

using namespace std;

enum EType {
	eINT, eREAL, eSTRING, eCHAR, eBOOLEAN, eARRAY, eSUBRANGE, eENUM, eNONE
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

/// <summary>
/// ����� ��������������� ������� (�������) �����
/// </summary>
class CBaseType : public CType {
	using CType::CType;
public:
	CBaseType();
	CBaseType(EType type);
	~CBaseType();
	// const int MAXINT = 2147483647;		// TODO(�������� ������ �������� ������������ ���������)
	// const int MININT = -2147483648;
};
class CEnumType : public CType {
	using CType::CType;
private:
	list<string> constants;			// ������ ��������
public:
	CEnumType(list<string> constants);
	CEnumType();
	~CEnumType();
};
class CArrayType : public CType {
	using CType::CType;
private:
	CType* elType;
	CType* indexType;
	int dimension;				// ����������� �������
public:
	const CType* getElType() { return elType; }
	const CType* getIndexType() { return indexType; }
	int getDimension() const { return dimension; }
	CArrayType(CType* elType, CType* indexType);
	CArrayType();
	~CArrayType();
};
/// <summary>
/// ������������ ���
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

	void addEl(string el);					// ������ �����, ������, ����� � string
	const CType* getElType() { return elType; }
	bool setElType(CType* type);			// ������ ������, ���� ��� �� ������
	
	string getStart();
	string getEnd();
};
