#include "Semantic.h"

void CSemantic::createFictiveScope()
{
	CScope fictiveScp = CScope();
	fictiveScp.createSymbolTbl();
	fictiveScp.addSymbols();
}

void CScope::createSymbolTbl() {
	this->identTbl = CSymbolTable();
}
