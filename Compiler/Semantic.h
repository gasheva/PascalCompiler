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

// переменная может быть не объявлена, однако использоваться, тогда она заносится с меткой BODY и типом NULL (универсальный тип)
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

	list<CType*> typesBuff;		// буфер создаваемых составных типов
	list<string> namesBuff;		// буфер имен однотипных переменных
	EBlock flagBlock;			// флаг, отвечающий за текущий блок

	CScope* outerScope;			// внешняя область действия
	struct identcomp {
		bool operator()(const CIdetificator ident, const CIdetificator ident2) const {
			return ident.getName().compare(ident2.getName());
		}
	};
	unordered_map<string , CIdetificator> identTbl;			// таблица идентификаторов <имя_идент, индекс_типа>	
	list<CType> typeTbl;		// таблица типов

	CType* findType(string name, set<EBlock> block);			// находит тип по строке, проходится по всем скоупам
	void writeMistake(int code);
public:
	CScope(CScope* outerScope, Lexic* lexic, CErrorManager* eManager);
	~CScope();				
	void createFictive();										// создание базовых типов для фиктивного скоупа
	
	CType defineAndCreateType(EType type);
	void clearTypesBuff();
	void clearNamesBuff();				// добавляет в ТИ идентификаторы 
	// очищает оба буфера
	void clearBuffs();				
	void addToNameBuffer(string name);
	void addToBuffer(EType type);		// создание объекта класса переданного типа (в буфер и в ТТ) (array, [], ())
	void addToBuffer(string type);		// создание объекта класса переданного типа (в буфер и в ТТ) (myType, INTEGER, BOOLEAN)

	void defineConst(EType type, string constName);			// создание определения константы
	void createNone();

	void setBlock(EBlock block);
};

class CSemantic {
private:
	list<CScope> scopesLst;		// нулевую позицию занимает фиктивный скоуп
	CErrorManager* eManager;
	Lexic* lexic;
public:
	CSemantic(CErrorManager* eManager, Lexic* lexic);
	~CSemantic();
	void createFictiveScope();
	void createScope();
	CScope* getLast() { return &scopesLst.back(); }
};

