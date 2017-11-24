// ConsoleApplication2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string>
#include<vector>
#include<fstream>
#include<iostream>
enum TokenType
{
	EndLineT,
	KeyWordT,
	AccessoryTextT,
};
class Token
{
public:
	TokenType Type;
	std::string content;
	//Prints data to file
	virtual void print(std::fstream* printer) = 0;
	//Prints data to console
	virtual void userdisplay()
	{
		std::cout << content;
	}
};
class EndLine : public Token
{
public:
	void userdisplay()
	{
		std::cout << "\n";
	}
	void print(std::fstream* printer)
	{
		*printer << '\n';
	}
	EndLine(std::string cont)
	{
		content = cont;
		Type = TokenType::EndLineT;
	}
};
class KeyWord : public Token
{
public:
	std::vector<Token*> Description;
	KeyWord(std::string cont)
	{
		Type = TokenType::KeyWordT;
		content = cont;
	}
	void print(std::fstream* printer)
	{
		*printer << ("<" + content + ">");
		*printer << "[";
		for (auto i : Description)
		{
			i->print(printer);
		}
		*printer << "]";
		//Creates a new line for the next statement.
	}
	void userdisplay()
	{
		std::cout << ("<" + content + ">");
		std::cout << "[";
		for (auto i : Description)
		{
			i->userdisplay();
		}
		std::cout << "]";
	}
	void clearDescription()
	{
		for (auto i : Description)
		{
			delete i;
		}
		Description.clear();
	}
	~KeyWord()
	{
		for (auto i : Description)
		{
			delete i;
		}
	}
};
class AccessoryText : public Token
{
public:
	void print(std::fstream* printer)
	{
		*printer << content;
	}
	AccessoryText(std::string cont)
	{
		Type = TokenType::AccessoryTextT;
		content = cont;
	}
};
class Space : public Token
{
public:
	void userdisplay()
	{
		std::cout << " ";
	}
	void print(std::fstream* printer)
	{
		*printer << " ";
	}
	Space(std::string cont)
	{
		//space content may be used to describe program-specific intermediate states, will not
		//be printed
		content = cont;
	}
};
//Injects tokens created from a string into a vector of tokens
void injectTokens(std::string origline, std::vector<Token*>& injectee)
{
	std::string tempstring;
	for (int i = 0; i < origline.size(); i++)
	{
		switch (origline[i])
		{
			//May have to revisit due to last word not being added to description.
		case ' ':
			injectee.push_back(new AccessoryText(tempstring));
			injectee.push_back(new Space(":|SPACE|:"));
			tempstring.clear();
			break;
		case '\n':
			injectee.push_back(new AccessoryText(tempstring));
			injectee.push_back(new EndLine(tempstring));
			tempstring.clear();
			break;
		default:
			tempstring.push_back(origline[i]);
		}
	}
}
//Takes a reference string and continues to put user input into it until the terminator is called. 
void getUserInput(std::string& storer, std::string terminator)
{
	std::string temp;
	//Keeps taking user input and storing it in addinput until ~terminate is called. 
	while (std::cin >> temp)
	{
		if (temp == terminator)
		{
			break;
		}
		else
		{
			for (auto i : temp)
			{
				storer.push_back(i);
			}
			storer.push_back(' ');
			temp.clear();
		}
	}
}
class OpenF
{
	std::fstream* stream;
	std::string filepath;
	//Entire document formatted into tokens, document is read and written in this format
	std::vector<Token*> filedata;
	//Specific keywords, list kept due to ease of keyword addition.
	std::vector<KeyWord*> keyWords;
	//This creates a token from the initial file read, cannot be called from outside OpenF class.
	Token* createReadToken(int flag, std::string data)
	{
		switch (flag)
		{
		case -2:
			return new Space(data);
			break;
		case -1:
			return new EndLine(data);
			break;
		case 0:
			return new AccessoryText(data);
			break;
		case 1:
			KeyWord* newkey = new KeyWord(data);
			keyWords.push_back(newkey);
			return newkey;
			break;
		}
	}
public:
	KeyWord* createKeyWord(std::string data)
	{
		std::cout << "Keyword In Creation!" << std::endl;
		bool cont = true;
		KeyWord* newkey = new KeyWord(data);
		for (auto i : keyWords)
		{
			if (i->content == data)
			{
				cont = false;
			}
		}
		if (cont)
		{
			std::cout << "KeyWord successfully created!" << std::endl;
			keyWords.push_back(newkey);
			return newkey;
		}
		else
		{
			std::cout << "Error, couldn't create KeyWord. KeyWord already created with same name." << std::endl;
			return nullptr;
		}
	}
	//Token creation called primarily by the user. Due to primary user use, it contains communication through cout to the user.
	Token* createToken(int flag, std::string data)
	{
		switch (flag)
		{
		case -2:
			return new Space(data);
			break;
		case -1:
			return new EndLine(data);
			break;
		case 0:
			return new AccessoryText(data);
			break;
		case 1:
			return createKeyWord(data);
			break;
		}
	}
	OpenF(std::string filepathC)
	{
		filepath = filepathC;
		stream = new std::fstream(filepath, std::fstream::out | std::fstream::in | std::fstream::app);
		if (!stream->is_open())
		{
			//If file doesn't open like this, it may not have been created. The 
			//Following statement attempts to add a trunc parameter(creating a new doc).
			//If this doesn't work, an error is reported.
			stream->open(filepath, std::fstream::in | std::fstream::out | std::fstream::trunc);
			if (!stream->is_open())
			{
				std::cout << "Error opening file for I/O, restart program and try again." << std::endl;
			}
			else
			{
				std::cout << "File opened successfully!" << std::endl;
			}
		}
		else
		{
			std::cout << "File opened successfully!" << std::endl;
		}
		stream->close();
		readFormattedFile();
	}
	//massive problem here
	void readFormattedFile()
	{
		stream->open(filepath, std::fstream::in | std::fstream::app);
		char currChar;
		//Type of Token, defaults to AccessoryText
		int TokenFlag = 0;
		//Raw text of a token
		std::string currTokenContent;
		//Implemented as the current place of data acquisition will switch from a keyword 
		//Acquiring tokens to the main list acquiring tokens. 
		std::vector<Token*>* currWritingPlace = &filedata;
		*stream >> std::noskipws;
		while (*stream >> currChar)
		{
			//<Key1>[DESCRIPKEY1]<Key2>[DESCRIPKEY2]<Key3>[VERYACCURATE]
			switch (currChar)
			{
				//Beginning of Token, sets TokenFlag to associated prevalue. 
			case '<':
				TokenFlag = 1;
				break;
			case '>':
				filedata.push_back(createReadToken(TokenFlag, currTokenContent));
				currTokenContent.clear();
				TokenFlag = 0;
			case '[':
				//Sets the writing location to the last known token's description vector.
				currWritingPlace = &keyWords[keyWords.size() - 1]->Description;
				break;
			case ']':
				currWritingPlace->push_back(createReadToken(TokenFlag, currTokenContent));
				currTokenContent.clear();
				//Resets the writing location to the normal place.
				currWritingPlace = &filedata;
				break;
			case ' ':
				std::cout << "DETECTEDSPACE";
				currWritingPlace->push_back(createReadToken(TokenFlag, currTokenContent));
				currWritingPlace->push_back(createReadToken(-2, ":|SPACE|:"));
				currTokenContent.clear();
				TokenFlag = 0;
				break;
			case '\n':
				currWritingPlace->push_back(createReadToken(TokenFlag, currTokenContent));
				filedata.push_back(createReadToken(-1, ":|ENDLINE|:"));
				currTokenContent.clear();
				TokenFlag = 0;
				break;
			default:
				currTokenContent.push_back(currChar);
			}
		}
		stream->close();
	}
	//Returns a key found in the key vector with an id identical to the id given. If no key is found, the function will return null. 
	KeyWord* findkey(std::string id)
	{
		for (auto i : keyWords)
		{
			if (i->content == id)
				return i;
		}
		return nullptr;
	}
	void deletekey(std::string id)
	{
		std::cout << "Starting Deletion..." << std::endl;
		KeyWord* temptok = findkey(id);
		for (int i = 0; i < keyWords.size(); i++)
		{
			if (keyWords[i] == temptok)
			{
				delete keyWords[i];
				keyWords.erase(keyWords.begin() + i);
				break;
			}
		}
		for (int i = 0; i < filedata.size(); i++)
		{
			if (filedata[i] == temptok)
			{
				//Does not delete since it was already deleted above
				filedata.erase(filedata.begin() + i);
			}
		}
		std::cout << "Deleted!" << std::endl;
	}
	void editterm(std::string term)
	{
	}
	void writekeyword(KeyWord* tok)
	{
		keyWords.push_back(tok);
		filedata.push_back(tok);
	}
	void performFullWrite()
	{
		stream->open(filepath, std::fstream::out | std::fstream::trunc);
		for (auto i : filedata)
		{
			i->print(stream);
		}
		stream->close();
	}
	void printToUser()
	{
		for (auto i : filedata)
		{
			i->userdisplay();
		}
	}
	void clearTokens()
	{
		//foreach loop through every token in readdata, deletes
		for (auto i : filedata)
		{
			delete i;
		}
	}
	void save()
	{
		std::cout << "Saving..." << std::endl;
		performFullWrite();
		std::cout << "Save complete!" << std::endl;
	}
	~OpenF()
	{
		std::cout << "Saving and Closing File..." << std::endl;
		performFullWrite();
		clearTokens();
		delete stream;
		std::cout << "File Successfully Closed!" << std::endl;
	}
};
void UserIO()
{
	bool fileopen = false;
	bool cont = true;
	std::string addinput;
	OpenF* curropenfile = nullptr;
	std::string userin;
	while (std::cin >> userin)
	{
		//Two major paths, one can be taken if a file is open, the other if zero files are open. 
		if (!fileopen)
		{
			if (userin == "editfile" || userin == "openfile" || userin == "createfile")
			{
				std::cout << "Provide file path without quotations." << std::endl;
				std::cin >> addinput;
				curropenfile = new OpenF(addinput);
				fileopen = true;
			}
			else
			{
				std::cout << "Invalid command, please reenter." << std::endl;
			}
		}
		else if (fileopen)
		{
			if (userin == "createkeyword" || userin == "createkey" || userin == "newkey")
			{
				std::cout << "Enter Keyword Name" << std::endl;
				std::cin >> addinput;
				KeyWord* temptok = curropenfile->createKeyWord(addinput);
				if (temptok != nullptr)
				{
					curropenfile->writekeyword(temptok);
					addinput.clear();
					//Finishes creating term, then asks to create a description.
					std::cout << "Add a description(Leave blank if no description is needed). To terminate description, type ~terminate." << std::endl;
					std::cin.ignore();
					getUserInput(addinput, "~terminate");
					injectTokens(addinput, temptok->Description);
					std::cout << "Description complete!" << std::endl;
				}
			}
			else if (userin == "editkey")
			{
				std::cout << "Enter Key Name." << std::endl;
				std::cin >> addinput;
				KeyWord* temptok = curropenfile->findkey(addinput);
				if (temptok != nullptr)
				{
					addinput.clear();
					std::cout << "Enter new description. To terminate description, type ~terminate." << std::endl;
					getUserInput(addinput, "~terminate");
					temptok->clearDescription();
					injectTokens(addinput, temptok->Description);
					std::cout << "Description Editing Complete!" << std::endl;
				}
				else
				{
					std::cout << "Could not isolate a key with specified name. Revise name and retry." << std::endl;
				}
			}
			else if (userin == "deletekey")
			{
				std::cout << "Enter Key Name." << std::endl;
				std::cin >> addinput;
				curropenfile->deletekey(addinput);

			}
			else if (userin == "displayfile")
			{
				std::cout << "File Contents Begin." << std::endl;
				curropenfile->printToUser();
				std::cout << "File Contents End." << std::endl;
			}
			else if (userin == "save" || userin == "savefile")
			{
				curropenfile->save();
			}
			else if (userin == "closefile" || userin == "close")
			{
				delete curropenfile;
				curropenfile = nullptr;
				fileopen = false;
			}
			else
			{
				std::cout << "Invalid command, please reenter." << std::endl;
			}
		}
		userin.clear();
		addinput.clear();
	}
}
int main()
{
	UserIO();
	return 0;
}

