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
	namesBuff.clear();
}
void CScope::addToNameBuffer(string name) {
	namesBuff.push_back(name);
}

void CScope::defineConst(EType type, string constRight) {
	// ��������� ��������� ���������� ��� ������������� ������
	// ��������� ��������� �� ��������� � ������ ������
	if (identTbl.find(namesBuff.front()) != identTbl.end()) {
		// TODO(throw already_defined)
	} else {
		// ������� ���������� ���������
		// ���� ������ �������������, ���� ��� ����� ����������� ��������
		if (type == eNONE) {
			findType(constRight, set<EBlock>{CONSTBL});
		} else {
			// ������� ���������� ��� ��� ������, ����� � ��
			typeTbl.push_back(defineAndCreateType(type));
			auto type = (&typeTbl.back());
			identTbl.insert(pair<string, CIdetificator>(namesBuff.front(), CIdetificator(namesBuff.front(), flagBlock, type)));
		}
	}
}
void CScope::addToBuffer(EType type) {
	if (flagBlock == TYPEBL) {
		typeTbl.push_back(defineAndCreateType(type));
	} else if (flagBlock == VARBL) {

	} else { 
		// flagBlock == BODYBL || CONSTBL
		// TODO(throw not_define)
	}
}

void CScope::addToBuffer(string typeName) {
	typeTbl.push_back(*(findType(typeName, set<EBlock>{CONSTBL, TYPEBL})));			// �������� ������ �� ������� ��� ��� ��������� ���

	// �������� ����
	if (flagBlock == TYPEBL) {
		// ���� � ������ ��� ������� ���, ������ ��� ���������� ���������� ���� (type compl = array[x..10] of INTEGER <- INTEGER ��� '1' ��� � ����� ������ ������
		if (typesBuff.size() > 0) {
			CType* type = typesBuff.front();
			auto typeName = (*type).getType();
			switch (typeName) {
			case eARRAY:
				//auto arType = (CArrayType*)type;
				//arType->getIndexType();		//TODO()
				break;
			case eSUBRANGE:
				break;
			case eENUM:
				break;
			default:
				break;
			}
		} else {
			// ��� ������� ��� ��� ���������������� (�� ���������) � �� �� �������� ����������� � ���������� ���� (type t = INTEGER)
			// ��������� ����� ���� ���������� ��� ������������� ������
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

CType* CScope::findType(string name, set<EBlock> block) {
	CType* type = nullptr;
	if (identTbl.find(name) < identTbl.end()) {
		// ������������� ������
		//if (block.find((identTbl[name]).getBlock()) != block.end()) {
		//	return (identTbl[name].getType());		// ��� ������, ������������
		//} else {
		//	// ������������� ������, �� ��� �� ���
		//}

	} else {
		if (outerScope != nullptr)
			type = outerScope->findIdent(name, block);
		else return nullptr;
	}
	return type;
	return nullptr;
}

void CScope::createFictive() {
	typeTbl.push_back(CBaseType(eINT));
	identTbl.insert(pair<string, CIdetificator>("integer", CIdetificator("integer", TYPEBL, &(typeTbl.back()))));


	typeTbl.push_back(CBaseType(eREAL));
	identTbl.insert(pair<string, CIdetificator>("real", CIdetificator("real", TYPEBL, &(typeTbl.back()))));

	typeTbl.push_back(CBaseType(eSTRING));
	identTbl.insert(pair<string, CIdetificator>("string", CIdetificator("string", TYPEBL, &(typeTbl.back()))));

	typeTbl.push_back(CBaseType(eCHAR));
	identTbl.insert(pair<string, CIdetificator>("char", CIdetificator("char", TYPEBL, &(typeTbl.back()))));

	typeTbl.push_back(CEnumType(list<string>{"true", "false"}));
	identTbl.insert(pair<string, CIdetificator>("boolean", CIdetificator("boolean", TYPEBL, &(typeTbl.back()))));
}

CIdetificator::CIdetificator(string name, EBlock block, CType* type) {

}

CIdetificator::CIdetificator(string name, EBlock block) {}

CIdetificator::CIdetificator(string name) {}

CIdetificator::~CIdetificator() {}
