#pragma once
#include"IErrorPrinter.h"

/*
* �������� ������ � ����
*/
class CFileOnlyErrorPrinter: public IErrorPrinter{
public:
	string path = "";		// ���� �� �������� ������� ��������� ������
	bool print(list<CError> errors) override;	// ����� ��� ������ ������ �� ���� path
	CFileOnlyErrorPrinter(string path);
	CFileOnlyErrorPrinter();
	~CFileOnlyErrorPrinter() override;
};
