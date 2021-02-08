#include "Lexic.h"
#include "CTokenFactory.h"

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

int Lexic::passWhitespaces(int pos, int &lineNum) {
	while ((*text)[pos] == ' ' || (*text)[pos] == '\r' || (*text)[pos] == '\t' || (*text)[pos] == '\n') {
		if ((*text)[pos] == '\r' || (*text)[pos] == '\n')
			lineNum++;
		pos++;
		if (pos == (*text).length())
			return pos;
	}
	return pos;
}

void Lexic::startVer()
{
	int lineNum = 0;
	int pos = passWhitespaces(0, lineNum); 
	bool hasMistake = false;
	
	CTokenFactory factory = CTokenFactory();
	cout << "OPER = " << OPER << " IDENT = " << IDENT << " VALUE = " << VALUE<<endl;
	while (pos < (*text).length()) {
		pos = passWhitespaces(pos, lineNum);
		if (pos >= (*text).length()) break;
		string lexem = getLexem(pos, lineNum, hasMistake);
		cout <<"[x] Lexem = " << lexem;
		pos += lexem.length();
		CToken* token = factory.createToken(lexem);
		cout << " token = " << token->m_T << endl;
		delete token;
	}

}

string Lexic::getLexem(int pos, int lineNum, bool &hasMistake)
{
	int oldPos = pos;
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
}
