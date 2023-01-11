#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <filesystem>
#include <direct.h>
#include "Parser.h"

#include <chrono>
#include <iomanip>

void printError(std::string massage)
{
	std::cout <<"Builder: error: " << massage << std::endl;
}

bool tryReadFile(std::string filePath, std::string& result)
{
	std::ifstream input(filePath);

	if (input.is_open())
	{
		result = { std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>() };
		input.close();
		return true;
	}

	return false;
}

void getFileDirs(std::string path, std::string extension, std::vector<DIRECTORY>& result)
{
	for (const auto& entry : std::filesystem::directory_iterator(path))
	{
		if (!std::filesystem::is_directory(entry))
			//if (entry.path().extension() == "")
				
				std::cout << entry.path().stem().string() << std::endl;
	}
}

time_t getWriteTime(const DIRECTORY& entry)
{
	std::filesystem::file_time_type fileTime = std::filesystem::last_write_time(entry.path().string());
	std::chrono::system_clock::time_point systemTime = std::chrono::clock_cast<std::chrono::system_clock>(fileTime);
	time_t time = std::chrono::system_clock::to_time_t(systemTime);

	return time;
}

int main00(int argc, char** argv)
{
	//(FATAL_EXIT_CODE);
	if (argc < 2)
	{
		std::cout << "specify the project directory" << std::endl;
		return 1;
	}
	std::string rootPath = "D:\\GitHub\\Builder\\CppTest";
	std::string objPath = "D:\\GitHub\\Builder\\CppTest\\src\\x64\\Debug";

	rootPath = argv[1];
	//objPath = argv[2];
	objPath = rootPath + "\\bin";

	if (!std::filesystem::exists(rootPath))
	{
		std::cout << "the directory does not exist: " << rootPath << std::endl;
		return 1;
	}

	if (!std::filesystem::exists(objPath))
	{
		std::filesystem::create_directory(objPath);
	}

	std::vector<DIRECTORY> cppFiles{};
	std::vector<DIRECTORY> objFiles{};
	std::vector<DIRECTORY> metaFiles{};
	std::vector<DIRECTORY*> incorrectCppFiles{};

	for (const DIRECTORY& entry : std::filesystem::recursive_directory_iterator(rootPath))
		if (entry.path().extension() == ".cpp" || entry.path().extension() == ".c")
			cppFiles.push_back(entry);
	
	for (const auto& entry : std::filesystem::directory_iterator(objPath))
	{
		if (!std::filesystem::is_directory(entry))
		{
			if (entry.path().extension() == ".o" || entry.path().extension() == ".obj")
				objFiles.push_back(entry);
			else if (entry.path().extension() == ".meta")
				metaFiles.push_back(entry);
		}
	}

	bool recom{};
	//delete extra .o
	for (int i = 0; i < objFiles.size(); i++)
	{
		bool isExist{};
		for (int j = 0; j < cppFiles.size(); j++)
		{
			if (objFiles[i].path().stem().string() == cppFiles[j].path().stem().string())
			{
				isExist = true;
				break;
			}
		}
		if (!isExist)
		{
			std::filesystem::remove(objFiles[i]);
			objFiles.erase(objFiles.begin() + i);
			recom = true;
		}
	}
	//delete extra .meta
	for (int i = 0; i < metaFiles.size(); i++)
	{
		bool isExist{};
		for (int j = 0; j < cppFiles.size(); j++)
		{
			if (metaFiles[i].path().stem().string() == cppFiles[j].path().stem().string())
			{
				isExist = true;
				break;
			}
		}
		if (!isExist)
		{
			std::filesystem::remove(metaFiles[i]);
			metaFiles.erase(metaFiles.begin() + i);
			recom = true;
		}
	}

	for (int i = 0; i < cppFiles.size(); i++)
	{
		bool isCorrectFile{};

		//check if .o exists
		for (int j = 0; j < objFiles.size(); j++)
		{
			if (cppFiles[i].path().stem().string() == objFiles[j].path().stem().string())
			{
				isCorrectFile = true;
				break;
			}
		}

		//check cpp write time
		if (isCorrectFile)
		{
			isCorrectFile = false;
			for (int j = 0; j < metaFiles.size(); j++)
			{
				if (cppFiles[i].path().stem().string() == metaFiles[j].path().stem().string())
				{
					time_t cppTime = getWriteTime(cppFiles[i]);
					std::ifstream input{ metaFiles[j].path().string(), std::ios::binary };
					if (input.is_open())
					{
						time_t oldTime{};
						input.read(reinterpret_cast<char*>(&oldTime), sizeof(oldTime));
						if (oldTime == cppTime) isCorrectFile = true;
						//else std::cout << "add by time" << std::endl;
							
						input.close();
					}
					break;
				}
			}
		}

		//add the current cpp for compilation
		if (!isCorrectFile)
			incorrectCppFiles.push_back(&cppFiles[i]);
	}

	std::cout << "compilation queue: " << incorrectCppFiles.size() << std::endl;

	std::string dd0 = "-I D:\\GitHub\\Deber\\Source -fdiagnostics-color=always ";

	for (const DIRECTORY* const& entry : incorrectCppFiles)
	{
		//g++
		std::string compCommand = std::string("g++ -c ") + dd0 + entry->path().string() + " -o " + objPath + "\\" +
			entry->path().stem().string() + ".o";

		if (system(compCommand.c_str()) == 0)
		{
			std::string curPath = objPath + "\\" + entry->path().stem().string() + ".meta";
			std::ofstream out{ curPath, std::ios::binary};
			if (out.is_open())
			{
				time_t time = getWriteTime(*entry);
				out.write(reinterpret_cast<const char*>(&time), sizeof(time));
				out.close();
			}
		}
		else std::cout << "compile error " << entry->path().string() << std::endl;
	}

	//compile all .o


	std::string dd = "-I D:\\GitHub\\Deber\\Source -fdiagnostics-color=always ";

	if(!std::filesystem::exists(objPath + "\\exe"))
		std::filesystem::create_directory(objPath + "\\exe");

	if (!std::filesystem::exists(objPath + "\\exe\\a.exe"))
		recom = true;

	if (incorrectCppFiles.size() > 0 || recom)
		return system(std::string("g++ " + dd + objPath + "\\*.o -o " + objPath + "\\exe\\a.exe").c_str());
	else return 0;
}

int main(int argc, char** argv)
{
	if (argc == 1)
	{
		printError("no input files");
		return 1;
	}

	std::string content{};
	if (!tryReadFile(argv[1], content))
	{
		printError("could not open the file");
		return 1;
	}

	Parser parser(content);
	parser.parse();

	return 0;
}