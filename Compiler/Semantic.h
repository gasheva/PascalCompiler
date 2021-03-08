#pragma once


using namespace std;

class CSemantic {
private:
	vector<CScope> scopesLst;		// ������� ������� �������� ��������� �����

public:
	CSemantic();
	~CSemantic();
	void createFictiveScope();
	void createScope();
};

class CScope{
private:
	CScope* outerScope;			// ������� ������� ��������
	CIdentTable identTbl;		// ������� ���������������
	vector<CType> typeTbl;		// ������� �����
	CType* findTypeByIdent(string name);		// ���������� ��� �� ��������������
public:
	CScope(CScope* outerScope);
	~CScope();
	void addSymbol(string name, EBlock block);
	void createType(string typeName);
};
class CIdentTable{
private:
	struct identcomp {
		bool operator()(const CIdetificator ident, const string str) const {
			return ident.getName().compare(str);
		}
	};
	map<CIdetificator, CType*, identcomp> identTbl;			// ������� ��������������� <���_�����, ������_����>	

public:
	CIdentTable();
	~CIdentTable();
	void addSymbol(string name, EBlock block);
	CType* findTypeByIdent(string name);
};

class CIdetificator {
	
private:
	string name;
	EBlock block;
public:
	CIdetificator(string name, EBlock block);
	string getName() const { return name; }
	EBlock getBlock() const { return block; }
};

enum EBlock { CONSTBL, TYPEBL, VARBL, BODYBL };
// ���������� ����� ���� �� ���������, ������ ��������������, ����� ��� ��������� � ������ BODY � ����� NULL (������������� ���)