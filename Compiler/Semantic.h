#pragma once
#include <list>
#include <map>
#include <string>
#include <vector>
#include <stack>
#include "CTypes.h"
#include <tuple>

using namespace std;

// переменная может быть не объявлена, однако использоваться, тогда она заносится с меткой BODY и типом NULL (универсальный тип)
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

class CScope {
private:
	list<CType*> typesBuff;		// буфер создаваемых типов
	list<string> namesBuff;		// буфер имен однотипных переменных

	CScope* outerScope;			// внешняя область действия
	struct identcomp {
		bool operator()(const CIdetificator ident, const CIdetificator ident2) const {
			return ident.getName().compare(ident2.getName());
		}
	};
	map<string , tuple<CIdetificator, CType*>, identcomp> identTbl;			// таблица идентификаторов <имя_идент, индекс_типа>	
	list<CType> typeTbl;		// таблица типов


	CType* findIdent(string name, set<EBlock> block);				// возвращает тип
	// CType* getBase(string typeName);			// проверить базовый тип
public:
	CScope(CScope* outerScope);
	~CScope();						
	CType* findType(string name, set<EBlock> block);			// находит тип по строке, проходится по всем скоупам
	void createFictive();										// создание базовых типов для фиктивного скоупа
	
	CType defineCompleteType(EType type);
	void clearTypesBuff();
	void clearNamesBuff();
	void addToNameBuffer(string name);
	void addToBuffer(EType type);		// создание объекта класса переданного типа (в буфер и в ТТ) (array, [], ())
	void addToBuffer(string type);		// создание объекта класса переданного типа (в буфер и в ТТ) (myType, INTEGER, BOOLEAN)
};

class CSemantic {
private:
	list<CScope> scopesLst;		// нулевую позицию занимает фиктивный скоуп
	
public:
	CSemantic();
	~CSemantic();
	void createFictiveScope();
	void createScope();
};
