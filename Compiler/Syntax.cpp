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
	try {
		ifNullThrowExcp();
	}
	catch (PascalExcp& e) {
		cout << "[!]ECXEPTION" << endl;
	}


	acceptSign();
	bool hasMistake = false;

	term(hasMistake);
	if (hasMistake) {
		cout << "[!] EXCEPTION" << endl;
		return;
	}
	// TODO(пропуск строки)

	// то, что в {}
	while (isAdditiveOper()) {
		getNext();
		
		term(hasMistake);
		if (hasMistake) {
			cout << "[!] EXCEPTION" << endl;
			return;
		}
	}
}

void Syntax::term(bool& hasMistake)
{
	cout << "Checking term" << endl;
	ifNullThrowExcp();

	factor(hasMistake);
	if (hasMistake) {
		cout << "[!] EXCEPTION" << endl;
		return;
	}

	while (curToken!=nullptr && isMultOper()) {
		getNext();
		factor(hasMistake);
		if (hasMistake) {
			cout << "[!] EXCEPTION" << endl;
			return;
		}
	}
}



// мб добавить переменную hasMistake?
void Syntax::factor(bool &hasMistake)
{
	cout << "Checking factor" << endl;
	ifNullThrowExcp();

	if (checkOper("not")) {						// not <множитель>
		getNext();
		factor(hasMistake);
		if (hasMistake) {
			cout << "[!] EXCEPTION" << endl;
			return;
		}
		return;
	}

	if (curToken->getType() == IDENT) {			// функци€ TODO(проверка параметров)
		peekNext();			
		if (curToken != nullptr) {
			if (tryAccept("(", hasMistake)) {
				if (hasMistake) {
					cout << "[!] EXCEPTION" << endl;
					return;
				}
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
	if (unsignedConst(hasMistake)) {
		if (hasMistake) {
			cout << "[!] EXCEPTION" << endl;
			return;
		}
		return;
	}
	if (hasMistake) {
		cout << "[!] EXCEPTION" << endl;
		return;
	}
		
	
	// если ничего не подошло, проверка на (<выражение>)
	accept("(", hasMistake);
	if (hasMistake) {
		cout << "[!] EXCEPTION" << endl;
		return;
	}
	simpleExpr();
	accept(")", hasMistake);
	if (hasMistake) {
		cout << "[!] EXCEPTION" << endl;
		return;
	}
}

bool Syntax::unsignedConst(bool& hasMistake) {
	hasMistake = ifNullThrowExcp();
	if (hasMistake) {
		cout << "[!] EXCEPTION" << endl;
		return false;
	}

	// число без знака
	if (curToken->getType() == VALUE && ((CValueToken*)curToken)->getType() != CHAR){		// TODO(а char?)
		cout << "Checking unsigned Const" << endl;
		getNext();
		return true;
	}
	return false;
	
}

bool Syntax::checkForbiddenSymbol()
{
	if (curToken->getType() == UNDEF) {
		writeMistake(6);
		return true;
	}
	return false;
}

void Syntax::writeMistake(int code)
{
	erManager->addError(lexic->getStartPosition(), lexic->getStartPosition(), code);
}



// "съедаем" токен, провер€€, что лексема нужна€
void Syntax::accept(string oper, bool& hasMistake) {
	hasMistake = ifNullThrowExcp();
	if (hasMistake) {
		cout << "[!] EXCEPTION" << endl;
		return;
	}

	if (curToken->getType() != OPER)
	{
		if (oper == ")") writeMistake(4);
		if (oper == ":") writeMistake(5);
		if (oper == "OF") writeMistake(8);
		if (oper == "(") writeMistake(9);
		if (oper == "[") writeMistake(11);
		if (oper == "]") writeMistake(12);
		if (oper == "END") writeMistake(13);
		if (oper == ";") writeMistake(14);
		hasMistake = true;
		return;
		// throw exception("Expected another op");
	}
	if (((COperToken*)curToken)->lexem != oper) {
		hasMistake = true;
		return;
		// throw exception("Expected another op");
	}
	getNext();
}

bool Syntax::tryAccept(string oper, bool &hasMistake) {
	if (curToken == nullptr) return false;

	if (curToken->getType() != OPER) {
		hasMistake = true;
		return;
		throw exception("Expected another op");
	}
	if (((COperToken*)curToken)->lexem != oper) {
		return false;
	}
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

bool Syntax::ifNullThrowExcp() throw(PascalExcp)
{
	if (curToken == nullptr)
	{
		return true;
		// throw exception("Reached eof");
	}
	return false;
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

