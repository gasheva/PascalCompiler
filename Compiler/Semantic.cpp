#include "Semantic.h"
#include <stdexcept>
#include "CVariant.h"
#include <set>

using namespace std;

CSemantic::CSemantic(CErrorManager* eManager, Lexic* lexic) {
	this->eManager = eManager;
	this->lexic = lexic;
}

CSemantic::~CSemantic() {}

void CSemantic::createFictiveScope() {
	// integer, real, string, char - BaseType, true, false - EnumType
	//scopesLst.push_back(CScope(nullptr, this));
	scopesLst.push_back(CScope(nullptr, this->lexic, this->eManager));
	auto fictiveScp = &scopesLst.back();
	(*fictiveScp).createFictive();
}
void CSemantic::createScope() {
	scopesLst.push_back(CScope(&scopesLst.back(), lexic, eManager));
}

CScope::CScope(CScope* outerScope, Lexic* lexic, CErrorManager* eManager) {
	this->outerScope = outerScope;
	this->lexic = lexic;
	this->eManager = eManager;
}

CScope::~CScope() {}

CType CScope::defineAndCreateType(EType type) {
	switch (type) {
	case eINT: case eREAL: case eSTRING: case eCHAR:
		return CBaseType(type);
	case eARRAY:
		return CArrayType();
	case eSUBRANGE:
		return CSubrangeType();
	case eENUM:
		return CEnumType();
	default:
		return CNoneType();
	}
}
void CScope::clearTypesBuff() {
	typesBuff.clear();
}
void CScope::clearNamesBuff() {
	CType* none = nullptr;
	// если произошла кака€-то ошибка в синтаксисе и не был передан тип
	if (typesBuff.empty()) {
		typeTbl.push_back(CNoneType());
		for (auto name : namesBuff) {
			identTbl.insert({ name, CIdetificator(name, flagBlock, &typeTbl.back()) });
		}
		namesBuff.clear();
		return;
	}
	// если хот€ бы один из типов не корректен, то и тип, включающий его также некорректен
	for (auto type : typesBuff) {
		if (type->getType() == eNONE) {
			none = type;
			break;
		}
	}
	if (none!=nullptr) {
		for (auto name : namesBuff) {
			identTbl.insert({ name, CIdetificator(name, flagBlock, none) });
		}
	}
	else
		// если тип определен корректно, то присваиваем его всем переменным
		for (auto name: namesBuff) {
			identTbl.insert({ name, CIdetificator(name, flagBlock, typesBuff.front()) });
		}
	namesBuff.clear();
}
void CScope::addToNameBuffer(string name) {
	namesBuff.push_back(name);
}

void CScope::defineConst(EType type, string constRight) {
	// провер€ем объ€влена ли константа в данном скоупе
	if (identTbl.find(namesBuff.front()) != identTbl.end()) {
		// если уже объвлена, то кидаем ошибку и оставл€ем тип первого об€ъвлени€
		writeMistake(101);
		namesBuff.clear();
	} else {
		// создаем объ€вление константы
		// если справа идентификатор, ищем его среди объ€вленных констант
		if (type == eNONE) {
			auto type = findType(constRight, set<EBlock>{CONSTBL});
			// если переменна€ не найдена
			if (type == nullptr) {
				writeMistake(1002);
			} else {
				typesBuff.push_back(type);
			}
		} else {
			// создаем безым€нный тип дл€ строки, числа и тд
			typeTbl.push_back(defineAndCreateType(type));
			auto type = (&typeTbl.back());
			typesBuff.push_back(type);
			//identTbl.insert(pair<string, CIdetificator>(namesBuff.front(), CIdetificator(namesBuff.front(), flagBlock, type)));
		}
	}
}
void CScope::createNone() {
	typeTbl.push_back(CNoneType());
	typesBuff.push_back(&typeTbl.back());
}
void CScope::addToBuffer(EType type) {
	if (flagBlock == TYPEBL) {
		typeTbl.push_back(defineAndCreateType(type));
	} else if (flagBlock == VARBL) {

	} else { 
		// flagBlock == BODYBL || CONSTBL
		// TODO(throw not_define)
	}
}

void CScope::clearBuffs() {
	clearNamesBuff();
	clearTypesBuff();
}

void CScope::addToBuffer(string typeName) {
	typeTbl.push_back(*(findType(typeName, set<EBlock>{CONSTBL, TYPEBL})));			// получаем ссылку на простой или уже созданный тип

	// создание типа
	if (flagBlock == TYPEBL) {
		// если в буфере уже имеетс€ тип, значит это дополнение составного типа (type compl = array[x..10] of INTEGER <- INTEGER или '1' или х будут такими типами
		if (typesBuff.size() > 0) {
			CType* type = typesBuff.front();
			auto typeName = (*type).getType();
			switch (typeName) {
			case eARRAY:
				//auto arType = (CArrayType*)type;
				//arType->getIndexType();		//TODO()
				break;
			case eSUBRANGE:
				break;
			case eENUM:
				break;
			default:
				break;
			}
		} else {
			// это базовый тип или пользовательский (не составной) и он не €вл€етс€ дополнением к составному типу (type t = INTEGER)
			// объвление этого типа происходит без использование буфера
			// провер€ем не было ли занесено название (name) нового типа в текущий скоуп
			if (identTbl.find(namesBuff.front()) != identTbl.end()) {		// им€ типа хранитс€ в буфере 
				// TODO(throw type_already_exists)
			} else {
				CType* type = findType(typeName, set<EBlock>{TYPEBL});	// ищем объ€вление типа (typeName)
				// если объ€вление типа не найдено, кидаем ошибку
				if (type == nullptr) {
					// TODO(throw no_type)
				} else {
					// если объ€вление типа найдено, то добавл€ем в скоуп новый идент и ссылку на найденный тип
					identTbl.insert(pair<string, CIdetificator>(typeName, CIdetificator(namesBuff.front(), TYPEBL, type)));
				}
			}
		}
	} else if (flagBlock == VARBL) {
		// объ€вление переменной

	} else {
		// flagBlock == BODYBL || CONSTBL
		// TODO(throw not_define)
	}
}

void CScope::setBlock(EBlock block) {
	flagBlock = block;
}

CType* CScope::findType(string name, const set<EBlock> block) {
	CType* type = nullptr;
	if (identTbl.find(name) != identTbl.end()) {
		// идентификатор найден
		if (block.find((identTbl[name]).getBlock()) != block.end()) {
			return (identTbl[name].getType());		// тип найден, возвращаемс€
		} else {
			// идентификатор найден, но это не того типа
			return nullptr;
		}

	} else {
		if (outerScope != nullptr)
			type = outerScope->findType(name, block);
		else return nullptr;
	}
	return type;
}

void CScope::writeMistake(int code) {
	cout << "Mistake " << code << ": " << lexic->getStartPosition() << endl;
	eManager->addError(lexic->getStartPosition(), lexic->getCurLine(), code);
}

void CScope::createFictive() {
	typeTbl.push_back(CBaseType(eINT));
	identTbl.insert({ "integer", CIdetificator("integer", TYPEBL, &(typeTbl.back())) });

	typeTbl.push_back(CBaseType(eREAL));
	identTbl.insert(pair<string, CIdetificator>("real", CIdetificator("real", TYPEBL, &(typeTbl.back()))));

	typeTbl.push_back(CBaseType(eSTRING));
	identTbl.insert(pair<string, CIdetificator>("string", CIdetificator("string", TYPEBL, &(typeTbl.back()))));

	typeTbl.push_back(CBaseType(eCHAR));
	identTbl.insert(pair<string, CIdetificator>("char", CIdetificator("char", TYPEBL, &(typeTbl.back()))));

	typeTbl.push_back(CEnumType(list<string>{"true", "false"}));
	identTbl.insert(pair<string, CIdetificator>("boolean", CIdetificator("boolean", TYPEBL, &(typeTbl.back()))));
}

CIdetificator::CIdetificator(string name, EBlock block, CType* type) {
	this->name = name;
	this->block = block;
	this->type = type;
}

CIdetificator::CIdetificator() {}

CIdetificator::~CIdetificator() {}
