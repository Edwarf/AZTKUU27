#pragma once
#include <string>
#include<vector>
#include<fstream>
#include<iostream>
enum TokenType
{
	EndLineT,
	KeyWordT,
	AccessoryTextT,
	SpaceT,
	KeyRelationT,
};
class Token
{
public:
	TokenType Type;
	std::string content;
	//Prints data to file
	virtual void print(std::fstream* printer) = 0;
	//Prints data to console
	virtual void userdisplay();
};
class EndLine : public Token
{
public:
	void userdisplay();
	void print(std::fstream* printer);
	EndLine(std::string cont);
};
class KeyWord : public Token
{
public:
	std::vector<Token*> Description;
	KeyWord(std::string cont);
	void print(std::fstream* printer);
	void userdisplay();
	void clearDescription();
	~KeyWord();
};
class KeyRelation : public Token
{
	//Potential bug in having :'s in key names and not in relations.
public:
	std::vector<Token*> Relations;
	//Adds a KeyWord to the relatiion vector.
	void addRelation(KeyWord* relator);
	bool viable();
	KeyRelation(std::string cont);
	void print(std::fstream* printer);
	void userdisplay();
	//Takes a KeyWord identifier and attempts to find and delete the key in Relations vector. Only safe if being used in a document made exclusively of Relations.
	void deleteRelation(std::string id);
	~KeyRelation();
};
class AccessoryText : public Token
{
public:
	void print(std::fstream* printer);
	AccessoryText(std::string cont);
};
class Space : public Token
{
public:
	void userdisplay();
	void print(std::fstream* printer);
	Space(std::string cont);
};
//Tokenizes a string and injects tokens into a referenced token vector.
void injectTokens(std::string origline, std::vector<Token*>& injectee);
//Takes a reference string and continues to put user input into it until the terminator is called. 
void getUserInput(std::string& storer, std::string terminator);
class OpenF
{
	std::fstream* stream;
	std::string filepath;
	//Entire document formatted into tokens, document is read and written in this format
	std::vector<Token*> filedata;
	//Specific keywords, list kept due to ease of keyword addition.
	std::vector<KeyWord*> keyWords;
	std::vector<KeyRelation*> keyRelations;
	//This creates a token from the initial file read, cannot be called from outside OpenF class.
	Token* createReadToken(int flag, std::string data);
public:
	std::string getFilePath();
	KeyWord* findkey(std::string id);
	void deletekey(std::string id);
	KeyWord* createKeyWord(std::string data);
	KeyRelation* createKeyRelation(std::string data);
	//Token creation called primarily by the user. Due to primary user use, it contains communication through cout to the user.
	Token* createToken(int flag, std::string data);
	OpenF(std::string filepathC);
	//massive problem here
	void readFormattedFile();
	void editterm(std::string term);
	void writekeyword(KeyWord* tok);
	void writerelation(KeyRelation* relation);
	void performFullWrite();
	void printToUser();
	void clearTokens();
	void save();
	void clearKeyWords();
	void clearRelations();
	~OpenF();
};