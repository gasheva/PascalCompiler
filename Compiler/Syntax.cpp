#include "Syntax.h"


Syntax::Syntax(CErrorManager* erManager, Lexic* lexic, Semantic* semantic) {
	this->erManager = erManager;
	this->lexic = lexic;
	this->semantic = semantic;
}

Syntax::~Syntax(){}


float Syntax::simpleExpr() { return NULL; }

void Syntax::startVer()
{
	float res;
	auto left = factor(res);
	getNext();

	// пока не достигнем конца файла
	while (curToken!= nullptr) {
		simpleExpr();

		getNext();
	}
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

EVarType Syntax::term(float &res)
{
	return;
}

// мб добавить переменную hasMistake?
EVarType Syntax::factor(float& res)
{
	/*if (curToken == nullptr) {
		throw exception("Token expected");
		return;
	}


	if (curToken->m_T == OPER) {
		accept("(");
		auto left = simpleExpr();
		accept(")");
		res = left;
		return;
	}
	return ((CValueToken*)curToken)->m_val
	getNext();*/
	return;
}

void Syntax::accept(string oper) {
	if (curToken == nullptr)
		throw new exception("Expected another op");
	if (curToken->m_T!=OPER)
		throw new exception("Expected another op");
	if (((COperToken*)curToken)->lexem!=oper)
		throw new exception("Expected another op");
	getNext();
}


bool Syntax::isAdditiveOper() {
	if (curToken->m_T != OPER) return false;
	return ((COperToken*)curToken)->lexem == string("+") ||
		((COperToken*)curToken)->lexem == string("-") ||
		((COperToken*)curToken)->lexem == string("or");
}

bool Syntax::isMultOper() {
	if (curToken->m_T != OPER) return false;
	return ((COperToken*)curToken)->lexem == string("*") ||
		((COperToken*)curToken)->lexem == string("/") ||
		((COperToken*)curToken)->lexem == string("div") ||
		((COperToken*)curToken)->lexem == string("mod") ||
		((COperToken*)curToken)->lexem == string("and");
}


// accept sign
bool Syntax::isSign() {

	if (curToken->m_T != OPER) return false;
	if (((COperToken*)curToken)->lexem == "+" ||
		((COperToken*)curToken)->lexem == "-")
		getNext();
	return true;
}

float Syntax::constWithoutSign() {
	return NULL;
}
