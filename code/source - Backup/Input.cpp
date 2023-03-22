#include <Input.h>
#include <Debug.h>
#include <iostream>
#include <fstream>
#include <string>

#include <glew/glew.h>
#include <glfw/glfw3.h>

#define KEYBINDLINELEN (200)	//Maximum length of a line of text in the keybinding file, exceeding this value should generate alert

//this needs to account for some default set of keys
//and custom saved files based on scancodes
void Input::initialize(GLFWwindow* window, const char* keyBindingsFileName/*SOME KINDA LIST POINTER I DUNNO*/)
{
	//register callback function
	glfwSetKeyCallback(window, keyResponse);

	std::fstream kbFile(keyBindingsFileName, std::fstream::in);
	if (kbFile.fail())	//init failed
	{
		LOGERROR("Cannot open file: " + std::string(keyBindingsFileName));
		return;
	}
	char inBuffer[KEYBINDLINELEN];
	char validchars[KEYBINDLINELEN];
	int i = 0;
	do
	{
		i++;
		kbFile.getline(inBuffer, (std::streamsize)KEYBINDLINELEN);		//https://www.cplusplus.com/reference/istream/istream/getline/
		//handle errors
		if (kbFile.fail())	//read failed
		{
			LOGALERT("Internal issue reading line #" + std::to_string(i));
		}
		else		//read successful
		{
			LOG("keyboard bindings line #" + std::to_string(i) + ": " + std::string(inBuffer));			//DEBUG
			//seperate into components
			int j = 0, v = 0;	//j counts inBuffer index, v counts validchars index
			//std::string command;
			int commandValue = -1;
			bool awaitingKeyTag = false;	//indicates if '=' has been read yet
			while (j < KEYBINDLINELEN)	//TODO: work here and match with scancodes or macros (call findTypicalKeyMacro)
			{
				if (inBuffer[j] == ' ' || inBuffer[j] == '\t')	//skip spaces and tabs
				{
					//do nothing
				}
				else if (!awaitingKeyTag)
				{
					if (inBuffer[j] == '#' || inBuffer[j] == '\0')	//stop reading at comment sign or end of string
					{
						//dud line, skip
						LOG("comment line, skipping");	//DEBUG
						break;
					}
					else if (inBuffer[j] == '=')
					{
						//inBuffer[j] = '\0';
						//command = std::string(inBuffer);
						//strt = j + 1;
						//finalize
						validchars[v] = '\0';
						//command = std::string(validchars);
						commandValue = std::atoi(validchars);
						v = 0;
						LOG("command recognised as: " + std::to_string(commandValue));			//DEBUG
						awaitingKeyTag = true;
					}
					else //character
					{
						validchars[v++] = inBuffer[j];
					}
				}
				else if (awaitingKeyTag)
				{
					if (inBuffer[j] == '#' || inBuffer[j] == '\0')	//stop reading at comment sign or end of string
					{
						//finalize last value
						validchars[v] = '\0';
						//command = std::string(validchars);
						v = 0;
						int keyValue = std::atoi(validchars);
						addKey(glfwGetKeyScancode(keyValue), commandValue);		//add Key to tree, also adds button object if it doesn't exist yet
						LOG("key recognised as: " + std::to_string(keyValue) + "\n\twith scancode: " + std::to_string(glfwGetKeyScancode(keyValue)));				//DEBUG
						break;
					}
					else if (inBuffer[j] == ',')	//if split command
					{
						//inBuffer[j] = '\0';
						//addKey(inBuffer + strt)
						validchars[v] = '\0';
						v = 0;
						int keyValue = std::atoi(validchars);
						if (keyValue == 0)	LOGWARNING("key 0 does not correspond to any real key and most likely indicates failure to parse line #" + i);
						addKey(glfwGetKeyScancode(keyValue), commandValue);		//add Key to tree, also adds button object if it doesn't exist yet
						LOG("key recognised as: " + std::to_string(keyValue) + "\n\twith scancode: " + std::to_string(glfwGetKeyScancode(keyValue)));				//DEBUG
					}
					else //character
					{
						validchars[v++] = inBuffer[j];
					}
				}

				j++;
			}

		}

	} while(kbFile.good());												//https://www.cplusplus.com/reference/ios/ios/good/
	if (kbFile.eof())	//end of file
	{
		LOGNOTE("End of file reached");
	}

	kbFile.close();
}

void Input::destroy()	//I don't think static destructors are a thing, just incase we need to reset things though
{
	delete buttonListRoot;
	buttonListRoot = nullptr;
	keyTree.empty();
}


void Input::keyResponse(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	//LOG("key-action");
	Button* button = (Button*)keyTree.get(scancode);
	if (button != nullptr)
	{
		button->state = action;
	}

	/*
	//DEBUGGING STUFF
	if (action == GLFW_PRESS)										//DEBUG
	{
		LOG("Reacting to key with scancode: " + std::to_string(scancode));			//DEBUG
		if(button == nullptr)	 LOG("\tDoes not correspond to any action.");			//DEBUG
		else LOG("\tCorresponds to action: " + std::to_string(button->command));			//DEBUG
	}
	else if (action == GLFW_RELEASE)									//DEBUG
	{
		LOG("Reacting to key with scancode: " + std::to_string(scancode));			//DEBUG
		if (button == nullptr)	 LOG("\tDoes not correspond to any action.");			//DEBUG
		else LOG("\tCorresponds to action: " + std::to_string(button->command));			//DEBUG
	}
	*/
}


//the Input class deals with two linked structures, a keytree and a buttonlist.
//the keytree is only accesssed by this class, and contains mappings from key to buttons (in-game actions)
//example, two entries (W,jump) and (S,jump) map both keys W and S to the same button "jump"
//the buttonlist is populated by this class but accessed by anything that wishes to monitor a button.
//a class that wishes to do so should first retrieve a pointer to the button with findButton(), preferrably only once during initialization
//and then check during its update function what state the button is in with functions active(), inactive(), or is there is need to be specific: hit(), held(), let(), rest().

//adds a key to the keyTree, with a pointer to a Button struct as its data
//returns true on success, false on failure which probably means the key was already added
RBTree Input::keyTree = RBTree();
bool Input::addKey(int scancode, int command)
{
	//search through keytree to make sure key hasn't been registered before.
	Button* b;
	if ((b = (Button*)keyTree.get(scancode)) != nullptr)	//if it HAS been registered before
	{
		LOGWARNING("Cannot bind key (scancode #" + std::to_string(scancode) + ") to button #" + std::to_string(command) + " as it has already been registered to button #" + std::to_string(b->command));
		return false;
	}
	//try to find Button corresponding to command in buttonlist
		//if found, save pointer
		//if not, call addButton()	--->create new Button struct on the buttonlist
	if ((b = findButton(command)) == nullptr)
	{
		b = addButton(command);
	}
	//add a new entry in the keytree
	if (keyTree.set(scancode, b))	//true response indicates a node in the tree with the same scancode was reused, which should never happen due to the safety check earlier in this function
	{
		LOGALERT("Strange behaviour, safety-check earlier in this function failed. A scancode was re-registered in an unexpected manner");
	}
	return true;
}

Input::ButtonList* Input::buttonListRoot = nullptr;
Input::Button* Input::addButton(int command)
{
	//create a new button struct and add it to the buttonlist
	Button* newButton = new Button(command);
	ButtonList* listEntry = new ButtonList(newButton);
	listEntry->next = buttonListRoot;
	buttonListRoot = listEntry;
	return newButton;
}

//returns corresponding button object, or nullptr if not found
Input::Button* Input::findButton(int command)
{
	Input::ButtonList* node = buttonListRoot;
	while (node != nullptr)
	{
		if (node->button->command == command)
			return node->button;
		node = node->next;
	}
	return nullptr;
}

//returns a ticket corresponding to the button handling the requested command, or nullptr if none exist.
//use this ticket to retrive button states with function calls active(), inactive(), hit(), held(), let(), rest()
Input::ButtonTicket* Input::getButtonTicket(int command)
{
	return (ButtonTicket*)findButton(command);
}

//0:Release, 1:Press, 2:Repeat. We'll also use -1:Rested
bool Input::active(ButtonTicket* ticket)		//the button is pressed, equals hit OR held
{
	if (ticket == nullptr)
	{
		LOGALERT("Button reference sent to Input:active() is nullptr");
		return false;
	}
	return ((Button*)ticket)->state > 0;
}
bool Input::inactive(ButtonTicket* ticket)	//the button is not pressed, equal let OR rest
{
	if (ticket == nullptr)
	{
		LOGALERT("Button reference sent to Input:inactive() is nullptr");
		return false;
	}
	return ((Button*)ticket)->state < 1;
}
bool Input::hit(ButtonTicket* ticket)			//the button was just hit
{
	if (ticket == nullptr)
	{
		LOGALERT("Button reference sent to Input:hit() is nullptr");
		return false;
	}
	return ((Button*)ticket)->state == 1;
}
bool Input::held(ButtonTicket* ticket)		//the button is being held down
{
	if (ticket == nullptr)
	{
		LOGALERT("Button reference sent to Input:hit() is nullptr");
		return false;
	}
	return ((Button*)ticket)->state == 2;
}
bool Input::let(ButtonTicket* ticket)			//the button has just been let go
{
	if (ticket == nullptr)
	{
		LOGALERT("Button reference sent to Input:hit() is nullptr");
		return false;
	}
	return ((Button*)ticket)->state == 0;
}
bool Input::rest(ButtonTicket* ticket)		//the button hasn't been pressed
{
	if (ticket == nullptr)
	{
		LOGALERT("Button reference sent to Input:hit() is nullptr");
		return false;
	}
	return ((Button*)ticket)->state == -1;
}

//reset key states to -1:Rested ahead of new frame, MUST be called each frame shortly before polling glfw window events, In between which Input functions incorrectly
// *ehem* so apparently a key doesn't start "repeating" until about half a second into the press. I'm going to try to override this behaviour here
void Input::resetKeys()
{
	ButtonList* b = buttonListRoot;
	while (b != nullptr)
	{
		//b->button->state = -1;	//Regular behaviour

		//rapid behaviour
		if (b->button->state == GLFW_RELEASE) b->button->state--;		//if it was released (let) this frame, change into off (rest)
		else if (b->button->state == GLFW_PRESS) b->button->state++;		//if it was pressed (hit) this frame, change into repeated (held)

		b = b->next;
	}
}

Input::ButtonTicket::ButtonTicket()
{
	LOGWARNING("An instance of Input::ButtonTicket was created, this is faulty use, only ever create ButtonTicket pointers for communication with the Input class.");
}