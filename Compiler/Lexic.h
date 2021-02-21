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
* Класс, отвечающий за лексический анализ
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
	string getLexem();			// получить лексему, начиная с позиции pos
	bool isLetter(char ch);			// проверка является ли передаваемый символ буквой
	bool isDigit(char ch);			// проверка является ли передаваемый символ цифрой
	void passWhitespaces();		// возвращает позицию очередного символа или позицию конца файла, если очередной символ не был найден
	void skipComments();

public:
	Lexic(CErrorManager* errorManager, const string *text);
	CToken* getNext(bool get);		// получить или посмотреть следующий токен
	int getStartPosition();
	int getCurLine();
	void passToNewLine();			// пропуск символов до начала новой строки или до конца файла
	int getCurPos();
	CToken* skip(set<string> lexemes);
};
