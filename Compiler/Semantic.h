#pragma once
#include <map>
#include <string>
#include <vector>
#include "CTypes.h"

using namespace std;

class CSemantic {
private:
	vector<CScope> scopesLst;		// ������� ������� �������� ��������� �����

public:
	CSemantic();
	~CSemantic();
	void createFictiveScope();
	void createScope();
};

class CScope{
private:
	CScope* outerScope;			// ������� ������� ��������
	CSymbolTable identTbl;		// ������� ���������������
	vector<CType> typeTbl;		// ������� �����
public:
	CScope();
	~CScope();
	void createSymbolTbl();
	void addSymbols();
};
class CSymbolTable{
private:
	map<CIdetificator, bool(*)(int)> identTbl;			// ������� ��������������� <���_�����, �����������>	

public:
	CSymbolTable();
	~CSymbolTable();
	void addSymbol();
};

class CIdetificator {
private:
	string name;
	int typeInd;
	EBlock block;
public:
	CIdetificator(string name, EBlock block);
	string getName() const { return name; }
	int getTypeInd() const { return typeInd; }
	void setTypeInd(int typeInd);
	EBlock getBlock() const { return block; }
};

enum EBlock { CONSTBL, TYPEBL, VARBL, BODYBL };
// ���������� ����� ���� �� ���������, ������ ��������������, ����� ��� ��������� � ������ BODY � ����� NULL (������������� ���)