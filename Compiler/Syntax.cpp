#include "Syntax.h"
#include <iomanip>


Syntax::Syntax(CErrorManager* erManager, Lexic* lexic, Semantic* semantic) {
	this->erManager = erManager;
	this->lexic = lexic;
	this->semantic = semantic;
}

Syntax::~Syntax(){}


void Syntax::startVer()
{
	try { getNext(); }
	catch (PascalExcp& e) {
		cout << "[!]EXCEPTION" << endl;
		return;
	}

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

void Syntax::getNext() throw(PascalExcp){
	removeToken();
	curToken = this->lexic->getNext(true);
	try {
		checkForbiddenSymbol();
	}catch (PascalExcp& e) {
		throw PascalExcp();
	}
}

void Syntax::peekNext() {
	removeToken();
	curToken = this->lexic->getNext(false);
	try {checkForbiddenSymbol();}
	catch (PascalExcp& e) {
		throw PascalExcp();
	}
}

void Syntax::removeToken()
{
	if (curToken!=nullptr)
		delete curToken;
}

void Syntax::simpleExpr() throw(PascalExcp) {
	cout <<setw(offset)<<" "<<std::left<< "Checking simple Expr" << endl;
	offset += offsetD;
	try {
		ifNullThrowExcp();
	}
	catch (PascalExcp& e) {
		throw PascalExcp();
	}

	try { acceptSign(); }
	catch (PascalExcp& e) {
		throw PascalExcp();
	}
	
	try {term();}
	catch (PascalExcp& e) {
		throw PascalExcp();
	}
	
	

	// то, что в {}
	while (isAdditiveOper()) {
		try { getNext(); }
		catch (PascalExcp& e) {
			throw PascalExcp();
		}
		
		try { term(); }
		catch (PascalExcp& e) {
			throw PascalExcp();
		}
	}
	offset -= offsetD;
}

void Syntax::term() throw(PascalExcp)
{
	cout <<setw(offset)<<" "<<std::left<< "Checking term" << endl; 
	offset += offsetD;
	try { ifNullThrowExcp();}
	catch (PascalExcp& e) {
		throw PascalExcp();
	}

	try { factor(); }
	catch (PascalExcp& e) {
		throw PascalExcp();
	}
	while (curToken!=nullptr && isMultOper()) {
		try { getNext(); }
		catch (PascalExcp& e) {
			throw PascalExcp();
		}
		try { factor(); }
		catch (PascalExcp& e) {
			throw PascalExcp();
		}
	}
	offset -= offsetD;
}



// мб добавить переменную hasMistake?
void Syntax::factor() throw(PascalExcp)
{
	cout <<setw(offset)<<" "<<std::left<< "Checking factor" << endl;
	offset += offsetD;
	try { ifNullThrowExcp(); }
	catch (PascalExcp& e) {
		throw PascalExcp();
	}
	

	if (checkOper("not")) {						// not <множитель>
		try { getNext(); }
		catch (PascalExcp& e) {
			throw PascalExcp();
		}
		offset -= offsetD;
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
									// = accept("("), поскольку до этого было неприн€тое значение
					try { getNext(); }
					catch (PascalExcp& e) {
						throw PascalExcp();
					}
					while (curToken != nullptr && !checkOper(")"))
						try { getNext(); }
						catch (PascalExcp& e) {
							throw PascalExcp();
						}
					if (curToken == nullptr) throw exception("Reached eof. Expected ')'");
					else 
						try { getNext(); }
						catch (PascalExcp& e) {
							throw PascalExcp();
						}// accept(")")
					cout <<setw(offset)<<" "<<std::left<< "Checking func" << endl;
					offset -= offsetD;
					return;
				}
			}
			catch (PascalExcp& e) {
				throw PascalExcp();
			}
		}
		cout <<setw(offset)<<" "<<std::left<< "Checking ident" << endl;
		try { getNext(); }
		catch (PascalExcp& e) {
			throw PascalExcp();
		}	// если не функци€, то переменна€ 
		offset -= offsetD;
		return;	
	}

	// константа без знака
	try {
		if (unsignedConst()) {
			offset -= offsetD;
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
	offset -= offsetD;
}

bool Syntax::unsignedConst()throw(PascalExcp) {
	try { ifNullThrowExcp(); }
	catch (PascalExcp& e) {
		throw PascalExcp();
	}

	// число без знака
	if (curToken->getType() == VALUE && ((CValueToken*)curToken)->getType() != CHAR){		// TODO(а char?)
		cout <<setw(offset)<<" "<<std::left<< "Checking unsigned Const" << endl;
		try { getNext(); }
		catch (PascalExcp& e) {
			throw PascalExcp();
		}
		return true;
	}
	return false;
	
}

void Syntax::checkForbiddenSymbol() throw(PascalExcp)
{
	if (curToken->getType() == UNDEF) {
		writeMistake(6);
		throw PascalExcp();
	}
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
	try { getNext(); }
	catch (PascalExcp& e) {
		throw PascalExcp();
	}
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
	try { getNext(); }
	catch (PascalExcp& e) {
		throw PascalExcp();
	}
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
		erManager->addError(lexic->getCurPos(), lexic->getCurLine(), 14);		//TODO(по факту здесь достижение конца файла, но кинем "ожидалась ;"
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
bool Syntax::acceptSign() throw (PascalExcp){
	ifNullThrowExcp();

	if (curToken->getType() != OPER) return false;
	if (((COperToken*)curToken)->lexem == "+" ||
		((COperToken*)curToken)->lexem == "-")
		try { getNext(); }
		catch (PascalExcp& e) {
			throw PascalExcp();
		}
	return true;
}

