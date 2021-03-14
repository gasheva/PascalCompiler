#include "CVariant.h"
#include <iostream>

CVariant::CVariant(EVarType type, string lexem)
{
	this->T = type;
	this->lexem = lexem;
}
EVarType CVariant::getType()
{
	return T;
}
CVariant::CVariant(){ }
CVariant::~CVariant(){}


CIntVariant::CIntVariant(int val) : CVariant(INT, to_string(val))
{
	// std::cout << "[C] CIntVariant constructor" << std::endl;
	this->m_val = val;
}
CIntVariant::~CIntVariant() { 
	// std::cout << "[C] CIntVariant destructor" << std::endl; 
};


CRealVariant::CRealVariant(float val) : CVariant(REAL, to_string(val))
{
	this->m_val = val;
}
CRealVariant::~CRealVariant() {};


CStringVariant::CStringVariant(string lexem) : CVariant(STRING, lexem) {
	this->m_val = lexem;
}
CStringVariant::~CStringVariant(){}


CCharVariant::CCharVariant(char ch) : CVariant(CHAR, to_string(ch)) {
	this->m_val = ch;
}
CCharVariant::~CCharVariant(){}