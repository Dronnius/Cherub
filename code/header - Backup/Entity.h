#pragma once
#include <string>

//Cherub class structure

//NOT IN USE
//Entity - Superclass, don't use
//> Logical - non-Renderable
// >
//> Visual - Renderable

//IN USE
// Logical
//  > update function
// Transform
//  > position
//  > rotation
//  > scale
// Shape : Transform
//  > collider shape
//  > intersect function
// Newton : Transform
//  > velocity
//  > acceleration
//  > mass
//  > force function
// Visual : Transform
//  > mesh
//  > textures


class Entity //This class might not be needed...
{
private:
	std::string name = "Nameless Entity";

public:
	//Call update each frame
	// Arguments:
	//  dTime - time in milliseconds(?) since last frame
	// Returns:
	//  0 if update succeeded, nothing to report
	//  > 0 if update encountered a minor issue, worth investigation (should also log warning)
	//  < 0 if update encountered a major issue, worth stopping program (should also log error)
	virtual int update(float dTime);

	//Call render each frame
	// Arguments:
	//  none
	// Returns:
	//  0 if render succeeded, nothing to report
	//  > 0 if render encountered a minor issue, worth investigation (should also log warning)
	//  < 0 if render encountered a major issue, worth stopping program (should also log error)
	virtual int render();

	//Call info each frame during debugging
	// Arguments:
	//  detail level - greater value -> more info
	// Returns:
	//  0 if info succeeded, nothing to report
	//  > 0 if info encountered a minor issue, worth investigation (should also log warning)
	//  < 0 if info encountered a major issue, worth stopping program (should also log error)
	virtual int info(int detail);
};