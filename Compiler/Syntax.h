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

	void accept(string oper);	// ������� ��������
	void simpleExpr();
	bool acceptSign();			// ������� ����, ���� �� ����
	void term();		// ���������
	bool isAdditiveOper();		// �������� �� �����. ���������
	void factor();		// ���������
	bool isMultOper();			// �������� �� ��������. ���������
	bool tryAccept(string oper);	// ������� ������� ��������
	bool checkOper(string oper);	// ��������� �� ��������
	bool ifNullThrowExcp();
	bool unsignedConst();

public:
	Syntax(CErrorManager* erManager, Lexic* lexic, Semantic* semantic);
	~Syntax();
	void startVer();
};
