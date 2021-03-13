#pragma once
#include <list>
#include <map>
#include <string>
#include <vector>
#include <stack>
#include "CTypes.h"
#include <tuple>

using namespace std;

// ���������� ����� ���� �� ���������, ������ ��������������, ����� ��� ��������� � ������ BODY � ����� NULL (������������� ���)
enum EBlock { FICTIVEBL, CONSTBL, TYPEBL, VARBL, BODYBL };

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
	map<string , tuple<CIdetificator, CType*>, identcomp> identTbl;			// ������� ��������������� <���_�����, ������_����>	
	list<CType> typeTbl;		// ������� �����


	CType* findIdent(string name, set<EBlock> block);				// ���������� ���
	// CType* getBase(string typeName);			// ��������� ������� ���
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
