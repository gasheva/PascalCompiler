// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"
#include <iostream>
#include<iomanip>
#include<string>
#include <fstream>
#include <vector>

//#include "CError.h"
//#include "CErrorManager.h"
//#include "CFileOnlyErrorPrinter.h"
//#include "CVariant.h"
//#include "Lexic.h"
//#include "CTokenFactory.h"


#include "Syntax.h"

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
    /*string text("");
    text = readFile("C:/Users/DocGashe/source/repos/Compiler/Compiler/resources/input.txt");
    if (text != "")
        cout << text;
    else
        cout << "empty";
   

    CErrorManager eManager = CErrorManager();
    eManager.readException("C:/Users/DocGashe/source/repos/Compiler/Compiler/resources/errors.txt");
    Lexic lexic = Lexic(&eManager, &text);
    lexic.getNext();


     CFileOnlyErrorPrinter filePrinter = CFileOnlyErrorPrinter();
     filePrinter.path = "C:/Users/DocGashe/source/repos/Compiler/Compiler/resources/errorsFound.txt";

     IErrorPrinter* printer = new CFileOnlyErrorPrinter();
     ((CFileOnlyErrorPrinter*)printer)->path = "C:/Users/DocGashe/source/repos/Compiler/Compiler/resources/errorsFound.txt";
    
    eManager.printErrors(*printer);

     delete printer;*/

    /*CErrorManager eManager = CErrorManager();
    string text("1+3*32-(9+1)/43");
    eManager.readException("C:/Users/DocGashe/source/repos/Compiler/Compiler/resources/errors.txt");
    Lexic lexic = Lexic(&eManager, &text);
    Syntax syntax = Syntax(&eManager, &lexic , nullptr);
    syntax.startVer();*/


    CErrorManager eManager = CErrorManager();
    string text("1+3*32-(9+1)/43");
    Lexic lexic = Lexic(&eManager, &text);
    CValueToken* token = ((CValueToken*)lexic.getNext());
    CVariant* variant = token->m_val;
    ((CIntVariant*)variant)->m_val;
  
    delete token;
}

