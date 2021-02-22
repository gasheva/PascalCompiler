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
	// если уже на нужном слове
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
	// проверка, что после end. больше ничего нет
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

bool Syntax::skipUntilBlock(set<string> searchingLexemes, string searchingWord) throw(PascalExcp, EOFExcp) {
	// проверяем находимся ли на лексема const, 
	// если нет, пытаемся скипнуть до const или до начала следующих блоков
	ifNullThrowExcp();
	int oldPosInLine = lexic->getStartPosition();
	int oldLineNum = lexic->getCurLine();
	
	if (curToken->getType() != OPER) {
		skip(searchingLexemes);
		writeMistake(6, oldPosInLine, oldLineNum);
		return true;
	}
	if (curToken->getType() == OPER)
		if (((COperToken*)curToken)->lexem != searchingWord) {
			int oldPos = lexic->getCurPos();
			skip(searchingLexemes);
			int curNewPos = lexic->getCurPos();
			if (curNewPos!= oldPos)
				writeMistake(6, oldPosInLine, oldLineNum);
			return true;
		}

	return false;
}

void Syntax::block() throw(PascalExcp, EOFExcp) {
	ifNullThrowExcp();
	set <string> blockConstSet = { "const", "type", "var", "begin" };
	set <string> blockTypeSet = { "type", "var", "begin" };
	set <string> blockVarsSet = { "var","begin" };
	set <string> blockOpersSet = { "begin" };

	blockMarks();

	ifNullThrowExcp();
	skipUntilBlock(blockConstSet, "const");		// пропускаем символы до начала какого-н блока (любого)
	blockConst();

	ifNullThrowExcp();
	skipUntilBlock(blockTypeSet, "type");
	blockTypes(); 

	ifNullThrowExcp();
	skipUntilBlock(blockVarsSet, "var");
	blockVars();

	blockFunc();

	ifNullThrowExcp();
	skipUntilBlock(blockOpersSet, "begin");
	blockOpers();

}
void Syntax::blockMarks()
{
}
void Syntax::blockConst() throw(PascalExcp, EOFExcp) {
	ifNullThrowExcp();		//TODO(кидать не кидать)
	set <string> constDefSet = { ";", "var", "begin", "type" };

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
					((COperToken*)curToken)->lexem == "var" ||	
					((COperToken*)curToken)->lexem == "type" ||		
					((COperToken*)curToken)->lexem == "begin"))		
					return;
			}
			else return;		// наткнулись на конец файла
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
	//<знак>
	if (acceptSign()) {
		// <число без знака>
		try {
			unsignedNum();
		}
		catch (PascalExcp& e) {
			try {
				// <имя константы>
				name();
			}
			catch (PascalExcp& e) {
				throw e;		// варианты начинания со знака закончились
			}
		}
	}
	else {
		// <имя константы>
		if (curToken->getType() == IDENT) {
			getNext();
			return;
		}
		// <строка>
		if (curToken->getType() == VALUE &&
			((CValueToken*)curToken)->getVariant().getType() == STRING) {
			getNext();		// приняли строку
			return;
		}

		// <число без знака>
		unsignedNum();		// кинет ошибку
	}
}
void Syntax::blockTypes() {
	//<раздел типов>::=<пусто>|type <определение типа>;{<определение типа>; }
	ifNullThrowExcp();
	set <string> skipSet = { ";", "begin", "var" };
	if (checkOper("type")) {
		getNext();		// accept("type");
		try { typeDef(); }		//<определение типа>
		catch (PascalExcp& e) {
			skip(skipSet);
		}
		// {<определение типа>;}
		while (checkOper(";")) {
			getNext();
			if (curToken != nullptr) {
				if (curToken->getType() == OPER && (
					((COperToken*)curToken)->lexem == "begin" ||
					((COperToken*)curToken)->lexem == "var"))
					return;
			}
			else return; //throw PascalExcp();		// неожиданный конец файла
			try { typeDef(); }
			catch (PascalExcp& e) {
				skip(skipSet);
			}
		}
	}
}

void Syntax::typeDef()throw(PascalExcp, EOFExcp) {

}
void Syntax::blockVars() throw(PascalExcp, EOFExcp)
{
	//<раздел переменных>::= var <описание однотипных переменных>;{<описание однотипных переменных>; } | <пусто>
	ifNullThrowExcp();
	set <string> skipSet = { ";", "begin"};
	// var <описание однотипных переменных>;
	if (checkOper("var")) {
		getNext();		// accept("var");
		try { descrMonotypeVars(); }
		catch (PascalExcp& e) {
			skip(skipSet);
		}
		// {<описание однотипных переменных>;}
		while (checkOper(";")) {
			getNext();
			if (curToken != nullptr) {
				if (curToken->getType() == OPER && (
					((COperToken*)curToken)->lexem == "begin"))		//TODO(не только эти)
					return;
			}
			else return; //throw PascalExcp();		// неожиданный конец файла
			try { descrMonotypeVars(); }
			catch (PascalExcp& e) {
				skip(skipSet);
			}
		}
	}
}
void Syntax::descrMonotypeVars() throw(PascalExcp, EOFExcp)
{	
	// <описание однотипных переменных>::=<имя>{,<имя>}:<тип>
	set <string> constDefSet = { ",", ":", ";", "begin", "integer", "real", "string", "char" };
	try { name(); }
	catch (PascalExcp& e) {
		skip(constDefSet);
	}
	while (checkOper(",")) {
		getNext();		//accept(",")
		try { name(); }
		catch (PascalExcp& e) {
			skip(constDefSet);
		}
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
	// <раздел операторов>::=<составной оператор>
	compoundOper();

}
void Syntax::diffOper()throw(PascalExcp, EOFExcp) {
	// <сложный оператор>::=<составной оператор>|<выбирающий оператор>|
	// <оператор цикла> |<оператор присоединения>
	ifNullThrowExcp();
	if (curToken->getType() == OPER) {
		if (((COperToken*)curToken)->lexem == "if") {
			ifOper();
		}
		else
			if (((COperToken*)curToken)->lexem == "while") {
				whileOper();
			}
	}
	try {
		compoundOper();
	}
	catch (PascalExcp& e) {

	}
}
void Syntax::ifOper() throw(PascalExcp, EOFExcp) {
	ifNullThrowExcp();
	accept("if");
	expression();
	accept("then");
	oper();
	if (checkOper("else")) {
		accept("else");
		oper();
	}
	
}
void Syntax::whileOper() throw(PascalExcp, EOFExcp) {

}
void Syntax::compoundOper() throw(PascalExcp, EOFExcp) {
	// <составной оператор>::= begin <оператор>{;<оператор>} end
	ifNullThrowExcp();
	set <string> skipSet = { ";", "end" };
	accept("begin");
	// проверяем, есть ли операторы или сразу end
	if (curToken != nullptr) {
		if (curToken->getType() == OPER && ((COperToken*)curToken)->lexem == "end") {
			getNext();
			return;
		}
	}
	try { oper(); }
	catch (PascalExcp& e) {
		skip(skipSet);
	}
	while (!checkOper("end")) {
		try{ accept(";"); }
		catch(PascalExcp &e){}
		//getNext();
		try { oper(); }
		catch (PascalExcp& e) {
			skip(skipSet);
		}
	}

	accept("end");
}

void Syntax::oper() throw(PascalExcp, EOFExcp) {
	// <оператор>::=<непомеченный оператор>|<метка><непомеченный оператор>
	unmarkedOper();
}
void Syntax::unmarkedOper() throw(PascalExcp, EOFExcp) {
	// <непомеченный оператор>:: = <простой оператор> |<сложный оператор>
	ifNullThrowExcp();
	if (curToken->getType() == OPER) {
		if (((COperToken*)curToken)->lexem == "begin") {
			compoundOper();
		}
		else
			if (((COperToken*)curToken)->lexem == "if") {
				ifOper();
			}
			else if (((COperToken*)curToken)->lexem == "while") {
				whileOper();
			}
	}
	else
		simpleOper();
}

void Syntax::simpleOper() throw(PascalExcp, EOFExcp) {
	// <простой оператор>::=<оператор присваивания>|<оператор процедуры> | <оператор перехода> |<пустой оператор>
	// TODO(<пустой оператор>::= <пусто>::= - что это вообще?)
	
	ifNullThrowExcp();
	// <пусто>
	if (curToken->getType() == OPER && (checkOper(";")||checkOper("end")))
		return;
	assignOper();
}
void Syntax::assignOper()throw(PascalExcp, EOFExcp) {
	// <оператор присваивания>:: = <переменная>: = <выражение> |<имя функции> : = <выражение>
	ifNullThrowExcp();
	name();				//TODO(здесь как бы переменная)
	ifNullThrowExcp();
	accept(":=");
	expression();
}
void Syntax::expression() throw(PascalExcp, EOFExcp) {
	// <выражение>::=<простое выражение>|<простое выражение><операция отношения><простое выражение>
	simpleExpr();
	if (isBoolOper()) {
		getNext();		// accept
		simpleExpr();
	}
}


void Syntax::simpleExpr() throw(PascalExcp, EOFExcp) {
	//<простое выражение>:: = <знак><слагаемое>{ <аддитивная операция><слагаемое> }
	// cout <<setw(offset)<<" "<<std::left<< "Checking simple Expr" << endl;
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


	if (checkOper("not")) {						// not <множитель>
		getNext();
		offset -= offsetD;
		factor();
		return;
	}

	if (curToken->getType() == IDENT) {			// функция TODO(проверка параметров)
		peekNext();
		if (curToken != nullptr) {
			if (tryAccept("(")) {
				// = accept("("), поскольку до этого было непринятое значение
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
		getNext(); // если не функция, то переменная 
		offset -= offsetD;
		return;
	}

	// константа без знака
	if (unsignedConst()) {
		offset -= offsetD;
		return;
	}

	// (<выражение>)
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

	// число без знака
	if (curToken->getType() == VALUE && ((CValueToken*)curToken)->getVariant().getType() != CHAR) {		// TODO(а char?)
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

void Syntax::writeMistake(int code, int pos, int oldLineNum)
{
	erManager->addError(pos, oldLineNum, code);
}

void Syntax::writeMistakeCurPos(int code)
{
	erManager->addError(lexic->getCurPosInLine(), lexic->getCurLine(), code);
}



// "съедаем" токен, проверяя, что лексема нужная
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
		erManager->addError(lexic->getCurPosInLine(), lexic->getCurLine(), 1000);		//TODO(по факту здесь достижение конца файла, но кинем "ожидалась ;"
		throw EOFExcp();
		// throw exception("Reached eof");
	}
	return false;
}

bool Syntax::isBoolOper()
{
	// <операция отношения>::==|<>|<|<=|>=|>|in
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


// "съедаем" знак, если он есть
bool Syntax::acceptSign() throw (PascalExcp, EOFExcp) {
	ifNullThrowExcp();

	if (curToken->getType() != OPER) return false;
	if (((COperToken*)curToken)->lexem == "+" ||
		((COperToken*)curToken)->lexem == "-")
		getNext();
	return true;
}

