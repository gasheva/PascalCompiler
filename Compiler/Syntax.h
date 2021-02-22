#pragma once
#include "Lexic.h"
#include "Semantic.h"

class Syntax {
	//TODO( if, while, ������ �������� ����� (������������, ������������(������������)))
	// ������� ��������
	// �������, ������������ � is � check �� "�������" �����
private:
	CErrorManager* erManager;
	Lexic* lexic;
	Semantic* semantic;
	CToken* curToken;

	int offset = 0;
	int offsetD = 5;

	void getNext();		// �������� ��������� �����
	void peekNext();	// ���������� ��������� ����� (������� �� ����������)
	void removeToken();	// �������� ������
	void skip(set<string> lexem);	// ������� �� ������������ �������. ������� curToken ����� ����� �� ���������� ��� nullptr
	bool skipUntilBlock(set<string> lexemes, string word);			// ������� �� ������ ������-���� �����. ���������� ������, ���� ��� ����������� �������

	void accept(string oper);	// ������� ��������
	bool acceptSign();			// ������� ����, ���� �� ����
	bool tryAccept(string oper);	// ������� ������� ��������


	void program();
	void name();
	void block();

	void blockMarks();
	void blockConst();		// ������� �� ���� ����������� ��������� ��� �� ���� �����
	void blockTypes();
	void blockVars();		// ������� �� ���� ����������� ���������� ��� �� ���� �����
	void blockFunc();
	void blockOpers();
	
	void constDef();
	void constanta();
	void descrMonotypeVars();	// TODO() ������� �� ���� �����
	void simpleType();		// TODO()

	void typeDef();

	void diffOper();		// ������� ��������
	void compoundOper();	// ��������� ��������
	void oper();			// ��������
	void unmarkedOper();
	void ifOper();
	void whileOper();
	void simpleOper();
	void assignOper();		// �������� ������������		//TODO 
	void goOper();			// �������� ��������
	void emptyOper();		// ������ ��������


	void expression();		// ���������
	void simpleExpr();
	void term();		// ���������
	void factor();		// ���������
	bool unsignedConst();
	void unsignedNum();

	bool isBoolOper();			// �������� �� ��������� ���������
	bool isAdditiveOper();		// �������� �� �����. ���������
	bool isMultOper();			// �������� �� ��������. ���������
	bool checkOper(string oper);	// ��������� �� ��������

	bool ifNullThrowExcp();
	void checkForbiddenSymbol();
	void writeMistake(int code);
	void writeMistake(int code, int pos, int line);
	void writeMistakeCurPos(int code);


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