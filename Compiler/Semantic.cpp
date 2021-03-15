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
void CScope::clearTypesBuff() {
	typesBuff.clear();
}
void CScope::clearNamesBuff() {
	CType* none = nullptr;
	// ���� ��������� �����-�� ������ � ���������� � �� ��� ������� ���
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
			identTbl.insert({ name, CIdetificator(name, flagBlock, none) });
		}
	}
	else
		// ���� ��� ��������� ���������, �� ����������� ��� ���� ����������
		for (auto name: namesBuff) {
			identTbl.insert({ name, CIdetificator(name, flagBlock, typesBuff.front()) });
		}
	namesBuff.clear();
}
void CScope::addToNameBuffer(string name) {
	namesBuff.push_back(name);
}

void CScope::defineConst(EType type, string constRight) {
	// ��������� ��������� �� ��������� � ������ ������
	if (identTbl.find(namesBuff.front()) != identTbl.end()) {
		// ���� ��� ��������, �� ������ ������ � ��������� ��� ������� ����������
		writeMistake(101);
		namesBuff.clear();
	} else {
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
			//identTbl.insert(pair<string, CIdetificator>(namesBuff.front(), CIdetificator(namesBuff.front(), flagBlock, type)));
		}
	}
}
void CScope::createNone() {
	typeTbl.push_back(CNoneType());
	typesBuff.push_back(&typeTbl.back());
}
void CScope::addToBuffer(EType type) {
	if (flagBlock == TYPEBL) {
		typeTbl.push_back(defineAndCreateType(type));
	} else if (flagBlock == VARBL) {

	}
}

void CScope::clearBuffs() {
	clearNamesBuff();
	clearTypesBuff();
}

void CScope::addToBuffer(string typeName, EType type) {

	// ��������� �� �������� �� ���������� ��� ������� ������ ��� �������
	//if (type != eNONE) {
	//	// ����� ��� ������
	//	typeTbl.push_back(defineAndCreateType(type));
	//	typesBuff.push_back(&typeTbl.back());

	//}

	//typeTbl.push_back(*(findType(typeName, set<EBlock>{CONSTBL, TYPEBL})));			// �������� ������ �� ������� ��� ��� ��������� ���

	// �������� ����
	if (flagBlock == TYPEBL) {
		// ��� ������� ��� ��� ���������������� (�� ���������) � �� �� �������� ����������� � ���������� ���� (type t = INTEGER)
		// ��������� �� ���� �� �������� �������� (name) ������ ���� � ������� �����
			
		if (identTbl.find(namesBuff.front()) != identTbl.end()) {		// ��� ���� �������� � ������ 
			// TODO(throw type_already_exists)
		} else {
			CType* type = findType(typeName, set<EBlock>{TYPEBL});	// ���� ���������� ���� (typeName)
			// ���� ���������� ���� �� �������, ������ ������
			if (type == nullptr) {
				// TODO(throw no_type)
			} else {
				// ���� ���������� ���� �������, �� ��������� � ����� ����� ����� � ������ �� ��������� ���
				identTbl.insert(pair<string, CIdetificator>(typeName, CIdetificator(namesBuff.front(), TYPEBL, type)));
			}
		}
		
	} else if (flagBlock == VARBL) {
		// ���������� ����������

	} else {
		// flagBlock == BODYBL || CONSTBL
		// TODO(throw not_define)
	}
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
