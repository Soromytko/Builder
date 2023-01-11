#include "Parser.h"

time_t getWriteTime0(const DIRECTORY& entry)
{
	std::filesystem::file_time_type fileTime = std::filesystem::last_write_time(entry.path().string());
	std::chrono::system_clock::time_point systemTime = std::chrono::clock_cast<std::chrono::system_clock>(fileTime);
	time_t time = std::chrono::system_clock::to_time_t(systemTime);

	return time;
}

int g_do()
{
	std::string rootPath{};// = "D:\\GitHub\\Builder\\CppTest";
	std::string binPath{};// = "D:\\GitHub\\Builder\\CppTest\\src\\x64\\Debug";

	rootPath = params::rootPath;
	binPath = rootPath + params::binPath;

	if (!std::filesystem::exists(rootPath))
		exitFailure("the directory does not exist: " + rootPath);

	if (!std::filesystem::exists(binPath))
		std::filesystem::create_directory(binPath);

	std::vector<DIRECTORY> cppFiles{};
	std::vector<DIRECTORY> objFiles{};
	std::vector<DIRECTORY> metaFiles{};
	std::vector<DIRECTORY*> incorrectCppFiles{};

	for (const DIRECTORY& entry : std::filesystem::recursive_directory_iterator(rootPath))
		if (entry.path().extension() == ".cpp" || entry.path().extension() == ".c")
			cppFiles.push_back(entry);

	for (const auto& entry : std::filesystem::directory_iterator(binPath))
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
					time_t cppTime = getWriteTime0(cppFiles[i]);
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

	//std::string dd0 = "-I D:\\GitHub\\Deber\\Source -fdiagnostics-color=always ";
	std::string dd0 = "-I " + params::inclusions + " -fdiagnostics-color=always ";
	if (params::inclusions == "") dd0 = "-fdiagnostics-color=always ";


	std::string macros{};
	for (int i = 0; i < params::macros.size(); i++)
		macros += "-D " + params::macros[i] + " ";

	for (const DIRECTORY* const& entry : incorrectCppFiles)
	{
		//g++
		/*std::string compCommand = std::string("g++ -c ") + dd0 + entry->path().string() + " -o " + binPath + "\\" +
			entry->path().stem().string() + ".o";*/
		std::string compCommand = std::string("g++ ") + macros + dd0 + " -c " + entry->path().string() + " -o " + binPath + "\\" +
			entry->path().stem().string() + ".o";

		if (system(compCommand.c_str()) == 0)
		{
			std::string curPath = binPath + "\\" + entry->path().stem().string() + ".meta";
			std::ofstream out{ curPath, std::ios::binary };
			if (out.is_open())
			{
				time_t time = getWriteTime0(*entry);
				out.write(reinterpret_cast<const char*>(&time), sizeof(time));
				out.close();
			}
		}
		else std::cout << "compile error " << entry->path().string() << std::endl;
	}

	//compile all .o


	//std::string dd = "-I D:\\GitHub\\Deber\\Source -fdiagnostics-color=always ";

	std::cout << "linking..." << std::endl;

	if (!std::filesystem::exists(binPath + "\\exe"))
		std::filesystem::create_directory(binPath + "\\exe");

	std::string name = "\\exe\\" + params::projectName + ".exe";

	if (!std::filesystem::exists(binPath + name))
		recom = true;


	std::string linkCommand = std::string("g++ " + binPath + "\\*.o -o " + binPath + name + " " + params::dop);

	std::cout << linkCommand << std::endl;

	if (incorrectCppFiles.size() > 0 || recom)
		//return system(std::string("g++ " + binPath + "\\*.o -o " + binPath + name).c_str());
		//return system(std::string(linkCommand + " -municode -mwindows").c_str());
		return system(std::string(linkCommand).c_str());
	else return 0;
}

Parser::Parser(std::string content)
{
	m_lineNum = 1;
	m_content = content;
}

void Parser::parse()
{
	//for (m_it = 0; m_it < m_content.size(); m_it++)
	FOR
	{
		m_skipComment();
		m_exeCommand();
	}
}

void Parser::m_inc(int value)
{
	m_it++;
	if (m_is('\n')) m_lineNum++;
}

bool Parser::m_is(char value)
{
	return m_content[m_it] == value;
}

bool Parser::m_isLetter()
{
	return m_content[m_it] >= 'A' && m_content[m_it] <= 'Z' || m_content[m_it] >= 'a' && m_content[m_it] <= 'z';
}

bool Parser::m_isExc(std::string& exc)
{
	for (int i = 0; i < exc.size(); i++)
		if (m_is(exc[i])) return true;
	return false;
}

bool Parser::tryGetWord(std::string& result, std::string exc)
{
	result.clear();
	//for (; m_it < m_content.size(); m_it++)
	FOR
	{
		if (m_isLetter() || m_isExc(exc)) result += m_content[m_it];
		else break;
	}
	return !result.empty();
}

void Parser::getArgs(std::vector<std::string>& result)
{
	result.clear();

	FOR
	{
		if (m_is('[')) break;
		else if (m_is(' ')) continue;
		else exitFailure("unexpected character, expected '[' instead '" + std::string() + m_content[m_it] + "' , line: " + std::to_string(m_lineNum));

	}
	m_inc();

	std::string arg{};
	FOR
	{
		if (tryGetWord(arg, ":$\\+-_0123456789"))
		{
			if (m_content[m_it - arg.size()] == '$')
			{
				std::string currentPath = std::filesystem::current_path().string();
				ext::string::replace(arg, "$", currentPath);
				//ext::string::replace(arg, "@", "");
			}
			result.push_back(arg);
		}
		switch (m_content[m_it])
		{
		case ']': return;
		case ',': case ' ': continue;
		default: exitFailure("unexpected character, expected ']' instead '" + std::string() + m_content[m_it] + "' , line: " + std::to_string(m_lineNum));
		}
	}
}

void Parser::m_skipComment()
{
	if (m_is(mnemonics::comment))
	{
		FOR if (m_is(mnemonics::newLine))
		{
			m_inc();
			m_skipComment();
			break;
		}
	}
}

void Parser::m_exeCommand()
{
	std::string commandType{};
	std::string command{};
	std::vector<std::string> args{};

	if (tryGetWord(commandType))
	{
		if (commandType == commandType::inCommand && m_content[m_it] == ':')
		{
			m_inc();
			if (tryGetWord(command))
			{
				getArgs(args);
				if (command == mnemonics::command::set)
				{
					if (args.size() != 2) exitFailure("expected arguments: (configName, value)");
					//if (args.size() < 2) exitFailure("expected arguments: (configName, value)");
					
					if (args[0] == mnemonics::projectName) params::projectName = args[1];
					else if (args[0] == mnemonics::mainTool) params::mainTool = args[1];
					else if (args[0] == mnemonics::rootPath) params::rootPath = args[1];
					else if (args[0] == mnemonics::binPath) params::binPath = args[1];
					else if (args[0] == mnemonics::include) params::inclusions = args[1];
					else exitFailure("unknown argument: " + args[0]);
				}
				else if (command == mnemonics::command::macros)
				{
					params::macros = args;
					/*params::macros = std::string().empty();
					for (int i = 0; i < args.size(); i++)
						params::macros += args[i];*/
				}
				else if (command == mnemonics::command::dop)
				{
					params::dop = std::string().empty();
					params::dop = "-" + args[0];
					for (int i = 1; i < args.size(); i++)
						params::dop += " -" + args[i];
				}
				else if (command == mnemonics::command::createDir)
				{
					if (args.size() != 1) exitFailure("expected arguments: (path)");
					//if (!std::filesystem::exists(params::rootPath))
					//	exitFailure("the directory does not exist: " + params::rootPath);
					std::filesystem::create_directories(args[0]);
				}
				else if (command == "do")
				{
					//std::cout << "do" << std::endl;
					exit(g_do());
				}
				else exitFailure("unknown commad: " + command);
			}
		}
		else exitFailure("unknown commad type: " + commandType);
	}
}


