#pragma once
#include"IErrorPrinter.h"

/*
* Печатает ошибки в файл
*/
class CFileOnlyErrorPrinter: public IErrorPrinter{
public:
	string path = "";		// путь по которому следует сохранять ошибки
	bool print(list<CError> errors) override;	// метод для печати ошибок по пути path
	CFileOnlyErrorPrinter(string path);
	CFileOnlyErrorPrinter();
	~CFileOnlyErrorPrinter() override;
};
