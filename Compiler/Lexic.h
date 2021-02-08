#pragma once
#include <string>
#include "CErrorManager.h"
#include <regex>
#include <string>
#include <iostream>


using namespace std;

/*
* �����, ���������� �� ����������� ������
*/
class Lexic {
private:
	const string *text;
	const char NEW_LINE_SYMBOL = '\n';
	CErrorManager* errorManager;
	string getLexem(int pos, int lineNum, bool& hasMistake);			// �������� �������, ������� � ������� pos
	bool isLetter(char ch);			// �������� �������� �� ������������ ������ ������
	bool isDigit(char ch);			// �������� �������� �� ������������ ������ ������
	int passWhitespaces(int pos, int& lineNum);		// ���������� ������� ���������� ������� ��� ������� ����� �����, ���� ��������� ������ �� ��� ������
public:
	Lexic(CErrorManager* errorManager, const string *text);
	void startVer();		// ������ ����������� ������
};
