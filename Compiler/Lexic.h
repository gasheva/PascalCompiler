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
	void passWhitespaces();		// ���������� ������� ����� ������, ����������� �������
	void skipComments();
	void setOldPos(int _lineNum, int _lastNewLinePos, int _lastLexemStartPos);

public:
	Lexic(CErrorManager* errorManager, const string *text);
	CToken* getNext(bool get);		// �������� ��� ���������� ��������� �����
	string peekNext();				// ���������� ��������� �������
	int getStartPosition();			// ���������� ������ ������ ��������� ������� � ������
	int getCurLine();
	void passToNewLine();			// ������� �������� �� ������ ����� ������ ��� �� ����� �����
	int getCurPos();				// ���������� ������� ������� � �����
	void setCurPos();
	int getCurPosInLine();			// ���������� ������� ������� � ������
	CToken* skip(set<string> lexemes);
};
