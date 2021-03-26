#pragma once
#include <list>
#include <map>
#include <string>
#include <vector>
#include <stack>
#include "CTypes.h"
#include <set>
#include <unordered_map>
#include "CErrorManager.h"
#include "Lexic.h"

using namespace std;

// ���������� ����� ���� �� ���������, ������ ��������������, ����� ��� ��������� � ������ BODY � ����� NULL (������������� ���)
enum EBlock { CONSTBL, TYPEBL, VARBL, BODYBL };

class CIdetificator {
private:
	string name;
	EBlock block;
	CType* type;
public:
	CIdetificator(string name, EBlock block, CType* type);
	CIdetificator();
	~CIdetificator();
	string getName() const { return name; }
	EBlock getBlock() const { return block; }
	CType* getType() { return type; }
};

class CScope {
private:
	CLexic* lexic;
	CErrorManager* eManager;

	list<CType*> typesBuff;		// ����� ����������� ��������� �����
	list<string> namesBuff;		// ����� ���� ���������� ����������
	EBlock flagBlock;			// ����, ���������� �� ������� ����

	CScope* outerScope;			// ������� ������� ��������
	unordered_map<string , CIdetificator> identTbl;			// ������� ���������������
	list<CType> typeTbl;		// ������� �����

	CIdetificator* findType(string name, set<EBlock> block);			// ������� ������������� �� ��������, ���������� �� ���� �������
	void writeMistake(int code);
public:
	CScope(CScope* outerScope, CLexic* lexic, CErrorManager* eManager);
	~CScope();				
	void createFictive();										// �������� ������� ����� ��� ���������� ������
	
	CType makeType(EType type);		// ������� ��� ����\��� �����. ���������� ����� ������ ����� ������
	void clearTypesBuff();				// ������� ������ �����
	void clearNamesBuff();				// ��������� � �� �������������� 
	void clearBuffs();				// ������� ��� ������
	void addToNameBuffer(string name);		// ���������� � ����� ����
	void addToBuffer(string typeName);		// ���������� � ����� �����

	void defineConst(EType type, string constName);			// �������� ����������� ���������
	void createNone();		// �������� ���� none

	void setBlock(EBlock block);		// ���������� �������� �����

	EType defineType(EVarType type, string identName);		// ���������� ������ ��� ���������� �� ����, ������� ��� ������������� �����
	EType checkAssignTypes(string name, EType right);

	EType unionBoolExprTypes(EType left, EType right);
	EType unionTypes(EType left, EType right, string oper);

	list<string> getNamesBuff();
	EType getBuffType();
};

class CSemantic {
private:
	list<CScope> scopesLst;		// ������� ������� �������� ��������� �����
	CErrorManager* eManager;
	CLexic* lexic;
public:
	CSemantic(CErrorManager* eManager, CLexic* lexic);
	~CSemantic();
	void createFictiveScope();
	void createScope();
	CScope* getLast() { return &scopesLst.back(); }

	EType unionBoolExprTypes(EType left, EType right);
	EType unionTypes(EType left, EType right, string oper);
};

