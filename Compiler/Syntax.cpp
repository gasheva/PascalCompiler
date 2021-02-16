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
	try { constanta(); }
	catch (PascalExcp& e) {
		cout << "[!]EXCEPTION" << endl;
		return;
	}

	//// TODO(пропуск строки)
	//try {
	//	simpleExpr();
	//}
	//catch (PascalExcp& e) {
	//	cout << "[!]EXCEPTION" << endl;
	//}


	// пока не достигли конца файла
	/*while (curToken!= nullptr) {
		

		getNext();
	}*/
}

void Syntax::getNext() throw(PascalExcp){
	removeToken();
	curToken = this->lexic->getNext(true);
		checkForbiddenSymbol();
	
}

void Syntax::peekNext() {
	removeToken();
	curToken = this->lexic->getNext(false);
	checkForbiddenSymbol();
	
}

void Syntax::removeToken()
{
	if (curToken!=nullptr)
		delete curToken;
}

void Syntax::program() throw(PascalExcp){
	ifNullThrowExcp();
	accept("program");
	name();
	accept("(");
	name();
	while (checkOper(",")) {
		getNext();
		name();
	}
	accept(")");
	accept(";");
	//block();
	
	accept(".");		// end.
}

void Syntax::name() {
	ifNullThrowExcp();
	if (curToken->getType() != IDENT) throw PascalExcp();
	getNext();
}

void Syntax::block() {
	ifNullThrowExcp();

	blockMarks();
	blockConst();
	blockTypes();
	blockVars();
	blockFunc();
	blockOpers();
	blockMarks();
}
void Syntax::blockMarks()
{
}
void Syntax::blockConst() throw(PascalExcp){
	ifNullThrowExcp();

	if (checkOper("const")) {
		unsignedConst();
		accept(";");
		constDef();
		while (checkOper(";")) {
			try {
				constDef();
			}
			catch (PascalExcp& e) {
				// проверка, что закончили с разделом констант
				if (curToken != nullptr) {
					if (curToken->getType() == OPER && (
						((COperToken*)curToken)->lexem == "type" ||
						((COperToken*)curToken)->lexem == "var"))
						return;
					else throw e;	// кидаем ошибку TODO(пропуск константы до ;)
				}
				else throw e;
			}
		};
	}
}
void Syntax::constDef()throw(PascalExcp)
{
	ifNullThrowExcp();
	name();
	accept("=");
	constanta();
}
void Syntax::constanta()throw(PascalExcp)
{
	//<знак>
	if (acceptSign()) {
		// <число без знака>
		try {
			unsignedNum();
		}
		catch (PascalExcp& e) {
			try {
				// <им€ константы>
				name();
			}
			catch (PascalExcp& e) {
				throw e;		// варианты начинани€ со знака закончились
			}
		}
	}
	else {
		// <им€ константы>
		if (curToken->getType() == IDENT) {
			getNext();
			return;
		}
		// <строка>
		if (curToken->getType() == VALUE &&
			((CValueToken*)curToken)->getType() == STRING){
			getNext();		// прин€ли строку
			return;
		}

		// <число без знака>
		unsignedNum();		// кинет ошибку
	}
}

void Syntax::blockTypes()
{

}

void Syntax::blockVars()
{
}

void Syntax::blockFunc()
{
}

void Syntax::blockOpers()
{
}



void Syntax::simpleExpr() throw(PascalExcp) {
	cout <<setw(offset)<<" "<<std::left<< "Checking simple Expr" << endl;
	offset += offsetD;
	ifNullThrowExcp();

	acceptSign();
	term();
	
	// то, что в {}
	while (isAdditiveOper()) {
		getNext();	
		term();
	}
	offset -= offsetD;
}

void Syntax::term() throw(PascalExcp)
{
	cout <<setw(offset)<<" "<<std::left<< "Checking term" << endl; 
	offset += offsetD;
	ifNullThrowExcp();

	factor();
	while (curToken!=nullptr && isMultOper()) {
		getNext(); 
		factor();
	}
	offset -= offsetD;
}



// мб добавить переменную hasMistake?
void Syntax::factor() throw(PascalExcp)
{
	cout <<setw(offset)<<" "<<std::left<< "Checking factor" << endl;
	offset += offsetD;
	ifNullThrowExcp();
	

	if (checkOper("not")) {						// not <множитель>
		getNext();
		offset -= offsetD;
		factor(); 
		return;
	}

	if (curToken->getType() == IDENT) {			// функци€ TODO(проверка параметров)
		peekNext();			
		if (curToken != nullptr) {
				if (tryAccept("(")) {
									// = accept("("), поскольку до этого было неприн€тое значение
					getNext(); 
					while (curToken != nullptr && !checkOper(")"))
						getNext();
					if (curToken == nullptr) throw exception("Reached eof. Expected ')'");
					else 
						getNext(); // accept(")")
					cout <<setw(offset)<<" "<<std::left<< "Checking func" << endl;
					offset -= offsetD;
					return;
				}
			
		}
		cout <<setw(offset)<<" "<<std::left<< "Checking ident" << endl;
		getNext(); // если не функци€, то переменна€ 
		offset -= offsetD;
		return;	
	}

	// константа без знака
	
		if (unsignedConst()) {
			offset -= offsetD;
			return;
		}
	
		
	
	// если ничего не подошло, проверка на (<выражение>)
	accept("("); 
	simpleExpr(); 
	accept(")");
	offset -= offsetD;
}

bool Syntax::unsignedConst()throw(PascalExcp) {
	ifNullThrowExcp(); 

	// число без знака
	if (curToken->getType() == VALUE && ((CValueToken*)curToken)->getType() != CHAR){		// TODO(а char?)
		cout <<setw(offset)<<" "<<std::left<< "Checking unsigned Const" << endl;
		getNext(); 
		return true;
	}
	return false;
	
}
void Syntax::unsignedNum() throw(PascalExcp) {
	if (curToken->getType() != VALUE)
		throw PascalExcp();
	auto tokType = ((CValueToken*)curToken)->getType();
	if (tokType != INT || tokType != REAL)
		throw PascalExcp();
	getNext();
}

void Syntax::checkForbiddenSymbol() throw(PascalExcp)
{
	if (curToken!=nullptr && curToken->getType() == UNDEF) {
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
	ifNullThrowExcp(); 

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
		getNext();
	return true;
}

