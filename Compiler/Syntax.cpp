#include "Syntax.h"
#include <iomanip>
#include "FStringFunc.h"


Syntax::Syntax(CErrorManager* erManager, Lexic* lexic, Semantic* semantic) {
	this->erManager = erManager;
	this->lexic = lexic;
	this->semantic = semantic;
}

Syntax::~Syntax() {}


void Syntax::startVer()
{
	try { getNext(); }
	catch (PascalExcp& e) {
		cout << "[!]EXCEPTION" << endl;
		return;
	}
	catch (EOFExcp& e) {
		cout << "[!]EOF EXCEPTION" << endl;
		return;
	}
	

	try { ifNullThrowExcp(); }
	catch (PascalExcp& e) {
		cout << "[!]EXCEPTION" << endl;
		return;
	}
	catch (EOFExcp& e) {
		cout << "[!]EOF EXCEPTION" << endl;
		return;
	}

	try { program(); }
	catch (PascalExcp& e) {
		cout << "[!]EXCEPTION" << endl;
		return;
	}
	catch (EOFExcp& e) {
		cout << "[!]EOF EXCEPTION" << endl;
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

void Syntax::getNext() throw(PascalExcp, EOFExcp) {
	removeToken();
	curToken = this->lexic->getNext(true);
	// checkForbiddenSymbol();
}

void Syntax::peekNext() {
	removeToken();
	curToken = this->lexic->getNext(false);
	checkForbiddenSymbol();

}

void Syntax::skip(set<string> lexemes) {
	if (curToken == nullptr) return;
	// ���� ��� �� ������ �����
	if (curToken->getType() == OPER && lexemes.find(((COperToken*)curToken)->lexem) != lexemes.end()) {
		return;
	}
	removeToken();
	curToken = this->lexic->skip(lexemes);
}

void Syntax::removeToken()
{
	if (curToken != nullptr)
		delete curToken;
}


void Syntax::program() throw(PascalExcp, EOFExcp) {
	ifNullThrowExcp();
	accept("program");
	set<string> nameSet = { "(", ",", ";", "const", "var", "begin" };
	set <string> branchSet = { ";", "const", "var", "begin" };
	set <string> semicolSet = { "const", "var", "begin" };

	try { name(); }
	catch (PascalExcp& e) {
		skip(nameSet);
	}
	try { accept("("); }
	catch (PascalExcp& e) {
		skip(nameSet);
	}
	try { name(); }
	catch (PascalExcp& e) {
		skip(nameSet);
	}
	while (checkOper(",")) {
		getNext();
		try { name(); }
		catch (PascalExcp& e) {
			skip(nameSet);
		}

	}
	try { accept(")"); }
	catch (PascalExcp& e) {
		skip(branchSet);
	}
	try { accept(";"); }
	catch (PascalExcp& e) {
		skip(branchSet);
	}

	block();

	accept(".");		// end.
	// ��������, ��� ����� end. ������ ������ ���
	if (curToken != nullptr) {
		writeMistake(1000);
		throw PascalExcp();
	}
}

void Syntax::name() throw(PascalExcp, EOFExcp) {
	ifNullThrowExcp();
	if (curToken->getType() != IDENT) {
		writeMistake(2);
		throw PascalExcp();
	}
	getNext();
}

void Syntax::block() throw(PascalExcp, EOFExcp) {
	ifNullThrowExcp();

	blockMarks();

	blockConst();
	blockTypes();
	blockVars();
	blockFunc();
	blockOpers();

}
void Syntax::blockMarks()
{
}
void Syntax::blockConst() throw(PascalExcp, EOFExcp) {
	ifNullThrowExcp();		//TODO(������ �� ������)
	set <string> constDefSet = { ";", "var", "begin" };

	if (checkOper("const")) {
		getNext();
		try {constDef();}
		catch (PascalExcp& e) {
			skip(constDefSet);
		}
		while (checkOper(";")) {
			getNext();
			if (curToken != nullptr) {
				if (curToken->getType() == OPER && (
					((COperToken*)curToken)->lexem == "var" ||		//TODO(�� ������ ���)
					((COperToken*)curToken)->lexem == "begin"))		
					return;
			}
			else return;		// ���������� �� ����� �����
			try { constDef(); }
			catch (PascalExcp& e) {
				skip(constDefSet);
			}
		}
	}
}
void Syntax::constDef()throw(PascalExcp, EOFExcp)
{
	ifNullThrowExcp();
	name();
	accept("=");
	constanta();
}
void Syntax::constanta()throw(PascalExcp, EOFExcp)
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
			((CValueToken*)curToken)->getVariant().getType() == STRING) {
			getNext();		// ������� ������
			return;
		}

		// <����� ��� �����>
		unsignedNum();		// ����� ������
	}
}
void Syntax::blockTypes() {}
void Syntax::blockVars() throw(PascalExcp, EOFExcp)
{
	//<������ ����������>::= var <�������� ���������� ����������>;{<�������� ���������� ����������>; } | <�����>
	ifNullThrowExcp();
	// var <�������� ���������� ����������>;
	if (checkOper("var")) {
		getNext();		// accept("var");
		descrMonotypeVars();
		// {<�������� ���������� ����������>;}
		while (checkOper(";")) {
			getNext();
			if (curToken != nullptr) {
				if (curToken->getType() == OPER && (
					((COperToken*)curToken)->lexem == "begin"))		//TODO(�� ������ ���)
					return;
			}
			else return; //throw PascalExcp();		// ����������� ����� �����
			descrMonotypeVars();
		}
	}
}
void Syntax::descrMonotypeVars() throw(PascalExcp, EOFExcp)
{
	// <�������� ���������� ����������>::=<���>{,<���>}:<���>
	name();
	while (checkOper(",")) {
		getNext();		//accept(",")
		name();
	}
	accept(":");
	simpleType();
}
void Syntax::simpleType() throw(PascalExcp, EOFExcp)
{
	ifNullThrowExcp();
	if (curToken->getType() == OPER &&
		(((COperToken*)curToken)->lexem == "integer" || ((COperToken*)curToken)->lexem == "string") ||
		((COperToken*)curToken)->lexem == "char" || ((COperToken*)curToken)->lexem == "real") {
		getNext();
		return;
	}
	writeMistake(324);
	throw PascalExcp();
}
void Syntax::blockFunc() {}

void Syntax::blockOpers()throw(PascalExcp, EOFExcp)
{
	// <������ ����������>::=<��������� ��������>
	compoundOper();

}
void Syntax::compoundOper() throw(PascalExcp, EOFExcp) {
	// <��������� ��������>::= begin <��������>{;<��������>} end
	ifNullThrowExcp();
	accept("begin");
	// ���������, ���� �� ��������� ��� ����� end
	if (curToken != nullptr) {
		if (curToken->getType() == OPER && ((COperToken*)curToken)->lexem == "end") {
			getNext();
			return;
		}
	}

	oper();
	while (checkOper(";")) {
		getNext();
		oper();
	}

	accept("end");
}

void Syntax::oper() throw(PascalExcp, EOFExcp) {
	// <��������>::=<������������ ��������>|<�����><������������ ��������>
	unmarkedOper();
}
void Syntax::unmarkedOper() throw(PascalExcp, EOFExcp) {
	// <������������ ��������>:: = <������� ��������> |<������� ��������>
	simpleOper();
}
void Syntax::simpleOper() throw(PascalExcp, EOFExcp) {
	// <������� ��������>::=<�������� ������������>|<�������� ���������> | <�������� ��������> |<������ ��������>
	// TODO(<������ ��������>::= <�����>::= - ��� ��� ������?)
	assignOper();
}
void Syntax::assignOper()throw(PascalExcp, EOFExcp) {
	// <�������� ������������>:: = <����������>: = <���������> |<��� �������> : = <���������>
	ifNullThrowExcp();
	name();				//TODO(����� ��� �� ����������)
	ifNullThrowExcp();
	accept(":=");
	expression();
}
void Syntax::expression() throw(PascalExcp, EOFExcp) {
	// <���������>::=<������� ���������>|<������� ���������><�������� ���������><������� ���������>
	simpleExpr();
	if (isBoolOper()) {
		getNext();		// accept
		simpleExpr();
	}
}


void Syntax::simpleExpr() throw(PascalExcp, EOFExcp) {
	//<������� ���������>:: = <����><���������>{ <���������� ��������><���������> }
	// cout <<setw(offset)<<" "<<std::left<< "Checking simple Expr" << endl;
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

void Syntax::term() throw(PascalExcp, EOFExcp)
{
	// cout <<setw(offset)<<" "<<std::left<< "Checking term" << endl; 
	offset += offsetD;
	ifNullThrowExcp();

	factor();
	while (curToken != nullptr && isMultOper()) {
		getNext();
		factor();
	}
	offset -= offsetD;
}

void Syntax::factor() throw(PascalExcp, EOFExcp)
{
	// cout <<setw(offset)<<" "<<std::left<< "Checking factor" << endl;
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
				// cout <<setw(offset)<<" "<<std::left<< "Checking func" << endl;
				offset -= offsetD;
				return;
			}

		}
		// cout <<setw(offset)<<" "<<std::left<< "Checking ident" << endl;
		getNext(); // ���� �� �������, �� ���������� 
		offset -= offsetD;
		return;
	}

	// ��������� ��� �����
	if (unsignedConst()) {
		offset -= offsetD;
		return;
	}

	// (<���������>)
	if (checkOper("(")) {
		accept("(");
		simpleExpr();
		accept(")");
	}
	else {
		writeMistake(6);
		throw PascalExcp();
	}
	offset -= offsetD;
}

bool Syntax::unsignedConst()throw(PascalExcp, EOFExcp) {
	ifNullThrowExcp();

	// ����� ��� �����
	if (curToken->getType() == VALUE && ((CValueToken*)curToken)->getVariant().getType() != CHAR) {		// TODO(� char?)
		// cout <<setw(offset)<<" "<<std::left<< "Checking unsigned Const" << endl;
		getNext();
		return true;
	}
	return false;

}
void Syntax::unsignedNum() throw(PascalExcp, EOFExcp) {
	if (curToken->getType() != VALUE)
		throw PascalExcp();
	auto tokType = ((CValueToken*)curToken)->getVariant().getType();
	if (tokType != INT && tokType != REAL)
		throw PascalExcp();
	getNext();
}

void Syntax::checkForbiddenSymbol() throw(PascalExcp, EOFExcp)
{
	if (curToken != nullptr && curToken->getType() == UNDEF) {
		writeMistake(6);
		throw PascalExcp();
	}
}

void Syntax::writeMistake(int code)
{
	erManager->addError(lexic->getStartPosition(), lexic->getCurLine(), code);
}



// "�������" �����, ��������, ��� ������� ������
void Syntax::accept(string oper) throw(PascalExcp, EOFExcp) {
	ifNullThrowExcp();

	oper = toLower(oper);
	if (curToken->getType() != OPER || ((COperToken*)curToken)->lexem != oper)
	{
		if (oper == "program") writeMistake(3);
		else if (oper == ")") writeMistake(4);
		else if (oper == ":") writeMistake(5);
		else if (oper == "OF") writeMistake(8);
		else if (oper == "(") writeMistake(9);
		else if (oper == "[") writeMistake(11);
		else if (oper == "]") writeMistake(12);
		else if (oper == "end") writeMistake(13);
		else if (oper == ";") writeMistake(14);
		else if (oper == "=") writeMistake(16);
		else if (oper == "begin") writeMistake(17);
		else if (oper == ",") writeMistake(20);
		else if (oper == ":=") writeMistake(51);
		else if (oper == "then") writeMistake(52);
		else if (oper == "until") writeMistake(53);
		else if (oper == "do") writeMistake(54);
		else if (oper == "to" || oper == "downto") writeMistake(55);
		else if (oper == "if") writeMistake(56);
		else if (oper == ".") writeMistake(61);
		else if (oper == "..") writeMistake(74);
		throw PascalExcp();
		return;
	}
	if (((COperToken*)curToken)->lexem != oper) {
		throw PascalExcp();
		return;
	}
	getNext();
}

bool Syntax::tryAccept(string oper) throw(PascalExcp, EOFExcp) {
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
	if (curToken == nullptr) return false;
	if (curToken->getType() == OPER &&
		((COperToken*)curToken)->lexem == oper)
		return true;
	return false;
}

bool Syntax::ifNullThrowExcp() throw(PascalExcp, EOFExcp)
{
	if (curToken == nullptr)
	{
		erManager->addError(lexic->getCurPos(), lexic->getCurLine(), 1000);		//TODO(�� ����� ����� ���������� ����� �����, �� ����� "��������� ;"
		throw EOFExcp();
		// throw exception("Reached eof");
	}
	return false;
}

bool Syntax::isBoolOper()
{
	// <�������� ���������>::==|<>|<|<=|>=|>|in
	if (curToken == nullptr) return false;

	if (curToken->getType() != OPER) return false;
	return ((COperToken*)curToken)->lexem == string("==") ||
		((COperToken*)curToken)->lexem == string("<>") ||
		((COperToken*)curToken)->lexem == string("<") ||
		((COperToken*)curToken)->lexem == string("<=") ||
		((COperToken*)curToken)->lexem == string(">=") ||
		((COperToken*)curToken)->lexem == string(">") ||
		((COperToken*)curToken)->lexem == string("in");
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
bool Syntax::acceptSign() throw (PascalExcp, EOFExcp) {
	ifNullThrowExcp();

	if (curToken->getType() != OPER) return false;
	if (((COperToken*)curToken)->lexem == "+" ||
		((COperToken*)curToken)->lexem == "-")
		getNext();
	return true;
}

