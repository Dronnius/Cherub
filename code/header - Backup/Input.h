#pragma once
#include <glew/glew.h>
#include <glfw/glfw3.h>
#include <RBTree.h>


//TODO: MOST OF THIS
//Register GLFW callback functions for mouse, keyboard and gamepad (joystick) inputs
//Update an array of variables that can be polled later in various update functions
//Try to allow for rebinding keys if possible, probably related to "Scancodes"
//Pay attention to action values GLFW_PRESS, GLFW_REPEAT and GLFW_RELEASE (Error handle GLFW_KEY_UNKNOWN once only)
//Ideally a list of used hotkeys is populated at initialization of scenes, or the start of the entire game might be better
	//Then, they can be rebound from a menu (or console commands idk)
	//Then, update functions poll this Input class to read the state


static class Input
{
private:
	struct Button
	{
		int command;	//the "name" of the action
		int state = -1;	//the Button state of the button		GLFW docs: https://www.glfw.org/docs/latest/group__input.html#ga2485743d0b59df3791c45951c4195265 0:Release, 1:Press, 2:Repeat. We'll also use -1:Rested

		Button(int command)
		{
			this->command = command;
		}
	};
	//red black tree used to update button states
	//glfw scancodes are the keys, and Button pointers are the data
	static RBTree keyTree;
	//a list keeps track of buttons
	struct ButtonList
	{
		Input::Button* button = nullptr;
		ButtonList* next = nullptr;

		ButtonList(Input::Button* button)
		{
			this->button = button;
		}
		//deleting this struct will remove all following elements in the list
		//apparently delete nullptr is safe, and doesn't do anything, so we don't need to check for it: https://stackoverflow.com/questions/4190703/is-it-safe-to-delete-a-null-pointer
		~ButtonList()
		{
			delete next;
		}
	};
	//button list operates as FILO, beacuse that will be easier
	static ButtonList* buttonListRoot;

	//this is for identifying default keys, not custom ones //CHANGED MY MIND, IGNORE THIS, remove later unless you change your mind yet again
	//static int findTypicalKeyMacro(const char* name);

	//add a key to the keytree
	static bool addKey(int scancode, int command);
	//add a button to the buttonlist
	static Input::Button* addButton(int command);

	//returns corresponding button object, or nullptr if not found
	static Button* findButton(int command);

public:	
	//this is only to be used as pointers to button objects and NOTHING else. NEVER create one of these structs!
	//why not just use pointers to the button objects? Just to hide/protect them from manipulation
	struct ButtonTicket {
		ButtonTicket();
	};
	//returns a ticket corresponding to the button handling the requested command, or nullptr if none exist.
	//use this ticket to retrive button states with function calls active(), inactive(), hit(), held(), let(), rest()
	static ButtonTicket* getButtonTicket(int command);


	//Initialize the list of actions requiring hotkeys
		//Give them simple names like "jump" maybe?
	//Also, register callback functions
	static void initialize(GLFWwindow* window, const char* keyBindingsFileName/*SOME KINDA LIST POINTER I DUNNO*/);

	//destroy to get rid of tree and list
	static void destroy();

	//reset key states to -1:Rested ahead of new frame, MUST be called each frame shortly before polling glfw window events, In between which Input functions incorrectly
	static void resetKeys();

	//Keyboard Input: https://www.glfw.org/docs/latest/input_guide.html#input_keyboard
	//Window might be useful if we use several...
	//Key values correspond to macros like GLFW_KEY_A, full list: https://www.glfw.org/docs/latest/group__keys.html (may be undefined)
	//Scancode is always defined, but varies by system.
	//Action values are GLFW_PRESS, GLFW_REPEAT, GLFW_RELEASE or GLFW_KEY_UNKNOWN (Error handle GLFW_KEY_UNKNOWN once only)
	static void keyResponse(GLFWwindow* window, int key, int scancode, int action, int mods);

	static bool active(ButtonTicket* ticket);	//the button is pressed, equals hit OR held
	static bool inactive(ButtonTicket* ticket);	//the button is not pressed, equal let OR rest
	static bool hit(ButtonTicket* ticket);		//the button was just hit
	static bool held(ButtonTicket* ticket);		//the button is being held down
	static bool let(ButtonTicket* ticket);		//the button has just been let go
	static bool rest(ButtonTicket* ticket);		//the button hasn't been pressed
};