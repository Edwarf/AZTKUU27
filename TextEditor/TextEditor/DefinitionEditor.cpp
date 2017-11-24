#include"stdafx.h"
#include "DefinitionEditor.h"

void Token::userdisplay()
{
	std::cout << content;
}

void EndLine::userdisplay()
{
	std::cout << "\n";
}
void EndLine::print(std::fstream* printer)
{
	*printer << '\n';
}
EndLine::EndLine(std::string cont)
{
	content = cont;
	Type = TokenType::EndLineT;
}
KeyWord::KeyWord(std::string cont)
{
	Type = TokenType::KeyWordT;
	content = cont;
}
void KeyWord::print(std::fstream* printer)
{
	*printer << ("<" + content + ">");
	for (auto i : Description)
	{
		i->print(printer);
	}
	//Creates a new line for the next statement.
}
void KeyWord::userdisplay()
{
	std::cout << ("<" + content + ">");
	for (auto i : Description)
	{
		i->userdisplay();
	}
}
void KeyWord::clearDescription()
{
	for (auto i : Description)
	{
		delete i;
	}
	Description.clear();
}
KeyWord::~KeyWord()
{
	for (auto i : Description)
	{
		delete i;
	}
}
void AccessoryText::print(std::fstream* printer)
{
	*printer << content;
}
AccessoryText::AccessoryText(std::string cont)
{
	Type = TokenType::AccessoryTextT;
	content = cont;
}
void Space::userdisplay()
{
	std::cout << " ";
}
void Space::print(std::fstream* printer)
{
	*printer << " ";
}
Space::Space(std::string cont)
{
	//space content may be used to describe program-specific intermediate states, will not
	//be printed
	content = cont;
}
Token* OpenF::createReadToken(int flag, std::string data)
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
KeyWord* OpenF::createKeyWord(std::string data)
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
Token* OpenF::createToken(int flag, std::string data)
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
OpenF::OpenF(std::string filepathC)
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
void OpenF::readFormattedFile()
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
KeyWord* OpenF::findkey(std::string id)
{
	for (auto i : keyWords)
	{
		if (i->content == id)
			return i;
	}
	return nullptr;
}
void OpenF::deletekey(std::string id)
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
void OpenF::editterm(std::string term)
{
}
void OpenF::writekeyword(KeyWord* tok)
{
	keyWords.push_back(tok);
	filedata.push_back(tok);
}
void OpenF::performFullWrite()
{
	stream->open(filepath, std::fstream::out | std::fstream::trunc);
	for (auto i : filedata)
	{
		i->print(stream);
	}
	stream->close();
}
void OpenF::printToUser()
{
	for (auto i : filedata)
	{
		i->userdisplay();
	}
}
void OpenF::clearTokens()
{
	//foreach loop through every token in readdata, deletes
	for (auto i : filedata)
	{
		delete i;
	}
}
void OpenF::save()
{
	std::cout << "Saving..." << std::endl;
	performFullWrite();
	std::cout << "Save complete!" << std::endl;
}
OpenF::~OpenF()
{
	std::cout << "Saving and Closing File..." << std::endl;
	performFullWrite();
	clearTokens();
	delete stream;
	std::cout << "File Successfully Closed!" << std::endl;
}
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
	//Creates new line @	
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
