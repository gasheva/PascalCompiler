#pragma once
#include <map>
#include <string>
#include <vector>
#include "CTypes.h"

using namespace std;

class CSemantic {
private:
	vector<CScope> scopesLst;		// нулевую позицию занимает фиктивный скоуп

public:
	CSemantic();
	~CSemantic();
	void createFictiveScope();
	void createScope();
};

class CScope{
private:
	CScope* outerScope;			// внешняя область действия
	CSymbolTable identTbl;		// таблица идентификаторов
	vector<CType> typeTbl;		// таблица типов
public:
	CScope();
	~CScope();
	void createSymbolTbl();
	void addSymbols();
};
class CSymbolTable{
private:
	map<CIdetificator, bool(*)(int)> identTbl;			// таблица идентификаторов <имя_идент, ограничения>	

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
// переменная может быть не объявлена, однако использоваться, тогда она заносится с меткой BODY и типом NULL (универсальный тип)