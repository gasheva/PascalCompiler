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
	int offsetD = 5;

	void getNext();		// �������� ��������� �����
	void peekNext();	// ���������� ��������� ����� (������� �� ����������)
	void removeToken();	// �������� ������

	void accept(string oper);	// ������� ��������
	bool acceptSign();			// ������� ����, ���� �� ����
	bool tryAccept(string oper);	// ������� ������� ��������

	void simpleExpr();
	void term();		// ���������
	void factor();		// ���������
	bool unsignedConst();

	bool isAdditiveOper();		// �������� �� �����. ���������
	bool isMultOper();			// �������� �� ��������. ���������
	bool checkOper(string oper);	// ��������� �� ��������

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