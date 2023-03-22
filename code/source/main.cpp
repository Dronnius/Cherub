#include <glew/glew.h>
#include <glfw/glfw3.h>

#include <Input.h>

#include <iostream>
#include <Transform.h> //unit testing only
#include <Visual.h> //unit testing only
#include <Debug.h>
#include <RBTree.h>
#include <Painter.h>
#include <tools/DebugTransformableCube.h>

//this function is intended to serve as a "unit test" for the engine
int main(void)
{
	GLFWwindow* window;
	if (!glfwInit()) return -1;	//initialize GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);  //OpenGL version to use (3,...)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);  //OpenGL version to use (...,3)
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);     //create window, include openGL context
    //window = glfwCreateWindow(1920, 1080, "Hello World", NULL, NULL);     //create window, include openGL context
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window); //make window's context the current one (state-machine things I think)

    if (glewInit() != GLEW_OK)  //GLEWINIT MUST BE CALLED AFTER GLFWMAKECONTEXTCURRENT()
    {
        std::cout << "GLEW failed to initialize" << std::endl;
        return -2;
    }
    std::cout << glGetString(GL_VERSION) << std::endl;  //print version, because why not



    //quick glm test
    //glm::vec3 a(0.1f);
    //LOG("X = " + std::to_string(a.x) + "\nY = " + std::to_string(a.y) + "\nZ = " + std::to_string(a.z));

    //Attempt to fix error callback
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(debugResponse, nullptr);

    //initialize Painter
    Painter::initialize();

    //initialise desired console functionality
    DebugTransformableCube::addConsoleCommands();

    //UNIT TESTING
    //Transform::debug_unitTest();      //Rotations should be looked at again, world and local rotations are relevant (ON IT)
    //RBTree::debug_unitTest();
    Visual::debug_unitTest(window);

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        //frame content goes here (call updates and stuff)
        glfwSwapBuffers(window);
        Input::resetKeys();
        glfwPollEvents();
        checkConsoleCommands(0.0f);     //time goes in here
    }

    consoleWatcherDestroy();
    glfwTerminate();
    return 0;
}