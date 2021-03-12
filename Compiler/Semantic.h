#pragma once
#include <list>
#include <map>
#include <string>
#include <vector>
#include <stack>
#include "CTypes.h"

using namespace std;

enum EBlock { CONSTBL, TYPEBL, VARBL, BODYBL };

class CIdetificator {

private:
	string name;
	EBlock block;
public:
	CIdetificator(string name, EBlock block);
	CIdetificator(string name);
	~CIdetificator();
	string getName() const { return name; }
	EBlock getBlock() const { return block; }
};
class CIdentTable {
private:
	struct identcomp {
		bool operator()(const CIdetificator ident, const CIdetificator ident2) const {
			return ident.getName().compare(ident2.getName());
		}
	};
	map<CIdetificator, CType*, identcomp> identTbl;			// ������� ��������������� <���_�����, ������_����>	

public:
	CIdentTable();
	~CIdentTable();
	void addIdent(string name, EBlock block);
	CType* findTypeByIdent(string name);
};

class CScope {
private:
	CScope* outerScope;			// ������� ������� ��������
	CIdentTable identTbl;		// ������� ���������������
	list<CType> typeTbl;		// ������� �����
	CType* findTypeByIdent(string name);		// ���������� ��� �� ��������������
public:
	CScope(CScope* outerScope);
	~CScope();
	void addIdent(string name, EBlock block, string typeName);
	void createType(string typeName);		// CBaseType
	void createType(string typeName, list<string> constants);		// CEnumType
	void createType(string typeName, string elTypeName);	// CSubrangeType
	void createType(string typeName, string elTypeName, string indexTypeName,
		int dimension);	// CArrayType
};
// ���������� ����� ���� �� ���������, ������ ��������������, ����� ��� ��������� � ������ BODY � ����� NULL (������������� ���)

class CSemantic {
private:
	list<CScope> scopesLst;		// ������� ������� �������� ��������� �����

public:
	CSemantic();
	~CSemantic();
	void createFictiveScope();
	void createScope();
};
