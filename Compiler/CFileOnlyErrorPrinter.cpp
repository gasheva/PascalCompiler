#include "CFileOnlyErrorPrinter.h"
#include <fstream>
#include <iostream>

/*
* ѕечать ошибок по указанному раннее пути
* ¬озвращает true - в случае успеха, иначе false
*/
bool CFileOnlyErrorPrinter::print(list<CError> errors)
{
    if (path == "") return false;
    ofstream myfile;
    myfile.open(path);
    if (!myfile.is_open())
        return false;

    for (CError er : errors) {
        myfile << "[" << er.code << "] line = "<<(er.line)<< " pos = "<<er.pos<< " - "<< er.description <<endl;
    }
    myfile.close();
    return true;
}

CFileOnlyErrorPrinter::CFileOnlyErrorPrinter(string path)
{
    this->path = path;
}

CFileOnlyErrorPrinter::CFileOnlyErrorPrinter(){}

CFileOnlyErrorPrinter::~CFileOnlyErrorPrinter()
{
    //std::cout << "[D] CFileOnlyErrorPrinter destructor" << endl;
}
