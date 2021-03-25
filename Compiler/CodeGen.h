#pragma once
#include "CTypes.h"
#include <fstream>
#include <iostream>
#include <stack>

class CCodeGen {
	// ��� ���������� ������� �� �����, ��������� ���������� �������������� 
	// � ������������� ���
private:
	ofstream genFile;
	string programName;
	string typeToString(EType type);
	string typeToString(string type);

	stack<string> marksElse;
	string markCont = "";
	int markNum = 0;

	void stackNeg();		// ������� ���������� ����� �������������
	void stackAdd();
	void stackSub();
	void stackMul();
	void stackDiv();
	void stackMod();
	void stackAssignConv(EType varType);		// ������������ � ��� ����������
	void stackConv(EType termType);			// ������������ �� �����

	void stackGenMark();			// ���������� ����� else � ��������� � ������� � � ����
	void stackGenMarkCont();		// ���������� ���� continue

	void stackPopMark();			// ����� ������� ����� � ���� � pop
	void stackBrfalse();
	void stackBrS();

public:
	CCodeGen();
	void openFile(string path, string name);
	void closeFile();

	void stackMaxStack(int count);
	void stackWriteBegin();
	void stackWriteEnd();

	void stackInitConst();
	void stackInitType();
	void stackInitVar(string type, string lexem);
	void stackLdcNum(EType type, string num);		// �������� ����� � ����
	void stackLdloc(string varName, EType type);		// �������� ���������� � ����
	void stackStloc(string varName, EType type);		// �������� �� ����� � ����������
	
	void stackSign(string sign);		// ���� ���� "-", ����� �������� neg

	void stakBoolOper(string oper);		// ��� <, > ...
	void stakMultOper(string oper);
	void stackAdditOper(string oper);

	

	void stackIf();					// ������� ����� else
	void stackThenEnd();			// ������� ����� continue
	void stackElse();				// ����� else
	void stackIfEnd();				// ����� continue (����� if)


	void stackPrint(string lexem, EType type);
};
