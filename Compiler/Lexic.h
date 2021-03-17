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
*  ласс, отвечающий за лексический анализ
*/
class CLexic {
private:
	const string *text;
	const char NEW_LINE_SYMBOL = '\n';
	int pos, lineNum = 1;
	int lastLexemStartPos = 0;
	int lastNewLinePos = 0;
	
	CTokenFactory factory;
	CErrorManager* errorManager;
	string getLexem();			// получить лексему, начина€ с позиции pos
	bool isLetter(char ch);			// проверка €вл€етс€ ли передаваемый символ буквой
	bool isDigit(char ch);			// проверка €вл€етс€ ли передаваемый символ цифрой
	void passWhitespaces();		// возвращает текущий номер строки, увеличивает позицию
	void skipComments();
	void setOldPos(int _pos, int _lineNum, int _lastNewLinePos, int _lastLexemStartPos);

public:
	CLexic(CErrorManager* errorManager, const string *text);
	CToken* getNext(bool get);		// получить или посмотреть следующий токен
	string peekNext();				// посмотреть следующую лексему
	string peek2Next();		// посмотреть через лексему
	int getStartPosition();			// ¬озвращает индекс начала последней лексемы в строке
	int getCurLine();
	void passToNewLine();			// пропуск символов до начала новой строки или до конца файла
	int getCurPos();				// возвращает текущую позицию в файле
	int getCurPosInLine();			// возвращает текущую позицию в строке
	CToken* skip(set<string> lexemes);
};
