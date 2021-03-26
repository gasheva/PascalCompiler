#include "CCompiler.h"
#include <iomanip>
#include "FStringFunc.h"



EType CCompiler::ssTypeAdapter(EVarType type) {
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

CCompiler::CCompiler(CErrorManager* erManager, CLexic* lexic, CSemantic* semantic, CCodeGen* codeGen) {
	this->erManager = erManager;
	this->lexic = lexic;
	this->semantic = semantic;
	this->codeGen = codeGen;
}

CCompiler::~CCompiler() {}


void CCompiler::startVer() {
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

void CCompiler::getNext() throw(PascalExcp, EOFExcp) {
	removeToken();
	curToken = this->lexic->getNext(true);
}

void CCompiler::peekNext() {
	removeToken();
	curToken = this->lexic->getNext(false);
	checkForbiddenSymbol();

}

void CCompiler::skip(set<string> lexemes) {
	if (curToken == nullptr) return;
	// если уже на нужном слове
	if (curToken->getType() == OPER && lexemes.find(((COperToken*)curToken)->getLexem()) != lexemes.end()) {
		return;
	}
	removeToken();
	curToken = this->lexic->skip(lexemes);
}

void CCompiler::removeToken() {
	if (curToken != nullptr)
		delete curToken;
}

void CCompiler::program() throw(PascalExcp, EOFExcp) {
	ifNullThrowExcp();
	set<string> nameSet = { "(", ",", ";", "const", "var", "begin" };
	set <string> branchSet = { ";", "const", "var", "begin" };
	set <string> semicolSet = { "const", "var", "begin" };
	accept("program");

	string programName = "";
	try { programName = name(); } catch (PascalExcp& e) {
		skip(nameSet);
	}
	codeGen->openFile("", programName);

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
	codeGen->stackWriteBegin();

	block();

	accept(".");		// end.
	// проверка, что после end. больше ничего нет
	if (curToken != nullptr) {
		writeMistake(1000);
		throw PascalExcp();
	}
	// === ТЕСТИРОВАНИЕ. ВЫВОД ЗНАЧЕНИЙ В ГЕНЕРАЦИИ  ====
	codeGen->stackPrint("b3", eBOOLEAN);

	codeGen->stackWriteEnd();
	codeGen->closeFile();
}

string CCompiler::name() throw(PascalExcp, EOFExcp) {
	ifNullThrowExcp();
	if (curToken->getType() != IDENT) {
		writeMistake(2);
		throw PascalExcp();
	}
	string nameStr = ((CIdentToken*)curToken)->getLexem();
	getNext();
	return nameStr;
}

bool CCompiler::skipUntilBlock(set<string> searchingLexemes, string searchingWord) throw(PascalExcp, EOFExcp) {
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

string CCompiler::var(set<string> skippingSet)throw(PascalExcp, EOFExcp) {
	//<переменная>::=<полная переменная>|<компонента переменной> | <указанная переменная>
	ifNullThrowExcp();
	return fullVar();			// name()
}

string CCompiler::fullVar() throw(PascalExcp, EOFExcp) {
	//<полная переменная>::=<имя переменной>
	return name();
}

void CCompiler::block() throw(PascalExcp, EOFExcp) {
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
void CCompiler::blockMarks() {}
void CCompiler::blockConst() throw(PascalExcp, EOFExcp) {
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
void CCompiler::constDef()throw(PascalExcp, EOFExcp) {
	ifNullThrowExcp();
	auto constLeft = name();
	semantic->getLast()->addToNameBuffer(constLeft);
	accept("=");
	auto constRight = constanta();
	semantic->getLast()->defineConst(get<0>(constRight), get<1>(constRight));

	// add variables from buffer to gen
	auto names = semantic->getLast()->getNamesBuff();
	auto curType = semantic->getLast()->getBuffType();
	string sign = "";
	if (constRight.second[0] == '-') {
		sign = "-";
		constRight.second.erase(0, 1);
	}
		
	for (auto name : names) {
		codeGen->stackInitVar(curType, name);
		if (curType == eNONE || (curType==eBOOLEAN && constRight.second!="true" && constRight.second!="false")) {
			// если справа другая константа, то в генераторе кладем ее значение в стек и переписываем в новую
			codeGen->stackLdloc(constRight.second, curType);

		}
		else {
			// если справа значение, то кладем значение.
			// если это был true\false
			if (curType == eBOOLEAN)
				if (constRight.second == "true")
					codeGen->stackLdcNum(eINT, "1");
				else
						codeGen->stackLdcNum(eINT, "0");
			else
				codeGen->stackLdcNum(curType, constRight.second);		// кладем в стек
			
		}
		codeGen->stackSign(sign);		// кладем знак
		codeGen->stackStloc(constLeft, curType);						// читаем в константу из стека
	}
}
pair<EType, string> CCompiler::constanta()throw(PascalExcp, EOFExcp) {
	//<константа>::=<число без знака>|<знак><число без знака>|
	//<имя константы> | <знак><имя константы> | <строка>
	//<знак>
	auto sign = acceptSign() ;
	if (sign != "") {
		// <число без знака>
		if (curToken->getType() == VALUE) {
			try {
				auto res = unsignedNum();
				if (sign=="-")
					res.second = sign + res.second;
				return res;
			} catch (PascalExcp& e) {
				writeMistake(50);
				throw e;
			}
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
			return unsignedNum();
		} catch (PascalExcp& e) {
			writeMistake(50);
			throw e;
		}
	}
}
void CCompiler::blockTypes() {
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

void CCompiler::typeDef()throw(PascalExcp, EOFExcp) {
	//<определение типа>::=<имя>=<тип>
	semantic->getLast()->addToNameBuffer(name());
	accept("=");
	auto typeName = type();
	semantic->getLast()->addToBuffer(typeName);
}
string CCompiler::type()throw(PascalExcp, EOFExcp) {
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
void CCompiler::regularType()  throw(PascalExcp, EOFExcp) {
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
string CCompiler::simpleType() throw(PascalExcp, EOFExcp) {
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
void CCompiler::enumaratedType()throw(PascalExcp, EOFExcp) {
	// <перечислимый тип>::=(<имя>{,<имя>})
	accept("(");
	name();
	while (checkOper(",")) {
		getNext();
		name();
	}
	accept(")");
}
void CCompiler::limitedType()throw(PascalExcp, EOFExcp) {
	//<ограниченный тип>::=<константа>..<константа>
	constanta();
	accept("..");
	constanta();
}

void CCompiler::blockVars() throw(PascalExcp, EOFExcp) {
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
					((COperToken*)curToken)->getLexem() == "begin"))
					return;
			} else return; //throw PascalExcp();		// неожиданный конец файла
			try { 
				descrMonotypeVars();
			} catch (PascalExcp& e) {
				skip(skipSet);
				semantic->getLast()->createNone();
			}
			semantic->getLast()->clearBuffs();

		}
	}
}
void CCompiler::descrMonotypeVars() throw(PascalExcp, EOFExcp) {
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


	// add variables from buffer to gen
	auto names = semantic->getLast()->getNamesBuff();
	auto curType = semantic->getLast()->getBuffType();
	for (auto name: names) {
		codeGen->stackInitVar(curType, name);
	}
}

void CCompiler::blockFunc() {}

void CCompiler::blockOpers()throw(PascalExcp, EOFExcp) {
	semantic->getLast()->setBlock(BODYBL);
	semantic->getLast()->clearBuffs();
	// <раздел операторов>::=<составной оператор>
	set<string> skipSet = { "end" };
	try { compoundOper(skipSet); } catch (PascalExcp& e) {
		skip(skipSet);
	}
}
void CCompiler::diffOper(set<string> skippingSet)throw(PascalExcp, EOFExcp) {
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
void CCompiler::ifOper(set<string> skippingSet) throw(PascalExcp, EOFExcp) {
	// <условный оператор>::= if <выражение> then <оператор>|
	// if <выражение> then <оператор> else <оператор>
	ifNullThrowExcp();
	set<string> skipIfSet(skippingSet);	// copy set
	skipIfSet.insert("then");
	accept("if");
	try { expression(skipIfSet); } catch (PascalExcp& e) {
		skip(skipIfSet);
	}
	codeGen->stackIf();

	accept("then");
	set<string> skipThenSet(skippingSet);
	skipThenSet.insert("else");
	try { oper(skipThenSet); } catch (PascalExcp& e) {
		skip(skipThenSet);
	}
	codeGen->stackThenEnd();
	codeGen->stackElse();

	if (checkOper("else")) {
		accept("else");
		try { oper(skippingSet); } catch (PascalExcp& e) {
			skip(skippingSet);
		}
	}
	codeGen->stackIfEnd();
}
void CCompiler::whileOper(set<string> skippingSet) throw(PascalExcp, EOFExcp) {
	//<цикл с предусловием>::= while <выражение> do <оператор>
	ifNullThrowExcp();
	set<string> skipIfSet(skippingSet);	// copy set
	skipIfSet.insert("do");
	accept("while");
	codeGen->stackWhileExpr();

	try { expression(skipIfSet); } catch (PascalExcp& e) {
		skip(skipIfSet);
	}
	accept("do");
	codeGen->stackWhileGenCont();

	try { oper(skippingSet); } catch (PascalExcp& e) {
		skip(skippingSet);
	}
	codeGen->stackWhileWhileEnd();
	codeGen->stackWhileCont();
}
void CCompiler::compoundOper(set<string> skippingSet) throw(PascalExcp, EOFExcp) {
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

void CCompiler::oper(set<string> skippingSet) throw(PascalExcp, EOFExcp) {
	// <оператор>::=<непомеченный оператор>|<метка><непомеченный оператор>
	unmarkedOper(skippingSet);
}
void CCompiler::unmarkedOper(set<string> skippingSet) throw(PascalExcp, EOFExcp) {
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
		if (curToken->getType() != OPER || ((COperToken*)curToken)->getLexem() == ";" ||
			((COperToken*)curToken)->getLexem() == "end")
			try { simpleOper(skippingSet); } catch (PascalExcp& e) {
			skip(skippingSet);
		}
	// встречен неожиданный оператор
		else {
			writeMistake(6);
			throw PascalExcp();
		}
}

void CCompiler::simpleOper(set<string> skippingSet) throw(PascalExcp, EOFExcp) {
	// <простой оператор>::=<оператор присваивания>|<оператор процедуры> | <оператор перехода> |<пустой оператор>
	ifNullThrowExcp();
	// <пусто>
	if (curToken->getType() == OPER && (checkOper(";") || checkOper("end")))
		return;
	assignOper(skippingSet);
}
void CCompiler::assignOper(set<string> skippingSet)throw(PascalExcp, EOFExcp) {
	// <оператор присваивания>:: = <переменная>: = <выражение> |<имя функции> : = <выражение>
	ifNullThrowExcp();
	auto varName = var(set<string>());
	ifNullThrowExcp();
	accept(":=");
	auto rightType = expression(skippingSet);
	auto leftType = semantic->getLast()->checkAssignTypes(varName, rightType);
	codeGen->stackStloc(varName, leftType);
	
	//codeGen->stackPrint(varName, leftType);
}
EType CCompiler::expression(set<string> skippingSet) throw(PascalExcp, EOFExcp) {
	// <выражение>::=<простое выражение>|<простое выражение><операция отношения><простое выражение>
	EType leftType = EType();
	EType rightType = EType();
	try { 
		leftType = simpleExpr(); 
	} catch (PascalExcp& e) {
		skip(skippingSet);
	}
	if (isBoolOper()) {
		string oper = ((COperToken*)curToken)->getLexem();
		getNext();		// accept
		try { 
			rightType = simpleExpr(); 
			if(eTypeIsDefine(leftType)&& eTypeIsDefine(rightType)) leftType = (*semantic).unionBoolExprTypes(leftType, rightType);
		} catch (PascalExcp& e) {
			skip(skippingSet);
			leftType = eNONE;
		}
		codeGen->stakBoolOper(oper);
	}
	return leftType;
}


EType CCompiler::simpleExpr() throw(PascalExcp, EOFExcp) {
	//<простое выражение>:: = <знак><слагаемое>{ <аддитивная операция><слагаемое> }
	ifNullThrowExcp();
	auto sign = acceptSign();
	EType leftType = term();
	codeGen->stackSign(sign);

	// то, что в {}
	while (isAdditiveOper()) {
		string oper = ((COperToken*)curToken)->getLexem();
		getNext();
		EType rightType = term();
		if (eTypeIsDefine(leftType) && eTypeIsDefine(rightType)) leftType = (*semantic).unionTypes(leftType, rightType, oper);
		codeGen->stackAdditOper(oper);
	}
	return leftType;
}

EType CCompiler::term() throw(PascalExcp, EOFExcp) {
	ifNullThrowExcp();
	EType leftType = factor();
	while (curToken != nullptr && isMultOper()) {
		string oper = ((COperToken*)curToken)->getLexem();
		getNext();
		EType rightType = factor();
		if (eTypeIsDefine(leftType) && eTypeIsDefine(rightType)) leftType = (*semantic).unionTypes(leftType, rightType, oper);
		codeGen->stakMultOper(oper);
	}
	return leftType;
}

EType CCompiler::factor() throw(PascalExcp, EOFExcp) {
	//<множитель>::=<переменная>|<константа без знака>|
	//(<выражение>) | <обозначение функции> | <множество> | not <множитель>
	ifNullThrowExcp();

	if (checkOper("not")) {		// not <множитель>
		getNext();
		auto type = factor();
		if (type != eBOOLEAN) {
			writeMistake(1004);
			return eNONE;
		}
		return type;
	}
	if (curToken->getType() == VALUE) {		// константа без знака
		auto curTokenType = ((CValueToken*)curToken)->getVariant().getType();
		auto eType = semantic->getLast()->defineType(curTokenType, "");
		codeGen->stackLdcNum(eType, ((CValueToken*)curToken)->getVariant().getLexem());		// кладем в стек
		getNext();

		return eType;
	}

	if (checkOper("(")) {		// (<выражение>)
		accept("(");
		auto factorType = expression(set<string>{"and", "or", "then", "else", ";" });
		accept(")");
		return factorType;
	}
	string varName = var(set<string>());
	auto varType = semantic->getLast()->defineType(EVarType(), varName);				// <переменная>

	// если это был true\false
	if (varType == eBOOLEAN)
		if (varName == "true")
			codeGen->stackLdcNum(eINT, "1");
		else 
			if (varName == "false")
				codeGen->stackLdcNum(eINT, "0");
	else codeGen->stackLdloc(varName, varType);

	return varType;
}

pair<EType, string> CCompiler::unsignedNum() throw(PascalExcp, EOFExcp) {
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

void CCompiler::checkForbiddenSymbol() throw(PascalExcp, EOFExcp) {
	if (curToken != nullptr && curToken->getType() == UNDEF) {
		writeMistake(6);
		throw PascalExcp();
	}
}

void CCompiler::writeMistake(int code) {
	cout << "Mistake " << code << ": " << lexic->getStartPosition() << endl;
	erManager->addError(lexic->getStartPosition(), lexic->getCurLine(), code);
}

void CCompiler::writeMistake(int code, int pos, int oldLineNum) {
	cout << "Mistake2 " << code << ": " << pos << endl;
	erManager->addError(pos, oldLineNum, code);
}


// "съедаем" токен, проверяя, что лексема нужная
void CCompiler::accept(string oper) throw(PascalExcp, EOFExcp) {
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

bool CCompiler::tryAccept(string oper) throw(PascalExcp, EOFExcp) {
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

bool CCompiler::checkOper(string oper) {
	if (curToken == nullptr) return false;
	if (curToken->getType() == OPER &&
		((COperToken*)curToken)->getLexem() == oper)
		return true;
	return false;
}

bool CCompiler::ifNullThrowExcp() throw(PascalExcp, EOFExcp) {
	if (curToken == nullptr) {
		erManager->addError(lexic->getCurPosInLine(), lexic->getCurLine(), 1000);		//TODO(по факту здесь достижение конца файла, но кинем "ожидалась ;"
		throw EOFExcp();
		// throw exception("Reached eof");
	}
	return false;
}

bool CCompiler::isBoolOper() {
	// <операция отношения>::==|<>|<|<=|>=|>|in
	if (curToken == nullptr) return false;

	if (curToken->getType() != OPER) return false;
	return ((COperToken*)curToken)->getLexem() == string("=") ||
		((COperToken*)curToken)->getLexem() == string("<>") ||
		((COperToken*)curToken)->getLexem() == string("<") ||
		((COperToken*)curToken)->getLexem() == string("<=") ||
		((COperToken*)curToken)->getLexem() == string(">=") ||
		((COperToken*)curToken)->getLexem() == string(">") ||
		//((COperToken*)curToken)->getLexem() == string("and") ||
		//((COperToken*)curToken)->getLexem() == string("or") ||
		((COperToken*)curToken)->getLexem() == string("in");

}

bool CCompiler::isAdditiveOper() {
	if (curToken == nullptr) return false;

	if (curToken->getType() != OPER) return false;
	return ((COperToken*)curToken)->getLexem() == string("+") ||
		((COperToken*)curToken)->getLexem() == string("or") ||
		((COperToken*)curToken)->getLexem() == string("-");
}

bool CCompiler::isMultOper() {
	if (curToken == nullptr) return false;

	if (curToken->getType() != OPER) return false;
	return ((COperToken*)curToken)->getLexem() == string("*") ||
		((COperToken*)curToken)->getLexem() == string("/") ||
		((COperToken*)curToken)->getLexem() == string("div") ||
		((COperToken*)curToken)->getLexem() == string("and") ||
		((COperToken*)curToken)->getLexem() == string("mod");
}


// "съедаем" знак, если он есть
string CCompiler::acceptSign() throw (PascalExcp, EOFExcp) {
	ifNullThrowExcp();

	if (curToken->getType() != OPER) return "";
	string sign = ((COperToken*)curToken)->getLexem();
	if (sign == "+" || sign == "-")
		getNext();
	return sign;
}

bool CCompiler::eTypeIsDefine(EType type) {
	return !(type <= INT_MIN || type >= INT_MAX);
}