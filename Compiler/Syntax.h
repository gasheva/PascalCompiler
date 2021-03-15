#pragma once
#include "Lexic.h"
#include "Semantic.h"

class Syntax {
	//if, while, ������ �������� ����� (������������, ������������(������������))

	// �������, ������������ � is � check �� "�������" �����
private:
	CErrorManager* erManager;
	Lexic* lexic;
	CSemantic* semantic;
	CToken* curToken;

	void getNext();		// �������� ��������� �����
	void peekNext();	// ���������� ��������� ����� (������� �� ����������)
	void removeToken();	// �������� ������
	void skip(set<string> lexem);	// ������� �� ������������ �������. ������� curToken ����� ����� �� ���������� ��� nullptr
	bool skipUntilBlock(set<string> lexemes, string word);			// ������� �� ������ ������-���� �����. ���������� ������, ���� ��� ����������� �������

	void accept(string oper);	// ������� ��������
	bool acceptSign();			// ������� ����, ���� �� ����
	bool tryAccept(string oper);	// ������� ������� ��������


	void program();
	string name();
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
	pair<EType, string> constanta();			// ���������� ��� ���������, eNONE - ���� ��� �����, �� �������� �����
	void descrMonotypeVars();	// TODO() ������� �� ���� �����
	
	string type();
	void regularType();		// ������
	string simpleType();		// TODO()
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
	void assignOper(set<string> skippingSet);		// �������� ������������


	void expression(set<string> skippingSet);		// ���������
	void simpleExpr();
	void term();		// ���������
	EType factor();		// ���������
	pair<EType, string> unsignedNum();

	bool isBoolOper();			// �������� �� ��������� ���������
	bool isAdditiveOper();		// �������� �� �����. ���������
	bool isMultOper();			// �������� �� ��������. ���������
	bool checkOper(string oper);	// ��������� �� ��������

	bool ifNullThrowExcp();
	void checkForbiddenSymbol();
	void writeMistake(int code);
	void writeMistake(int code, int pos, int line);


	EType ssTypeAdapter(EVarType type);		// ������� ����� �������������� � ��������
public:
	Syntax(CErrorManager* erManager, Lexic* lexic, CSemantic* semantic);
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