#pragma once

#include <algorithm>
#include <vector>
#include <string>

class ConsoleArgParser
{
public:
	ConsoleArgParser(int& argc, char** argv);

	const std::string& GetCmdOption(const std::string& option) const;

	bool CmdOptionExists(const std::string& option) const;

private:
	std::vector <std::string> tokens;
};