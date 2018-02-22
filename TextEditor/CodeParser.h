#pragma once
#include"Translator.h"
class LiveToken;
class Parser
{
	
	std::vector<LiveToken> parsedFile;
public:
	void ParseCode(std::string configFilePath, std::string codeFilePath);
};