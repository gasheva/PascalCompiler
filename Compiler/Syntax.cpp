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

	//// TODO(������� ������)
	//try {
	//	simpleExpr();
	//}
	//catch (PascalExcp& e) {
	//	cout << "[!]EXCEPTION" << endl;
	//}


	// ���� �� �������� ����� �����
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
				// ��������, ��� ��������� � �������� ��������
				if (curToken != nullptr) {
					if (curToken->getType() == OPER && (
						((COperToken*)curToken)->lexem == "type" ||
						((COperToken*)curToken)->lexem == "var"))
						return;
					else throw e;	// ������ ������ TODO(������� ��������� �� ;)
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
	//<����>
	if (acceptSign()) {
		// <����� ��� �����>
		try {
			unsignedNum();
		}
		catch (PascalExcp& e) {
			try {
				// <��� ���������>
				name();
			}
			catch (PascalExcp& e) {
				throw e;		// �������� ��������� �� ����� �����������
			}
		}
	}
	else {
		// <��� ���������>
		if (curToken->getType() == IDENT) {
			getNext();
			return;
		}
		// <������>
		if (curToken->getType() == VALUE &&
			((CValueToken*)curToken)->getType() == STRING){
			getNext();		// ������� ������
			return;
		}

		// <����� ��� �����>
		unsignedNum();		// ����� ������
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
	
	// ��, ��� � {}
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



// �� �������� ���������� hasMistake?
void Syntax::factor() throw(PascalExcp)
{
	cout <<setw(offset)<<" "<<std::left<< "Checking factor" << endl;
	offset += offsetD;
	ifNullThrowExcp();
	

	if (checkOper("not")) {						// not <���������>
		getNext();
		offset -= offsetD;
		factor(); 
		return;
	}

	if (curToken->getType() == IDENT) {			// ������� TODO(�������� ����������)
		peekNext();			
		if (curToken != nullptr) {
				if (tryAccept("(")) {
									// = accept("("), ��������� �� ����� ���� ���������� ��������
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
		getNext(); // ���� �� �������, �� ���������� 
		offset -= offsetD;
		return;	
	}

	// ��������� ��� �����
	
		if (unsignedConst()) {
			offset -= offsetD;
			return;
		}
	
		
	
	// ���� ������ �� �������, �������� �� (<���������>)
	accept("("); 
	simpleExpr(); 
	accept(")");
	offset -= offsetD;
}

bool Syntax::unsignedConst()throw(PascalExcp) {
	ifNullThrowExcp(); 

	// ����� ��� �����
	if (curToken->getType() == VALUE && ((CValueToken*)curToken)->getType() != CHAR){		// TODO(� char?)
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



// "�������" �����, ��������, ��� ������� ������
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
		erManager->addError(lexic->getCurPos(), lexic->getCurLine(), 14);		//TODO(�� ����� ����� ���������� ����� �����, �� ����� "��������� ;"
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


// "�������" ����, ���� �� ����
bool Syntax::acceptSign() throw (PascalExcp){
	ifNullThrowExcp();

	if (curToken->getType() != OPER) return false;
	if (((COperToken*)curToken)->lexem == "+" ||
		((COperToken*)curToken)->lexem == "-")
		getNext();
	return true;
}

