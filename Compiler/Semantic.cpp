#include "Semantic.h"
#include <stdexcept>
#include "CVariant.h"
#include <set>

using namespace std;

CSemantic::CSemantic() {}

CSemantic::~CSemantic() {}

void CSemantic::createFictiveScope() {
	// integer, real, string, char - BaseType, true, false - EnumType
	CScope fictiveScp = CScope(nullptr);
	scopesLst.push_back(fictiveScp);
	fictiveScp.createType("integer");
	fictiveScp.addIdent("integer", TYPEBL, "integer");
	fictiveScp.createType("none");
	fictiveScp.addIdent("none", TYPEBL, "none");
	fictiveScp.createType("string");
	fictiveScp.addIdent("string", TYPEBL, "string");
	fictiveScp.createType("char");
	fictiveScp.addIdent("char", TYPEBL, "char");
	fictiveScp.createType("real");
	fictiveScp.addIdent("real", TYPEBL, "real");
	list<string> trueFalse("true", "false");
	fictiveScp.createType("bool", trueFalse);
	fictiveScp.addIdent("bool", TYPEBL, "bool");
}

CScope::CScope(CScope* outerScopes) {
	this->outerScope = outerScope;
}

CScope::~CScope() {}

// ���� ����
void CScope::addIdent(string name, EBlock block, string typeName) {
	CType* type = findIdent(typeName);				// ���� ���
	if (type != nullptr) {
		identTbl[name] = std::make_tuple(CIdetificator(name, block), type);
	} else throw exception;			// ��� �� ������
}

void CScope::addIdent(string name, EBlock block, string typeName, list<string> constants) {
	CType* type = createType(typeName, constants);				// ������� ��� enum
	identTbl[name] = std::make_tuple(CIdetificator(name, block), type);
}

// �������� ���� enum
CType* CScope::createType(string typeName, list<string> constants) {
	typeTbl.push_back(CEnumType(constants));
	return &typeTbl.back();
}

// �������� ���� subrange 
CType* CScope::createType(string typeName, string elTypeName, list<string>childConstNames, EVarType childType) {
	typeTbl.push_back(CSubrangeType());
	CSubrangeType* type = (CSubrangeType*)&typeTbl.back();
	CType* childeType;
	for (auto childName : childConstNames) {
		// �������� 'string', 423, 'c', 8.9
		if (childType != NULL) {
			switch (childType) {
			case STRING: childeType = findType("string", set<EBlock>{TYPEBL}); break;
			case INT: childeType= findType("int", set<EBlock>{TYPEBL}); break;
			case REAL: childeType = findType("real", set<EBlock>{TYPEBL}); break;
			case CHAR: childeType = findType("char", set<EBlock>{TYPEBL}); break;
			}
		}
		// ����� ��� ������������� - ���������
		else {
			childeType = findType(typeName, set<EBlock>{CONSTBL});				// ��������� 
		}
		// ���� ��� �� ��� ������ ��� ��� �� ������ � ����������
		if (childeType == nullptr || !(*type).setElType(childeType)) {
			childeType = findType("none", set<EBlock>{TYPEBL}); break;
			break;
		} else {
			(*type).addEl(childName);		// ��� � �������
		}
	}
	return nullptr;
}

// �������� ���� array
CType* CScope::createType(string typeName, string elTypeName, string indexTypeName, int dimension) {}

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
	typeTbl.push_back(*(findType(type, set<EBlock>{CONSTBL, TYPEBL})));			// �������� ������ �� ������� ��� ��� ��������� ���
}

CType* CScope::findType(string name, set<EBlock> block) {
	CType* type = nullptr;
	if (identTbl.find(name) != identTbl.end()) {
		// ������������� ������
		if (block.find(get<0>(identTbl[name]).getBlock())!=block.end()) {
			return get<1>(identTbl[name]);		// ��� ������, ������������
		} else {
			// ������������� ������, �� ��� �� ���
		}

	} else {
		if (outerScope != nullptr)
			type = outerScope->findIdent(name);
		else return nullptr;		// TODO(���� ���)
	}
	return type;
}

bool CScope::overrideBaseType(string name) {
	
	return false;
}

CIdetificator::CIdetificator(string name, EBlock block) {}

CIdetificator::CIdetificator(string name) {}

CIdetificator::~CIdetificator() {}
