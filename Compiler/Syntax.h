#pragma once
#include "Lexic.h"
#include "Semantic.h"

class Syntax {
	//TODO( if, while, раздел описания типов (перечислимый, интервальный(ограниченный)))
	// сложный оператор

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
	bool skipUntilBlock(set<string> lexemes, string word);			// пропуск до начала какого-либо блока. Возвращает истину, если был осуществлен пропуск

	void accept(string oper);	// принять операцию
	bool acceptSign();			// принять знак, если он есть
	bool tryAccept(string oper);	// попытка принять операцию


	void program();
	void name();
	void indexVar();
	void arrayVar();
	void var(set<string> skippingSet);		// если пустой пропускной сет, кидает ошибку
	void fullVar();
	void compVar();			// <компонента переменной>

	void block();

	void blockMarks();
	void blockConst();		// пропуск до след определения константы или до след блока
	void blockTypes();
	void blockVars();		// пропуск до след определения переменной или до след блока
	void blockFunc();
	void blockOpers();
	
	void constDef();
	void constanta();
	void descrMonotypeVars();	// TODO() пропуск до след имени
	
	void type();
	void regularType();		// массив
	void simpleType();		// TODO()
	void enumaratedType();	// перечислимый тип
	void limitedType();		// ограниченный тип
	void typeDef();

	void diffOper(set<string> skippingSet);		// сложный оператор
	void compoundOper(set<string> skippingSet);	// составной оператор
	void oper(set<string> skippingSet);			// оператор
	void unmarkedOper(set<string> skippingSet);
	void ifOper(set<string> skippingSet);
	void whileOper(set<string> skippingSet);
	void simpleOper(set<string> skippingSet);
	void assignOper(set<string> skippingSet);		// оператор присваивания		//TODO 
	void goOper();			// оператор перехода
	void emptyOper();		// пустой оператор


	void expression(set<string> skippingSet);		// выражение
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
	void writeMistake(int code, int pos, int line);


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