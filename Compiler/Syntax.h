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

	void getNext();		// получить следующий токен
	void peekNext();	// посмотреть следующий токен (позиция не сдвигается)
	void removeToken();	// очистить память

	void accept(string oper);	// принять операцию
	bool acceptSign();			// принять знак, если он есть
	bool tryAccept(string oper);	// попытка принять операцию

	void simpleExpr();
	void term();		// слагаемое
	void factor();		// множитель
	bool unsignedConst();

	bool isAdditiveOper();		// является ли аддит. операцией
	bool isMultOper();			// является ли мультипл. операцией
	bool checkOper(string oper);	// совпадает ли оператор

	bool ifNullThrowExcp();
	bool checkForbiddenSymbol();
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