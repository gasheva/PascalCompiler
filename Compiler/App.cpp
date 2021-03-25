// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"
#include <iostream>
#include<iomanip>
#include<string>
#include <fstream>
#include <vector>

#include "CFileOnlyErrorPrinter.h"
#include "CCompiler.h"

using namespace std;

string readFile(string path) {
    string line, text("");
    ifstream myfile;
    myfile.open(path);
    if (!myfile.is_open())
        return string("");

    while (getline(myfile, line)) {
        text+=line + '\n';
    }
    myfile.close();
    return text;
}

int main()
{
    CErrorManager eManager = CErrorManager();
    string text;
    text = readFile("C:/Users/DocGashe/source/repos/Compiler/Compiler/resources/input.txt");
    if (text != "")
        cout << text;
    else
        cout << "empty";

    eManager.readException("C:/Users/DocGashe/source/repos/Compiler/Compiler/resources/errors.txt");
    CLexic lexic = CLexic(&eManager, &text);
    CSemantic semantic = CSemantic(&eManager, &lexic);
    CCodeGen codeGen = CCodeGen();
    
    semantic.createFictiveScope();
    semantic.createScope();

    CCompiler syntax = CCompiler(&eManager, &lexic , &semantic, &codeGen);
    syntax.startVer();

    IErrorPrinter* printer = new CFileOnlyErrorPrinter();
    ((CFileOnlyErrorPrinter*)printer)->path = "C:/Users/DocGashe/source/repos/Compiler/Compiler/resources/errorsFound.txt";

    eManager.printErrors(*printer);

    delete printer;
}

