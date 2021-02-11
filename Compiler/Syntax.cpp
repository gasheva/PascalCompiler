#include "Syntax.h"


Syntax::Syntax(CErrorManager* erManager, Lexic* lexic, Semantic* semantic) {
	this->erManager = erManager;
	this->lexic = lexic;
	this->semantic = semantic;
}

Syntax::~Syntax(){}


void Syntax::startVer()
{
	simpleExpr();

	// пока не достигли конца файла
	/*while (curToken!= nullptr) {
		

		getNext();
	}*/
}

void Syntax::getNext() {
	removeToken();
	curToken = this->lexic->getNext();
}

void Syntax::removeToken()
{
	if (curToken!=nullptr)
		delete curToken;
}


// TODO(замени флоат возврат на воид, дл€ синтаксиса этого достаточно)
float Syntax::simpleExpr() { 
	float left;
	term(left);
	while (isMultOper()) {
		auto oper = ((COperToken*)curToken)->lexem;
		getNext();
		float right;
		term(right);
		switch (oper)
		{
		case "+":
			left = left + right;
			break;
		case Oper.oMinus:
			left = left - right;
			break;
		default:
			throw new Exception("bad addit oper");
		}
	}
	
}

EVarType Syntax::term(float &res)
{
	return EVarType();
}

// мб добавить переменную hasMistake?
EVarType Syntax::factor(float& res)
{
	auto tokenTMP = CValueToken(CIntVariant(2));
	CVariant* v = ((CIntVariant*)(tokenTMP.getType()));
	

	/*if (curToken == nullptr) {
		throw exception("Token expected");
		return;
	}


	if (curToken->getType() == OPER) {
		accept("(");
		auto left = simpleExpr();
		accept(")");
		res = left;
		return;
	}
	return ((CValueToken*)curToken)->m_val
	getNext();*/
	return EVarType();
}

// "съедаем" токен, провер€€, что лексема нужна€
void Syntax::accept(string oper) {
	if (curToken == nullptr)
		throw new exception("Expected another op");
	if (curToken->getType()!=OPER)
		throw new exception("Expected another op");
	if (((COperToken*)curToken)->lexem!=oper)
		throw new exception("Expected another op");
	getNext();
}


bool Syntax::isAdditiveOper() {
	if (curToken->getType() != OPER) return false;
	return ((COperToken*)curToken)->lexem == string("+") ||
		((COperToken*)curToken)->lexem == string("-") ||
		((COperToken*)curToken)->lexem == string("or");
}

bool Syntax::isMultOper() {
	if (curToken->getType() != OPER) return false;
	return ((COperToken*)curToken)->lexem == string("*") ||
		((COperToken*)curToken)->lexem == string("/") ||
		((COperToken*)curToken)->lexem == string("div") ||
		((COperToken*)curToken)->lexem == string("mod") ||
		((COperToken*)curToken)->lexem == string("and");
}


// "съедаем" знак, если он есть
bool Syntax::isSign() {

	if (curToken->getType() != OPER) return false;
	if (((COperToken*)curToken)->lexem == "+" ||
		((COperToken*)curToken)->lexem == "-")
		getNext();
	return true;
}

float Syntax::constWithoutSign() {
	return NULL;
}
