#include "CVariant.h"

CVariant::CVariant(EVarType type)
{
	this->T = type;
}
CVariant::CVariant(){ }
CVariant::~CVariant(){}


CIntVariant::CIntVariant(int val) : CVariant(INT)
{
	this->m_val = val;
}
CIntVariant::~CIntVariant() {};


CRealVariant::CRealVariant(float val) : CVariant(REAL)
{
	this->m_val = val;
}
CRealVariant::~CRealVariant() {};


CStringVariant::CStringVariant(string lexem) : CVariant(STRING) {
	this->m_val = lexem;
}
CStringVariant::~CStringVariant(){}


CCharVariant::CCharVariant(char ch) : CVariant(CHAR) {
	this->m_val = ch;
}
CCharVariant::~CCharVariant(){}
