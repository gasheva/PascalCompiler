#include "Syntax.h"


Syntax::Syntax(CErrorManager* erManager, Lexic* lexic, Semantic* semantic) {
	this->erManager = erManager;
	this->lexic = lexic;
	this->semantic = semantic;
}

Syntax::~Syntax(){}


void Syntax::startVer()
{
	getNext();
	factor();

	// simpleExpr();


	// ���� �� �������� ����� �����
	/*while (curToken!= nullptr) {
		

		getNext();
	}*/
}

void Syntax::getNext() {
	removeToken();
	curToken = this->lexic->getNext(true);
}

void Syntax::peekNext() {
	removeToken();
	curToken = this->lexic->getNext(false);
}

void Syntax::removeToken()
{
	if (curToken!=nullptr)
		delete curToken;
}

void Syntax::simpleExpr() { 
	ifNullThrowExcp();

	acceptSign();
	term();
	// �� ��� � {}
	while (isAdditiveOper()) {
		getNext();
		term();
	}
}

void Syntax::term()
{
	ifNullThrowExcp();

	factor();
	while (curToken!=nullptr && isMultOper()) {
		getNext();
		factor();
	}
}



// �� �������� ���������� hasMistake?
void Syntax::factor()
{
	ifNullThrowExcp();

	if (checkOper("not")) {						// not <���������>
		getNext();
		term();
		return;
	}

	if (curToken->getType() == IDENT) {			// ������� TODO(�������� ����������)
		peekNext();
		if (tryAccept("(")) {
			while (checkOper(")"))
				getNext();
			if (curToken == nullptr) throw new exception("Reached eof. Expected ')'");
			else getNext();			// accept(")")
			return;
		}
		getNext();	// ���� �� �������, �� ���������� 
		return;	
	}

	// ��������� ��� �����
	if (unsignedConst())
		return;
		
	
	// ���� ������ �� �������, �������� �� (<���������>)
	accept("(");
	simpleExpr();
	accept(")");
}

bool Syntax::unsignedConst() {
	ifNullThrowExcp();

	// ����� ��� �����
	if (curToken->getType() == VALUE ||
		((CValueToken*)curToken)->getType() == INT ||
		((CValueToken*)curToken)->getType() == REAL ||
		((CValueToken*)curToken)->getType() == STRING){		// TODO(� char?)
		getNext();
		return true;
	}
	return false;
	
}

// "�������" �����, ��������, ��� ������� ������
void Syntax::accept(string oper) {
	ifNullThrowExcp();

	if (curToken->getType()!=OPER)
		throw new exception("Expected another op");
	if (((COperToken*)curToken)->lexem!=oper)
		throw new exception("Expected another op");
	getNext();
}

bool Syntax::tryAccept(string oper) {
	ifNullThrowExcp();

	if (curToken->getType() != OPER)
		throw new exception("Expected another op");
	if (((COperToken*)curToken)->lexem != oper)
		return false;
	getNext();
	return true;
}

bool Syntax::checkOper(string oper)
{
	ifNullThrowExcp();

	if (curToken->getType() == OPER &&
		((COperToken*)curToken)->lexem == oper)
		return true;
	return false;
}

bool Syntax::ifNullThrowExcp()
{
	if (curToken == nullptr)
		throw new exception("Reached eof");
	return curToken == nullptr;
}

bool Syntax::isAdditiveOper() {
	ifNullThrowExcp();

	if (curToken->getType() != OPER) return false;
	return ((COperToken*)curToken)->lexem == string("+") ||
		((COperToken*)curToken)->lexem == string("-") ||
		((COperToken*)curToken)->lexem == string("or");
}

bool Syntax::isMultOper() {
	ifNullThrowExcp();

	if (curToken->getType() != OPER) return false;
	return ((COperToken*)curToken)->lexem == string("*") ||
		((COperToken*)curToken)->lexem == string("/") ||
		((COperToken*)curToken)->lexem == string("div") ||
		((COperToken*)curToken)->lexem == string("mod") ||
		((COperToken*)curToken)->lexem == string("and");
}


// "�������" ����, ���� �� ����
bool Syntax::acceptSign() {
	ifNullThrowExcp();

	if (curToken->getType() != OPER) return false;
	if (((COperToken*)curToken)->lexem == "+" ||
		((COperToken*)curToken)->lexem == "-")
		getNext();
	return true;
}

