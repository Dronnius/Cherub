#pragma once
#include <vector>

//The function of a command should return false upon errors, otherwise true
struct Command
{
	std::string name;
	bool (*func)(std::vector<std::string>);		//parameters: vector arguments_as_string
	std::string desc;
	Command() { name = std::string(); func = nullptr; desc = std::string(); }
	Command(const char* name, bool (*func)(std::vector<std::string>), const char* description)
	{
		this->name = std::string(name);
		this->func = func;
		this->desc = std::string(description);
	}
};

//defined in Debug.cpp
void addToConsoleCommandList(Command c);