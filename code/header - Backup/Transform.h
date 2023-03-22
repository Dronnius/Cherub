#pragma once

//If you ever make a cherub2, consider writing your own math library, as recommended by the Cherno
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

// "glm/glm.hpp" "glm/gtc/matrix_transform.hpp" "glm/gtc/type_ptr.hpp"

class Transform
{
private:
	//Vector3 position
	glm::vec3 position;
	//Vector3 rotation, direction shows axis, magintude shows angle in radians
	glm::vec3 rotation;
	//Vector3 scale
	glm::vec3 scale;
	//Transformation matrix
	glm::mat4x4 matrix;
	//Is the transform matrix updated and ready for use? (used in getTransform)
	bool matrixReady = true;

public:
	//Constructor
	Transform()
	{
		position = glm::vec3(0.0f);
		rotation = glm::vec3(0.0f);
		scale = glm::vec3(1.0f);
		matrix = glm::identity<glm::mat4x4>();
	}

	Transform(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
	{
		this->position = position;
		this->rotation = rotation;
		this->scale = scale;
		updateMatrix(); //sets matrixReady to true
	}

	//useful function, not sure how to implement yet (TODO)
	//pointAt(glm::vec3* target);

	//Getters & Setters
	const glm::vec3* getPosition() { return &position; }
	const glm::vec3* getRotattion() { return &rotation; }
	const glm::vec3* getScale() { return &scale; }

	void setPosition(const glm::vec3 newPosition) { position = newPosition; matrixReady = false; }
	void setRotattion(const glm::vec3 newRotation) { rotation = newRotation; matrixReady = false; }
	void setScale(const glm::vec3 newScale) { scale = newScale; matrixReady = false; }

	void modPosition(const glm::vec3 positionMod) { position += positionMod; matrixReady = false; }
	void modRotation(const glm::vec3 rotationMod) { rotation += rotationMod; matrixReady = false; }
	void modScale(const glm::vec3 scaleMod) { scale += scaleMod; matrixReady = false; }
	void multScale(const glm::vec3 scaleMult) { scale *= scaleMult; matrixReady = false; }

	const glm::mat4x4* getMatrix() { if (!matrixReady) updateMatrix(); return &matrix; }
	const float* getMatrixArray() { if (!matrixReady) updateMatrix(); return glm::value_ptr(matrix); }

	static void debug_unitTest();

private:
	//Updates the transformation matrix, based on position, rotation, and scale vectors, so it is ready for usage
	void updateMatrix();
};

//TODO:
//	fix rotation in updateMatrix
//	unit testing (rotation missing)