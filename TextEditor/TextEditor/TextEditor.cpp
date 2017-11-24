// ConsoleApplication2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string>
#include<vector>
#include<fstream>
#include<iostream>
#include"DefinitionEditor.h"
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
					addinput.insert(addinput.begin(), '[');
					addinput.insert(addinput.end(), ']');
					addinput.insert(addinput.end(), '\n');
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

