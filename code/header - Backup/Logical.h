#pragma once
#include <string>
#include <iostream>

//<string> <iostream>


class Logical
{
private:
	std::string name = "Nameless Logical Entity";
	float birth = -1.0f; //time after program start, that object was instantiated	(-1 indicates unspecified)

	//Severity values:
	//  0: notes / info
	// >0: warnings, greater value -> more severe (max 10)
	// <0: errors, lesser value -> more severe (min -10)
	/*void log(std::string message, int severity)
	{
		long time = 0; //Get time somehow

		std::string type;
		if (severity == 0)
			type = " INFO";
		else if (severity > 0)
			type = "WARNING";
		else
		{
			type = "ERROR"; 
			severity *= -1;
		}
		std::cout << type << " [" << severity << "] from " << name << "(time = " << time << "):\t" << message << std::endl;
	}*/

public:
	//constructors
	Logical(){}		//one use this for debugging purposes, not sure what that would be though
	Logical(std::string name, float timestamp)	//use this one
	{
		this->name = name;
		this->birth = timestamp;
	}

	//Call update each frame
	// Arguments:
	//  dTime - time in milliseconds(?) since last frame
	// Returns:
	//  0 if update succeeded, nothing to report
	//  > 0 if update encountered a minor issue, worth investigation (should also log warning)
	//  < 0 if update encountered a major issue, worth stopping program (should also log error)
	virtual int update(float dTime) = 0; // = 0 means pure virtual, no definition required (makes class virtual, cannot be instantiated)
};