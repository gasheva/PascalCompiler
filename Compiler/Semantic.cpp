#include "Semantic.h"
#include <stdexcept>
#include "CVariant.h"
#include <set>

using namespace std;

CSemantic::CSemantic() {}

CSemantic::~CSemantic() {}

void CSemantic::createFictiveScope() {
	// integer, real, string, char - BaseType, true, false - EnumType
	scopesLst.push_back(CScope(nullptr));
	auto fictiveScp = &scopesLst.back();
	scopesLst.push_back(fictiveScp);
	(*fictiveScp).createFictive();
}

CScope::CScope(CScope* outerScopes) {
	this->outerScope = outerScope;
}

CScope::~CScope() {}

CType CScope::defineCompleteType(EType type) {
	switch (type) {
	case eARRAY:
		return CArrayType();
	case eSUBRANGE:
		return CSubrangeType();
	case eENUM:
		return CEnumType();
	default:
		return CNoneType();
	}
}
void CScope::clearTypesBuff() {
	typesBuff.clear();
}
void CScope::clearNamesBuff() {
	namesBuff.clear();
}
void CScope::addToNameBuffer(string name) {
	namesBuff.push_back(name);
}

void CScope::addToBuffer(EType type) {
	typeTbl.push_back(defineCompleteType(type));
}

void CScope::addToBuffer(string type) {
	typeTbl.push_back(*(findType(type, set<EBlock>{CONSTBL, TYPEBL})));			// получаем ссылку на простой или уже созданный тип
	// если в буфере уже имеется тип, значит это дополнение составного типа
	if (typesBuff.size() > 0) {
		CType* type = typesBuff.front();
		auto typeName = (*type).getType();
		switch (typeName) {
		case eARRAY:
			auto arType = (CArrayType*)type;
			arType->getIndexType();		//TODO()
			break;
		case eSUBRANGE:
			break;
		case eENUM:
			break;
		default:
			break;
		}
	}
}

CType* CScope::findType(string name, set<EBlock> block) {
	CType* type = nullptr;
	if (identTbl.find(name) != identTbl.end()) {
		// идентификатор найден
		if (block.find(get<0>(identTbl[name]).getBlock())!=block.end()) {
			return get<1>(identTbl[name]);		// тип найден, возвращаемся
		} else {
			// идентификатор найден, но это не тип
		}

	} else {
		if (outerScope != nullptr)
			type = outerScope->findIdent(name);
		else return nullptr;		// TODO(типа нет)
	}
	return type;
}

void CScope::createFictive() {
	typeTbl.push_back(CBaseType(eINT));
	identTbl["integer"] = std::make_tuple<CIdetificator, CType*>(CIdetificator("integer", TYPEBL), &(typeTbl.back()));

	typeTbl.push_back(CBaseType(eREAL));
	identTbl["real"] = std::make_tuple<CIdetificator, CType*>(CIdetificator("real", TYPEBL), &(typeTbl.back()));

	typeTbl.push_back(CBaseType(eSTRING));
	identTbl["string"] = std::make_tuple<CIdetificator, CType*>(CIdetificator("string", TYPEBL), &(typeTbl.back()));

	typeTbl.push_back(CBaseType(eCHAR));
	identTbl["char"] = std::make_tuple<CIdetificator, CType*>(CIdetificator("char", TYPEBL), &(typeTbl.back()));

	typeTbl.push_back(CEnumType(list<string>{"true", "false"}));
	identTbl["boolean"] = std::make_tuple<CIdetificator, CType*>(CIdetificator("boolean", TYPEBL), &(typeTbl.back()));
}

bool CScope::overrideBaseType(string name) {
	
	return false;
}

CIdetificator::CIdetificator(string name, EBlock block) {}

CIdetificator::CIdetificator(string name) {}

CIdetificator::~CIdetificator() {}
