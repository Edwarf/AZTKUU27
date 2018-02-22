#include "stdafx.h"
#include "Translator.h"
void LiveToken::print(std::fstream* Fabs)
{
	*Fabs << Value.content;
}
void LiveToken::convert(KeyWord newvalue)
{
	Value = newvalue;
}
LiveToken::LiveToken(KeyWord valueC)
{
	Value = valueC;
}
void Translator::ConvertFromTable(std::vector<KeyRelation> Table, std::vector<LiveToken> convertees, int relatestart, int relateend)
{
	for (auto z : convertees)
	{
		for (auto i : Table)
		{
			//If the raw text values of the two strings are identical. Replace with type enum at later date.
			if (z.Value.content == i.Relations[relatestart]->content);
			{
				//Replaces the value of z with the associated relation
				z.Value = *dynamic_cast<KeyWord*>(i.Relations[relateend]);
			}
		}
	}
}
void Translator::phaseOne()
{
	ConvertFromTable(ICT, RD, 0, 1);
}
void Translator::phaseTwo()
{
	ConvertFromTable(ACT, RD, 0, 1);
}
Translator::Translator(std::vector<LiveToken> RDC)
{
	RD = RDC;
}                                                                                                                                                                                             
