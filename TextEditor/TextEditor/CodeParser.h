#pragma once
#include"Translator.h"
class LiveToken;
class Parser : public OpenF
{
	std::vector<LiveToken> parsedFile;
public:
	void ParseCode(std::string configFilePath, std::string codeFilePath);
};