#pragma once
#include"DefinitionEditor.h"
class LiveToken : public Token
{
public:
	KeyWord Value;
	void print(std::fstream * Fabs);
	//Takes a keyword and converts Value into the specified keyword
	void convert(KeyWord newvalue);
	LiveToken(KeyWord valueC);
};
class Translator
{
	//Raw Data
	std::vector<LiveToken> RD;
	//Intermediate Conversion Table
	std::vector<KeyRelation> ICT;
	//Arbitrary Conversion Table
	std::vector<KeyRelation> ACT;
	void ConvertFromTable(std::vector<KeyRelation> Table, std::vector<LiveToken> convertees, int relatestart, int relateend);
	void phaseOne();
	void phaseTwo();
	Translator(std::vector<LiveToken> RDC);
};