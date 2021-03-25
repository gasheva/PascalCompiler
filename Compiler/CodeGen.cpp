#include "CodeGen.h"
#include <string>
using namespace std;

string CCodeGen::typeToString(EType type) {
	switch (type) {
	case eINT:
		return "int32";
		break;
	case eREAL:
		return "float32";
		break;
	case eSTRING:
		return "string";
		break;
	case eCHAR:
		return "char";
		break;
	case eBOOLEAN:
		return "bool";
		break;
	default:
		break;
	}
	return "";
}
string CCodeGen::typeToString(string type) {
	if (type=="integer")
		return "int32";
	if (type == "real")
		return "float32";
	if (type == "boolean")
		return "bool";
	return type;
}

string CCodeGen::boolOperToString(string oper) {
	if (oper == "<")
		return "clt\n";
	if (oper == "=")
		return "ceq\n";
	if (oper == "<>")
		return "ceq \n"
			"ldc.i4.0\n"
			"ceq\n";
	if (oper == ">")
		return "cgt\n";
	if (oper == ">=")
		return "clt\n"
		"ldc.i4.0\n"
		"ceq";
	if (oper == "<=")
		return "cgt\n"
		"ldc.i4.0\n"
		"ceq\n";
	return "";
}

CCodeGen::CCodeGen() {}

void CCodeGen::openFile(string path, string name) {
	//if (path == "") return;
	programName = name;
	path = "C:/Users/DocGashe/source/repos/Compiler/Compiler/resources/" + programName + ".il";
	genFile.open(path);
	if (!genFile.is_open())
		return;
}

void CCodeGen::closeFile() {
	genFile.close();
}

void CCodeGen::stackMaxStack(int count) {
	genFile << ".maxstack " + count;
}

void CCodeGen::stackWriteBegin() {
	genFile <<
		".assembly extern mscorlib {} \n"
		".assembly "+ programName+" {} \n"
		".module " + programName + ".exe \n"
		".class " + programName + ".Program \n"
		"extends [mscorlib]System.Object \n"
		"{ \n"
		".method static void Main(string[] args) \n"
		"cil managed \n"
		"{\n"
		".entrypoint\n";
}
void CCodeGen::stackWriteEnd() {
	genFile << "ret }}";
}
void CCodeGen::stackInitConst() {

}
void CCodeGen::stackInitType() {

}
void CCodeGen::stackInitVar(string type, string lexem) {
	genFile << ".locals init(" + typeToString(type) + " " + lexem + ")\n";
}

void CCodeGen::stackLdcNum(EType type, string num) {
	if (type==eREAL || type == eINT)
	genFile << "ldc.r4 "+num + "\n";
}

void CCodeGen::stackLdloc(string varName, EType type) {
	genFile << "ldloc "+varName+"\n";
	stackConv(type);
}

void CCodeGen::stackStloc(string varName, EType type) {
	stackAssignConv(type);
	genFile << "stloc " + varName + "\n";
}

void CCodeGen::stackNeg() {
	genFile << "neg\n";
}

void CCodeGen::stackSign(string sign) {
	if (sign == "-")
		stackNeg();
}

void CCodeGen::stackAdd() {
	genFile << "add\n";
}
void CCodeGen::stackSub() {
	genFile << "sub\n";
}
void CCodeGen::stackMul() {
	genFile << "mul\n";
}
void CCodeGen::stackDiv() {
	genFile << "div\n";
}

void CCodeGen::stackMod() {
	genFile << "rem\n";
}

void CCodeGen::stackAssignConv(EType type) {
	if (type == eREAL)
		genFile << "conv.r4\n";
	else if (type == eINT)
		genFile << "conv.i4\n";
}

void CCodeGen::stackConv(EType termType) {
	if (termType == eINT)
		genFile << "conv.r4\n";
}

void CCodeGen::stakMultOper(string oper) {
	if (oper == string("*"))
		stackMul();
	else if (oper == string("/") || oper == string("div"))
		stackDiv();
	else if (oper == string("mod"))
		return stackMod();
}

void CCodeGen::stackAdditOper(string oper) {
	if (oper == string("-"))
		return stackSub();
	else if (oper == string("+"))
		return stackAdd();
}

void CCodeGen::stackGenMark() {
	string mark = "L_" + markNum;
	marks.push(mark);
	genFile << " " + mark + "\n";
	markNum++;
}

void CCodeGen::stackPopMark() {
	if (!marks.empty()) {
		genFile << marks.front()<<":\n";
		marks.pop();
	}
}

void CCodeGen::stackBrfalse() {
	genFile << "brfalse ";
	stackGenMark();
}

void CCodeGen::stackBrS() {
	genFile << "br.s ";
	stackGenMark();
}

void CCodeGen::stackPrint(string lexem, EType type) {
	genFile << 
		"ldstr \" " + lexem + " = {0}\"\n"
		"ldloc " + lexem + "\n"
		"box " + typeToString(type) + "\n"
		"call void [mscorlib]System.Console::\n"
		"WriteLine(string, object)\n";
}
