#pragma once


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
	CIdentTable identTbl;		// таблица идентификаторов
	vector<CType> typeTbl;		// таблица типов
	CType* findTypeByIdent(string name);		// возвращает тип по идентификатору
public:
	CScope(CScope* outerScope);
	~CScope();
	void addSymbol(string name, EBlock block);
	void createType(string typeName);
};
class CIdentTable{
private:
	struct identcomp {
		bool operator()(const CIdetificator ident, const string str) const {
			return ident.getName().compare(str);
		}
	};
	map<CIdetificator, CType*, identcomp> identTbl;			// таблица идентификаторов <имя_идент, индекс_типа>	

public:
	CIdentTable();
	~CIdentTable();
	void addSymbol(string name, EBlock block);
	CType* findTypeByIdent(string name);
};

class CIdetificator {
	
private:
	string name;
	EBlock block;
public:
	CIdetificator(string name, EBlock block);
	string getName() const { return name; }
	EBlock getBlock() const { return block; }
};

enum EBlock { CONSTBL, TYPEBL, VARBL, BODYBL };
// переменная может быть не объявлена, однако использоваться, тогда она заносится с меткой BODY и типом NULL (универсальный тип)