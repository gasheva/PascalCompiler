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

#include "CFileOnlyErrorPrinter.h"
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

    CErrorManager eManager = CErrorManager();
    //string text("const _y1o='YO!'; f2 = -1; ");         
    //string text("const _y1o='YO!'; f2 = -1; var v1, v2: Integer; v3, v4: String;");         
    //string text("var v1, v2: Integer; v3, v4: String;");
    //string text("BeGin x:=12; v2:='fds' End");
    //string text("var v1, v2: Integer; v3, v4: String; BeGin x:=12; v2:='fds' End");
    //string text("var v1, v2: Integer; v3, v4: String; BeGin x:=");
    string text("program pr1(f1, f2); var v1, v2: Integer; v3, v4: String; BeGin x:=1.2; v2:='fds' End.");
    eManager.readException("C:/Users/DocGashe/source/repos/Compiler/Compiler/resources/errors.txt");
    Lexic lexic = Lexic(&eManager, &text);
    Syntax syntax = Syntax(&eManager, &lexic , nullptr);
    syntax.startVer();

    IErrorPrinter* printer = new CFileOnlyErrorPrinter();
    ((CFileOnlyErrorPrinter*)printer)->path = "C:/Users/DocGashe/source/repos/Compiler/Compiler/resources/errorsFound.txt";

    eManager.printErrors(*printer);

    delete printer;

    // 43*6 + ( 7)
    // 2 + (- 7 *(5-two))
    // func() + (-2)

    /*CTokenFactory factory = CTokenFactory();
    unique_ptr<CToken> unq(factory.createToken("+"));
    unique_ptr<CToken> mov(factory.createToken("-"));
    mov = move(unq);
    auto type = mov->getType();
    type = IDENT;
    cout << mov->getType() << IDENT << endl;*/
    


    /*CErrorManager eManager = CErrorManager();
    string text("1+3*32-(9+1)/43");
    Lexic lexic = Lexic(&eManager, &text);
    CValueToken* token = ((CValueToken*)lexic.getNext());
    CVariant* variant = token->m_val;
    ((CIntVariant*)variant)->m_val;
  
    delete token;*/
}

