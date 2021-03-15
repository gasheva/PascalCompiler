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

// переменна€ может быть не объ€влена, однако использоватьс€, тогда она заноситс€ с меткой BODY и типом NULL (универсальный тип)
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

	CScope* outerScope;			// внешн€€ область действи€
	struct identcomp {
		bool operator()(const CIdetificator ident, const CIdetificator ident2) const {
			return ident.getName().compare(ident2.getName());
		}
	};
	unordered_map<string , CIdetificator> identTbl;			// таблица идентификаторов <им€_идент, индекс_типа>	
	list<CType> typeTbl;		// таблица типов

	CType* findType(string name, set<EBlock> block);			// находит тип по строке, проходитс€ по всем скоупам
	void writeMistake(int code);
public:
	CScope(CScope* outerScope, Lexic* lexic, CErrorManager* eManager);
	~CScope();				
	void createFictive();										// создание базовых типов дл€ фиктивного скоупа
	
	CType defineAndCreateType(EType type);
	void clearTypesBuff();
	void clearNamesBuff();				// добавл€ет в “» идентификаторы 
	// очищает оба буфера
	void clearBuffs();				
	void addToNameBuffer(string name);
	void addToBuffer(string typeName);

	void defineConst(EType type, string constName);			// создание определени€ константы
	void createNone();

	void setBlock(EBlock block);

	EType defineType(EVarType type, string identName);		// определ€ет семантический тип переменной из синтаксического, также находит тип идентификатора
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

