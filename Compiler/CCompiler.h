#pragma once
#include "Lexic.h"
#include "Semantic.h"
#include "CodeGen.h"

class CCompiler {
	// �������, ������������ � is � check �� "�������" �����
private:
	CErrorManager* erManager;
	CLexic* lexic;
	CSemantic* semantic;
	CToken* curToken;
	CCodeGen* codeGen;

	void getNext();		// �������� ��������� �����
	void peekNext();	// ���������� ��������� ����� (������� �� ����������)
	void removeToken();	// �������� ������
	void skip(set<string> lexem);	// ������� �� ������������ �������. ������� curToken ����� ����� �� ���������� ��� nullptr
	bool skipUntilBlock(set<string> lexemes, string word);			// ������� �� ������ ������-���� �����. ���������� ������, ���� ��� ����������� �������

	void accept(string oper);	// ������� ��������
	string acceptSign();			// ���������� ���� ��� ������ ������, ��������� ����
	bool tryAccept(string oper);	// ������� ������� ��������


	void program();
	string name();
	string var(set<string> skippingSet);		// ���� ������ ���������� ���, ������ ������
	string fullVar();

	void block();

	void blockMarks();
	void blockConst();		// ������� �� ���� ����������� ��������� ��� �� ���� �����
	void blockTypes();
	void blockVars();		// ������� �� ���� ����������� ���������� ��� �� ���� �����
	void blockFunc();
	void blockOpers();
	
	void constDef();
	pair<EType, string> constanta();			// ���������� ��� ���������, eNONE - ���� ��� �����, �� �������� �����
	void descrMonotypeVars();	// ������� �� ���� �����
	
	string type();
	void regularType();		// ������
	string simpleType();		
	void enumaratedType();	// ������������ ���
	void limitedType();		// ������������ ���
	void typeDef();

	void diffOper(set<string> skippingSet);		// ������� ��������
	void compoundOper(set<string> skippingSet);	// ��������� ��������
	void oper(set<string> skippingSet);			// ��������
	void unmarkedOper(set<string> skippingSet);
	void ifOper(set<string> skippingSet);
	void whileOper(set<string> skippingSet);
	void simpleOper(set<string> skippingSet);
	void assignOper(set<string> skippingSet);		// �������� ������������


	EType expression(set<string> skippingSet);		// ���������
	EType simpleExpr();
	EType term();		// ���������
	EType factor();		// ���������
	pair<EType, string> unsignedNum();

	bool isBoolOper();			// �������� �� ��������� ���������
	bool isAdditiveOper();		// �������� �� �����. ���������
	bool isMultOper();			// �������� �� ��������. ���������
	bool checkOper(string oper);	// ��������� �� ��������

	bool ifNullThrowExcp();
	void checkForbiddenSymbol();
	void writeMistake(int code);
	void writeMistake(int code, int pos, int line);


	EType ssTypeAdapter(EVarType type);		// ������� ����� �������������� � ��������
	bool eTypeIsDefine(EType type);
public:
	CCompiler(CErrorManager* erManager, CLexic* lexic, CSemantic* semantic, CCodeGen* codeGen);
	~CCompiler();
	void startVer();
};

/// <summary>
/// ������� - ������
/// </summary>
struct PascalExcp : public std::exception {
	const char* what() const throw() {
		return "Some exception in Pascal code";
	}
};

/// <summary>
/// �������-������ ����� �����
/// </summary>
struct EOFExcp : public std::exception {
	const char* what() const throw() {
		return "Reached eof";
	}
};