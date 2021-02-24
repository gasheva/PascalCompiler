#include "Lexic.h"
#include <string>
#include "FStringFunc.h"
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
		if ((*text)[pos] == '\n') {
			lineNum++;
			lastNewLinePos = pos;
		}
		pos++;
		if (pos == (*text).length())
			return;
	}
}


void Lexic::skipComments()
{
	while (pos < (*text).length() && (*text)[pos] != '}') {
		if ((*text)[pos] == '\n') {
			lineNum++;
			lastNewLinePos = pos;
		}
		pos++;
	}
	// пропускаем '}'
	if (pos < (*text).length())
		pos++;
	else 
		errorManager->addError(pos - lastNewLinePos, lineNum, 86);
}


string Lexic::getLexem()
{
	int oldPos = pos;
    string res = "";
	switch ((*text)[oldPos])
	{
		// проверка на string
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

	// однострочный комментарий 
	case '/': {
		if ((*text)[oldPos + 1] == '/') {
			passToNewLine();
			passWhitespaces();
		}
		return getLexem();
	}


	case '+': case '-': case '*': case '^':
	case '(': case ')': case '[': case ']':
	case ';': case ',': case '=':
		return res += (*text)[oldPos];

	// многострочный комментарий
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

	// провер€ем, что это не €вл€етс€ неопознанным символом
	// возможно число или ид
	if (isDigit((*text)[oldPos]) || isLetter((*text)[oldPos])||
		(*text)[oldPos]=='_') {
		while (isDigit((*text)[oldPos]) || isLetter((*text)[oldPos]) ||
			(*text)[oldPos] == '_' || (*text)[oldPos] == '.') {

			// отдельно провер€етс€ не €вл€етс€ ли лексема "end" (отделить end и .)
			res += (*text)[oldPos];
			if (toLower(res) == "end") {
				if (oldPos < (*text).length() && (*text)[oldPos + 1]=='.') {
					return res;
				}
				
			}
			oldPos++;
		}
		// если есть 
		auto found = res.find("..");
		if (found != std::string::npos) {
			res = res.substr(0, found);
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
	factory = CTokenFactory();
}

CToken* Lexic::getNext(bool get)
{
	int _lineNum = lineNum;
	int _lastNewLinePos = lastNewLinePos;
	int _lastLexemStartPos = lastLexemStartPos;
	int _pos = pos;
	if (pos >= (*text).length()) return nullptr;		// возвращаем null, если достигли конца файла
	
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

void Lexic::setOldPos(int _pos, int _lineNum, int _lastNewLinePos, int _lastLexemStartPos) {
	lineNum = _lineNum;
	lastNewLinePos = _lastNewLinePos;
	lastLexemStartPos = _lastLexemStartPos;
	if (_pos>0) pos = _pos;
}
string Lexic::peekNext() {
	int _lineNum = lineNum;
	int _lastNewLinePos = lastNewLinePos;
	int _lastLexemStartPos = lastLexemStartPos;
	int _pos = pos;

	if (pos >= (*text).length()) return "";		// возвращаем null, если достигли конца файла

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
string Lexic::peek2Next() {
	// сохран€ем старые позиции, чтобы восстановить их при выходе
	int _lineNum = lineNum;
	int _lastNewLinePos = lastNewLinePos;
	int _lastLexemStartPos = lastLexemStartPos;
	int _pos = pos;
	string lexem = "";

	for (int i = 0; i < 2; i++) {
		if (pos >= (*text).length()) {
			setOldPos(_pos, _lineNum, _lastNewLinePos, _lastLexemStartPos);
			return "";		// возвращаем null, если достигли конца файла
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
// ¬озвращает индекс начала последней лексемы в строке
int Lexic::getStartPosition()
{
	return lastLexemStartPos - lastNewLinePos;
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
	lastNewLinePos = pos;
}

CToken* Lexic::skip(set<string> lexemes) {
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
/* возвращает текущую позицию в файле*/
int Lexic::getCurPos()
{
	return pos;
}

// возвращает текущую позицию в строке
int Lexic::getCurPosInLine() {
	return pos - lastNewLinePos;
}

