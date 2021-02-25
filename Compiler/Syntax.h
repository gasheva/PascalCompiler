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
	void indexVar();
	void arrayVar();
	void var(set<string> skippingSet);		// ���� ������ ���������� ���, ������ ������
	void fullVar();
	void compVar();			// <���������� ����������>

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
	
	void type();
	void regularType();		// ������
	void simpleType();		// TODO()
	void enumaratedType();	// ������������ ���
	void limitedType();		// ������������ ���
	void typeDef();

	void diffOper(set<string> skippingSet);		// ������� ��������
	void compoundOper(set<string> skippingSet);	// ��������� ��������
	void oper(set<string> skippingSet);			// ��������
	void unmarkedOper(set<string> skippingSet);
	void ifOper(set<string> skippingSet);
	void whileOper(set<string> skippingSet);
	void simpleOper(set<string> skippingSet);
	void assignOper(set<string> skippingSet);		// �������� ������������		//TODO 
	void goOper();			// �������� ��������
	void emptyOper();		// ������ ��������


	void expression(set<string> skippingSet);		// ���������
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