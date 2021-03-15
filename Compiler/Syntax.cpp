#include "Syntax.h"
#include <iomanip>
#include "FStringFunc.h"


EType Syntax::ssTypeAdapter(EVarType type) {
	switch (type) {
	case INT:
		return eINT;
	case REAL:
		return eREAL;
	case STRING:
		return eSTRING;
	case CHAR:
		return eCHAR;
	default:
		return eNONE;
	}
}

Syntax::Syntax(CErrorManager* erManager, Lexic* lexic, CSemantic* semantic) {
	this->erManager = erManager;
	this->lexic = lexic;
	this->semantic = semantic;
}

Syntax::~Syntax() {}


void Syntax::startVer() {
	try { getNext(); } catch (PascalExcp& e) {
		cout << "[!]EXCEPTION" << endl;
		return;
	} catch (EOFExcp& e) {
		cout << "[!]EOF EXCEPTION" << endl;
		return;
	}


	try { ifNullThrowExcp(); } catch (PascalExcp& e) {
		cout << "[!]EXCEPTION" << endl;
		return;
	} catch (EOFExcp& e) {
		cout << "[!]EOF EXCEPTION" << endl;
		return;
	}

	try { program(); } catch (PascalExcp& e) {
		cout << "[!]EXCEPTION" << endl;
		return;
	} catch (EOFExcp& e) {
		cout << "[!]EOF EXCEPTION" << endl;
		return;
	}
}

void Syntax::getNext() throw(PascalExcp, EOFExcp) {
	removeToken();
	curToken = this->lexic->getNext(true);
}

void Syntax::peekNext() {
	removeToken();
	curToken = this->lexic->getNext(false);
	checkForbiddenSymbol();

}

void Syntax::skip(set<string> lexemes) {
	if (curToken == nullptr) return;
	// если уже на нужном слове
	if (curToken->getType() == OPER && lexemes.find(((COperToken*)curToken)->getLexem()) != lexemes.end()) {
		return;
	}
	removeToken();
	curToken = this->lexic->skip(lexemes);
}

void Syntax::removeToken() {
	if (curToken != nullptr)
		delete curToken;
}

void Syntax::program() throw(PascalExcp, EOFExcp) {
	ifNullThrowExcp();
	set<string> nameSet = { "(", ",", ";", "const", "var", "begin" };
	set <string> branchSet = { ";", "const", "var", "begin" };
	set <string> semicolSet = { "const", "var", "begin" };
	accept("program");

	try { name(); } catch (PascalExcp& e) {
		skip(nameSet);
	}
	try { accept("("); } catch (PascalExcp& e) {
		skip(nameSet);
	}
	try { name(); } catch (PascalExcp& e) {
		skip(nameSet);
	}
	while (checkOper(",")) {
		getNext();
		try { name(); } catch (PascalExcp& e) {
			skip(nameSet);
		}
	}
	try { accept(")"); } catch (PascalExcp& e) {
		skip(branchSet);
	}
	try { accept(";"); } catch (PascalExcp& e) {
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

string Syntax::name() throw(PascalExcp, EOFExcp) {
	ifNullThrowExcp();
	if (curToken->getType() != IDENT) {
		writeMistake(2);
		throw PascalExcp();
	}
	string nameStr = ((CIdentToken*)curToken)->getLexem();
	getNext();
	return nameStr;
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
		if (((COperToken*)curToken)->getLexem() != searchingWord) {
			int oldPos = lexic->getCurPos();
			skip(searchingLexemes);
			int curNewPos = lexic->getCurPos();
			if (curNewPos != oldPos)
				writeMistake(6, oldPosInLine, oldLineNum);
			return true;
		}

	return false;
}

void Syntax::var(set<string> skippingSet)throw(PascalExcp, EOFExcp) {
	//<переменная>::=<полная переменная>|<компонента переменной> | <указанная переменная>
	ifNullThrowExcp();

	string lexem = lexic->peekNext();
	//если после имени переменной стоит "["
	if (lexem != "" && lexem == "[")
		try {
		compVar();		// indexVar()
	} catch (PascalExcp& e) {
		if (skippingSet.empty())
			throw e;
		else
			skip(skippingSet);
	} else
		fullVar();			// name()
}

void Syntax::fullVar() throw(PascalExcp, EOFExcp) {
	//<полная переменная>::=<имя переменной>
	name();
}
void Syntax::compVar()throw(PascalExcp, EOFExcp) {
	//<компонента переменной>:: = <индексированная переменная>| <обозначение поля> | <буфер файла>
	indexVar();
}
void Syntax::indexVar()throw(PascalExcp, EOFExcp) {
	// <индексированная переменная>::=<переменная-массив>[<выражение>{, <выражение>}]
	ifNullThrowExcp();
	fullVar();
	accept("[");
	set<string> skipExprSet = { ";", "]",",", "end" };
	string lexem = lexic->peekNext();
	//если после имени переменной стоит "["
	if (lexem != "" && lexem == "[")
		arrayVar();
	else {
		try {
			expression(skipExprSet);	//TODO
		} catch (PascalExcp& e) {
			writeMistake(2);
			skip(skipExprSet);
		}
	}
	while (checkOper(",")) {
		getNext();
		expression(skipExprSet);
	}
	accept("]");
}
void Syntax::arrayVar()throw(PascalExcp, EOFExcp) {
	// <переменная - массив>:: = <переменная>
	set<string> skipSet = { ";", "]",",", "end" };
	var(skipSet);
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
void Syntax::blockMarks() {}
void Syntax::blockConst() throw(PascalExcp, EOFExcp) {
	semantic->getLast()->clearBuffs();
	ifNullThrowExcp();
	set <string> constDefSet = { ";", "var", "begin", "type" };
	semantic->getLast()->setBlock(CONSTBL);

	if (checkOper("const")) {
		getNext();
		try { constDef(); 
		} catch (PascalExcp& e) {
			skip(constDefSet);
			semantic->getLast()->createNone();
		}
		semantic->getLast()->clearBuffs();

		while (checkOper(";")) {
			getNext();
			if (curToken != nullptr) {
				if (curToken->getType() == OPER && (
					((COperToken*)curToken)->getLexem() == "var" ||
					((COperToken*)curToken)->getLexem() == "type" ||
					((COperToken*)curToken)->getLexem() == "begin"))
					return;
			} else return;		// наткнулись на конец файла
			try { constDef(); 
			} catch (PascalExcp& e) {
				skip(constDefSet);
				semantic->getLast()->createNone();
			}
			semantic->getLast()->clearBuffs();
		}
	}
	semantic->getLast()->clearBuffs();
}
void Syntax::constDef()throw(PascalExcp, EOFExcp) {
	ifNullThrowExcp();
	auto constLeft = name();
	semantic->getLast()->addToNameBuffer(constLeft);
	accept("=");
	auto constRight = constanta();
	semantic->getLast()->defineConst(get<0>(constRight), get<1>(constRight));
}
pair<EType, string> Syntax::constanta()throw(PascalExcp, EOFExcp) {
	//<константа>::=<число без знака>|<знак><число без знака>|
	//<имя константы> | <знак><имя константы> | <строка>
	//<знак>
	if (acceptSign()) {
		// <число без знака>
		if (curToken->getType() == VALUE) {
			unsignedNum();
		} else {
			// <имя константы>
			return std::make_pair(eNONE, name());
		}
	} else {
		// <имя константы>
		if (curToken->getType() == IDENT) {
			return  std::make_pair(eNONE, name());
		}
		// <строка>
		if (curToken->getType() == VALUE &&((
			((CValueToken*)curToken)->getVariant().getType() == STRING) || ((CValueToken*)curToken)->getVariant().getType() == CHAR)) {
			auto constStr = ((CValueToken*)curToken)->getVariant().getLexem();
			auto resPair = std::make_pair(ssTypeAdapter(((CValueToken*)curToken)->getVariant().getType()), constStr);
			getNext();		// приняли строку
			return resPair;
		}
		// <число без знака>
		try {
			return unsignedNum();		// кинет ошибку
		} catch (PascalExcp& e) {
			writeMistake(50);
			throw e;
		}
	}
}
void Syntax::blockTypes() {
	//<раздел типов>::=<пусто>|type <определение типа>;{<определение типа>; }
	semantic->getLast()->setBlock(TYPEBL);
	semantic->getLast()->clearBuffs();
	ifNullThrowExcp();
	set <string> skipSet = { ";", "begin", "var" };
	if (checkOper("type")) {
		getNext();		// accept("type");
		try { typeDef(); }		//<определение типа>
		catch (PascalExcp& e) {
			skip(skipSet);
			semantic->getLast()->createNone();
		}
		semantic->getLast()->clearBuffs();
		// {<определение типа>;}
		while (checkOper(";")) {
			getNext();
			if (curToken != nullptr) {
				if (curToken->getType() == OPER && (
					((COperToken*)curToken)->getLexem() == "begin" ||
					((COperToken*)curToken)->getLexem() == "var"))
					return;
			} else return; //throw PascalExcp();		// неожиданный конец файла
			try { typeDef(); } catch (PascalExcp& e) {
				skip(skipSet);
				semantic->getLast()->createNone();
			}
			semantic->getLast()->clearBuffs();
		}
	}
}

void Syntax::typeDef()throw(PascalExcp, EOFExcp) {
	//<определение типа>::=<имя>=<тип>
	semantic->getLast()->addToNameBuffer(name());
	accept("=");
	auto typeName = type();
	semantic->getLast()->addToBuffer(typeName);
}
string Syntax::type()throw(PascalExcp, EOFExcp) {
	// <тип>:: = <простой тип> | <составной тип> | <ссылочный тип>
	// <тип>:: = простой тип или массив
	ifNullThrowExcp();
	if (checkOper("array")) {
		regularType();
	} else {
		return simpleType();
	}
	return "";

}
void Syntax::regularType()  throw(PascalExcp, EOFExcp) {
	//<регулярный тип>:: = array[<простой тип>{, <простой тип>}] of <тип компоненты >
	accept("array");
	accept("[");
	simpleType();
	while (checkOper(",")) {
		getNext();
		simpleType();
	}
	accept("]");
	accept("of");
	type();
}
string Syntax::simpleType() throw(PascalExcp, EOFExcp) {
	//<простой тип>::=<перечислимый тип>|<ограниченный тип>|<имя типа>
	ifNullThrowExcp();
	//<перечислимый тип>
	try {
		if (checkOper("(")) {
			enumaratedType();
		} else {
			// заглядываем на один токен вперед и проверяем 
			// является ли он ".."
			string nextLexem = lexic->peekNext();
			if (nextLexem != "" && nextLexem == "..")		//<ограниченный тип>
			{
				limitedType();
			} else {
				// заглядываем еще на один токен вперед и проверяем 
				// является ли он ".."
				nextLexem = lexic->peek2Next();
				if (nextLexem != "" && nextLexem == "..")		//<ограниченный тип>
				{
					limitedType();
				} else
					return name();	//<имя типа>
			}
		}
	} catch (PascalExcp& e) {
		writeMistake(324);
		throw e;
	}
	return "";
	//writeMistake(324);
}
void Syntax::enumaratedType()throw(PascalExcp, EOFExcp) {
	// <перечислимый тип>::=(<имя>{,<имя>})
	accept("(");
	name();
	while (checkOper(",")) {
		getNext();
		name();
	}
	accept(")");
}
void Syntax::limitedType()throw(PascalExcp, EOFExcp) {
	//<ограниченный тип>::=<константа>..<константа>
	constanta();
	accept("..");
	constanta();
}

void Syntax::blockVars() throw(PascalExcp, EOFExcp) {
	semantic->getLast()->setBlock(VARBL);
	semantic->getLast()->clearBuffs();
	//<раздел переменных>::= var <описание однотипных переменных>;{<описание однотипных переменных>; } | <пусто>
	ifNullThrowExcp();
	set <string> skipSet = { ";", "begin" };
	// var <описание однотипных переменных>;
	if (checkOper("var")) {
		getNext();		// accept("var");
		try { descrMonotypeVars(); } catch (PascalExcp& e) {
			skip(skipSet);
			semantic->getLast()->createNone();
		}
		semantic->getLast()->clearBuffs();
		// {<описание однотипных переменных>;}
		while (checkOper(";")) {
			getNext();
			if (curToken != nullptr) {
				if (curToken->getType() == OPER && (
					((COperToken*)curToken)->getLexem() == "begin"))		//TODO(не только эти)
					return;
			} else return; //throw PascalExcp();		// неожиданный конец файла
			try { descrMonotypeVars(); 
			} catch (PascalExcp& e) {
				skip(skipSet);
				semantic->getLast()->createNone();
			}
			semantic->getLast()->clearBuffs();
		}
	}
}
void Syntax::descrMonotypeVars() throw(PascalExcp, EOFExcp) {
	// <описание однотипных переменных>::=<имя>{,<имя>}:<тип>
	set <string> constDefSet = { ",", ":", ";", "begin" };
	try { 
		semantic->getLast()->addToNameBuffer(name());
	} catch (PascalExcp& e) {
		skip(constDefSet);
	}
	while (checkOper(",")) {
		getNext();		//accept(",")
		try { 
			semantic->getLast()->addToNameBuffer(name());
		} catch (PascalExcp& e) {
			skip(constDefSet);
		}
	}
	accept(":");
	auto typeName = type();
	semantic->getLast()->addToBuffer(typeName);
}

void Syntax::blockFunc() {}

void Syntax::blockOpers()throw(PascalExcp, EOFExcp) {
	semantic->getLast()->setBlock(BODYBL);
	semantic->getLast()->clearBuffs();
	// <раздел операторов>::=<составной оператор>
	set<string> skipSet = { "end" };
	try { compoundOper(skipSet); } catch (PascalExcp& e) {
		skip(skipSet);
	}
}
void Syntax::diffOper(set<string> skippingSet)throw(PascalExcp, EOFExcp) {
	// <сложный оператор>::=<составной оператор>|<выбирающий оператор>|
	// <оператор цикла> |<оператор присоединения>
	ifNullThrowExcp();
	if (curToken->getType() == OPER && (checkOper("if") || checkOper("while"))) {
		if (checkOper("if")) {
			try { ifOper(skippingSet); } catch (PascalExcp& e) {
				skip(skippingSet);
			}
		} else
			if (checkOper("while")) {
				try { whileOper(skippingSet); } catch (PascalExcp& e) {
					skip(skippingSet);
				}
			}
	} else {
		try {
			if (skippingSet.find("end") == skippingSet.end()) skippingSet.insert("end");
			compoundOper(skippingSet);
		} catch (PascalExcp& e) {
			skip(skippingSet);
			cout << "compound excp" << endl;
		}

	}
}
void Syntax::ifOper(set<string> skippingSet) throw(PascalExcp, EOFExcp) {
	// <условный оператор>::= if <выражение> then <оператор>|
	// if <выражение> then <оператор> else <оператор>
	ifNullThrowExcp();
	set<string> skipIfSet(skippingSet);	// copy set
	skipIfSet.insert("then");
	accept("if");
	try { expression(skipIfSet); } catch (PascalExcp& e) {
		skip(skipIfSet);
	}
	accept("then");
	set<string> skipThenSet(skippingSet);
	skipThenSet.insert("else");
	try { oper(skipThenSet); } catch (PascalExcp& e) {
		skip(skipThenSet);
	}
	if (checkOper("else")) {
		accept("else");
		try { oper(skippingSet); } catch (PascalExcp& e) {
			skip(skippingSet);
		}
	}

}
void Syntax::whileOper(set<string> skippingSet) throw(PascalExcp, EOFExcp) {
	//<цикл с предусловием>::= while <выражение> do <оператор>
	ifNullThrowExcp();
	set<string> skipIfSet(skippingSet);	// copy set
	skipIfSet.insert("do");
	accept("while");
	try { expression(skipIfSet); } catch (PascalExcp& e) {
		skip(skipIfSet);
	}
	accept("do");
	try { oper(skippingSet); } catch (PascalExcp& e) {
		skip(skippingSet);
	}
}
void Syntax::compoundOper(set<string> skippingSet) throw(PascalExcp, EOFExcp) {
	// <составной оператор>::= begin <оператор>{;<оператор>} end
	ifNullThrowExcp();
	accept("begin");
	// проверяем, есть ли операторы или сразу end
	if (curToken != nullptr) {
		if (curToken->getType() == OPER && ((COperToken*)curToken)->getLexem() == "end") {
			getNext();
			return;
		}
	}
	skippingSet.insert(";");
	try { oper(skippingSet); } catch (PascalExcp& e) {
		// writeMistake(6);
		skip(skippingSet);
	}

	while (!checkOper("end")) {
		try { accept(";"); } catch (PascalExcp& e) {}
		//TODO(костыль, поскольку else никто, кроме if, не съедает и на нем зацикливание)
		if (checkOper("else")) {
			writeMistake(6);
			getNext();
		}
		try { oper(skippingSet); } catch (PascalExcp& e) {
			// writeMistake(6);
			skip(skippingSet);
		}
	}

	accept("end");
}

void Syntax::oper(set<string> skippingSet) throw(PascalExcp, EOFExcp) {
	// <оператор>::=<непомеченный оператор>|<метка><непомеченный оператор>
	unmarkedOper(skippingSet);
}
void Syntax::unmarkedOper(set<string> skippingSet) throw(PascalExcp, EOFExcp) {
	// <непомеченный оператор>:: = <простой оператор> |<сложный оператор>
	ifNullThrowExcp();
	// если ;, то в простом операторе есть <пусто>
	if (curToken->getType() == OPER && (checkOper("begin") || checkOper("if") || checkOper("while"))) {
		{
			try { diffOper(skippingSet); } catch (PascalExcp& e) {
				skippingSet.insert("end");
				skip(skippingSet);
			}
		}
	} else
		//TODO(не уверена насчет обработки неож оператора)
		if (curToken->getType() != OPER || ((COperToken*)curToken)->getLexem() == ";" ||
			((COperToken*)curToken)->getLexem() == "end")
			try { simpleOper(skippingSet); } catch (PascalExcp& e) {
			skip(skippingSet);
		}
	// встречен неожиданный оператор
		else {
			writeMistake(6);
			// cout << "Not sure" << endl;
			throw PascalExcp();
		}
}

void Syntax::simpleOper(set<string> skippingSet) throw(PascalExcp, EOFExcp) {
	// <простой оператор>::=<оператор присваивания>|<оператор процедуры> | <оператор перехода> |<пустой оператор>
	// TODO(<пустой оператор>::= <пусто>::= - что это вообще?)
	ifNullThrowExcp();
	// <пусто>
	if (curToken->getType() == OPER && (checkOper(";") || checkOper("end")))
		return;
	assignOper(skippingSet);
}
void Syntax::assignOper(set<string> skippingSet)throw(PascalExcp, EOFExcp) {
	// <оператор присваивания>:: = <переменная>: = <выражение> |<имя функции> : = <выражение>
	ifNullThrowExcp();
	set<string> s;
	var(s);
	ifNullThrowExcp();
	accept(":=");
	expression(skippingSet);
}
void Syntax::expression(set<string> skippingSet) throw(PascalExcp, EOFExcp) {
	// <выражение>::=<простое выражение>|<простое выражение><операция отношения><простое выражение>
	try { simpleExpr(); } catch (PascalExcp& e) {
		skip(skippingSet);
	}
	if (isBoolOper()) {
		getNext();		// accept
		try { simpleExpr(); } catch (PascalExcp& e) {
			skip(skippingSet);
		}
	}
}


void Syntax::simpleExpr() throw(PascalExcp, EOFExcp) {
	//<простое выражение>:: = <знак><слагаемое>{ <аддитивная операция><слагаемое> }
	ifNullThrowExcp();

	acceptSign();
	term();

	// то, что в {}
	while (isAdditiveOper()) {
		getNext();
		term();
	}
}

void Syntax::term() throw(PascalExcp, EOFExcp) {
	// cout <<setw(offset)<<" "<<std::left<< "Checking term" << endl; 
	ifNullThrowExcp();

	factor();
	while (curToken != nullptr && isMultOper()) {
		getNext();
		factor();
	}
}

EType Syntax::factor() throw(PascalExcp, EOFExcp) {
	//<множитель>::=<переменная>|<константа без знака>|
	//(<выражение>) | <обозначение функции> | <множество> | not <множитель>
	ifNullThrowExcp();

	if (checkOper("not")) {		// not <множитель>
		getNext();
		return factor();
	}
	if (curToken->getType() == VALUE) {		// константа без знака
		auto curTokenType = ((CValueToken*)curToken)->getVariant().getType();
		getNext();
		return semantic->getLast()->defineType(curTokenType, "");
	}

	if (checkOper("(")) {		// (<выражение>)
		accept("(");
		auto curTokenType = simpleExpr();
		accept(")");
		return semantic->getLast()->defineType(curTokenType, "");
	}
	var(set<string>());						// <переменная>
}

pair<EType, string> Syntax::unsignedNum() throw(PascalExcp, EOFExcp) {
	if (curToken->getType() != VALUE) {
		throw PascalExcp();
	}
	auto tokType = ((CValueToken*)curToken)->getVariant().getType();
	if (tokType != INT && tokType != REAL) {
		throw PascalExcp();
	}
	auto resPair = std::make_pair(ssTypeAdapter(tokType), ((CValueToken*)curToken)->getVariant().getLexem());
	getNext();
	return resPair;
}

void Syntax::checkForbiddenSymbol() throw(PascalExcp, EOFExcp) {
	if (curToken != nullptr && curToken->getType() == UNDEF) {
		writeMistake(6);
		throw PascalExcp();
	}
}

void Syntax::writeMistake(int code) {
	cout << "Mistake " << code << ": " << lexic->getStartPosition() << endl;
	erManager->addError(lexic->getStartPosition(), lexic->getCurLine(), code);
}

void Syntax::writeMistake(int code, int pos, int oldLineNum) {
	cout << "Mistake2 " << code << ": " << pos << endl;
	erManager->addError(pos, oldLineNum, code);
}


// "съедаем" токен, проверяя, что лексема нужная
void Syntax::accept(string oper) throw(PascalExcp, EOFExcp) {
	ifNullThrowExcp();

	oper = toLower(oper);
	if (curToken->getType() != OPER || ((COperToken*)curToken)->getLexem() != oper) {
		if (oper == "program") writeMistake(3);
		else if (oper == ")") writeMistake(4);
		else if (oper == ":") writeMistake(5);
		else if (oper == "OF") writeMistake(8);
		else if (oper == "(") writeMistake(9);
		else if (oper == "[") writeMistake(11);
		else if (oper == "]") writeMistake(12);
		else if (oper == "end") writeMistake(13);
		else if (oper == ";") writeMistake(14);			// ; записывается перед след. лексемой
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
	if (((COperToken*)curToken)->getLexem() != oper) {
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
	if (((COperToken*)curToken)->getLexem() != oper) {
		return false;
	}
	getNext();
	return true;
}

bool Syntax::checkOper(string oper) {
	if (curToken == nullptr) return false;
	if (curToken->getType() == OPER &&
		((COperToken*)curToken)->getLexem() == oper)
		return true;
	return false;
}

bool Syntax::ifNullThrowExcp() throw(PascalExcp, EOFExcp) {
	if (curToken == nullptr) {
		erManager->addError(lexic->getCurPosInLine(), lexic->getCurLine(), 1000);		//TODO(по факту здесь достижение конца файла, но кинем "ожидалась ;"
		throw EOFExcp();
		// throw exception("Reached eof");
	}
	return false;
}

bool Syntax::isBoolOper() {
	// <операция отношения>::==|<>|<|<=|>=|>|in
	if (curToken == nullptr) return false;

	if (curToken->getType() != OPER) return false;
	return ((COperToken*)curToken)->getLexem() == string("=") ||
		((COperToken*)curToken)->getLexem() == string("<>") ||
		((COperToken*)curToken)->getLexem() == string("<") ||
		((COperToken*)curToken)->getLexem() == string("<=") ||
		((COperToken*)curToken)->getLexem() == string(">=") ||
		((COperToken*)curToken)->getLexem() == string(">") ||
		((COperToken*)curToken)->getLexem() == string("in");
}

bool Syntax::isAdditiveOper() {
	if (curToken == nullptr) return false;

	if (curToken->getType() != OPER) return false;
	return ((COperToken*)curToken)->getLexem() == string("+") ||
		((COperToken*)curToken)->getLexem() == string("-") ||
		((COperToken*)curToken)->getLexem() == string("or");
}

bool Syntax::isMultOper() {
	if (curToken == nullptr) return false;

	if (curToken->getType() != OPER) return false;
	return ((COperToken*)curToken)->getLexem() == string("*") ||
		((COperToken*)curToken)->getLexem() == string("/") ||
		((COperToken*)curToken)->getLexem() == string("div") ||
		((COperToken*)curToken)->getLexem() == string("mod") ||
		((COperToken*)curToken)->getLexem() == string("and");
}


// "съедаем" знак, если он есть
bool Syntax::acceptSign() throw (PascalExcp, EOFExcp) {
	ifNullThrowExcp();

	if (curToken->getType() != OPER) return false;
	if (((COperToken*)curToken)->getLexem() == "+" ||
		((COperToken*)curToken)->getLexem() == "-")
		getNext();
	return true;
}

