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

	list<CType*> typesBuff;		// буфер создаваемых составных типов
	list<string> namesBuff;		// буфер имен однотипных переменных
	EBlock flagBlock;			// флаг, отвечающий за текущий блок

	CScope* outerScope;			// внешняя область действия
	unordered_map<string , CIdetificator> identTbl;			// таблица идентификаторов
	list<CType> typeTbl;		// таблица типов

	CIdetificator* findType(string name, set<EBlock> block);			// находит идентификатор по названию, проходится по всем скоупам
	void writeMistake(int code);
public:
	CScope(CScope* outerScope, CLexic* lexic, CErrorManager* eManager);
	~CScope();				
	void createFictive();										// создание базовых типов для фиктивного скоупа
	
	CType makeType(EType type);		// адаптер для синт\сем типов. Возвращает новый объект соотв класса
	void clearTypesBuff();				// очистка буфера типов
	void clearNamesBuff();				// добавляет в ТИ идентификаторы 
	void clearBuffs();				// очищает оба буфера
	void addToNameBuffer(string name);		// добавление в буфер имен
	void addToBuffer(string typeName);		// добавление в буфер типов

	void defineConst(EType type, string constName);			// создание определения константы
	void createNone();		// создание типа none

	void setBlock(EBlock block);		// назначение текущего блока

	EType defineType(EVarType type, string identName);		// определяет семант тип переменной из синт, находит тип передаваемого идент
	EType checkAssignTypes(string name, EType right);

	EType unionBoolExprTypes(EType left, EType right);
	EType unionTypes(EType left, EType right, string oper);

	list<string> getNamesBuff();
	EType getBuffType();
};

class CSemantic {
private:
	list<CScope> scopesLst;		// нулевую позицию занимает фиктивный скоуп
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

