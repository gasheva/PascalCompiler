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
	map<CIdetificator, CType*, identcomp> identTbl;			// таблица идентификаторов <имя_идент, индекс_типа>	

public:
	CIdentTable();
	~CIdentTable();
	void addIdent(string name, EBlock block);
	CType* findTypeByIdent(string name);
};

class CScope {
private:
	CScope* outerScope;			// внешняя область действия
	CIdentTable identTbl;		// таблица идентификаторов
	list<CType> typeTbl;		// таблица типов
	CType* findTypeByIdent(string name);		// возвращает тип по идентификатору
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
// переменная может быть не объявлена, однако использоваться, тогда она заносится с меткой BODY и типом NULL (универсальный тип)

class CSemantic {
private:
	list<CScope> scopesLst;		// нулевую позицию занимает фиктивный скоуп

public:
	CSemantic();
	~CSemantic();
	void createFictiveScope();
	void createScope();
};
