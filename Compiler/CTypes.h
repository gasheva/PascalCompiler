#pragma once
#include <string>
#include <vector>

using namespace std;

enum EType {
	INT, REAL, STRING, CHAR, BOOLEAN, ARRAY, SUBRANGE, ENUM, NONE
};

class CType {
protected:
	EType type;
	CType(EType type);

public:
	~CType();
};
class CNoneType : public CType {
public:
	CNoneType();
};

/// <summary>
/// ����� ��������������� ������� (�������) �����
/// </summary>
class CBaseType : public CType {
public:
	CBaseType();
	// const int MAXINT = 2147483647;		// TODO(�������� ������ �������� ������������ ���������)
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
	int dimension;				// ����������� �������
public:
	const CType* getElType() { return elType; }
	const CType* getIndexType() { return indexType; }
	int getDimension() const { return dimension; }
	CArrayType(CType* elType, CType* indexType);
};
/// <summary>
/// ������������ ���
/// </summary>
class CSubrangeType : public CType {
private:
	CType* elType;
public:
	const CType* getElType() { return elType; }
	CSubrangeType(CType* elType);

};
