#include "CTypes.h"

CType::CType(EType type)
{
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