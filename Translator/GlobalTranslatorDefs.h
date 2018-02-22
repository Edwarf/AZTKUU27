#pragma once
#include<vector>
#include<string>
#include<iostream>
#include<array>
std::string ConvertToStringLiteral(char tester);
std::string ConvertToCharLiteral(std::string tester);
enum TokenType
{
	UTOKEN,
	KEYWORDTOKEN,
};
//This file defines what we'll need to translate from a language to bytecode
class Token
{
protected:
	std::string content;
	//For a token, a TokNumber represents its place in its storage container.
	int TokNumber;
public:
	virtual void write(std::fstream* writer);
	std::string retrieveContent();
	TokenType type;
};
class RawToken : public Token
{
	std::string raw_content;
};
class UToken : public Token
{
public:
	UToken(std::string _content);
};
//When translator reads through source file, it will seperate the things it finds into keywords and utokens
class KeyWordToken : public Token
{
public:
	KeyWordToken(std::string _content);
};
//Note that when reading code from a file, we'll be reading ID1 and producing ID2. Further note
//ConversionTokens stand only as things to convert live tokens. Note that it is not a child class of Token and doe not have a TokenType
class ConversionToken 
{
	//Start of advanced array lookup definitions
	//End of advanced conversionTable lookup definitions
	//ID1 is the keyword from the original language
	std::string ID1;
	//ID2 is the keyword from the new language
	std::string ID2;
public:
	std::string retrieveID1();
public:
	//This takes an original  Token and converts it to a new one. Converts from ID1-> ID2. Might want to return array of possible tokens if unsure what type is. 
	KeyWordToken ProduceConvertedToken(KeyWordToken original);
	ConversionToken(std::string _ID1, std::string _ID2);
};

class ConversionTable
{
public:
	//Stores the biggest possible keyword(ID1). Will be used to limit lookahead in Translator
	int MaxID1 = 0;
	//Stores the biggest possible keyword(ID2). Will be used to limit lookahead in Translator
	int MaxID2 = 0;
	//Goes through the table to see if a conversion specification exists for the given string  
	bool ElementExists(std::string tester);
	//Array of conversion tables by first letter of ID1 in ConversionElements. Access with ASCII code. 
	std::array<std::vector<ConversionToken>, 127> Table;
public:
	int getMaxID1();
	int getMaxID2();
	//pathname is asking for the path of the relation file. 
	void InitializeTable(std::string pathname, int direc);
	//Convert
	KeyWordToken ConvertToken(KeyWordToken original);

};
//May have to add preprocessor file and handling later
//To maintain diferent versions of Token Vectors(at different stages in translation process), consider maintaining a vector of token vector pointers.
class Translator
{
	//Takes a string. If the string is char lit(\w, \n etc.), will return the actual literal. If not, will simply return the original characer. 

	std::string FirstLanguage;
	std::string SecondLanguage;
	ConversionTable Table;
	//Will take vector of tokens, delete its elements, and clear it. Don't use the vector after putting it through this.
	void deleteTokenVector(std::vector<Token*>& TokenList);
	//TOKENS IN THIS CLASS ARE DYNAMICALLY ALLOCATED AFTER LOADING. MAKE SURE TO CLEAN 
	std::vector<Token*> SourceText;
	//Loads the source text into SourceText module. 
	void LoadSourceText(std::string path);
	//Simply maps one keyword in one language to another in another language
	std::vector<Token*> DirectTranslate();
	//Utilizes additional information specified in additional files to create a progmatic representation of the original text to translate more efficiently
	std::vector<Token*> ComplexTranslate();
	//Utilizes information specified in formatting files to create a progmatic representation of the original text 
	std::vector<Token*> Format();
	//Writes a vector of tokens to a file
	void Write(std::string str, std::vector<Token*> text);
public:
	void Run();
};
