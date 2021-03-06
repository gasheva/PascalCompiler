#pragma once
#include <map>
#include <string>
#include <vector>

using namespace std;

class CSemantic {
private:
	vector<CScope> scopesLst;		// нулевую позицию занимает фиктивный скоуп
	//map<string, int> identTbl;			// таблица идентификаторов <имя_идент, индекс в ТТ>
	vector<string> typesTbl = {				// таблица типов
		"integer", "real", "boolean", "char", "string", "null"
	};				

public:
	CSemantic();
	~CSemantic();
	void createFictiveScope();
	void createScope();
};

class CScope{
private:
	CScope* outerScope;
	CSymbolTable symbolTbl;
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
	string getName();
	void setName(string name);
	int getTypeInd();
	void setTypeInd(int typeInd);
	EBlock getBlock();
	void setBlock();
};

enum EBlock { VAR, TYPE, CONST, BODY };	
// переменная может быть не объявлена, однако использоваться, тогда она заносится с меткой BODY и типом NULL (универсальный тип)