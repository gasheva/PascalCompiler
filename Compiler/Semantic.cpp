#include "Semantic.h"
#include <stdexcept>
#include <map>
#include <string>
#include <vector>
#include <stack>
#include "CTypes.h"

void CSemantic::createFictiveScope()
{
	// integer, real, string, char - BaseType, true, false - EnumType
	CScope fictiveScp = CScope(nullptr);
	fictiveScp.addSymbols();

}

CType* CScope::findTypeByIdent(string name)
{
	return identTbl.findTypeByIdent(name);
}

CScope::CScope(CScope* outerScopes)
{
	this->identTbl = CIdentTable();
	this->outerScope = outerScope;
}

CType* CIdentTable::findTypeByIdent(string name) throw(out_of_range)
{
	return identTbl.at(name);		// кидает out of range, если объект не найден
}
