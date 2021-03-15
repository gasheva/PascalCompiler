#include "Semantic.h"
#include <stdexcept>
#include "CVariant.h"
#include <set>

using namespace std;

CSemantic::CSemantic(CErrorManager* eManager, Lexic* lexic) {
	this->eManager = eManager;
	this->lexic = lexic;
}

CSemantic::~CSemantic() {}

void CSemantic::createFictiveScope() {
	// integer, real, string, char - BaseType, true, false - EnumType
	//scopesLst.push_back(CScope(nullptr, this));
	scopesLst.push_back(CScope(nullptr, this->lexic, this->eManager));
	auto fictiveScp = &scopesLst.back();
	(*fictiveScp).createFictive();
}
void CSemantic::createScope() {
	scopesLst.push_back(CScope(&scopesLst.back(), lexic, eManager));
}

EType CSemantic::unionTypes(EType left, EType right) {
	if (left == eREAL && (right == eREAL || right == eINT))
		return eREAL;
	if (right == eREAL && (left == eREAL || left == eINT))
		return eREAL;
	if ((left == eSTRING || left == eCHAR) && (right == eSTRING || right == eCHAR))
		return eSTRING;
	if (left == eBOOLEAN && right == eBOOLEAN)
		return eBOOLEAN;
	return EType();
}

CScope::CScope(CScope* outerScope, Lexic* lexic, CErrorManager* eManager) {
	this->outerScope = outerScope;
	this->lexic = lexic;
	this->eManager = eManager;
}

CScope::~CScope() {}

CType CScope::defineAndCreateType(EType type) {
	switch (type) {
	case eINT: case eREAL: case eSTRING: case eCHAR:
		return CBaseType(type);
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
EType CScope::defineType(EVarType type, string identName) {
	// ������� ����� � ���������� ��� ��������������
	if (identName != "") {
		auto typePtr = findType(identName, set<EBlock>{CONSTBL, VARBL, BODYBL});
		if (typePtr == nullptr) {
			writeMistake(1002);
			typeTbl.push_back(CNoneType());
			identTbl.insert({ identName, CIdetificator(identName, BODYBL, &typeTbl.back()) });
			return eNONE;
		}
		else return typePtr->getType();
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
	auto leftPtr = findType(name, set<EBlock>(VARBL, BODYBL));
	if (leftPtr == nullptr) {
		writeMistake(1002);
		typeTbl.push_back(CNoneType());
		identTbl.insert({ name, CIdetificator(name, BODYBL, &typeTbl.back()) });
	} else {
		if (leftPtr->getType() == eREAL && (right == eREAL || right == eINT))
			return;
		if (leftPtr->getType() == eINT && right == eINT)
			return;
		if (leftPtr->getType() == eSTRING && (right == eSTRING || right == eCHAR))
			return;
		if (leftPtr->getType() == eCHAR && right == eCHAR)
			return;
		if (leftPtr->getType() == eBOOLEAN && right == eBOOLEAN)
			return;
		writeMistake(328);
	}
}

void CScope::clearTypesBuff() {
	typesBuff.clear();
}

void CScope::clearNamesBuff() {
	CType* none = nullptr;
	// ���� ��������� �����-�� ������ � ���������� � �� ��� ������� ���
	if (namesBuff.empty()) return;
	if (typesBuff.empty()) {
		typeTbl.push_back(CNoneType());
		for (auto name : namesBuff) {
			identTbl.insert({ name, CIdetificator(name, flagBlock, &typeTbl.back()) });
		}
		namesBuff.clear();
		return;
	}
	// ���� ���� �� ���� �� ����� �� ���������, �� � ���, ���������� ��� ����� �����������
	for (auto type : typesBuff) {
		if (type->getType() == eNONE) {
			none = type;
			break;
		}
	}
	if (none!=nullptr) {
		for (auto name : namesBuff) {
			// ���� ��� ��������, �� ������ ������ � ����������� ��� NONE
			if (identTbl.find(namesBuff.front()) != identTbl.end())
				writeMistake(101);
			// identTbl.insert({ name, CIdetificator(name, flagBlock, none) });
			identTbl.find(name)->second = CIdetificator(name, flagBlock, &typeTbl.back());
		}
	}
	else
		// ���� ��� ��������� ���������, �� ����������� ��� ���� ����������
		for (auto name: namesBuff) {
			if (identTbl.find(name) != identTbl.end()) {
				// ���� ��� ��������, �� ������ ������ � ����������� ��� NONE
				writeMistake(101);
				typeTbl.push_back(CNoneType());
				// identTbl.insert({ name, CIdetificator(name, flagBlock, &typeTbl.back()) });
				//identTbl.emplace(name, CIdetificator(name, flagBlock, &typeTbl.back()));
				//identTbl.insert({ name, CIdetificator(name, flagBlock, &typeTbl.back()) });
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
	// ������� ���������� ���������
	// ���� ������ �������������, ���� ��� ����� ����������� ��������
	if (type == eNONE) {
		auto type = findType(constRight, set<EBlock>{CONSTBL});
		// ���� ���������� �� �������
		if (type == nullptr) {
			writeMistake(1002);
		} else {
			typesBuff.push_back(type);
		}
	} else {
		// ������� ���������� ��� ��� ������, ����� � ��
		typeTbl.push_back(defineAndCreateType(type));
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
	// ���� ��� �� ������
	if (type == nullptr) {
		writeMistake(1002);
	} else {
		typesBuff.push_back(type);
	}
}

void CScope::clearBuffs() {
	clearNamesBuff();
	clearTypesBuff();
}

void CScope::setBlock(EBlock block) {
	flagBlock = block;
}

CType* CScope::findType(string name, const set<EBlock> block) {
	CType* type = nullptr;
	if (identTbl.find(name) != identTbl.end()) {
		// ������������� ������
		if (block.find((identTbl[name]).getBlock()) != block.end()) {
			return (identTbl[name].getType());		// ��� ������, ������������
		} else {
			// ������������� ������, �� ��� �� ���� ����
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

	// �������� ���������������� ��������
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
