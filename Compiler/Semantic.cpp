#include "Semantic.h"
#include <stdexcept>

using namespace std;

CSemantic::CSemantic() {}

CSemantic::~CSemantic() {}

void CSemantic::createFictiveScope() {
	// integer, real, string, char - BaseType, true, false - EnumType
	CScope fictiveScp = CScope(nullptr);
	scopesLst.push_back(fictiveScp);
	fictiveScp.createType("integer");
	fictiveScp.addIdent("integer", CONSTBL, "integer");
	fictiveScp.createType("string");
	fictiveScp.addIdent("string", CONSTBL, "string");
	fictiveScp.createType("char");
	fictiveScp.addIdent("char", CONSTBL, "char");
	fictiveScp.createType("real");
	fictiveScp.addIdent("real", CONSTBL, "real");
	list<string> trueFalse("true", "false");
	fictiveScp.createType("bool", trueFalse);
	fictiveScp.addIdent("bool", CONSTBL, "bool");
}

CScope::CScope(CScope* outerScopes) {
	this->identTbl = CIdentTable();
	this->outerScope = outerScope;
}

CScope::~CScope() {}

void CScope::addIdent(string name, EBlock block, string typeName) {

	//identTbl.addIdent
}

void CScope::createType(string typeName) {}

void CScope::createType(string typeName, list<string> constants) {}

void CScope::createType(string typeName, string elTypeName) {}

void CScope::createType(string typeName, string elTypeName, string indexTypeName, int dimension) {}

CIdentTable::CIdentTable() {}

CIdentTable::~CIdentTable() {}

CType* CIdentTable::findTypeByIdent(string name) throw(out_of_range) {
	return identTbl.at(CIdetificator(name));		// кидает out of range, если объект не найден
}

CIdetificator::CIdetificator(string name, EBlock block) {}

CIdetificator::CIdetificator(string name) {}

CIdetificator::~CIdetificator() {}
