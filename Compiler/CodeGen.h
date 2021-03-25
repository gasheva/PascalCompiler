#pragma once
#include "CTypes.h"
#include <fstream>
#include <iostream>
#include <queue>

class CCodeGen {
	// ��� ���������� ������� �� �����, ��������� ���������� �������������� 
	// � ������������� ���
private:
	ofstream genFile;
	string programName;
	string typeToString(EType type);
	string typeToString(string type);
	string boolOperToString(string oper);

	queue<string> marks;
	int markNum = 0;

	void stackNeg();		// ������� ���������� ����� �������������
	void stackAdd();
	void stackSub();
	void stackMul();
	void stackDiv();
	void stackMod();
	void stackAssignConv(EType varType);		// ������������ � ��� ����������
	void stackConv(EType termType);			// ������������ �� �����

	void stackGenMark();			// ���������� ����� � ��������� � ������� � � ����

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
	void stackPushNum(EType type, string lexem);

	void stackLdc(EType type, string what, string varName);		
	void stackLdcNum(EType type, string num);		// �������� ����� � ����
	void stackLdloc(string varName, EType type);		// �������� ���������� � ����
	void stackStloc(string varName, EType type);		// �������� �� ����� � ����������
	
	void stackSign(string sign);		// ���� ���� "-", ����� �������� neg

	void stakMultOper(string oper);
	void stackAdditOper(string oper);

	
	void stackPopMark();			// ����� ������� ����� � ���� � pop
	void stackBrfalse();
	void stackBrS();

	void stackPrint(string lexem, EType type);
};
