#include "FStringFunc.h"
#include <regex>
using namespace std;

string toLower(string lexem)
{
	transform(lexem.begin(), lexem.end(), lexem.begin(), ::tolower);
	return lexem;
}
