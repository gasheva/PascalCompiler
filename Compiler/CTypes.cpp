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

CEnumType::CEnumType(list<string> constants) : CType(eENUM) {}

CEnumType::CEnumType() : CType(eENUM) {}

CEnumType::~CEnumType() {}

CArrayType::CArrayType(CType* elType, CType* indexType) : CType(eARRAY) {}

CArrayType::CArrayType() : CType(eARRAY) {}

CArrayType::~CArrayType() {}

CSubrangeType::CSubrangeType(): CType(eSUBRANGE){}
CSubrangeType::~CSubrangeType(){}
CSubrangeType::CSubrangeType(CType* elType): CType(eSUBRANGE){}