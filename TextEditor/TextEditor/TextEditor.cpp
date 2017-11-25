// ConsoleApplication2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string>
#include<vector>
#include<fstream>
#include<iostream>
#include"DefinitionEditor.h"
void CompareFileUserIO(OpenF* file1, OpenF* file2)
{
	std::cout << "Entering Relation Mode. To exit, type ~terminate" << std::endl;
	std::string userin;
	std::string addinput;
	bool cont = true;
	//Creates or opens a file which contains the relation between two language files
	OpenF* relatefile = new OpenF(file1->getFilePath() + "_RELATES_" + file2->getFilePath());
	while (std::cin >> userin && cont == true)
	{
		if (userin == "relatekeys")
		{
			std::cout << "Enter Key from File #1" << std::endl;
			std::cin >> addinput;
			KeyWord* key1 = file1->findkey(addinput);
			addinput.clear();
			std::cout << "Enter Key from File #2" << std::endl;
			std::cin >> addinput;
			KeyWord* key2 = file2->findkey(addinput);
			addinput.clear();

			//As long as both keys are found in the OpenFs, then operation can continue
			if (key1 != nullptr && key2 != nullptr)
			{
				KeyRelation* newrelate = relatefile->createKeyRelation("NONAME");
				newrelate->addRelation(key1);
				newrelate->addRelation(key2);
				relatefile->writerelation(newrelate);
				//Relation created text already said with createKeyRelation
			}
			else
			{
				std::cout << "Error, did not find one or both of the two specified keys. Please retry with existing key names." << std::endl;
			}
		}
		else if (userin == "save" || userin == "savefile")
		{
			relatefile->save();
		}
		else if (userin == "closefile" || userin == "close" || userin == "~terminate")
		{
			std::cout << "Exiting Relation Mode..." << std::endl;
			delete relatefile;
			relatefile = nullptr;
			cont = false;
			std::cout << "Exited Relation Mode!" << std::endl;
		}
	}
}
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
				curropenfile	 = new OpenF(addinput);
				fileopen = true;
			}
			else if (userin == "comparefiles" || userin == "relatefiles" || userin == "establishrelations")
			{
				OpenF* altfile = nullptr;
				std::cout << "Provide path of File #1." << std::endl;
				std::cin >> addinput;
				curropenfile = new OpenF(addinput);
				addinput.clear();
				std::cout << "Provide path of File #2" << std::endl;
				std::cin >> addinput;
				altfile = new OpenF(addinput);
				//Enters a new loop, when this loop completes it goes back into UserIO(). 
				CompareFileUserIO(curropenfile, altfile);
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
					//addinput.insert(addinput.begin(), '[');
					//addinput.insert(addinput.end(), ']');
					//addinput.insert(addinput.end(), '\n');
					//Removes extra whitespace at end.
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

