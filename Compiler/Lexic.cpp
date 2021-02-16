#include "Lexic.h"
#include <string>

using namespace std;

bool Lexic::isLetter(char ch) {
	if (ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z')
		return true;
	else return false;
}
bool Lexic::isDigit(char ch)
{
	if (ch >= '0' && ch <= '9')
		return true;
	else return false;
}

void Lexic::passWhitespaces() {
	while ((*text)[pos] == ' ' || (*text)[pos] == '\r' || (*text)[pos] == '\t' || (*text)[pos] == '\n') {
		if ((*text)[pos] == '\r' || (*text)[pos] == '\n')
			lineNum++;
		pos++;
		if (pos == (*text).length())
			return;
	}
}
string toLower(string lexem) {
	transform(lexem.begin(), lexem.end(), lexem.begin(), ::tolower);
	return lexem;
}


string Lexic::getLexem(bool &hasMistake)
{
	int oldPos = pos;
    string res("");
	switch ((*text)[oldPos])
	{
	case '\'':
		oldPos++;
		res += "\'";
		while ((*text)[oldPos] != '\'' && (*text)[oldPos] != NEW_LINE_SYMBOL) {
			res += (*text)[oldPos];
			oldPos++;
		}
		if ((*text)[oldPos] != '\'') {
			errorManager->addError(oldPos, lineNum, 75);
			return res;
		}
		res += "\'";
		return res;
	case ':':
		if ((*text)[oldPos + 1] == '=')
			return ":=";
		else return ":";
	case '<':
		if ((*text)[oldPos + 1] == '=')
			return "<=";
		else if (((*text)[oldPos + 1] == '>'))
				return "<>";
			else return "<";
	case '>':
		if ((*text)[oldPos + 1] == '=')
			return ">=";
		else return ">";
	case '+': case '-': case '*': case '^': case '/':
	case '(': case ')': case '[': case ']':
	case '{': case '}': case ';':
	case ',': case '=': case '.':
		return res += (*text)[oldPos];
		// <>
	default:
		break;
	}

	// проверяем, что это не является не опознанным символом
	// возможно число или ид
	if (isDigit((*text)[oldPos]) || isLetter((*text)[oldPos])||
		(*text)[oldPos]=='_') {
		while (isDigit((*text)[oldPos]) || isLetter((*text)[oldPos]) ||
			(*text)[oldPos] == '_' || (*text)[oldPos] == '.') {

			// отдельно проверяется не является ли лексема "end"
			res += (*text)[oldPos];
			if (toLower(res) == "end") {
				if (oldPos != (*text).length() && (*text)[oldPos + 1]=='.') {
					return res;
				}
				
			}
			oldPos++;
		}
		return res;
	}
	// если ничего из вышеперечисленного, то неопределенный символ
	res += (*text)[oldPos];
	// errorManager->addError(oldPos, lineNum, 6);

    return res;
}

Lexic::Lexic(CErrorManager* errorManager, const string *text)
{
	this->errorManager = errorManager;
	this->text = text;
	this->pos = 0;
	lineNum = 0;
	factory = CTokenFactory();
}

CToken* Lexic::getNext(bool get)
{
	if (pos >= (*text).length()) return nullptr;		// возвращаем null, если достигли конца файла
	bool hasMistake = false;

	//cout << "OPER = " << OPER << " IDENT = " << IDENT << " VALUE = " << VALUE << endl;

	passWhitespaces();
	if (pos >= (*text).length()) return nullptr;

	string lexem = getLexem(hasMistake);
	cout << "[x] Lexem = " << lexem << endl;
	lastLexemStartPos = pos;
	if (get) pos += lexem.length();
	return factory.createToken(lexem);
}

int Lexic::getStartPosition()
{
	return pos - lastLexemStartPos;
}

int Lexic::getCurLine()
{
	return lineNum;
}

void Lexic::passToNewLine()
{
	do {
		pos++;
		if (pos >= (*text).length())
			return;
	} while ((*text)[pos] != NEW_LINE_SYMBOL);
	lineNum++;
}

int Lexic::getCurPos()
{
	return pos;
}
