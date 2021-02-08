#pragma once
#include "CError.h"
#include <list>

/*
* Интерфейс для классов, выводящих ошибки
*/
class IErrorPrinter {
public:
	virtual bool print(list<CError> errors) = 0;	// метод печать ошибок в файл
	virtual ~IErrorPrinter() = default;
	
};