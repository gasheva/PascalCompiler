#include "Lexic.h"

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

int Lexic::passWhitespaces() {
	while ((*text)[pos] == ' ' || (*text)[pos] == '\r' || (*text)[pos] == '\t' || (*text)[pos] == '\n') {
		if ((*text)[pos] == '\r' || (*text)[pos] == '\n')
			lineNum++;
		pos++;
		if (pos == (*text).length())
			return pos;
	}
	return pos;
}



string Lexic::getLexem(bool &hasMistake)
{
	//int oldPos = pos;
	hasMistake = false;
    string res("");
	switch ((*text)[pos])
	{
	case '\'':
		pos++;
		res += "\'";
		while ((*text)[pos] != '\'' && (*text)[pos] != NEW_LINE_SYMBOL) {
			res += (*text)[pos];
			pos++;
		}
		if ((*text)[pos] != '\'') {
			errorManager->addError(pos, lineNum, 75);
			hasMistake = true;
			return res;
		}
		res += "\'";
		return res;
	case ':':
		if ((*text)[pos + 1] == '=')
			return ":=";
		else return ":";
	case '<':
		if ((*text)[pos + 1] == '=')
			return "<=";
		else if (((*text)[pos + 1] == '>'))
				return "<>";
			else return "<";
	case '>':
		if ((*text)[pos + 1] == '=')
			return ">=";
		else return ">";
	case '+': case '-': case '*': case '^': case '/':
	case '(': case ')': case '[': case ']':
	case '{': case '}': case ';':
	case ',': case '=': case '.':
		return res += (*text)[pos];
		// <>
	default:
		break;
	}

	// проверяем, что это не является не опознанным символом
	// возможно число или ид
	if (isDigit((*text)[pos]) || isLetter((*text)[pos])||
		(*text)[pos]=='_') {
		while (isDigit((*text)[pos]) || isLetter((*text)[pos]) ||
			(*text)[pos] == '_' || (*text)[pos] == '.') {
			res += (*text)[pos];
			pos++;
		}
		return res;
	}
	// если ничего из вышеперечисленного, то неопределенный символ
	res += (*text)[pos];
	errorManager->addError(pos, lineNum, 6);
	hasMistake = true;

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

CToken* Lexic::getNext()
{
	pos = passWhitespaces();
	bool hasMistake = false;

	cout << "OPER = " << OPER << " IDENT = " << IDENT << " VALUE = " << VALUE << endl;
	if (pos >= (*text).length()) return nullptr;		// возвращаем null, если достигли конца файла

	pos = passWhitespaces();
	if (pos >= (*text).length()) return nullptr;

	string lexem = getLexem(hasMistake);
	cout << "[x] Lexem = " << lexem;
	pos += lexem.length();
	return factory.createToken(lexem);
}
