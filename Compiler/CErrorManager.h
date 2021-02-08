#pragma once
#include<list>
#include "CError.h"
#include <unordered_map>
#include "IErrorPrinter.h"
#include <vector>

using namespace std;

/*
* �������� ������. �������� ������� ����� � �������� ������
* ������ ������ ������
*/
class CErrorManager {
public:
	list<CError> errors;		// ������ ������
	unordered_map<int, string> codeDict;		// ��� ������ � �� ��������
	CErrorManager();
	~CErrorManager();
	bool readException(string path);		// ��������� ���� ������ � �� �������� �� �����. ���������� false ��� �������
	void printErrors(IErrorPrinter& printer);	// �������� ��������������� ������� � ������, ����������� ������
	void addError(int pos, int line, int code);		// �������� ������ � ���������� �� � ������ errors

};
