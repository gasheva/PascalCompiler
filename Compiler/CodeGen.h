#pragma once
#include "CTypes.h"
#include <fstream>
#include <iostream>
#include <stack>

class CCodeGen {
	// ¬се вычислени€ ведутс€ во флоат, результат вычислений конвертируетс€ 
	// в левосторонний тип
private:
	ofstream genFile;
	string programName;
	string typeToString(EType type);
	string typeToString(string type);

	stack<string> marksElse;
	string markContIf = "";
	stack<string> marksWhileExpr;
	stack<string> marksWhileCont;

	int markNum = 0;

	void stackNeg();		// сделать предыдущее число отрицательным
	void stackAdd();
	void stackSub();
	void stackMul();
	void stackDiv();
	void stackMod();
	void stackAssignConv(EType varType);		// конвертирует в тип результата
	void stackConv(EType termType);			// конвертирует во флоат

	string genMark(string str);
	void stackGenMarkElse();			// генерирует метку else и добавл€ет в очередь
	void stackGenMarkContIf();			// генерирует метку continue if
	void stackGenMarkContWhile();		// генерирует метку continue while
	void stackGenMarkWhileExpr();		// генерирует метку expression


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

	void stakBoolOper(string oper);		// дл€ <, > ...
	void stakMultOper(string oper);
	void stackAdditOper(string oper);

	

	void stackIf();					// создать метку else
	void stackThenEnd();			// создать метку continue
	void stackElse();				// метка else
	void stackIfEnd();				// метка continue (конец if)

	void stackWhileExpr();			// генерирует метку выражени€ в while
	void stackWhileGenCont();		// генерирует метку конца while
	void stackWhileWhileEnd();		// возвращает к выражению
	void stackWhileCont();			// метка continue после while

	void stackPrint(string lexem, EType type);
};
