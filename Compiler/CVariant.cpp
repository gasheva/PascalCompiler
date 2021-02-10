#include "CVariant.h"
#include <iostream>

CVariant::CVariant(EVarType type)
{
	this->T = type;
}
EVarType CVariant::getType()
{
	return T;
}
CVariant::CVariant(){ }
CVariant::~CVariant(){}


CIntVariant::CIntVariant(int val) : CVariant(INT)
{
	std::cout << "[C] CIntVariant constructor" << std::endl;
	this->m_val = val;
}
CIntVariant::~CIntVariant() { std::cout << "[C] CIntVariant destructor" << std::endl; };


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
