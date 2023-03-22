#include "Entity.h"
#include <iostream>

//this class is intetionally pointless.
//that said, it might not be needed at all

int Entity::update(float dTime)
{
	//log.warning() - Make a logging system!
	std::cout << "Warning level 5: " << name << " does called the update function from Entity. Please override this function." << std::endl;
	return 5;	//return 5: mid-level warning
}

int Entity::render()
{
	//log.warning() - Make a logging system!
	std::cout << "Warning level 5: " << name << " called the render function from the Entity class. Please override this function." << std::endl;
	return 5;	//return 5: mid-level warning
}

int Entity::info(int detail)
{
	//log.warning() - Make a logging system!
	std::cout << "Warning level 5: " << name << " called the debug function from the Entity class. Please override this function." << std::endl;
	return 5;	//return 5: mid-level warning
}