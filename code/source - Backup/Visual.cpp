#include <glew/glew.h>
#include <glfw/glfw3.h>


#include <iostream> //debug only
#include <Visual.h>
#include <Shader.h>
#include "../source/ExplicitShader.cpp"	//there's gotta be a better way to do this...
#include <Debug.h>
#include <Cameraman.h>
#include <Input.h>

//tools
#include <tools/DebugCamera.h>

//load mesh through pre-made arrays
//arrays are copied, so the originals are safe to be removed
//make sure the object's destructor is called properly
bool Visual::load(float* verticesInput, unsigned int vertexCount, unsigned int* indicesInput, unsigned int indexCount, GLvao* vao, Shader* shaderInput, GLenum drawMode)
{
	if (vao == nullptr)	//no vao
		vao = new GLvao();
	vao->bind();

	if (shaderInput == nullptr)
	{
		//create default shader here, if desired, fail load for now.
		return false;
	}
	this->shader = shader;

	//update internal values
	this->vertexSize = vertexCount * sizeof(float);
	this->indexSize = indexCount * sizeof(unsigned int);
	this->indexCount = indexCount;

	//copy vertex data
	this->vertices = (float*)malloc(this->vertexSize);
	if (this->vertices == nullptr)
	{
		LOGERROR("loading mesh vertices failed");
		//std::cout << "Error: " << "loading mesh vertices failed (line = " << __LINE__ << ", file = " << __FILE__ << std::endl;
		return false;
	}
	heapedVertices = true;
	memcpy((void*)this->vertices, verticesInput, this->vertexSize);

	//copy index data
	this->indices = (unsigned int*)malloc(indexCount * sizeof(unsigned int));
	if (this->indices == nullptr)
	{
		LOGERROR("loading mesh indices failed");
		//std::cout << "Error: " << "loading mesh indices failed (line = " << __LINE__ << ", file = " << __FILE__ << std::endl;
		return false;
	}
	heapedIndices = true;
	memcpy((void*)this->indices, indicesInput, indexSize);

	//Retrive an ID for the vertex buffer & index buffer
	GLuint buffers[2];
	glGenBuffers(2, buffers);
	this->vertexBuffer = buffers[0];
	this->indexBuffer = buffers[1];
	if (!CHECK("load")) return false;	//DEBUG

	//Declare the buffer to be a vertex buffer (yes, GL_ARRAY_BUFFER)
	glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer);
	if (!CHECK("load")) return false;	//DEBUG
	//Declare the buffer to be an index buffer (yes, GL_ARRAY_BUFFER)
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->indexBuffer);
	if (!CHECK("load")) return false;	//DEBUG

	//Create the buffers
	//Size: defaulted to max 1000 triangles where each vertex has 10 elements
	//Mode: defaulted to static draw will usually do, objects with skeletan animation may need a dynamic buffer
	glBufferData(GL_ARRAY_BUFFER, this->vertexSize, this->vertices, drawMode);
	if (!CHECK("load")) return false;	//DEBUG
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indexSize, this->indices, drawMode);
	if (!CHECK("load")) return false;	//DEBUG

	return true;
}


bool Visual::draw(/*glm::mat4* camera,*/ GLvao* vao)
{
	if (this->shader == nullptr/* || camera == nullptr*/)
		return false;
	//this->camera = camera;
	this->shader->prep(this, vao);
	//const GLfloat* mvpMatrix = glm::value_ptr(*camera * *this->getMatrix());	//get matrix
	//GLuint uniformHandle = glGetUniformLocation(this->shader->getID(), "modelMatrix");	//ASSUMING ID HAS BEEN SET BY SUPER-CLASS CONSTRUCTOR ALREADY, should be the case...
	//glUniformMatrix4fv(uniformHandle, 1, GL_FALSE, mvpMatrix);	//register changes (send to shader)	(This should be handled by a call to some subclass of Shader (somehow idk)
	if (!CHECK("draw")) return false;
	glDrawElements(GL_TRIANGLES, this->indexCount, GL_UNSIGNED_INT, 0);	//last argument is an offset into bound index buffer, not the index buffer itself!
	if (!CHECK("draw")) return false;
	return true;
}

//Static
void Visual::debug_unitTest(GLFWwindow* window) //https://learnopengl.com/In-Practice/Debugging
{
	//I'm viewing the back (bottom according to .h file) of the cube for some reason
	//IT'S NOT TRANSPARENT, IT'S JUST REPLACING EACH NEW LAYER ISTEAD OF COMPARING THE Z-BUFFER VALUES
	//VERY IMPORTANT: Enable Z-buffer (sorts layers of polygons based on distance) (will be inverted without a projection matrix, for some reason)
	glEnable(GL_DEPTH_TEST);

	//Supposedly needed to remove polygons facing away
	glEnable(GL_CULL_FACE);

	
	//ABSTRACT AWAY AS MUCH AS POSSIBLE!
	Visual visi = Visual();

	// Vertex data [EXAMPLE DATA]
	float cubeVerts[7 * 8] =
	{
		+0.5f, +0.5f, +0.5f,		//0
			+0.0f, +0.0f, +0.0f, +1.0f,		//0 colour (black)

		+0.5f, +0.5f, -0.5f,		//1
			+1.0f, +0.0f, +0.0f, +1.0f,		//1 colour (red)

		+0.5f, -0.5f, +0.5f,		//2
			+1.0f, +1.0f, +0.0f, +1.0f,		//2 colour (yellow)

		+0.5f, -0.5f, -0.5f,		//3
			+0.0f, +1.0f, +0.0f, +1.0f,		//3 colour (green)

		-0.5f, +0.5f, +0.5f,		//4
			+0.0f, +1.0f, +1.0f, +1.0f,		//4 colour (cyan)

		-0.5f, +0.5f, -0.5f,		//5
			+0.0f, +0.0f, +1.0f, +1.0f,		//5 colour (blue)

		-0.5f, -0.5f, +0.5f,		//6
			+1.0f, +0.0f, +1.0f, +1.0f,		//6 colour (magenta)

		-0.5f, -0.5f, -0.5f,		//7
			+1.0f, +1.0f, +1.0f, +1.0f,		//7 colour (white)
	};
	// Index data [EXAMPLE DATA]
	unsigned int cubeFaces[6 * 6] =
	{
		0, 2, 3,		//Front face
		0, 3, 1,		// x = 0.5f

		4, 5, 6,		//Back face
		5, 7, 6,		// x = -0.5f

		0, 1, 4,		//Right face
		1, 5, 4,		// y = 0.5f

		2, 6, 7, 		//Left face
		2, 7, 3, 		// y = -0.5f

		0, 6, 2,		//Top face
		0, 4, 6,		// z = 0.5f

		1, 3, 7,		//Bottom face
		1, 7, 5,		// z = -0.5f
	};
	/*visi.vertexSize = 7 * 8 * sizeof(float);
	//visi.vertexCount = 7 * 8;
	visi.indexSize = 6 * 6 * sizeof(unsigned int);
	//visi.indexCount = 6 * 6;
	visi.vertices = &cubeVerts[0];
	visi.indices = &cubeFaces[0];*/

	//Shader
	ExplicitShader shader = ExplicitShader();
	//shader.bind();
	visi.setShader(&shader);

	//Vertex array object
	GLvao vao = GLvao();//GLuint vao;
	//glGenVertexArrays(1, &vao);
	vao.bind();//glBindVertexArray(vao);
	VERIFY("main");		//DEBUG
	if (!visi.load(cubeVerts, 7 * 8, cubeFaces, 6 * 6, &vao, &shader))
		LOGERROR("loading mesh failed");
		//std::cout << "loading mesh failed" << std::endl;

	//Vertex Buffer
	//GLuint buf[2] = { 0, 0 };	//[0]: vertex buffer, [1]: index buffer
	//GLuint& vertexBuffer = buf[0];
	//glGenBuffers(2, buf);
	//std::cout << buf[0] << " = " << vertexBuffer << ", " << buf[1] << std::endl;	//debug
	//if (!checkError("main", "Visual.cpp", 45)) return;	//
	////by using glNamedBufferData instead of glBufferData, I can select which buffer ID to create the buffer for, so I can bind it later! (WRONG)
	//	//this is not the case, sort of. I think it allows you to upload data without binding, yes. But they need to have been bound at least once in order to properly "exist"!
	//glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	//glNamedBufferData(vertexBuffer, sizeof(visi.cubeVerts), visi.cubeVerts, GL_STATIC_DRAW);
	//if (!checkError("main", "Visual.cpp", 50)) return;	//

	//Vertex Attribute Array
	//shader.prep(visi, vao);
	VERIFY("main");		//DEBUG

	//Index Buffer
	//GLuint indexBuffer;// = buf[1];
	//glGenBuffers(1, &indexBuffer);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	//glNamedBufferData(indexBuffer, visi.indexSize, visi.indices, GL_STATIC_DRAW); //sizeof(visi.cubeFaces), &visi.cubeFaces, GL_STATIC_DRAW);
	//if (!checkError("main", "Visual.cpp", 61)) return;


	//Uniforms
	//TRY A PROJECTION MATRIX, MAYBE THAT'LL FIX THE "BACKWARDS" ISSUE!
	visi.shader->bind();	//remove when uniforms are moved into shader
	//GLint modelMatrixLocation = glGetUniformLocation(shader.getID(), "modelMatrix");
	//std::cout << "modelMatrixLocation handle = " << modelMatrixLocation << std::endl;
	//glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, visi.getMatrixArray());

	//View matrix
	//int winWid, winHei;
	//glfwGetWindowSize(window, &winWid, &winHei);
	//glm::mat4 viewMatrix = glm::perspective(45.0f, (GLfloat)winWid / (GLfloat)winHei, 0.1f, 1000.0f);

	//Keybindings setup
	Input::initialize(window,"exampleKeyBinding.txt");
	//Input::ButtonTicket* cameraUp = Input::getButtonTicket(1);

	//cameraman setup
	Cameraman::initialize(window);
	DebugCamera cam0 = DebugCamera(0.0f);
	//Camera cam1 = Camera("main camera");
	//Camera cam2 = Camera("superior camera");
	//Camera cam3 = Camera("camera fo' real");
	Camera* mainCam = Cameraman::getCameraByName("Debug Camera");	//if you already have a refernce to the camera, this is obviously not ideal. Just for testing here.
	Cameraman::setActiveCamera(mainCam);
	cam0.setPosition(glm::vec3(-2.5f, -0.5f, 0.0f));


	//wireframe?
	//glPolygonMode(GL_FRONT, GL_LINE);	//causes GL error
	//DEBUG, REPLACE WITH SIMPLE POLYGON
	


	//visi.setRotattion(glm::vec3(0.0f, glm::pi<float>() / 6.0f, 0.0f));
	//visi.modRotation(glm::vec3(glm::pi<float>() / 6.0f, 0.0f, 0.0f));		//rotate a bit
	visi.modPosition(glm::vec3(0.0f, 0.0f, -3.0f));	//move further away, too close with out projection matrix
	visi.setScale(glm::vec3(0.3f, 0.3f, 0.3f));		//scale down a bunch, to avoid overlapping cubes (10x10 setup)
	//draw
	while (!glfwWindowShouldClose(window))
	{
		//std::cout << "---Let's GOOO!---" << std::endl;			//DEBUG
		//something is wrrong, error 1285 (out of memory?) line 190, I dunno, fix it please
		VERIFY("main");		//DEBUG

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		visi.modRotation(glm::vec3(0.01f, 0.01f, 0.01f));		//rotate a bit
		visi.multScale(glm::vec3(0.9995f, 0.9995f, 0.9995f));		//scale down a bit
		//cam1.modPosition(glm::vec3(0.001f, 0.0002f, 0.0f));		//move camera up (a little) right (a lot)

		//button-stuff
		/*if (Input::active(cameraUp))
		{
			//TODO
			LOG("MOVE CAMERA UP!");
		}*/
		cam0.update(1.0f);	//argument should be delta time, but I can't be bothered atm

		//const GLfloat* mvpMatrix;
		float x, y;
		for (int i = 0; i < 50; i++)
		{
			x = (i / 9.0f) * 3.0f - 1.5f;
			for (int j = 0; j < 50; j++)
			{
				y = (j / 9.0f) * 3.0f - 1.5f;
				visi.setPosition(glm::vec3(x, y, -3.0f));
				//mvpMatrix = glm::value_ptr(viewMatrix * *visi.getMatrix());	//get matrix
				//glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, mvpMatrix);	//register changes (send to shader)
				//glDrawElements(GL_TRIANGLES, 3 * 12, GL_UNSIGNED_INT, 0);	//last argument is an offset into bound index buffer, not the index buffer itself!
				if (!visi.draw(/*&viewMatrix,*/ &vao))
					LOGERROR("drawing failed");
					//std::cout << "drawing failed, shader probably not set!" << std::endl;
			}
		}


		//std::cout << "---Hey, I made it!---" << std::endl;			//DEBUG
		//glDrawArrays(GL_TRIANGLES, 0, 3);
		glfwSwapBuffers(window);		//error here???
		VERIFY("main");			//DEBUG
		Input::resetKeys();
		glfwPollEvents();
	}

	//vao needs to be deleted with glDeleteVertexArrays
}