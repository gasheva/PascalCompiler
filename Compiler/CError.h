#pragma once
#include<string>

using namespace std;

/*
* ����� ������� ������. ����� ������������� ������� ������������ �������� ������ 
* �������� ��� ������, ��������, �������, ��� ��������� ��� ��������
*/
class CError
{
public:
	int pos;		// ������� � ������
	int line;		// ����� ������
	int code;		// ��� ������
	string description;	// �������� ������
	CError(int pos, int line, int code, string description);
	virtual ~CError();
};
