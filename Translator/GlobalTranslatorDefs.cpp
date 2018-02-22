#include "stdafx.h"
#include "GlobalTranslatorDefs.h"
#include <fstream>
//Should always return two strings in list
std::vector<std::string> split(std::string original, char delimiter)
{
	std::string preSplitStr;
	std::vector<std::string> returner;
	for (auto x : original)
	{
		if (x == delimiter)
		{
			preSplitStr.push_back(delimiter);
			returner.push_back(preSplitStr);
		}
		else
		{
			preSplitStr.push_back(x);
		}
	}
	return returner;
}
std::vector<std::string> obtainRelations(std::string original, char delimiter)
{
	std::string preStr = "";
	std::vector<std::string> returner;
	for (int i = 0; i < original.size(); i++)
	{
		switch (original[i])
		{
		case '<':
			i++;
			bool Continue = true;
			while (Continue)
			{
				if (original[i] == '>')
				{
					//Continue to check if there are any more >'s. Checks for final. 
					while (++i < original.size() && Continue == true)
					{
						if (original[i] != '>')
						{
							Continue = false;
							returner.push_back(preStr);
							preStr.clear();
							//Resets back a space before in order to ready original
							i-=2;
						}
						else
						{
							preStr.push_back(original[i]);
						}
					}
				}
				else
				{
					preStr.push_back(original[i]);
					i++;
				}
			}
			break;
		}
	}
	return returner;
}

bool ConversionTable::ElementExists(std::string tester)
{
	bool doesExist = false;
	if (tester.size() == 0)
	{
		//If the tester doesn't have a size, then an element of its type doesn't exist.
		return false;
	}
	//Accesses the table block indicated by the first letter of the testing name. 
	for (auto Element : Table[tester[0]])
	{
		if (Element.retrieveID1() == tester)
		{
			return true;
		}
	}
	return false;
}
int ConversionTable::getMaxID1()
{
	return MaxID1;
}
int ConversionTable::getMaxID2()
{
	return MaxID2;
}
void ConversionTable::InitializeTable(std::string pathname, int direc)
{
	std::ifstream stream;
	stream.open(pathname);
	if (!stream.is_open())
	{
		std::cout << "MASSIVE ABORTIONS";
	}
	std::string raw_input;
	stream >> std::noskipws;
	//Initializes Conversion Table
	while (std::getline(stream, raw_input))
	{
		std::vector<std::string> splitRaw_input;                   
		//Retrieve the two splitters
		splitRaw_input = obtainRelations(raw_input, ':');
		//Adds a conversionToken to table at the location of the first string in splitrawinputs first letter's integer ASCII value 
		Table[splitRaw_input[direc][0]].push_back(ConversionToken(ConvertToCharLiteral(splitRaw_input[direc]), ConvertToCharLiteral(splitRaw_input[std::abs(direc - 1)])));
		//Updates max keyword length values
		std::cout << splitRaw_input[direc] << "K" << std::endl;
		if (splitRaw_input[direc].size() > MaxID1)
		{
			MaxID1 = splitRaw_input[direc].size();
		}
		else if (splitRaw_input[std::abs(direc - 1)].size())
		{
			MaxID2 = splitRaw_input[std::abs(direc - 1)].size();
		}
	}
	stream.close();
}

KeyWordToken ConversionTable::ConvertToken(KeyWordToken original)
{
	for (auto ConvertTable : Table[original.retrieveContent()[0]])
	{
		if (ConvertTable.retrieveID1() == original.retrieveContent())
		{
			return ConvertTable.ProduceConvertedToken(original);
		}
	}
}

std::string ConversionToken::retrieveID1()
{
	return ID1;
}

KeyWordToken ConversionToken::ProduceConvertedToken(KeyWordToken original)
{
	return KeyWordToken(ID2);
}

ConversionToken::ConversionToken(std::string _ID1, std::string _ID2)
{
	ID1 = _ID1;
	ID2 = _ID2;
}

std::string ConvertToStringLiteral(char tester)
{
	if (tester == '\n')
	{
		return "\\n";
	}
	else if (tester == '\w')
	{
	    return  " " ;
	}
	else
	{
		return std::string(1, tester);
	}
	return 0;
}

std::string ConvertToCharLiteral(std::string tester)
{
	if (tester == "\\n")
		return std::string(1,'\n');
	else if (tester == "\w")
		return std::string(1, ' ');
	else
		return tester;

	return std::string();
}

void Translator::deleteTokenVector(std::vector<Token*>& TokenList)
{
	for (auto Token : TokenList)
	{
		delete Token;
	}
	TokenList.clear();
}
void Translator::LoadSourceText(std::string path)
{
	//This version operates under the assumption that a space will indicate a utoken(as is the case in the majority of programming languages). If this rule is broken,
	//implement functionality within a preparsing file to specify what the variable delimiter should be 
	std::vector<Token*> VirtualRepresentation;
	std::string rawStreamData;
	std::ifstream stream;
	stream.open(path);
	if(stream.is_open() != true)
	{
		std::cout << "MASSIVE ABORTIONS";
	}
	stream >> std::noskipws;
	char currChar;
	while (stream.get(currChar))
	{
		rawStreamData += currChar;
		std::cout << currChar;
	}
	//Now, analyze/split raw stream data
	std::string TempStr;
	for (int i = 0; i < rawStreamData.size(); i++)
	{
		//Checks for single character priority 1 delimiters and spaces
		
		if (Table.ElementExists(TempStr))//COME BACK TO THIS LINE
		{
			if (Table.ElementExists(TempStr))
			{
				VirtualRepresentation.push_back(new KeyWordToken(TempStr));
			}
			else
			{
				VirtualRepresentation.push_back(new UToken(TempStr));
			}
			TempStr.clear();
			TempStr.push_back(rawStreamData[i]);
		}
		else
		{
			TempStr.push_back(rawStreamData[i]);
			int counter = i;
			std::string lookahead;
			//Look ahead to see if a keyword is there. If so, we will jump ahead in the program, pushing back a UToken and a KeyWordToken 
			for (int counter = i + 1; counter <= i + Table.getMaxID1() && counter < rawStreamData.size(); counter++)
			{
				lookahead.push_back(rawStreamData[counter]);
				if(Table.ElementExists(lookahead))
				{
					i = (counter + 1) < rawStreamData.size() ? counter : counter + 1 ;
					if (Table.ElementExists(TempStr))
					{
						VirtualRepresentation.push_back(new KeyWordToken(TempStr));
					}
					else
					{
						VirtualRepresentation.push_back(new UToken(TempStr));
					}
					VirtualRepresentation.push_back(new KeyWordToken(lookahead));
					TempStr.clear();
					break;
				}
			}
		}
	}
	SourceText = VirtualRepresentation;
	stream.close();
}
std::vector<Token*> Translator::DirectTranslate()
{
	std::vector<Token*> UntranslatedList = SourceText;
	std::vector<Token*> TranslatedList;
	for (auto Word : UntranslatedList)
	{
		if (Word->type == KEYWORDTOKEN)
		{
			TranslatedList.push_back(new KeyWordToken(Table.ConvertToken(*static_cast<KeyWordToken*>(Word))));
		}
		else if(Word->type == UTOKEN)
		{
			TranslatedList.push_back(new UToken(*static_cast<UToken*>(Word)));
		}
	}
	return TranslatedList;
}
std::vector<Token*> Translator::ComplexTranslate()
{
	return std::vector<Token*>();
}
std::vector<Token*> Translator::Format()
{
	return std::vector<Token*>();
}
void Translator::Write(std::string str, std::vector<Token*> writeCont)
{
	std::fstream writer;
	writer.open(str, std::fstream::out | std::fstream::trunc);
	for (auto x : writeCont)
	{
		std::cout << "Writing!" << x->retrieveContent() <<  std::endl;
		x->write(&writer);
	}
	writer.close();
}
void Translator::Run()
{
	bool Continue = true;
	std::string userIn;
	while (Continue)
	{
		std::cout << "Welcome, please enter the configuration file for the language you'd like to translate to.(Direction of Translate)" << std::endl;
		std::cin >> userIn;
		Table.InitializeTable("TestLang1.txt_RELATES_TestLang2.txt", std::stoi(userIn));
		userIn.clear();
		std::cout << "Please enter the file you'd like to translate." << std::endl;
		std::cin >> userIn;
		LoadSourceText("TestLang1.txt");
		userIn.clear();
		std::cout << "Beginning Translation..." << std::endl;
		std::vector<Token*> TLText = DirectTranslate();
		Format();
		std::cout << "Please enter the file you'd like to write to.";
		std::cin >> userIn;
		Write("Test.txt", TLText);
		std::cout << "Success!" << std::endl;
	}
}
UToken::UToken(std::string _content)
{
	this->content = _content;
	type = TokenType::UTOKEN;
}
KeyWordToken::KeyWordToken(std::string _content)
{
	this->content = _content;
	type = TokenType::KEYWORDTOKEN;
}

void Token::write(std::fstream * writer)
{
	(*writer) << content;
}

std::string Token::retrieveContent()
{
	return content;
}
