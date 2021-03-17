#include "Semantic.h"
#include <stdexcept>
#include "CVariant.h"
#include <set>

using namespace std;

CSemantic::CSemantic(CErrorManager* eManager, CLexic* lexic) {
	this->eManager = eManager;
	this->lexic = lexic;
}
CSemantic::~CSemantic() {}

void CSemantic::createFictiveScope() {
	// integer, real, string, char - BaseType, true, false - EnumType
	scopesLst.push_back(CScope(nullptr, this->lexic, this->eManager));
	auto fictiveScp = &scopesLst.back();
	(*fictiveScp).createFictive();
}
void CSemantic::createScope() {
	scopesLst.push_back(CScope(&scopesLst.back(), lexic, eManager));
}
EType CScope::unionTypes(EType left, EType right) {
	if (left == eREAL && (right == eREAL || right == eINT))
		return eREAL;
	if (right == eREAL && (left == eREAL || left == eINT))
		return eREAL;
	if (left == eINT && right == eINT)
		return eINT;
	if ((left == eSTRING || left == eCHAR) && (right == eSTRING || right == eCHAR))
		return eSTRING;
	if (left == eBOOLEAN && right == eBOOLEAN)
		return eBOOLEAN;
	if (left == eNONE || right == eNONE)
		return eNONE;
	writeMistake(328);
	return eNONE;
}
EType CScope::unionTypes(EType left, EType right, string oper) {
	if (left == eNONE || right == eNONE)
		return eNONE;
	if (left == eBOOLEAN || right == eBOOLEAN) {
		writeMistake(1004);
		return eNONE;
	}
	if (oper == "/") {
		if ((right == eREAL || right == eINT) && (left == eREAL || left == eINT))
			return eREAL;
		writeMistake(1004);
		return eNONE;
	}
	if (oper == "*") {
		if ((left == eSTRING || left==eCHAR) && right == eINT || (right == eSTRING || right == eCHAR)&& left == eINT)
			return eSTRING;
		if ((left == eREAL) && (right == eREAL || right == eINT))
			return eREAL;
		if ((right == eREAL) && (left == eREAL || left == eINT))
			return eREAL;
		if (right == eINT && left == eINT)
			return eINT;
		writeMistake(1004);
		return eNONE;
	}
	if (oper == "+" || oper == "-") {
		if (oper == "+" && (left == eSTRING || left == eCHAR) && (right == eSTRING || right == eCHAR))
			return eSTRING;
		if ((left == eREAL) && (right == eREAL || right == eINT))
			return eREAL;
		if ((right == eREAL) && (left == eREAL || left == eINT))
			return eREAL;
		if (right == eINT && left == eINT)
			return eINT;
		writeMistake(1004);
		return eNONE;
	}
	if (oper == "div" || oper == "mod") {
		if ((left == eREAL) && (right == eREAL || right == eINT))
			return eREAL;
		if ((right == eREAL) && (left == eREAL || left == eINT))
			return eREAL;
		if (right == eINT && left == eINT)
			return eINT;
		writeMistake(1004);
		return eNONE;
	}


}
EType CSemantic::unionTypes(EType left, EType right) {
	return scopesLst.back().unionTypes(left, right);
}

EType CSemantic::unionTypes(EType left, EType right, string oper) {
	return scopesLst.back().unionTypes(left, right, oper);
}

CScope::CScope(CScope* outerScope, CLexic* lexic, CErrorManager* eManager) {
	this->outerScope = outerScope;
	this->lexic = lexic;
	this->eManager = eManager;
}

CScope::~CScope() {}

CType CScope::makeType(EType type) {
	switch (type) {
	case eINT: case eREAL: case eSTRING: case eCHAR: 
		return CBaseType(type);
	default:
		return CNoneType();
	}
}
EType CScope::defineType(EVarType type, string identName) {
	// попытка найти и определить тип идентификатора
	if (identName != "") {
		auto typePtr = findType(identName, set<EBlock>{CONSTBL, VARBL, BODYBL});
		if (typePtr == nullptr) {
			writeMistake(1002);
			typeTbl.push_back(CNoneType());
			identTbl.insert({ identName, CIdetificator(identName, BODYBL, &typeTbl.back()) });
			return eNONE;
		}
		else return typePtr->getType()->getType();
	}
	switch (type) {
	case INT:
		return eINT;
	case REAL:
		return eREAL;
	case STRING:
		return eSTRING;
	case CHAR:
		return eCHAR;
	default:
		return eNONE;
	}
}

void CScope::checkAssignTypes(string name, EType right) {
	auto leftPtr = findType(name, set<EBlock>{CONSTBL, VARBL, BODYBL});
	if (leftPtr == nullptr) {
		writeMistake(1002);
		typeTbl.push_back(CNoneType());
		identTbl.insert({ name, CIdetificator(name, BODYBL, &typeTbl.back()) });
	} else if (leftPtr->getType()->getType()==eNONE || right == eNONE) {
		return;
	} else if (leftPtr->getBlock() == CONSTBL) {
		writeMistake(1003);
	} else {
		if (leftPtr->getType()->getType() == eREAL && (right == eREAL || right == eINT))
			return;
		if (leftPtr->getType()->getType() == eINT && right == eINT)
			return;
		if (leftPtr->getType()->getType() == eSTRING && (right == eSTRING || right == eCHAR))
			return;
		if (leftPtr->getType()->getType() == eCHAR && right == eCHAR)
			return;
		if (leftPtr->getType()->getType() == eBOOLEAN && right == eBOOLEAN)
			return;
		writeMistake(328);
	}
}

void CScope::clearTypesBuff() {
	typesBuff.clear();
}

void CScope::clearNamesBuff() {
	CType* none = nullptr;
	// если произошла кака€-то ошибка в синтаксисе и не был передан тип
	if (namesBuff.empty()) return;
	if (typesBuff.empty()) {
		typeTbl.push_back(CNoneType());
		for (auto name : namesBuff) {
			identTbl.insert({ name, CIdetificator(name, flagBlock, &typeTbl.back()) });
		}
		namesBuff.clear();
		return;
	}
	// если хот€ бы один из типов не корректен, то и тип, включающий его также некорректен
	for (auto type : typesBuff) {
		if (type->getType() == eNONE) {
			none = type;
			break;
		}
	}
	if (none!=nullptr) {
		for (auto name : namesBuff) {
			// если уже объвлена, то кидаем ошибку и присваиваем тип NONE
			if (identTbl.find(namesBuff.front()) != identTbl.end()) {
				writeMistake(101);
				identTbl.find(name)->second = CIdetificator(name, flagBlock, none);
			}
			else identTbl.insert({ name, CIdetificator(name, flagBlock, none) });		// создаем новый идентификатор типа eNONE
		}
	}
	else
		// если тип определен корректно, то присваиваем его всем переменным
		for (auto name: namesBuff) {
			if (identTbl.find(name) != identTbl.end()) {
				// если уже объвлена, то кидаем ошибку и присваиваем тип NONE
				writeMistake(101);
				typeTbl.push_back(CNoneType());
				identTbl.find(name)->second = CIdetificator(name, flagBlock, &typeTbl.back());
			} else
				identTbl.insert({ name, CIdetificator(name, flagBlock, typesBuff.front()) });
		}
	namesBuff.clear();
}
void CScope::addToNameBuffer(string name) {
	namesBuff.push_back(name);
}

void CScope::defineConst(EType type, string constRight) {
	// создаем объ€вление константы
	// если справа идентификатор, ищем его среди объ€вленных констант
	if (type == eNONE) {
		auto type = findType(constRight, set<EBlock>{CONSTBL, VARBL});
		// если переменна€ не найдена
		if (type == nullptr) {
			writeMistake(1002);
		} else {
			typesBuff.push_back(type->getType());
		}
	} else {
		// создаем безым€нный тип дл€ строки, числа и тд
		typeTbl.push_back(makeType(type));
		auto type = (&typeTbl.back());
		typesBuff.push_back(type);
	}
}
void CScope::createNone() {
	typeTbl.push_back(CNoneType());
	typesBuff.push_back(&typeTbl.back());
}

void CScope::addToBuffer(string typeName) {
	auto type = findType(typeName, set<EBlock>{TYPEBL});
	// если тип не найден
	if (type == nullptr) {
		writeMistake(1002);
	} else {
		typesBuff.push_back(type->getType());
	}
}

void CScope::clearBuffs() {
	clearNamesBuff();
	clearTypesBuff();
}

void CScope::setBlock(EBlock block) {
	flagBlock = block;
}

CIdetificator* CScope::findType(string name, const set<EBlock> block) {
	CIdetificator* type = nullptr;
	if (identTbl.find(name) != identTbl.end()) {
		// идентификатор найден
		if (block.find((identTbl[name]).getBlock()) != block.end()) {
			return (&identTbl[name]);		// тип найден, возвращаемс€
		} else {
			// идентификатор найден, но это не того типа
			return nullptr;
		}
	} else {
		if (outerScope != nullptr)
			type = outerScope->findType(name, block);
		else return nullptr;
	}
	return type;
}

void CScope::writeMistake(int code) {
	cout << "Mistake " << code << ": " << lexic->getStartPosition() << endl;
	eManager->addError(lexic->getStartPosition(), lexic->getCurLine(), code);
}

void CScope::createFictive() {
	typeTbl.push_back(CBaseType(eINT));
	identTbl.insert({ "integer", CIdetificator("integer", TYPEBL, &(typeTbl.back())) });

	typeTbl.push_back(CBaseType(eREAL));
	identTbl.insert(pair<string, CIdetificator>("real", CIdetificator("real", TYPEBL, &(typeTbl.back()))));

	typeTbl.push_back(CBaseType(eSTRING));
	identTbl.insert(pair<string, CIdetificator>("string", CIdetificator("string", TYPEBL, &(typeTbl.back()))));

	typeTbl.push_back(CBaseType(eCHAR));
	identTbl.insert(pair<string, CIdetificator>("char", CIdetificator("char", TYPEBL, &(typeTbl.back()))));

	// boolean
	typeTbl.push_back(CEnumType());
	auto boolPtr = &(typeTbl.back());
	identTbl.insert({ "boolean", CIdetificator("boolean", TYPEBL, boolPtr) });

	typeTbl.push_back(CEnumElType());
	identTbl.insert({ "true", CIdetificator("true", VARBL, &(typeTbl.back())) });
	typeTbl.push_back(CEnumElType());
	identTbl.insert({ "false", CIdetificator("false", VARBL, &(typeTbl.back())) });

	// создание предопределенных констант
	typeTbl.push_back(CBaseType(eINT));
	identTbl.insert({ "maxint", CIdetificator("maxint", VARBL, &(typeTbl.back())) });
	typeTbl.push_back(CBaseType(eINT));
	identTbl.insert({ "minint", CIdetificator("minint", VARBL, &(typeTbl.back())) });
	typeTbl.push_back(CBaseType(eREAL));
	identTbl.insert({ "pi", CIdetificator("pi", VARBL, &(typeTbl.back())) });
}

CIdetificator::CIdetificator(string name, EBlock block, CType* type) {
	this->name = name;
	this->block = block;
	this->type = type;
}
CIdetificator::CIdetificator() {}
CIdetificator::~CIdetificator() {}
