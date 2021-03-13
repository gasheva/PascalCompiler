#pragma once
#include <list>
#include <map>
#include <string>
#include <vector>
#include <stack>
#include "CTypes.h"
#include <set>
#include <unordered_map>

using namespace std;

// ���������� ����� ���� �� ���������, ������ ��������������, ����� ��� ��������� � ������ BODY � ����� NULL (������������� ���)
enum EBlock { FICTIVEBL, CONSTBL, TYPEBL, VARBL, BODYBL };

class CIdetificator {
private:
	string name;
	EBlock block;
	CType* type;
public:
	CIdetificator(string name, EBlock block, CType* type);
	CIdetificator(string name, EBlock block);
	CIdetificator(string name);
	CIdetificator();
	~CIdetificator();
	string getName() const { return name; }
	EBlock getBlock() const { return block; }
	CType* getType() { return type; }
};

class CScope {
private:
	list<CType*> typesBuff;		// ����� ����������� ��������� �����
	list<string> namesBuff;		// ����� ���� ���������� ����������
	EBlock flagBlock;			// ����, ���������� �� ������� ����

	CScope* outerScope;			// ������� ������� ��������
	struct identcomp {
		bool operator()(const CIdetificator ident, const CIdetificator ident2) const {
			return ident.getName().compare(ident2.getName());
		}
	};
	unordered_map<string , CIdetificator> identTbl;			// ������� ��������������� <���_�����, ������_����>	
	list<CType> typeTbl;		// ������� �����

public:
	CScope(CScope* outerScope);
	~CScope();						
	CType* findType(string name, set<EBlock> block);			// ������� ��� �� ������, ���������� �� ���� �������
	void createFictive();										// �������� ������� ����� ��� ���������� ������
	
	CType defineAndCreateType(EType type);
	void clearTypesBuff();
	void clearNamesBuff();
	void addToNameBuffer(string name);
	void addToBuffer(EType type);		// �������� ������� ������ ����������� ���� (� ����� � � ��) (array, [], ())
	void addToBuffer(string type);		// �������� ������� ������ ����������� ���� (� ����� � � ��) (myType, INTEGER, BOOLEAN)

	void defineConst(EType type, string constName);			// �������� ����������� ���������

	void setBlock(EBlock block);
};

class CSemantic {
private:
	list<CScope> scopesLst;		// ������� ������� �������� ��������� �����
	
public:
	CSemantic();
	~CSemantic();
	void createFictiveScope();
	void createScope();
};
