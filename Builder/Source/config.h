#pragma once
#include <vector>
#include <string>

namespace params
{
	static std::string projectName{};
	static std::string mainTool{};
	static std::string rootPath{};
	static std::string binPath{};
	static std::string inclusions{};
	static std::string compileFlag{};
	static std::vector<std::string> macros{};
	static std::string dop{};
}

namespace mnemonics
{
	static const char comment = '#';
	static const char newLine = '\n';
	static const std::string projectName = "projectName";
	static const std::string mainTool = "mainTool";
	static const std::string rootPath = "rootPath";
	static const std::string binPath = "binPath";
	static const std::string include = "include";

	namespace command
	{
		static const std::string set = "set";
		static const std::string macros = "macros";
		static const std::string createDir = "createDir";
		static const std::string dop = "dop";
	}
}

namespace commandType
{
	static const std::string inCommand = "icom";
	static const std::string outCommand = "ocom";
}
