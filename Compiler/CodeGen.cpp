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
void CCodeGen::stackInitVar(EType type, string lexem) {
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

void CCodeGen::stakBoolOper(string oper) {
	if (oper == "<")
		genFile << "clt\n";
	else if (oper == "=")
		genFile << "ceq\n";
	else if (oper == "<>")
		genFile << "ceq \n"
		"ldc.i4.0\n"
		"ceq\n";
	else if (oper == ">")
		genFile << "cgt\n";
	else if (oper == ">=")
		genFile << "clt\n"
		"ldc.i4.0\n"
		"ceq\n";
	else if (oper == "<=")
		genFile << "cgt\n"
		"ldc.i4.0\n"
		"ceq\n";
	stackConv(eINT);		// все вычисления в float
}

void CCodeGen::stakMultOper(string oper) {
	if (oper == string("*"))
		stackMul();
	else if (oper == string("/") || oper == string("div"))
		stackDiv();
	else if (oper == string("mod"))
		return stackMod();
	else if (oper == string("and"))
		genFile << "and\n";
}

void CCodeGen::stackAdditOper(string oper) {
	if (oper == string("-"))
		return stackSub();
	else if (oper == string("+"))
		return stackAdd();
	else if (oper == string("or"))
		genFile << "or\n";
}

string CCodeGen::genMark(string str) {
	auto res = str + std::to_string(markNum);
	markNum++;
	return res;
}

void CCodeGen::stackGenMarkElse() {
	marksElse.push(genMark("LE_"));
}

void CCodeGen::stackGenMarkContIf() {
	markContIf = genMark("LC_");
}

void CCodeGen::stackGenMarkContWhile() {
	marksWhileCont.push(genMark("LWC_"));
}

void CCodeGen::stackGenMarkWhileExpr() {
	marksWhileExpr.push(genMark("LWE_"));
}

void CCodeGen::stackPopMark() {
	if (!marksElse.empty()) {
		genFile << marksElse.top()<<":\n";
		marksElse.pop();
	}
}

void CCodeGen::stackBrfalse() {
	genFile << "brfalse ";
	stackGenMarkElse();
	genFile << " " + marksElse.top() + "\n";
}

void CCodeGen::stackBrS() {
	genFile << "br.s ";
	stackGenMarkContIf();
	genFile << " " + markContIf + "\n";
}

void CCodeGen::stackIf() {
	// в стеке лежит посчитанное выражение, сравниваем его с 1 (true)
	genFile << "ldc.r4 1\n"
		"ceq\n";
	stackBrfalse();
}

void CCodeGen::stackThenEnd() {
	stackBrS();
}

void CCodeGen::stackElse() {
	stackPopMark();
}

void CCodeGen::stackIfEnd() {
	genFile << markContIf << ":\n";
}

void CCodeGen::stackWhileExpr() {
	stackGenMarkWhileExpr();
	genFile << marksWhileExpr.top() + ":\n";
}

void CCodeGen::stackWhileGenCont() {
	stackGenMarkContWhile();
	genFile <<"brfalse " + marksWhileCont.top() + "\n";
}

void CCodeGen::stackWhileWhileEnd() {
	genFile << "br.s " + marksWhileExpr.top() + "\n";
	marksWhileExpr.pop();
}

void CCodeGen::stackWhileCont() {
	genFile << marksWhileCont.top()+":\n";
	marksWhileCont.pop();
}

void CCodeGen::stackPrint(string lexem, EType type) {
	genFile << 
		"ldstr \" " + lexem + " = {0}\"\n"
		"ldloc " + lexem + "\n"
		"box " + typeToString(type) + "\n"
		"call void [mscorlib]System.Console::\n"
		"WriteLine(string, object)\n";
}
