// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"
#include <iostream>
#include<iomanip>
#include<string>
#include <fstream>
#include <vector>

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
    //string text("progRam pr1(f1, f2); var //v1, v2: Integer; \n {vhjhugv3,{}}//\n v4: String; BeGin x:=3; y:=2 enD.");
    // string text("progRam pr1(f1, f2); var v1, v2: Integer; \n vhjhugv3, v4: String; BeGin x:=3; y:=2 enD.");
    string text ="PROGRAM pre(f1); \n" 
        //" CONST x=8; z=9; \n" 
        //" VAR z, t: INTEGER; \n" 
        " BEGIN \n"
        " {y:=3+; \n" 
        " x:='red'} \n"
        //" if x<>4 then \n"
        //" BEGIN \n"
        " if x[x1,x2]<>4 then \n"
            " zhjk:='gred' else t:=6;\n"
        //" ux:=2+(x*7/(2-y))+45; \n"
        //" xy:=kl; \n"
        //" else el2:=6; \n"
        " a[b[0,2]]:='gred'; \n"
        //" END; \n"
        //" else \n"
        //" BEGIN \n"
        //" el:=6; \n"
        //" END; \n"
        " END. ";
    text = "PROGRAM pre(f1); \n"
        " BEGIN \n"
        " bool_var_true:= true; \n"
        " bool_var_false:= false; \n"
        " while x<>4 do \n"
        //" else el2:=6; \n"
            " BEGIN \n"
                " if x[x1,x2]<>4 then \n"
                    " 8zhjk:='gred' else t:=6;\n"
                " a[b[0,2]]:='gred'; \n"
            //" END; \n"
        " END. ";
   /* text = "PROGRAM pre(f1); \n"
        "TYPE metall = (fe, al)"
        "begin end.";*/
   /* text = "PROGRAM pre(f1); \n"
        " CONST x=8; z=9; \n"
        " TYPE ar = array [1..Sz] of integer; \n"
        "      metall = (Al, Fe); \n"
        "      years = 2001..2010; \n"
        " VAR n1, n2: -3..6; \n"
        " str1, str2: (Ge, O2); \n"
        " ar1: array [1..Sz] of integer; \n"
        " r1, r2: REALL; \n"
        "begin end."; */
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

