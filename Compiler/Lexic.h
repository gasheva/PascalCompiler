#pragma once
#include <string>
#include "CErrorManager.h"
#include <regex>
#include <string>
#include <iostream>
#include "CToken.h"
#include "CTokenFactory.h"

using namespace std;

/*
* �����, ���������� �� ����������� ������
*/
class Lexic {
private:
	const string *text;
	const char NEW_LINE_SYMBOL = '\n';
	int pos, lineNum = 1;
	int lastLexemStartPos = 0;
	int lastNewLinePos = 0;
	
	CTokenFactory factory;
	CErrorManager* errorManager;
	string getLexem();			// �������� �������, ������� � ������� pos
	bool isLetter(char ch);			// �������� �������� �� ������������ ������ ������
	bool isDigit(char ch);			// �������� �������� �� ������������ ������ ������
	void passWhitespaces();		// ���������� ������� ���������� ������� ��� ������� ����� �����, ���� ��������� ������ �� ��� ������
	void skipComments();

public:
	Lexic(CErrorManager* errorManager, const string *text);
	CToken* getNext(bool get);		// �������� ��� ���������� ��������� �����
	int getStartPosition();
	int getCurLine();
	void passToNewLine();			// ������� �������� �� ������ ����� ������ ��� �� ����� �����
	int getCurPos();
	CToken* skip(set<string> lexemes);
};
