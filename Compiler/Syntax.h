#pragma once
#include "Lexic.h"
#include "Semantic.h"

class Syntax {
private:
	CErrorManager* erManager;
	Lexic* lexic;
	Semantic* semantic;
	CToken* curToken;

	void getNext();
	void removeToken();

	void accept();
	float simpleExpr();
	bool isSign(COperToken* token);
	float term();		// слагаемое
	bool isAdditiveOper(COperToken* token);
	float factor();		// множитель
	bool isMultOper(COperToken* token);
	float variable();
	float constWithoutSign();

public:
	Syntax(CErrorManager* erManager, Lexic* lexic, Semantic* semantic);
	~Syntax();
	void startVer();
};
