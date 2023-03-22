#include <tools/DebugCamera.h>
#include <Debug.h>

//<string> <iostream> <Camera.h> <Transform.h> <string> "glm/glm.hpp" "glm/gtc/matrix_transform.hpp" "glm/gtc/type_ptr.hpp" <glew/glew.h> <glfw/glfw3.h> <RBTree.h>



DebugCamera::DebugCamera(float timeStamp) : Camera("Debug Camera"), Logical("Debug Camera", timeStamp)
{
	//this->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
	cameraUp = Input::getButtonTicket(1);
	cameraLeft = Input::getButtonTicket(3);
	cameraDown = Input::getButtonTicket(2);
	cameraRight = Input::getButtonTicket(4);
	cameraShift = Input::getButtonTicket(5);
	cameraFaster = Input::getButtonTicket(7);
	cameraSlower = Input::getButtonTicket(6);
	//if ((long)cameraSlower * (long)cameraRight * (long)cameraUp == (long)nullptr)	LOGERROR("This is an issue");//DEBUG
}


	//Call update each frame
	// Arguments:
	//  dTime - time in milliseconds(?) since last frame
	// Returns:
	//  0 if update succeeded, nothing to report
	//  > 0 if update encountered a minor issue, worth investigation (should also log warning)
	//  < 0 if update encountered a major issue, worth stopping program (should also log error)
int DebugCamera::update(float dTime)
{
	//LOG("UPDATE");
	if (Cameraman::isActiveCamera(this))
	{
		if (Input::hit(cameraFaster))	//increase speed
		{
			this->speed *= this->modifier;
		}
		if (Input::hit(cameraSlower))	//decrease speed
		{
			if (modifier == 0.0f)
				LOGERROR("Modifier at zero, division requires non-zero denominator");
			else this->speed /= this->modifier;
		}
			
		float diff = dTime * speed;

		if (Input::active(cameraUp))
		{
			//LOG("UPBUTTON");
			if (Input::active(cameraShift))		//move forward	(inverted)
			{
				//LOG("FORWARD");
				this->modPosition(glm::vec3(0.0f, 0.0f, -1.0f) * diff);
			}
			else								//move up
			{
				this->modPosition(glm::vec3(0.0f, 1.0f, 0.0f) * diff);
			}
		}
		if (Input::active(cameraLeft))			//move left
		{
			this->modPosition(glm::vec3(-1.0f, 0.0f, 0.0f) * diff);
		}
		if (Input::active(cameraDown))
		{
			//LOG("DOWNBUTTON");
			if (Input::active(cameraShift))		//move back		(inverted)
			{
				//LOG("BACK");
				this->modPosition(glm::vec3(0.0f, 0.0f, 1.0f) * diff);
			}
			else								//move down
			{
				this->modPosition(glm::vec3(0.0f, -1.0f, 0.0f) * diff);
			}
		}
		if (Input::active(cameraRight))			//move right
		{
			this->modPosition(glm::vec3(1.0f, 0.0f, 0.0f) * diff);
		}
	}
	return 0;
}