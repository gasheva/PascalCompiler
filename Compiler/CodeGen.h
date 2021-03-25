#pragma once
#include "CTypes.h"
#include <fstream>
#include <iostream>
#include <stack>

class CCodeGen {
	// Все вычисления ведутся во флоат, результат вычислений конвертируется 
	// в левосторонний тип
private:
	ofstream genFile;
	string programName;
	string typeToString(EType type);
	string typeToString(string type);

	stack<string> marksElse;
	string markCont = "";
	int markNum = 0;

	void stackNeg();		// сделать предыдущее число отрицательным
	void stackAdd();
	void stackSub();
	void stackMul();
	void stackDiv();
	void stackMod();
	void stackAssignConv(EType varType);		// конвертирует в тип результата
	void stackConv(EType termType);			// конвертирует во флоат

	void stackGenMark();			// генерирует метку else и добавляет в очередь и в файл
	void stackGenMarkCont();		// генерирует мету continue

	void stackPopMark();			// пишет верхнюю метку в файл и pop
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
	void stackLdcNum(EType type, string num);		// положить число в стек
	void stackLdloc(string varName, EType type);		// положить переменную в стек
	void stackStloc(string varName, EType type);		// записать из стека в переменную
	
	void stackSign(string sign);		// если знак "-", пишет операцию neg

	void stakBoolOper(string oper);		// для <, > ...
	void stakMultOper(string oper);
	void stackAdditOper(string oper);

	

	void stackIf();					// создать метку else
	void stackThenEnd();			// создать метку continue
	void stackElse();				// метка else
	void stackIfEnd();				// метка continue (конец if)


	void stackPrint(string lexem, EType type);
};
