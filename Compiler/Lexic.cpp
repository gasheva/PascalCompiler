#include "Lexic.h"
#include <string>
#include "FStringFunc.h"
using namespace std;

bool CLexic::isLetter(char ch) {
	if (ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z')
		return true;
	else return false;
}
bool CLexic::isDigit(char ch)
{
	if (ch >= '0' && ch <= '9')
		return true;
	else return false;
}

void CLexic::passWhitespaces() {
	
	while ((*text)[pos] == ' ' || (*text)[pos] == '\r' || (*text)[pos] == '\t' || (*text)[pos] == '\n') {
		if ((*text)[pos] == '\n') {
			lineNum++;
			lastNewLinePos = pos;
		}
		pos++;
		if (pos == (*text).length())
			return;
	}
}


void CLexic::skipComments()
{
	while (pos < (*text).length() && (*text)[pos] != '}') {
		if ((*text)[pos] == '\n') {
			lineNum++;
			lastNewLinePos = pos;
		}
		pos++;
	}
	// ���������� '}'
	if (pos < (*text).length())
		pos++;
	else 
		errorManager->addError(pos - lastNewLinePos, lineNum, 86);
}


string CLexic::getLexem()
{
	int oldPos = pos;
    string res = "";
	switch ((*text)[oldPos])
	{
		// �������� �� string
	case '\'':
		oldPos++;
		res += "\'";
		while (oldPos<(*text).length() && (*text)[oldPos] != '\'' && (*text)[oldPos] != NEW_LINE_SYMBOL) {
			res += (*text)[oldPos];
			oldPos++;
		}
		if (oldPos == (*text).length()) {
			errorManager->addError(oldPos - lastNewLinePos, lineNum, 75);
			return res;
		}

		if ((*text)[oldPos] != '\'') {
			errorManager->addError(oldPos - lastNewLinePos, lineNum, 75);
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
	case '.':
		if ((*text)[oldPos + 1] == '.')
			return "..";
		else return ".";

	// ������������ ����������� 
	case '/': {
		if ((*text)[oldPos + 1] == '/') {
			passToNewLine();
			passWhitespaces();
			return getLexem();
		}
		else return "/";
		
	}


	case '+': case '-': case '*': case '^':
	case '(': case ')': case '[': case ']':
	case ';': case ',': case '=':
		return res += (*text)[oldPos];

	// ������������� �����������
	case '{': {
		skipComments();
		if (pos < (*text).length()) {
			passWhitespaces();
			string tmp = getLexem();
			return tmp;
		}
		else {
			return "";
		}
	}
	default:
		break;
	}

	// ���������, ��� ��� �� �������� ������������ ��������
	// �������� ����� ��� ��
	if (isDigit((*text)[oldPos]) || isLetter((*text)[oldPos])||
		(*text)[oldPos]=='_') {
		while (isDigit((*text)[oldPos]) || isLetter((*text)[oldPos]) ||
			(*text)[oldPos] == '_' || (*text)[oldPos] == '.') {

			// �������� ����������� �� �������� �� ������� "end" (�������� end � .)
			res += (*text)[oldPos];
			if (toLower(res) == "end") {
				if (oldPos < (*text).length() && (*text)[oldPos + 1]=='.') {
					return toLower(res);
				}
				
			}
			oldPos++;
		}
		// ���� ���� 
		auto found = res.find("..");
		if (found != std::string::npos) {
			res = res.substr(0, found);
		}
		return toLower(res);
	}
	// ���� ������ �� ������������������, �� �������������� ������
	res += (*text)[oldPos];
	// errorManager->addError(oldPos, lineNum, 6);

	return toLower(res);
}

CLexic::CLexic(CErrorManager* errorManager, const string *text)
{
	this->errorManager = errorManager;
	this->text = text;
	this->pos = 0;
	factory = CTokenFactory();
}

CToken* CLexic::getNext(bool get)
{
	int _lineNum = lineNum;
	int _lastNewLinePos = lastNewLinePos;
	int _lastLexemStartPos = lastLexemStartPos;
	int _pos = pos;
	if (pos >= (*text).length()) return nullptr;		// ���������� null, ���� �������� ����� �����
	
	//cout << "OPER = " << OPER << " IDENT = " << IDENT << " VALUE = " << VALUE << endl;

	passWhitespaces();
	if (pos >= (*text).length()) {
		if (!get)setOldPos(_pos, _lineNum, _lastNewLinePos, _lastLexemStartPos);
		return nullptr;
	}

	string lexem = getLexem();
	if (pos >= (*text).length()) {
		if (!get)setOldPos(_pos, _lineNum, _lastNewLinePos, _lastLexemStartPos);
		return nullptr;
	}
	cout << "[x] Lexem = " << lexem << endl;
	lastLexemStartPos = pos;
	
	if (get) {
		pos += lexem.length();
	}
	else setOldPos(_pos, _lineNum, _lastNewLinePos, _lastLexemStartPos);
	return factory.createToken(lexem);
}

void CLexic::setOldPos(int _pos, int _lineNum, int _lastNewLinePos, int _lastLexemStartPos) {
	lineNum = _lineNum;
	lastNewLinePos = _lastNewLinePos;
	lastLexemStartPos = _lastLexemStartPos;
	if (_pos>0) pos = _pos;
}
string CLexic::peekNext() {
	int _lineNum = lineNum;
	int _lastNewLinePos = lastNewLinePos;
	int _lastLexemStartPos = lastLexemStartPos;
	int _pos = pos;

	if (pos >= (*text).length()) return "";		// ���������� null, ���� �������� ����� �����

	//cout << "OPER = " << OPER << " IDENT = " << IDENT << " VALUE = " << VALUE << endl;

	passWhitespaces();
	
	if (pos >= (*text).length()) {
		setOldPos(_pos, _lineNum, _lastNewLinePos, _lastLexemStartPos);
		return "";
	}

	string lexem = getLexem();
	if (pos >= (*text).length()) {
		setOldPos(_pos, _lineNum, _lastNewLinePos, _lastLexemStartPos);
		return "";
	}
	cout << "[x] Lexem = " << lexem << endl;
	//lastLexemStartPos = pos;

	return lexem;
}
string CLexic::peek2Next() {
	// ��������� ������ �������, ����� ������������ �� ��� ������
	int _lineNum = lineNum;
	int _lastNewLinePos = lastNewLinePos;
	int _lastLexemStartPos = lastLexemStartPos;
	int _pos = pos;
	string lexem = "";

	for (int i = 0; i < 2; i++) {
		if (pos >= (*text).length()) {
			setOldPos(_pos, _lineNum, _lastNewLinePos, _lastLexemStartPos);
			return "";		// ���������� null, ���� �������� ����� �����
		}
		passWhitespaces();
		if (pos >= (*text).length()) {
			setOldPos(_pos, _lineNum, _lastNewLinePos, _lastLexemStartPos);
			return "";
		}

		lexem = getLexem();
		pos += lexem.length();
		if (pos >= (*text).length()) {
			setOldPos(_pos, _lineNum, _lastNewLinePos, _lastLexemStartPos);
			return "";
		}
	}
	setOldPos(_pos, _lineNum, _lastNewLinePos, _lastLexemStartPos);
	return lexem;
}
// ���������� ������ ������ ��������� ������� � ������
int CLexic::getStartPosition()
{
	return lastLexemStartPos - lastNewLinePos;
}

int CLexic::getCurLine()
{
	return lineNum;
}

void CLexic::passToNewLine()
{
	do {
		pos++;
		if (pos >= (*text).length())
			return;
	} while ((*text)[pos] != NEW_LINE_SYMBOL);
	pos++;	// ������� \n
	lineNum++;
	lastNewLinePos = pos;
}

CToken* CLexic::skip(set<string> lexemes) {
	string lexem = "";
	if (pos >= (*text).length()) return nullptr;

	while (pos < (*text).length() && lexemes.find(lexem)==lexemes.end()) {
		passWhitespaces();
		if (pos >= (*text).length()) return nullptr;

		lexem = toLower(getLexem());
		lastLexemStartPos = pos;
		passWhitespaces();
		pos += lexem.length();
	}
	if (pos >= (*text).length()) return nullptr;
	cout << "[x] Lexem = " << lexem << endl;
	return factory.createToken(lexem);
}
/* ���������� ������� ������� � �����*/
int CLexic::getCurPos()
{
	return pos;
}

// ���������� ������� ������� � ������
int CLexic::getCurPosInLine() {
	return pos - lastNewLinePos;
}

