#include"stdafx.h"
#include "DefinitionEditor.h"
//Naming uses arbitrary names in certain categories for language parameters
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
	//Removes whitespace
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
	Type = TokenType::SpaceT;
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
KeyRelation* OpenF::createKeyRelation(std::string data)
{
	std::cout << "KeyRelation Created!" << std::endl;
	KeyRelation* keyre = new KeyRelation(data);
	keyRelations.push_back(keyre);
	return keyre;
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
	case 2:
		return createKeyRelation(data);
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
	std::vector<Token*>* lastWritingPlace = &filedata;
	std::vector<Token*>* currWritingPlace = &filedata;
	*stream >> std::noskipws;
	int currSection = 0;
	while (*stream >> currChar)
	{
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
			break;
		case '[':
			currTokenContent.push_back(currChar);
			//Sets the writing location to the last known token's description vector.
			currWritingPlace = &keyWords[keyWords.size() - 1]->Description;
			break;
		case ']':
			currTokenContent.push_back(currChar);
			currWritingPlace->push_back(createReadToken(TokenFlag, currTokenContent));
			currTokenContent.clear();
			//Resets the writing location to the normal place.
			switch (currSection)
			{
			case 0:
				currWritingPlace = &filedata;
				break;
			case 1:
				currWritingPlace = &keyRelations[keyRelations.size() - 1]->Relations;
				break;
			}
			break;
		case '{':
			//Directly creates key relation.
			filedata.push_back(createKeyRelation("NOTEXT"));
			//Changes writing place
			currWritingPlace = &keyRelations[keyRelations.size() - 1]->Relations;
			currSection = 1;
			break;
		case '}':
			TokenFlag = 0;
			//Resets Writing Place
			currWritingPlace = &filedata;
			currSection = 0;
		case ' ':
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
std::string OpenF::getFilePath()
{
	return filepath;
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
void OpenF::writerelation(KeyRelation* relation)
{
	filedata.push_back(relation);
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
void OpenF::clearKeyWords()
{
	for (auto i : keyWords)
	{
		delete i;
	}
	keyWords.clear();
}
void OpenF::clearRelations()
{
	for (auto i : keyRelations)
	{
		delete i;
	}
	keyRelations.clear();
}
OpenF::~OpenF()
{
	std::cout << "Saving and Closing File..." << std::endl;
	performFullWrite();
	clearTokens();
	delete stream;
	std::cout << "File Successfully Closed!" << std::endl;
}
//Only used for Keywords
void injectTokens(std::string origline, std::vector<Token*>& injectee)
{
	std::string tempstring;
	injectee.push_back(new AccessoryText("["));
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
	injectee.pop_back();
	injectee.push_back(new AccessoryText("]"));
	injectee.push_back(new EndLine(tempstring));
	//Removes final whitespace

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
void KeyRelation::addRelation(KeyWord* relator)
{
	bool cont = true;
	for (auto i : Relations)
	{
		if (i == relator)
		{
			cont = false;
		}
	}
	if (cont)
	{
		Relations.push_back(relator);
	}
	else
	{
		std::cout << "Error, couldn't add value to KeyRelation due to the presence of an identical value in the vector." << std::endl;
	}
}

bool KeyRelation::viable()
{
	return Relations.size() > 1;
}

KeyRelation::KeyRelation(std::string cont)
{
	Type = TokenType::KeyRelationT;
	content = cont;
}

void KeyRelation::print(std::fstream* printer)
{
	//Curly brackets indicate start and end of relations.
	*printer << "{";
	for (auto i : Relations)
	{
		//prints keyword with description, then clarifies relation with :
		i->print(printer);
	}
	*printer << "}";
	*printer << std::endl;
}
void KeyRelation::userdisplay()
{
	//Curly brackets indicate start and end of relations.
	std::cout << "{";
	for (auto i : Relations)
	{
		//prints keyword with description, then clarifies relation with :
		i->userdisplay();
	}
	std::cout << "}";
	std::cout << std::endl;
}
void KeyRelation::deleteRelation(std::string id)
{
	Token* delrelate = nullptr;
	int loc = -1;
	for (int i = 0; i < Relations.size(); i++)
	{
		if (Relations[i]->content == id)
		{
			loc = i;
			delrelate = Relations[i];
		}
	}
	if (delrelate != nullptr)
	{
		Relations.erase(Relations.begin() + loc);
	}
	else
	{
		std::cout << "Could not delete element, was not found in storage" << std::endl;
	}
}
KeyRelation::~KeyRelation()
{
	Relations.clear();
}
