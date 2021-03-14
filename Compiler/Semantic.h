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
enum EBlock { FICTIVEBL, CONSTBL, TYPEBL, VARBL, BODYBL };

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
	//CSemantic* semantic;
	Lexic* lexic;
	CErrorManager* eManager;

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

	CType* findType(string name, set<EBlock> block);			// ������� ��� �� ������, ���������� �� ���� �������
	void writeMistake(int code);
public:
	CScope(CScope* outerScope, Lexic* lexic, CErrorManager* eManager);
	~CScope();				
	void createFictive();										// �������� ������� ����� ��� ���������� ������
	
	CType defineAndCreateType(EType type);
	void clearTypesBuff();
	void clearNamesBuff();				// ��������� � �� �������������� 
	// ������� ��� ������
	void clearBuffs();				
	void addToNameBuffer(string name);
	void addToBuffer(EType type);		// �������� ������� ������ ����������� ���� (� ����� � � ��) (array, [], ())
	void addToBuffer(string type);		// �������� ������� ������ ����������� ���� (� ����� � � ��) (myType, INTEGER, BOOLEAN)

	void defineConst(EType type, string constName);			// �������� ����������� ���������
	void createNone();

	void setBlock(EBlock block);
};

class CSemantic {
private:
	list<CScope> scopesLst;		// ������� ������� �������� ��������� �����
	CErrorManager* eManager;
	Lexic* lexic;
public:
	CSemantic(CErrorManager* eManager, Lexic* lexic);
	~CSemantic();
	void createFictiveScope();
	void createScope();
	CScope* getLast() { return &scopesLst.back(); }
};

