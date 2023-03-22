#include "Transform.h"

//consider removing for release
#include <Debug.h>

void Transform::updateMatrix()
{
		//---debug---
		//std::cout << "DEBUG (Transform::updateMatrix): updating matrix..." << std::endl;
	//INITIALIZE
	matrix = glm::identity<glm::mat4x4>();	//identity matrix


	//I've put translate first, becaue apparently new = transform(matrix, vector) implies new = matrix * transMatrix(vector)
	// so when chaining transformations, we want: translate * scale * rotation -> rotation(scale(translate(identity))) = identity * translate * scale * rotation
	
	//TRANSLATE
	matrix = glm::translate(matrix, position);

	//SCALE
	matrix = glm::scale(matrix, scale);

	//ROTATION
	//calculate rotation axis (right-hand-rule) and magnitude (radians)
	float angle = glm::length(rotation);
	if (angle != 0)
	{
		glm::vec3 axis = glm::normalize(rotation);
		matrix = glm::rotate(matrix, angle, axis);
	}

	//READY
	matrixReady = true;
}

//static
void Transform::debug_unitTest()
{
	/*
		//EXPERIMENT: This code prints: 
		// a = (1, 1, 2)
		// b = (3, 5, 2.5)

		// a = (3, 5, 2.5)
		// b = (-7, -5, -7.5)
		// showing that the = operator copies values member-wise when applied to two glm::vec3 objects
	glm::vec3 a = glm::vec3(1.0f, 1.0f, 2.0f);
	glm::vec3 b = glm::vec3(3.0f, 5.0f, 2.5f);
	std::cout + "a = (" + a.x + ", " + a.y + ", " + a.z + ")" + std::endl;
	std::cout + "b = (" + b.x + ", " + b.y + ", " + b.z + ")" + std::endl + std::endl;
	a = b;
	b += glm::vec3(-10.0f);
	std::cout + "a = ("+ a.x + ", " + a.y + ", " + a.z + ")" + std::endl;
	std::cout + "b = (" + b.x + ", " + b.y + ", " + b.z + ")" + std::endl + std::endl;
	*/

	glm::vec4 vertices[] = { glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec4(1.0f, 1.0f, -1.0f, 1.0f), glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f), glm::vec4(-1.0f, -1.0f, 1.0f, 1.0f), };
	for (int i = 0; i < 4; i++)
		LOG("vertex #" + std::to_string(i) + ": (" + std::to_string(vertices[i].x) + ", " + std::to_string(vertices[i].y) + ", " + std::to_string(vertices[i].z) + ", " + std::to_string(vertices[i].w) + ")");
	LOG("\n");

	Transform* t = new Transform();
	t->modPosition(glm::vec3(1.0f, 1.0f, 1.0f)); //move along all axes
	t->multScale(glm::vec3(0.5f, 0.5f, 0.5f)); //halve scale
	t->modRotation(glm::vec3(0.0f, 0.0f, glm::pi<float>() * 0.25)); //rotate 45 degrees
	//this is not recommended use, just to check that points are transformed correctly
	for (int i = 0; i < 4; i++)
	{
		vertices[i] = *t->getMatrix() * vertices[i];
		 + "vertex #" + std::to_string(i) + ": (" + std::to_string(vertices[i].x) + ", " + std::to_string(vertices[i].y) + ", " + std::to_string(vertices[i].z) + ", " + std::to_string(vertices[i].w) + ")");
	}
	delete t;
}