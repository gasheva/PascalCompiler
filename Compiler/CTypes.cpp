#include "CTypes.h"

CType::CType(EType type)
{
	this->type = type;
}

CType::~CType()
{
}

bool CSubrangeType::setElType(CType* type) {
	if (elType == nullptr) {
		elType = type;
		return true;
	}
	return elType==type;
}

void CSubrangeType::addEl(string el) {
	childEls.push_back(el);
}

string CSubrangeType::getStart() { return childEls.front(); }
string CSubrangeType::getEnd() { return childEls.back(); }

CNoneType::CNoneType():CType(eNONE) {}

CNoneType::~CNoneType() {}

CBaseType::CBaseType(EType type) : CType(type) {}

CBaseType::~CBaseType() {}

CEnumType::CEnumType() : CType(eENUM) {}

CEnumType::~CEnumType() {}

void CEnumType::addEl(CEnumElType* el) {
	els.push_back(el);
}

CArrayType::CArrayType(CType* elType) : CType(eARRAY) {
	this->elType = elType;
}

void CArrayType::addIndexType(CType* indexType) {
	this->indexType.push_back(indexType);
}

CArrayType::CArrayType() : CType(eARRAY) {}

CArrayType::~CArrayType() {}

CSubrangeType::CSubrangeType(): CType(eSUBRANGE){}
CSubrangeType::~CSubrangeType(){}
CSubrangeType::CSubrangeType(CType* elType): CType(eSUBRANGE){
	this->elType = elType;
}

CEnumElType::CEnumElType() : CType(eBOOLEAN) {}

CEnumElType::CEnumElType(CEnumType* parent) : CType(eBOOLEAN) {
	this->parent = parent;
}

CEnumElType::~CEnumElType() {}

void CEnumElType::setParent(CEnumType* parent) {
	this->parent = parent;
}
