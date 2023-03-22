#include "Transform.h"

//consider removing for release
#include <Debug.h>

void Transform::updateMatrix()
{
		//---debug---
		//std::cout << "DEBUG (Transform::updateMatrix): updating matrix..." << std::endl;
	
	//I've put translate first, becaue apparently new = transform(matrix, vector) implies new = matrix * transMatrix(vector)
	// so when chaining transformations, we want: translate * scale * rotation -> rotation(scale(translate(identity))) = identity * translate * scale * rotation

	//TRANSLATE
	if (!this->translateReady)
	{
		this->updateTranslateMatrix();
		//translateMatrix = glm::translate(glm::identity<glm::mat4x4>(), position);
		//translateReady = true;
	}

	//ROTATION
	if (!this->rotateReady)
	{
		this->updateRotateMatrix();
		//rotationMatrix = glm::mat4_cast(rotation);
		//rotateReady = true;
	}

	//SCALE
	if (!this->scaleReady)
	{
		this->updateScaleMatrix();
		//scaleMatrix = glm::scale(glm::identity<glm::mat4x4>(), scale);
		//scaleReady = true;
	}
////calculate rotation axis (right-hand-rule) and magnitude (radians)
//float angle = glm::length(rotation);
//if (angle != 0)
//{
//	glm::vec3 axis = glm::normalize(rotation);
//	matrix = glm::rotate(matrix, angle, axis);
//}
	//FINALIZE
	transformMatrix = translateMatrix * rotationMatrix * scaleMatrix;
	//READY UP
	matrixReady = true;
}

void Transform::updateTranslateMatrix()
{
	translateMatrix = glm::translate(glm::identity<glm::mat4x4>(), position);
	translateReady = true;
}
void Transform::updateRotateMatrix()
{
	rotationMatrix = glm::mat4_cast(rotation);
	rotateReady = true;
}
void Transform::updateScaleMatrix()
{
	scaleMatrix = glm::scale(glm::identity<glm::mat4x4>(), scale);
	scaleReady = true;
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
	/*
	glm::vec4 vertices[] = { 
		glm::vec4(0.5f, 0.5f, 1.0f/glm::sqrt(2.0f), 1.0f), 
		glm::vec4(0.5f, 0.5f, -1.0f / glm::sqrt(2.0f), 1.0f),
		glm::vec4(-0.5f, -0.5f, -1.0f / glm::sqrt(2.0f), 1.0f), 
		glm::vec4(-0.5f, -0.5f, 1.0f / glm::sqrt(2.0f), 1.0f), };
	glm::vec4 result;
	for (int i = 0; i < 4; i++)
		LOG("vertex #" + std::to_string(i) + ": (" + std::to_string(vertices[i].x) + ", " + std::to_string(vertices[i].y) + ", " + std::to_string(vertices[i].z) + ", " + std::to_string(vertices[i].w) + ")");
	LOG("\n");

	Transform* t = new Transform();
	t->modRotation(glm::vec3(0.0f, 0.0f, 1.2f), glm::pi<float>() * 0.25f);	//rotate() function seems to normalise vector 
	//this is not recommended use, just to check that points are transformed correctly
	//expected:
	//	0:		0,	 0.7 ,	 0.7
	//	1:		0,	 0.7 ,	-0.7
	//	2:		0,	-0.7 ,	-0.7
	//	3:		0,	-0.7 ,	 0.7
	for (int i = 0; i < 4; i++)
	{
		result = *t->getMatrix() * vertices[i];
		LOG("vertex #" + std::to_string(i) + ": (" + std::to_string(result.x) + ", " + std::to_string(result.y) + ", " + std::to_string(result.z) + ", " + std::to_string(result.w) + ")");
	}
	LOG("\n");
	t->modRotation(glm::vec3(1.0f, -1.0f, 0.0f), glm::pi<float>() * 0.25f);
	//this is not recommended use, just to check that points are transformed correctly
	//expected:
	//	0:		0,	  0,	 1
	//	1:		0,	  1,	 0
	//	2:		0,	  0,	-1
	//	3:		0,	 -1,	 0
	for (int i = 0; i < 4; i++)
	{
		result = *t->getMatrix() * vertices[i];
		LOG("vertex #" + std::to_string(i) + ": (" + std::to_string(result.x) + ", " + std::to_string(result.y) + ", " + std::to_string(result.z) + ", " + std::to_string(result.w) + ")");
	}
	delete t;*/

	//ok so here's the thing... worldScale things don't take rotations into account.
	//I'm not sure if they should, but if they don't the multiplicative worldScale modification is pointless, and additative worldScale modification is... difficult.
	//Make up your mind regarding how this should behave.
	//also verify that a scale 1-2-1 object does not warp when spun (OK I checked it, it did, and I think I fixed it by reordering things in "updateMatrix")
	//if the warping while spinning effect is desired, I think it should work by scaling a parent object and rotating the child
	Transform a = Transform();
	a.setScale(glm::vec3(1.0f, 2.0f, 0.5f));
	a.setRotation(glm::vec3(glm::pi<float>() * 0.5f, 0.0f, 0.0f));
	Transform b(&a);
	b.setScale(glm::vec3(1.0f, 2.0f, 0.5f));
	glm::vec3 worScale = b.getWorldScale();
	LOG("world scale of B: (" + std::to_string(worScale.x) + ", " + std::to_string(worScale.y) + ", " + std::to_string(worScale.z) + ")");
	//Transform c(&b);


	glm::vec4 square[] = {
		glm::vec4(1.0f, 1.0f, 0.0f, 1.0f),
		glm::vec4(1.0f, -1.0f, 0.0f, 1.0f),
		glm::vec4(-1.0f, -1.0f, 0.0f, 1.0f),
		glm::vec4(-1.0f, 1.0f, 0.0f, 1.0f),
	};
	glm::vec4 result;
	//glm::vec4 mod(3.0f, 0.0f, 0.0f, -1.0f);
	//result = square[0] / mod;		//yes, this performs element-wise division (same with *), but keep in mind that division with 0 causes inf or NaN
	//LOG("TEST");
	//LOG("result: (" + std::to_string(result.x) + ", " + std::to_string(result.y) + ", " + std::to_string(result.z) + ", " + std::to_string(result.w) + ")");



	Transform parent = Transform();
	Transform t = Transform(&parent);

	LOG("Initial status:");	//INITIAL
	for (int i = 0; i < 4; i++)
	{
		result = *t.getWorldMatrix() * square[i];
		LOG("vertex #" + std::to_string(i) + ": (" + std::to_string(result.x) + ", " + std::to_string(result.y) + ", " + std::to_string(result.z) + ", " + std::to_string(result.w) + ")");
	}

	parent.modPosition(glm::vec3(0.0f, -1.0f, 0.0f));		//MOVE PARENT DOWN
	LOG("After down-move of parent");
	for (int i = 0; i < 4; i++)
	{
		result = *t.getWorldMatrix() * square[i];
		LOG("vertex #" + std::to_string(i) + ": (" + std::to_string(result.x) + ", " + std::to_string(result.y) + ", " + std::to_string(result.z) + ", " + std::to_string(result.w) + ")");
	}

	parent.modRotation(glm::vec3(0.0f, 0.0f, 1.0f), -glm::pi<float>() / 2);		//ROTATE PARENT CLOCKWISE
	LOG("After clockwise-rot of parent");
	for (int i = 0; i < 4; i++)
	{
		result = *t.getWorldMatrix() * square[i];
		LOG("vertex #" + std::to_string(i) + ": (" + std::to_string(result.x) + ", " + std::to_string(result.y) + ", " + std::to_string(result.z) + ", " + std::to_string(result.w) + ")");
	}

	parent.modScale(glm::vec3(1.0f, 0.5f, 0.5f));		//HALV SCALE OF PARENT, EXCEPT OVER X-AXIS
	LOG("After halving scale (except X-axis) of parent");
	for (int i = 0; i < 4; i++)
	{
		result = *t.getWorldMatrix() * square[i];
		LOG("vertex #" + std::to_string(i) + ": (" + std::to_string(result.x) + ", " + std::to_string(result.y) + ", " + std::to_string(result.z) + ", " + std::to_string(result.w) + ")");
	}

	t.modScale(glm::vec3(1.0f, 2.0f, 2.0f));		//MULTIPLY CHILD LOCAL SCALE BY 2, except x-axis
	LOG("After doubling scale (exept X-axis) of child");
	for (int i = 0; i < 4; i++)
	{
		result = *t.getWorldMatrix() * square[i];
		LOG("vertex #" + std::to_string(i) + ": (" + std::to_string(result.x) + ", " + std::to_string(result.y) + ", " + std::to_string(result.z) + ", " + std::to_string(result.w) + ")");
	}

	t.setWorldScale(glm::vec3(1.0f, 1.0f, 1.0f));		//RESET CHILD WORLD SCALE
	LOG("Reset world scale of child to 1-1-1");
	for (int i = 0; i < 4; i++)
	{
		result = *t.getWorldMatrix() * square[i];
		LOG("vertex #" + std::to_string(i) + ": (" + std::to_string(result.x) + ", " + std::to_string(result.y) + ", " + std::to_string(result.z) + ", " + std::to_string(result.w) + ")");
	}
	t.setWorldPosition(glm::vec3(-1.0f, 0.0f, 0.0f));		//RESET CHILD WORLD POSITION
	LOG("Set world position of child to -1, 0, 0");
	for (int i = 0; i < 4; i++)
	{
		result = *t.getWorldMatrix() * square[i];
		LOG("vertex #" + std::to_string(i) + ": (" + std::to_string(result.x) + ", " + std::to_string(result.y) + ", " + std::to_string(result.z) + ", " + std::to_string(result.w) + ")");
	}
	LOG("child local transform = " + t.toString() + "\nchild world transform = " + t.worldToString());
	Transform crazyNanny = Transform();
	crazyNanny.setPosition(glm::vec3(1.35f, 2.37f, 0.33f));
	crazyNanny.setRotation(glm::vec3(10.37f, 222.111f, 90.33f));
	crazyNanny.setScale(glm::vec3(1239.0f, 0.332f, 1.202f));
	t.setParent(nullptr);// &crazyNanny);											//SET NEW PARENT
	LOG("Adopted by a crazy nanny. Crazy nanny is our new parent");
	for (int i = 0; i < 4; i++)
	{
		result = *t.getWorldMatrix() * square[i];
		LOG("vertex #" + std::to_string(i) + ": (" + std::to_string(result.x) + ", " + std::to_string(result.y) + ", " + std::to_string(result.z) + ", " + std::to_string(result.w) + ")");
	}
	t.setParent(&parent);												//RETURN TO PREVIOUS PARENT
	LOG("Returned to original parent");
	for (int i = 0; i < 4; i++)
	{
		result = *t.getWorldMatrix() * square[i];
		LOG("vertex #" + std::to_string(i) + ": (" + std::to_string(result.x) + ", " + std::to_string(result.y) + ", " + std::to_string(result.z) + ", " + std::to_string(result.w) + ")");
	}
	
	LOG("If the last three printouts are not identical, then something is wrong!");

	/*
	t.modRotation(glm::vec3(1.0f, 0.0f, 0.0f), glm::pi<float>());		//ROTATE CHILD BY LOCAL X-AXIS 180 DEG
	LOG("After flipping over local X-axis of child");
	for (int i = 0; i < 4; i++)
	{
		result = *t.getWorldMatrix() * square[i];
		LOG("vertex #" + std::to_string(i) + ": (" + std::to_string(result.x) + ", " + std::to_string(result.y) + ", " + std::to_string(result.z) + ", " + std::to_string(result.w) + ")");
	}

	t.modRotationExtrinsic(glm::vec3(0.0f, 0.0f, 1.0f), glm::pi<float>() / 2.0f);		//ROTATE CHILD BY Extrinsic Z-AXIS 90 DEG
	LOG("After rotating 90 DEG over local extrinsic (original) Z-axis of child");
	for (int i = 0; i < 4; i++)
	{
		result = *t.getWorldMatrix() * square[i];
		LOG("vertex #" + std::to_string(i) + ": (" + std::to_string(result.x) + ", " + std::to_string(result.y) + ", " + std::to_string(result.z) + ", " + std::to_string(result.w) + ")");
	}

	t.setWorldPosition(glm::vec3(1.0f, 0.5f, 0.0f));		//SET POSITION BY WOLRD POSITION
	LOG("After setting world position");
	for (int i = 0; i < 4; i++)
	{
		result = *t.getWorldMatrix() * square[i];
		LOG("vertex #" + std::to_string(i) + ": (" + std::to_string(result.x) + ", " + std::to_string(result.y) + ", " + std::to_string(result.z) + ", " + std::to_string(result.w) + ")");
	}

	t.modWorldRotation(glm::vec3(1.0f, 0.0f, 0.0f), glm::pi<float>());		//ROTATE CHILD BY WORLD X-AXIS 180 DEG
	//t.modWorldRotation(glm::rotate(Transform::quatInit(), glm::pi<float>(), glm::vec3(1.0f, 0.0f, 0.0f)));
	LOG("After rotating over world X-axis");
	for (int i = 0; i < 4; i++)
	{
		result = *t.getWorldMatrix() * square[i];
		LOG("vertex #" + std::to_string(i) + ": (" + std::to_string(result.x) + ", " + std::to_string(result.y) + ", " + std::to_string(result.z) + ", " + std::to_string(result.w) + ")");
	}
	//t.setWorldPosition(glm::vec3(0.0f, 0.0f, 0.0f));			//OK SO I'M A LITTLE CONFUSED BUT I THINK MAYBE IT'S CORRECT ANYWAY, CHECK FOR DIVISION OF ZERO ERRORS THOUGH!
	t.modWorldScale(glm::vec3(-0.5f, 0.0f, 0.0f));		//MODIFY CHILD SCALE BY WORLD-ADJUSTED -0.5, 0.5, -0.5 (should make world scale:
	LOG("After changing scale by addition/subtraction");
	for (int i = 0; i < 4; i++)
	{
		result = *t.getWorldMatrix() * square[i];
		LOG("vertex #" + std::to_string(i) + ": (" + std::to_string(result.x) + ", " + std::to_string(result.y) + ", " + std::to_string(result.z) + ", " + std::to_string(result.w) + ")");
	}

	t.setWorldRotation(glm::vec3(0.0f, 0.0f, 0.0f));		//SET CHILD WORLD ROTATION TO NEUTRAL
	LOG("After setting child world rotation to neutral");
	for (int i = 0; i < 4; i++)
	{
		result = *t.getWorldMatrix() * square[i];
		LOG("vertex #" + std::to_string(i) + ": (" + std::to_string(result.x) + ", " + std::to_string(result.y) + ", " + std::to_string(result.z) + ", " + std::to_string(result.w) + ")");
	}*/


}



//WORLD DATA
const glm::mat4x4* Transform::getWorldMatrix()
{
	if (!matrixReady) updateMatrix();
	if (parentTransform == nullptr)
	{
		return &transformMatrix;
	}
	this->worldMatrix = *this->parentTransform->getWorldMatrix() * this->transformMatrix;
	return &worldMatrix;
}
const float* Transform::getWorldMatrixArray()
{
	if (!matrixReady) updateMatrix();
	if (parentTransform == nullptr)
	{
		return glm::value_ptr(transformMatrix);
	}
	this->worldMatrix = *this->parentTransform->getWorldMatrix() * this->transformMatrix;
	return glm::value_ptr(worldMatrix);
}