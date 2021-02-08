#pragma once
#include "CError.h"
#include <list>

/*
* ��������� ��� �������, ��������� ������
*/
class IErrorPrinter {
public:
	virtual bool print(list<CError> errors) = 0;	// ����� ������ ������ � ����
	virtual ~IErrorPrinter() = default;
	
};