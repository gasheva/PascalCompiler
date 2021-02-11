#pragma once
#include "Lexic.h"
#include "Semantic.h"

class Syntax {
	// �������, ������������ � is � check �� "�������" �����
private:
	CErrorManager* erManager;
	Lexic* lexic;
	Semantic* semantic;
	CToken* curToken;

	int offset = 0;

	void getNext();		// �������� ��������� �����
	void peekNext();	// ���������� ��������� ����� (������� �� ����������)
	void removeToken();	// �������� ������

	void accept(string oper, bool& hasMistake);	// ������� ��������
	bool acceptSign();			// ������� ����, ���� �� ����
	bool tryAccept(string oper, bool& hasMistake);	// ������� ������� ��������

	void simpleExpr();
	void term(bool& hasMistake);		// ���������
	void factor(bool& hasMistake);		// ���������
	bool unsignedConst(bool& hasMistake);

	bool isAdditiveOper();		// �������� �� �����. ���������
	bool isMultOper();			// �������� �� ��������. ���������
	bool checkOper(string oper);	// ��������� �� ��������

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