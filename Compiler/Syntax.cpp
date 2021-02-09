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
	// пока не достигнем конца файла
	while (lexic->getNext() != nullptr) {
		curToken = lexic->getNext();

		simpleExpr();
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

bool Syntax::isAdditiveOper(COperToken* token) {
	return token->lexem == string("+") ||
		token->lexem == string("-") ||
		token->lexem == string("or");
}

bool Syntax::isMultOper(COperToken* token) {
	return token->lexem == string("*") ||
		token->lexem == string("/") ||
		token->lexem == string("div") ||
		token->lexem == string("mod") ||
		token->lexem == string("and");
}

float Syntax::constWithoutSign() {
	return NULL;
}
