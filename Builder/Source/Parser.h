#pragma once
#include <filesystem>
#include <fstream>
#include "error.h"
#include "extension.h"
#include "config.h"

#define DIRECTORY std::filesystem::directory_entry
#define FOR for (; m_it < m_content.size(); m_inc())

class Parser
{
public:
	Parser(std::string content);
	void parse();

private:
	void m_inc(int value = 1);
	bool m_is(char value);
	bool m_isLetter();
	bool m_isExc(std::string& exc);
	bool tryGetWord(std::string& result, std::string exc = "");
	void getArgs(std::vector<std::string>& result);
	void m_skipComment();
	void m_exeCommand();

	int m_it{};
	uint32_t m_lineNum{};
	std::string m_content{};
};

