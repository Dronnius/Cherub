#pragma once

//If you ever make a cherub2, consider writing your own math library, as recommended by the Cherno
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include <Debug.h>
#include <cmath>

// "glm/glm.hpp" "glm/gtc/matrix_transform.hpp" "glm/gtc/type_ptr.hpp"


//for futute reference, quaternion constructor quat(vec1, vec2) constructs a quaternion that rotates vec1 to vec2, probably useful for lookAt methods


//TODO: (found these at the bottom of the file)
//	fix rotation in updateMatrix
//	unit testing (rotation missing)


class Transform
{
private:
	//Vector3 position
	glm::vec3 position;
	//Vector3 rotation, direction shows axis, magintude shows angle in radians
	//glm::vec3 rotation;
	glm::quat rotation;
	//float angle = 0.0f;	//this is needed... I think
	//Rotation matrix, saves previous rotation, modifying rotation updates this value
	//glm::mat4 rotation NO I DON'T THINK WE NEED THIS
	//Vector3 scale
	glm::vec3 scale;

	//Matrices
	glm::mat4x4 transformMatrix;	//Transformation matrix, usually the product of the three matrices below
	glm::mat4x4 translateMatrix;	//Translate matrix
	glm::mat4x4 rotationMatrix;		//Rotation matrix
	glm::mat4x4 scaleMatrix;		//Scale matrix

	bool matrixReady = false;		//Is the transform matrix updated and ready for use? (used in getTransform) (always false if any of below are false)
	bool translateReady = false;	//is the translation matrix ready for use?
	bool rotateReady = false;		//is the rotation matrix ready for use?
	bool scaleReady = false;		//is the scaling matrix ready for use?

	//bool worldMatrixReady;		//cannot be used, since we would need a notification when the parent transform is updated
	//Transform of parent object, used for worldLocations, nullptr implies it has no parent
	Transform* parentTransform = nullptr;
	glm::mat4x4 worldMatrix = glm::identity<glm::mat4x4>();		//variable to store return values of worldMatrix requests, recalculates to some degree on each call (inefficient?)



	//Updates the transformation matrix, based on position, rotation, and scale vectors, so it is ready for usage
	void updateMatrix();
	void updateTranslateMatrix();
	void updateRotateMatrix();
	void updateScaleMatrix();

public:
	//Constructor
	//Transform()
	//{
	//	position = glm::vec3(0.0f);
	//	rotation = glm::vec3(0.0f);
	//	scale = glm::vec3(1.0f);
	//	matrix = glm::identity<glm::mat4x4>();
	//}
	Transform(Transform* parentTransform) : position(glm::vec3(0.0f)), rotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f)), scale(glm::vec3(1.0f)), parentTransform(parentTransform)
	{
		updateMatrix();
	}

	Transform(glm::vec3 position = glm::vec3(0.0f), glm::vec3 eulerRotation = glm::vec3(0.0f), glm::vec3 scale = glm::vec3(1.0f), Transform* parentTransform = nullptr) :
		position(position), rotation(eulerToQuat(eulerRotation)), scale(scale), parentTransform(parentTransform)
	{
		updateMatrix(); //sets matrixReady to true
	}

	//useful function, not sure how to implement yet (TODO)
	//pointAt(glm::vec3* target);

	//no-rotation quaternion
	static glm::quat quatInit()
	{
		return glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	}

	//From euler to quaternion
	static glm::quat eulerToQuat(glm::vec3 eulerAngles)
	{
		glm::quat q = glm::rotate(glm::quat(1.0f, 0.0f, 0.0f, 0.0f), eulerAngles.x, glm::vec3(1.0f, 0.0f, 0.0f));
		q = glm::rotate(q, eulerAngles.y, glm::vec3(0.0f, 1.0f, 0.0f));
		return glm::rotate(q, eulerAngles.z, glm::vec3(0.0f, 0.0f, 1.0f));
	}

	//Getters & Setters----------- (Consider moving definitions to .cpp file, this is getting cluttered)
	 
	//LOCAL DATA
	const glm::vec3 getPosition() { return position; }
	const glm::quat getRotation() { return rotation; }
	const glm::vec3 getScale() { return scale; }

	void setPosition(const glm::vec3 newPosition) { position = newPosition; translateReady = false; matrixReady = false; }
	//roll(nose spin)-pitch(nose up/down)-yaw(nose left/right)
	void setRotation(const glm::vec3 newEulerRotation) { rotation = eulerToQuat(newEulerRotation); rotateReady = false; matrixReady = false; }
	void setRotation(const glm::quat newQuatRotation) { rotation = newQuatRotation; rotateReady = false; matrixReady = false; }
	void setScale(const glm::vec3 newScale) { scale = newScale; scaleReady = false; matrixReady = false; }

	void modPosition(const glm::vec3 positionMod) { position += positionMod; translateReady = false; matrixReady = false; }
		//NOTE THAT THIS IS A LOCAL TRANSFORMATION, the axes of rotation are dependent on previous rotations!
	void modRotation(const glm::vec3 rotationAxis, float radians) { rotation = glm::rotate(rotation, radians, rotationAxis); rotateReady = false; matrixReady = false; }
	void modRotation(const glm::quat quatRotation) { rotation = rotation * quatRotation; rotateReady = false; matrixReady = false; }	//mult-order uncertain TOTEST!
	void modRotationExtrinsic(const glm::vec3 rotationAxis, float radians) { rotation = (glm::rotate(glm::quat(1.0f, 0.0f, 0.0f, 0.0f), radians, rotationAxis) * rotation); rotateReady = false; matrixReady = false;}
	void modRotationExtrinsic(const glm::quat quatRotation) { rotation = quatRotation * rotation; rotateReady = false; matrixReady = false; }
	void modScale(const glm::vec3 scaleMod) { scale *= scaleMod; scaleReady = false; matrixReady = false; }		//multiplicative scale modification
	void modScaleAdditive(const glm::vec3 scaleMod) { scale += scaleMod; scaleReady = false; matrixReady = false; }		//additive scale modification

	const glm::mat4x4* getMatrix() { if (!matrixReady) updateMatrix(); return &transformMatrix; }
	const float* getMatrixArray() { if (!matrixReady) updateMatrix(); return glm::value_ptr(transformMatrix); }

	//WORLD DATA
	const glm::vec3 getWorldPosition() 
	{ 
		////update relevant matrices
		//this->updateRotateMatrix();
		//this->updateScaleMatrix();
		////prepare local worldPosition
		//glm::vec3 myWorldPos
		if (parentTransform == nullptr) return position; 
		else return  glm::inverse(*parentTransform->getWorldMatrix()) * glm::vec4(this->position, 1.0f);		//New method, I think it's correct
		//else return  this->parentTransform->getWorldPosition() + this->position;		//Old method, I think it's incorrect
	}
	const glm::quat getWorldRotation() { if (parentTransform == nullptr) return rotation; else return parentTransform->getWorldRotation() * this->rotation; }	//Wrong order? I switched earlier
	const glm::vec3 getWorldScale() { if (parentTransform == nullptr) return scale; else return this->scale * parentTransform->getWorldScale(); }

	
	void setWorldPosition(const glm::vec3 newWorldPosition)
	{
		if (parentTransform == nullptr) setPosition(newWorldPosition);
		///else setPosition(newWorldPosition - parentTransform->getWorldPosition());		//didn't work
		else setPosition(glm::affineInverse(*(parentTransform->getWorldMatrix())) * glm::vec4(newWorldPosition, 1.0f));			//seems to work, test more when suitable
		//this->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
		//setPosition(glm::affineInverse(*(this->getWorldMatrix())) * glm::vec4(newWorldPosition, 1.0f));
	}
	//these to world rotation things are a little uncertain to be honest, TOTEST
	void setWorldRotation(const glm::vec3 newWorldEulerRotation)
	{
		if (parentTransform == nullptr) 
			setRotation(newWorldEulerRotation);
		else 
			setRotation(glm::inverse(parentTransform->getWorldRotation()) * eulerToQuat(newWorldEulerRotation));
	}
	void setWorldRotation(const glm::quat newWorldQuatRotation)
	{
		if (parentTransform == nullptr) 
			setRotation(newWorldQuatRotation);
		else 
			setRotation(glm::inverse(parentTransform->getWorldRotation()) * newWorldQuatRotation);
	}
	void setWorldScale(const glm::vec3 newWorldScale)
	{
		if (this->parentTransform == nullptr) 
			setScale(newWorldScale); 
		else
		{
			glm::vec3 parentWorldScale = this->parentTransform->getWorldScale();
			if (std::abs(parentWorldScale.x) < std::numeric_limits<float>::epsilon()			 ///////////////
				|| std::abs(parentWorldScale.y) < std::numeric_limits<float>::epsilon()			// D E B U G //
				|| std::abs(parentWorldScale.z) < std::numeric_limits<float>::epsilon())	   ///////////////
				LOGERROR("Avoided division by zero, failed to set new world scale:\n" 
					"\tNumerator (newWorldScale) = [" + std::to_string(newWorldScale.x) + ", " + std::to_string(newWorldScale.y) + ", " + std::to_string(newWorldScale.z) + "]\n" 
					"\tDenominator (parentWorldScale) = [" + std::to_string(parentWorldScale.x) + ", " + std::to_string(parentWorldScale.y) + ", " + std::to_string(parentWorldScale.z) + "]");
			else
				setScale(newWorldScale / (this->parentTransform->getWorldScale()));			//Make changes
		}
	}
	//		//TOTEST
	//Move object in world-coordinates irresepective of parent rotation and scale
	void modWorldPosition(const glm::vec3 worldPositionMod)
	{
		if (parentTransform == nullptr)
			modPosition(worldPositionMod);
		else
			modPosition(glm::inverse(*parentTransform->getWorldMatrix()) * glm::vec4(worldPositionMod, 1.0f));//modPosition(parentTransform->getWorldScale() * glm::inverse(parentTransform->getWorldRotation()) * worldPositionMod);	//order of multiplication
	}
	
	//Rotate object along world axes, rather than local, rotated axes	(QUESTION, world-world axes or parents axes?) (doing world-world here)
	void modWorldRotation(const glm::vec3 rotationAxis, float radians) //seems to work! ? //TO TEST FURTHER
	{
		glm::quat worRot;
		if (this->parentTransform == nullptr)
		{
			LOG("worldRotation(by axis): No parent transform");
			worRot = this->quatInit();
		}
		else
		{
			LOG("worldRotation(by quaternion): Parent transform");
			worRot = this->parentTransform->getWorldRotation();
		}
		this->modRotationExtrinsic(glm::rotate(glm::inverse(worRot), radians, rotationAxis) * worRot);
	}
	void modWorldRotation(const glm::quat worldRotation)		//this also seems to work! :D //TEST a little more though
	{
		glm::quat worRot;
		if (this->parentTransform == nullptr)
		{
			LOG("worldRotation(by quaternion): No parent transform");
			worRot = this->quatInit();
		}
		else
		{
			LOG("worldRotation(by quaternion): Parent transform");
			worRot = this->parentTransform->getWorldRotation();
		}
		this->modRotationExtrinsic(glm::inverse(worRot) * worldRotation * worRot);
	}

	//TOTEST
	void modWorldScale(const glm::vec3 scaleMult) { LOGWARNING("modWorldScale is a redundant function, use modScale instead!"); scale *= scaleMult; scaleReady = false; matrixReady = false; }		//should be pointless, same as modScale (OR IS IT? CONSIDER ROTATIONS!!!)
	//newWorldscale = worldScale + mod = parentScale * (oldLocalScale + answer) => answer = (worldScale + mod)/parentScale - oldLocalScale
	//newLocalScale = oldLocalScale + answer = oldLocalScale (worldScale + mod)/parentScale - oldLocalScale = (worldScale + mod)/parentScale = (parentScale * oldLocalScale + mod)/parentScale =
	//	= oldLocalScale + mod/parentScale
	void modWorldScaleAdditive(const glm::vec3 worldScaleMod)
	{
		if (this->parentTransform == nullptr)
		{
			modScale(worldScaleMod);
		}
		else
		{
			glm::vec3 parentScale = this->parentTransform->getWorldScale();
			//glm::quat parentRotNeutralizer = glm::inverse(this->parentTransform->getWorldRotation());	//ok so maybe this was a mistake
			modScale(worldScaleMod / parentScale);
		}
	}
	
	//Set new parent, update transform so that the world transform matrix is identical
	//TOTEST (Not functioning correctly)
	void setParent(Transform* newParent)//this is a simple but rough solution, should be fine as long as parent-switching is rare
	{
		//save old world transform
		glm::vec3 oldWorldPosition = this->getWorldPosition();
		glm::vec3 oldWorldScale = this->getWorldScale();
		glm::quat oldWorldRotation = this->getWorldRotation();
			//DEBUG
			LOG("\tPosition Saved as:\t[" + std::to_string(oldWorldPosition.x) + ", " + std::to_string(oldWorldPosition.y) + ", " + std::to_string(oldWorldPosition.z) + "]");
			LOG("\tScale Saved as:   \t[" + std::to_string(oldWorldScale.x) + ", " + std::to_string(oldWorldScale.y) + ", " + std::to_string(oldWorldScale.z) + "]");
			LOG("\tRotation Saved as:\t[" + std::to_string(oldWorldRotation.w) + ", " + std::to_string(oldWorldRotation.x) + ", " + std::to_string(oldWorldRotation.y) + ", " + std::to_string(oldWorldRotation.z) + "]");
		//update parent pointer
		this->parentTransform = newParent;
		//set new transform values
		this->setWorldPosition(oldWorldPosition);	//Possible that position muse be altered last, not sure.
		this->setWorldScale(oldWorldScale);
		this->setWorldRotation(oldWorldRotation);
			//DEBUG
			glm::vec3 newWorldPosition = this->getWorldPosition();
			glm::vec3 newWorldScale = this->getWorldScale();
			glm::quat newWorldRotation = this->getWorldRotation();
			LOG("\tPosition Changed to:\t[" + std::to_string(newWorldPosition.x) + ", " + std::to_string(newWorldPosition.y) + ", " + std::to_string(newWorldPosition.z) + "]");
			LOG("\tScale Changed to:   \t[" + std::to_string(newWorldScale.x) + ", " + std::to_string(newWorldScale.y) + ", " + std::to_string(newWorldScale.z) + "]");
			LOG("\tRotation Changed to:\t[" + std::to_string(newWorldRotation.w) + ", " + std::to_string(newWorldRotation.x) + ", " + std::to_string(newWorldRotation.y) + ", " + std::to_string(newWorldRotation.z) + "]");


	}

	//Retrieve world matrices in one of two formats,
	const glm::mat4x4* getWorldMatrix();
	const float* getWorldMatrixArray();


	//Change parentTransform and compute local transform so that the resulting world transform remains identical
	//void setParentTransform(Transform* newParent);	//TODO


	static void debug_unitTest();

	//IMPLEMENT TOSTRING METHOD, for convenience
	static std::string toString(glm::vec3 v)	//3D vector to string
	{
		return ("[x=" + std::to_string(v.x) + ", y=" + std::to_string(v.y) + ", z=" + std::to_string(v.z) + "]");
	}
	static std::string toString(glm::vec4 v)	//4D vector to string
	{
		return ("[x=" + std::to_string(v.x) + ", y=" + std::to_string(v.y) + ", z=" + std::to_string(v.z) + ", w=" + std::to_string(v.w) + "]");
	}
	static std::string toString(glm::quat q)	//quaternion to string
	{
		return ("[w=" + std::to_string(q.w) + ", x=" + std::to_string(q.x) + ", y=" + std::to_string(q.y) + ", z=" + std::to_string(q.z) + "]");
	}
	std::string toString()	//this Transform to string
	{
		return ("<Position:" + Transform::toString(this->getPosition()) + ", Rotation:" + Transform::toString(this->getRotation()) + ", Scale:" + Transform::toString(this->getScale()) + ">");
	}
	std::string worldToString()	//this Transform in world perspective to string
	{
		return ("<Position:" + Transform::toString(this->getWorldPosition()) + ", Rotation:" + Transform::toString(this->getWorldRotation()) + ", Scale:" + Transform::toString(this->getWorldScale()) + ">");
	}
};

//TODO:
//I was trying to implement and test the setParent() function, but I've discovered that some of the used sub-functions are dis-functional
//Try verifying the integrity of the various setWorld/getWorld functions by repeatedly setting random values and making sure that get returns the same/similar value (print avg diff squared?)
//This says nothing about modWorld functions, which may still be broken