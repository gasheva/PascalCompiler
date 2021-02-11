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
	simpleExpr();

	// simpleExpr();


	// пока не достигли конца файла
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
	cout << "Checking simple Expr" << endl;
	ifNullThrowExcp();

	acceptSign();
	term();
	// то, что в {}
	while (isAdditiveOper()) {
		getNext();
		term();
	}
}

void Syntax::term()
{
	cout << "Checking term" << endl;
	ifNullThrowExcp();

	factor();
	while (curToken!=nullptr && isMultOper()) {
		getNext();
		factor();
	}
}



// мб добавить переменную hasMistake?
void Syntax::factor()
{
	cout << "Checking factor" << endl;
	ifNullThrowExcp();

	if (checkOper("not")) {						// not <множитель>
		getNext();
		factor();
		return;
	}

	if (curToken->getType() == IDENT) {			// функци€ TODO(проверка параметров)
		peekNext();			
		if (curToken != nullptr) {
			if (tryAccept("(")) {
				getNext();				// = accept("("), поскольку до этого было неприн€тое значение
				while (curToken != nullptr && !checkOper(")"))
					getNext();
				if (curToken == nullptr) throw exception("Reached eof. Expected ')'");
				else getNext();			// accept(")")
				cout << "Checking func" << endl;
				return;
			}
		}
		cout << "Checking ident" << endl;
		getNext();	// если не функци€, то переменна€ 
		return;	
	}

	// константа без знака
	if (unsignedConst())
		return;
		
	
	// если ничего не подошло, проверка на (<выражение>)
	accept("(");
	simpleExpr();
	accept(")");
}

bool Syntax::unsignedConst() {
	ifNullThrowExcp();

	// число без знака
	if (curToken->getType() == VALUE && ((CValueToken*)curToken)->getType() != CHAR){		// TODO(а char?)
		cout << "Checking unsigned Const" << endl;
		getNext();
		return true;
	}
	return false;
	
}

// "съедаем" токен, провер€€, что лексема нужна€
void Syntax::accept(string oper) {
	ifNullThrowExcp();

	if (curToken->getType()!=OPER)
		throw exception("Expected another op");
	if (((COperToken*)curToken)->lexem!=oper)
		throw exception("Expected another op");
	getNext();
}

bool Syntax::tryAccept(string oper) {
	if (curToken == nullptr) return false;

	if (curToken->getType() != OPER)
		throw exception("Expected another op");
	if (((COperToken*)curToken)->lexem != oper)
		return false;
	getNext();
	return true;
}

bool Syntax::checkOper(string oper)
{
	if (curToken->getType() == OPER &&
		((COperToken*)curToken)->lexem == oper)
		return true;
	return false;
}

bool Syntax::ifNullThrowExcp()
{
	if (curToken == nullptr)
		throw exception("Reached eof");
	return (curToken == nullptr);
}

bool Syntax::isAdditiveOper() {
	if (curToken == nullptr) return false;

	if (curToken->getType() != OPER) return false;
	return ((COperToken*)curToken)->lexem == string("+") ||
		((COperToken*)curToken)->lexem == string("-") ||
		((COperToken*)curToken)->lexem == string("or");
}

bool Syntax::isMultOper() {
	if (curToken == nullptr) return false;

	if (curToken->getType() != OPER) return false;
	return ((COperToken*)curToken)->lexem == string("*") ||
		((COperToken*)curToken)->lexem == string("/") ||
		((COperToken*)curToken)->lexem == string("div") ||
		((COperToken*)curToken)->lexem == string("mod") ||
		((COperToken*)curToken)->lexem == string("and");
}


// "съедаем" знак, если он есть
bool Syntax::acceptSign() {
	ifNullThrowExcp();

	if (curToken->getType() != OPER) return false;
	if (((COperToken*)curToken)->lexem == "+" ||
		((COperToken*)curToken)->lexem == "-")
		getNext();
	return true;
}

