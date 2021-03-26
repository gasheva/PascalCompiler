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
	// ���� ��� �� ������ �����
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
	// ��������, ��� ����� end. ������ ������ ���
	if (curToken != nullptr) {
		writeMistake(1000);
		throw PascalExcp();
	}
	// === ������������. ����� �������� � ���������  ====
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
	// ��������� ��������� �� �� ������� const, 
	// ���� ���, �������� �������� �� const ��� �� ������ ��������� ������
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
	//<����������>::=<������ ����������>|<���������� ����������> | <��������� ����������>
	ifNullThrowExcp();
	return fullVar();			// name()
}

string CCompiler::fullVar() throw(PascalExcp, EOFExcp) {
	//<������ ����������>::=<��� ����������>
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
	skipUntilBlock(blockConstSet, "const");		// ���������� ������� �� ������ ������-� ����� (������)
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
			} else return;		// ���������� �� ����� �����
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
			// ���� ������ ������ ���������, �� � ���������� ������ �� �������� � ���� � ������������ � �����
			codeGen->stackLdloc(constRight.second, curType);

		}
		else {
			// ���� ������ ��������, �� ������ ��������.
			// ���� ��� ��� true\false
			if (curType == eBOOLEAN)
				if (constRight.second == "true")
					codeGen->stackLdcNum(eINT, "1");
				else
						codeGen->stackLdcNum(eINT, "0");
			else
				codeGen->stackLdcNum(curType, constRight.second);		// ������ � ����
			
		}
		codeGen->stackSign(sign);		// ������ ����
		codeGen->stackStloc(constLeft, curType);						// ������ � ��������� �� �����
	}
}
pair<EType, string> CCompiler::constanta()throw(PascalExcp, EOFExcp) {
	//<���������>::=<����� ��� �����>|<����><����� ��� �����>|
	//<��� ���������> | <����><��� ���������> | <������>
	//<����>
	auto sign = acceptSign() ;
	if (sign != "") {
		// <����� ��� �����>
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
			// <��� ���������>
			return std::make_pair(eNONE, name());
		}
	} else {
		// <��� ���������>
		if (curToken->getType() == IDENT) {
			return  std::make_pair(eNONE, name());
		}
		// <������>
		if (curToken->getType() == VALUE &&((
			((CValueToken*)curToken)->getVariant().getType() == STRING) || ((CValueToken*)curToken)->getVariant().getType() == CHAR)) {
			auto constStr = ((CValueToken*)curToken)->getVariant().getLexem();
			auto resPair = std::make_pair(ssTypeAdapter(((CValueToken*)curToken)->getVariant().getType()), constStr);
			getNext();		// ������� ������
			return resPair;
		}
		// <����� ��� �����>
		try {
			return unsignedNum();
		} catch (PascalExcp& e) {
			writeMistake(50);
			throw e;
		}
	}
}
void CCompiler::blockTypes() {
	//<������ �����>::=<�����>|type <����������� ����>;{<����������� ����>; }
	semantic->getLast()->setBlock(TYPEBL);
	semantic->getLast()->clearBuffs();
	ifNullThrowExcp();
	set <string> skipSet = { ";", "begin", "var" };
	if (checkOper("type")) {
		getNext();		// accept("type");
		try { typeDef(); }		//<����������� ����>
		catch (PascalExcp& e) {
			skip(skipSet);
			semantic->getLast()->createNone();
		}
		semantic->getLast()->clearBuffs();
		// {<����������� ����>;}
		while (checkOper(";")) {
			getNext();
			if (curToken != nullptr) {
				if (curToken->getType() == OPER && (
					((COperToken*)curToken)->getLexem() == "begin" ||
					((COperToken*)curToken)->getLexem() == "var"))
					return;
			} else return; //throw PascalExcp();		// ����������� ����� �����
			try { typeDef(); } catch (PascalExcp& e) {
				skip(skipSet);
				semantic->getLast()->createNone();
			}
			semantic->getLast()->clearBuffs();
		}
	}
}

void CCompiler::typeDef()throw(PascalExcp, EOFExcp) {
	//<����������� ����>::=<���>=<���>
	semantic->getLast()->addToNameBuffer(name());
	accept("=");
	auto typeName = type();
	semantic->getLast()->addToBuffer(typeName);
}
string CCompiler::type()throw(PascalExcp, EOFExcp) {
	// <���>:: = <������� ���> | <��������� ���> | <��������� ���>
	// <���>:: = ������� ��� ��� ������
	ifNullThrowExcp();
	if (checkOper("array")) {
		regularType();
	} else {
		return simpleType();
	}
	return "";

}
void CCompiler::regularType()  throw(PascalExcp, EOFExcp) {
	//<���������� ���>:: = array[<������� ���>{, <������� ���>}] of <��� ���������� >
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
	//<������� ���>::=<������������ ���>|<������������ ���>|<��� ����>
	ifNullThrowExcp();
	//<������������ ���>
	try {
		if (checkOper("(")) {
			enumaratedType();
		} else {
			// ����������� �� ���� ����� ������ � ��������� 
			// �������� �� �� ".."
			string nextLexem = lexic->peekNext();
			if (nextLexem != "" && nextLexem == "..")		//<������������ ���>
			{
				limitedType();
			} else {
				// ����������� ��� �� ���� ����� ������ � ��������� 
				// �������� �� �� ".."
				nextLexem = lexic->peek2Next();
				if (nextLexem != "" && nextLexem == "..")		//<������������ ���>
				{
					limitedType();
				} else
					return name();	//<��� ����>
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
	// <������������ ���>::=(<���>{,<���>})
	accept("(");
	name();
	while (checkOper(",")) {
		getNext();
		name();
	}
	accept(")");
}
void CCompiler::limitedType()throw(PascalExcp, EOFExcp) {
	//<������������ ���>::=<���������>..<���������>
	constanta();
	accept("..");
	constanta();
}

void CCompiler::blockVars() throw(PascalExcp, EOFExcp) {
	semantic->getLast()->setBlock(VARBL);
	semantic->getLast()->clearBuffs();
	//<������ ����������>::= var <�������� ���������� ����������>;{<�������� ���������� ����������>; } | <�����>
	ifNullThrowExcp();
	set <string> skipSet = { ";", "begin" };
	// var <�������� ���������� ����������>;
	if (checkOper("var")) {
		getNext();		// accept("var");
		try { descrMonotypeVars(); } catch (PascalExcp& e) {
			skip(skipSet);
			semantic->getLast()->createNone();
		}
		semantic->getLast()->clearBuffs();
		// {<�������� ���������� ����������>;}
		while (checkOper(";")) {
			getNext();
			if (curToken != nullptr) {
				if (curToken->getType() == OPER && (
					((COperToken*)curToken)->getLexem() == "begin"))
					return;
			} else return; //throw PascalExcp();		// ����������� ����� �����
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
	// <�������� ���������� ����������>::=<���>{,<���>}:<���>
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
	// <������ ����������>::=<��������� ��������>
	set<string> skipSet = { "end" };
	try { compoundOper(skipSet); } catch (PascalExcp& e) {
		skip(skipSet);
	}
}
void CCompiler::diffOper(set<string> skippingSet)throw(PascalExcp, EOFExcp) {
	// <������� ��������>::=<��������� ��������>|<���������� ��������>|
	// <�������� �����> |<�������� �������������>
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
	// <�������� ��������>::= if <���������> then <��������>|
	// if <���������> then <��������> else <��������>
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
	//<���� � ������������>::= while <���������> do <��������>
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
	// <��������� ��������>::= begin <��������>{;<��������>} end
	ifNullThrowExcp();
	accept("begin");
	// ���������, ���� �� ��������� ��� ����� end
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
		//TODO(�������, ��������� else �����, ����� if, �� ������� � �� ��� ������������)
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
	// <��������>::=<������������ ��������>|<�����><������������ ��������>
	unmarkedOper(skippingSet);
}
void CCompiler::unmarkedOper(set<string> skippingSet) throw(PascalExcp, EOFExcp) {
	// <������������ ��������>:: = <������� ��������> |<������� ��������>
	ifNullThrowExcp();
	// ���� ;, �� � ������� ��������� ���� <�����>
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
	// �������� ����������� ��������
		else {
			writeMistake(6);
			throw PascalExcp();
		}
}

void CCompiler::simpleOper(set<string> skippingSet) throw(PascalExcp, EOFExcp) {
	// <������� ��������>::=<�������� ������������>|<�������� ���������> | <�������� ��������> |<������ ��������>
	ifNullThrowExcp();
	// <�����>
	if (curToken->getType() == OPER && (checkOper(";") || checkOper("end")))
		return;
	assignOper(skippingSet);
}
void CCompiler::assignOper(set<string> skippingSet)throw(PascalExcp, EOFExcp) {
	// <�������� ������������>:: = <����������>: = <���������> |<��� �������> : = <���������>
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
	// <���������>::=<������� ���������>|<������� ���������><�������� ���������><������� ���������>
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
	//<������� ���������>:: = <����><���������>{ <���������� ��������><���������> }
	ifNullThrowExcp();
	auto sign = acceptSign();
	EType leftType = term();
	codeGen->stackSign(sign);

	// ��, ��� � {}
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
	//<���������>::=<����������>|<��������� ��� �����>|
	//(<���������>) | <����������� �������> | <���������> | not <���������>
	ifNullThrowExcp();

	if (checkOper("not")) {		// not <���������>
		getNext();
		auto type = factor();
		if (type != eBOOLEAN) {
			writeMistake(1004);
			return eNONE;
		}
		return type;
	}
	if (curToken->getType() == VALUE) {		// ��������� ��� �����
		auto curTokenType = ((CValueToken*)curToken)->getVariant().getType();
		auto eType = semantic->getLast()->defineType(curTokenType, "");
		codeGen->stackLdcNum(eType, ((CValueToken*)curToken)->getVariant().getLexem());		// ������ � ����
		getNext();

		return eType;
	}

	if (checkOper("(")) {		// (<���������>)
		accept("(");
		auto factorType = expression(set<string>{"and", "or", "then", "else", ";" });
		accept(")");
		return factorType;
	}
	string varName = var(set<string>());
	auto varType = semantic->getLast()->defineType(EVarType(), varName);				// <����������>

	// ���� ��� ��� true\false
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


// "�������" �����, ��������, ��� ������� ������
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
		else if (oper == ";") writeMistake(14);			// ; ������������ ����� ����. ��������
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
		erManager->addError(lexic->getCurPosInLine(), lexic->getCurLine(), 1000);		//TODO(�� ����� ����� ���������� ����� �����, �� ����� "��������� ;"
		throw EOFExcp();
		// throw exception("Reached eof");
	}
	return false;
}

bool CCompiler::isBoolOper() {
	// <�������� ���������>::==|<>|<|<=|>=|>|in
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


// "�������" ����, ���� �� ����
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