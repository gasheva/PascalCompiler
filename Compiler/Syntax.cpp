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
	try { ifNullThrowExcp(); }
	catch (PascalExcp& e) {
		cout << "[!]EXCEPTION" << endl;
		return;
	}
	// TODO(пропуск строки)
	try {
		simpleExpr();
	}
	catch (PascalExcp& e) {
		cout << "[!]EXCEPTION" << endl;
	}


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

void Syntax::simpleExpr() throw(PascalExcp) {
	cout << "Checking simple Expr" << endl;
	try {
		ifNullThrowExcp();
	}
	catch (PascalExcp& e) {
		throw PascalExcp();
	}


	acceptSign();
	
	try {term();}
	catch (PascalExcp& e) {
		throw PascalExcp();
	}
	
	

	// то, что в {}
	while (isAdditiveOper()) {
		getNext();
		
		try { term(); }
		catch (PascalExcp& e) {
			throw PascalExcp();
		}
	}
}

void Syntax::term() throw(PascalExcp)
{
	cout << "Checking term" << endl;
	try { ifNullThrowExcp();}
	catch (PascalExcp& e) {
		throw PascalExcp();
	}

	try { factor(); }
	catch (PascalExcp& e) {
		throw PascalExcp();
	}
	while (curToken!=nullptr && isMultOper()) {
		getNext();
		try { factor(); }
		catch (PascalExcp& e) {
			throw PascalExcp();
		}
	}
}



// мб добавить переменную hasMistake?
void Syntax::factor() throw(PascalExcp)
{
	cout << "Checking factor" << endl;
	try { ifNullThrowExcp(); }
	catch (PascalExcp& e) {
		throw PascalExcp();
	}
	

	if (checkOper("not")) {						// not <множитель>
		getNext();
		try { factor(); }
		catch (PascalExcp& e) {
			throw PascalExcp();
		}
		return;
	}

	if (curToken->getType() == IDENT) {			// функци€ TODO(проверка параметров)
		peekNext();			
		if (curToken != nullptr) {
			try {
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
			catch (PascalExcp& e) {
				throw PascalExcp();
			}
		}
		cout << "Checking ident" << endl;
		getNext();	// если не функци€, то переменна€ 
		return;	
	}

	// константа без знака
	try {
		if (unsignedConst()) {
			return;
		}
	}
	catch (PascalExcp& e) {
		throw PascalExcp();
	}
		
	
	// если ничего не подошло, проверка на (<выражение>)
	try { accept("("); }
	catch (PascalExcp& e) {
		throw PascalExcp();
	}
	try { simpleExpr(); }
	catch (PascalExcp& e) {
		throw PascalExcp();
	}

	try { accept(")"); }
	catch (PascalExcp& e) {
		throw PascalExcp();
	}
}

bool Syntax::unsignedConst()throw(PascalExcp) {
	try { ifNullThrowExcp(); }
	catch (PascalExcp& e) {
		throw PascalExcp();
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
void Syntax::accept(string oper) throw(PascalExcp) {
	try { ifNullThrowExcp(); }
	catch (PascalExcp& e) {
		throw PascalExcp();
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
		throw PascalExcp();
		return;
		// throw exception("Expected another op");
	}
	if (((COperToken*)curToken)->lexem != oper) {
		throw PascalExcp();
		return;
		// throw exception("Expected another op");
	}
	getNext();
}

bool Syntax::tryAccept(string oper) throw(PascalExcp) {
	if (curToken == nullptr) return false;

	if (curToken->getType() != OPER) {
		throw PascalExcp();
		// throw exception("Expected another op");
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
		throw PascalExcp();
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

