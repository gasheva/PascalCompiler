#pragma once
#include <string>
#include "CErrorManager.h"
#include <regex>
#include <string>
#include <iostream>


using namespace std;

/*
* Класс, отвечающий за лексический анализ
*/
class Lexic {
private:
	const string *text;
	const char NEW_LINE_SYMBOL = '\n';
	CErrorManager* errorManager;
	string getLexem(int pos, int lineNum, bool& hasMistake);			// получить лексему, начиная с позиции pos
	bool isLetter(char ch);			// проверка является ли передаваемый символ буквой
	bool isDigit(char ch);			// проверка является ли передаваемый символ цифрой
	int passWhitespaces(int pos, int& lineNum);		// возвращает позицию очередного символа или позицию конца файла, если очередной символ не был найден
public:
	Lexic(CErrorManager* errorManager, const string *text);
	void startVer();		// начать лексический анализ
};
