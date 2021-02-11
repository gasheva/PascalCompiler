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
	void simpleExpr();
	bool acceptSign();			// принять знак, если он есть
	void term();		// слагаемое
	bool isAdditiveOper();		// является ли аддит. операцией
	void factor();		// множитель
	bool isMultOper();			// является ли мультипл. операцией
	bool tryAccept(string oper);	// попытка принять операцию
	bool checkOper(string oper);	// совпадает ли оператор
	bool ifNullThrowExcp();
	bool unsignedConst();

public:
	Syntax(CErrorManager* erManager, Lexic* lexic, Semantic* semantic);
	~Syntax();
	void startVer();
};
