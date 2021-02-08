#include "CErrorManager.h"
#include <fstream>
#include <sstream>
#include <iostream>
using namespace std;

CErrorManager::CErrorManager()
{
}

CErrorManager::~CErrorManager()
{
    //cout << "[D] ErrorManager destructor" << endl;
}

/*
* Чтение ошибок из файла и сохранение их в словарь ошибок codeDict
*/
bool CErrorManager::readException(string path)
{
    string line, text(""), descr("");
    ifstream myfile;
    int code;

    myfile.open(path);
    if (!myfile.is_open())
        return false;

    while (getline(myfile, line)) {
        int i = 0;

        while (isdigit(line[i])) {
            i++;
        }
        code = stoi(line.substr(0, i));
        descr = line.substr(i + 1, (line.length()-1-i+1));
        codeDict.insert({ code, descr });
    }
    myfile.close();
    
	return true;
}

/*
* Печать ошибок при помощи передаваемого в метод принтера
*/
void CErrorManager::printErrors(IErrorPrinter& printer)
{
    printer.print(errors);
}

/*
* Создать ошибку и добавить ее в список
*/
void CErrorManager::addError(int pos, int line, int code)
{
    errors.push_back(CError(pos, line, code, codeDict[code]));
}


