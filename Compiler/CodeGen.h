#pragma once
#include "CTypes.h"
#include <fstream>
#include <iostream>
#include <queue>

class CCodeGen {
	// Все вычисления ведутся во флоат, результат вычислений конвертируется 
	// в левосторонний тип
private:
	ofstream genFile;
	string programName;
	string typeToString(EType type);
	string typeToString(string type);
	string boolOperToString(string oper);

	queue<string> marks;
	int markNum = 0;

	void stackNeg();		// сделать предыдущее число отрицательным
	void stackAdd();
	void stackSub();
	void stackMul();
	void stackDiv();
	void stackMod();
	void stackAssignConv(EType varType);		// конвертирует в тип результата
	void stackConv(EType termType);			// конвертирует во флоат

	void stackGenMark();			// генерирует метку и добавляет в очередь и в файл

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
	void stackLdcNum(EType type, string num);		// положить число в стек
	void stackLdloc(string varName, EType type);		// положить переменную в стек
	void stackStloc(string varName, EType type);		// записать из стека в переменную
	
	void stackSign(string sign);		// если знак "-", пишет операцию neg

	void stakMultOper(string oper);
	void stackAdditOper(string oper);

	
	void stackPopMark();			// пишет верхнюю метку в файл и pop
	void stackBrfalse();
	void stackBrS();

	void stackPrint(string lexem, EType type);
};
