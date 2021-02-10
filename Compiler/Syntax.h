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

	void accept(string oper);
	float simpleExpr();
	bool isSign();
	EVarType term(float &res);		// слагаемое
	bool isAdditiveOper();
	EVarType factor(float &rest);		// множитель
	bool isMultOper();
	float variable();
	float constWithoutSign();

public:
	Syntax(CErrorManager* erManager, Lexic* lexic, Semantic* semantic);
	~Syntax();
	void startVer();
};
