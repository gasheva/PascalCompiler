#pragma once
#include "Lexic.h"
#include "Semantic.h"

class Syntax {
	// функции, начинающиеся с is и check не "съедают" токен
private:
	CErrorManager* erManager;
	Lexic* lexic;
	Semantic* semantic;
	CToken* curToken;

	int offset = 0;
	int offsetD = 5;

	void getNext();		// получить следующий токен
	void peekNext();	// посмотреть следующий токен (позиция не сдвигается)
	void removeToken();	// очистить память
	void skip(set<string> lexem);	// пропуск до определенной лексемы. Лексема curToken равна одной из переданных или nullptr

	void accept(string oper);	// принять операцию
	bool acceptSign();			// принять знак, если он есть
	bool tryAccept(string oper);	// попытка принять операцию


	void program();
	void name();
	void block();

	void blockMarks();
	void blockConst();
	void blockTypes();
	void blockVars();
	void blockFunc();
	void blockOpers();
	
	void constDef();
	void constanta();
	void descrMonotypeVars();	// TODO 
	void simpleType();		// TODO

	void compoundOper();	// составной оператор
	void oper();
	void unmarkedOper();
	void simpleOper();
	void complOper();		// сложный оператор
	void assignOper();		// оператор присваивания		//TODO 
	void procedureOper();	// оператор процедуры
	void goOper();			// оператор перехода
	void emptyOper();		// пустой оператор


	void expression();
	void simpleExpr();
	void term();		// слагаемое
	void factor();		// множитель
	bool unsignedConst();
	void unsignedNum();

	bool isBoolOper();			// является ли операцией отношения
	bool isAdditiveOper();		// является ли аддит. операцией
	bool isMultOper();			// является ли мультипл. операцией
	bool checkOper(string oper);	// совпадает ли оператор

	bool ifNullThrowExcp();
	void checkForbiddenSymbol();
	void writeMistake(int code);


public:
	Syntax(CErrorManager* erManager, Lexic* lexic, Semantic* semantic);
	~Syntax();
	void startVer();
};


struct PascalExcp : public std::exception {
	const char* what() const throw() {
		return "Some exception in Pascal code";
	}
};
struct EOFExcp : public std::exception {
	const char* what() const throw() {
		return "Reached eof";
	}
};