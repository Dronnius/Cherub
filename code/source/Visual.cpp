#include <glew/glew.h>
#include <glfw/glfw3.h>


#include <iostream> //debug only
#include <Visual.h>
#include <Shader.h>
#include "../source/ExplicitShader.cpp"	//there's gotta be a better way to do this...
#include "../source/TextureOnlyShader.cpp"	//there's gotta be a better way to do this...
#include <Debug.h>
#include <Cameraman.h>
#include <Input.h>

//tools
#include <tools/DebugCamera.h>

//load mesh through pre-made arrays
//arrays are copied, so the originals are safe to be removed
//make sure the object's destructor is called properly
//vertexDataCount asks for the number of floats in the array, not the number of vertices
bool Visual::load(float* verticesInput, unsigned int vertexDataCount, unsigned int* indicesInput, unsigned int indexCount, Shader* shaderInput, GLenum drawMode)
{
	//if (vao == nullptr)	//no vao	
	//	vao = new GLvao();		//should always exist
	//vao->bind();			//should always be bound

	if (shaderInput == nullptr)
	{
		//create default shader here, if desired, fail load for now.
		return false;
	}
	this->shader = shader;

	//update internal values
	this->vertexSize = vertexDataCount * sizeof(float);
	this->indexSize = indexCount * sizeof(unsigned int);
	this->indexCount = indexCount;

	//copy vertex data
	this->vertices = (float*)malloc(this->vertexSize);			//TODO: convert inte new and delete in destructor?
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


bool Visual::draw()
{
	if (this->textureSet)
	{
		this->textureSet->bind();
	}
	if (this->shader == nullptr/* || camera == nullptr*/)
		return false;
	//this->camera = camera;
	this->shader->prep(this);
	//const GLfloat* mvpMatrix = glm::value_ptr(*camera * *this->getMatrix());	//get matrix
	//GLuint uniformHandle = glGetUniformLocation(this->shader->getID(), "modelMatrix");	//ASSUMING ID HAS BEEN SET BY SUPER-CLASS CONSTRUCTOR ALREADY, should be the case...
	//glUniformMatrix4fv(uniformHandle, 1, GL_FALSE, mvpMatrix);	//register changes (send to shader)	(This should be handled by a call to some subclass of Shader (somehow idk)
	if (!CHECK("draw")) return false;
	glDrawElements(GL_TRIANGLES, this->indexCount, GL_UNSIGNED_INT, 0);	//last argument is an offset into bound index buffer, not the index buffer itself!
	if (!CHECK("draw")) return false;
	return true;
}

//constructor
Visual::Visual(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, Transform* parentTransform, Texture* texture)
	: Transform(position, rotation, scale, parentTransform)
{
	this->textureSet = texture;
	if (texture)
	{
		texture->uses++;
	}
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
	char faces[] = "assets/textures/debug/faces.png";
	char* f[1] = { faces };		//TODO: this is inconvenient, provide a simpler constructor for Texture class, maybe fix it up while you're at it?
	Transform parent = Transform(glm::vec3(0.0f, 0.0f, -3.0f), glm::vec3(0.0f), glm::vec3(0.5f, 0.5f, 0.5f));
	Visual visi = Visual(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f), &parent, new Texture(1u, f));

	// Vertex data [EXAMPLE DATA]
	/*float cubeVerts[7 * 8] =			//explicit version
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
	};*/
	//TODO: 
	// Textures fuzzy at shallow angles, why? mip-mapping?
	 
	//position coordinates
	//texture coordinates
	float cubeVerts[(3+2) * 14] =		//textured version
	{
		//Front face
		+0.5f, +0.5f, +0.5f,		//0
			2.0f / 3.0f, 0.75f,
		//	0.0f,
		+0.5f, +0.5f, -0.5f,		//1
			2.0f / 3.0f, 0.5f,
		//	0.0f,
		+0.5f, -0.5f, +0.5f,		//2
			1.0f / 3.0f, 0.75f,
		//	0.0f,
		+0.5f, -0.5f, -0.5f,		//3
			1.0f / 3.0f, 0.5f,
		//	0.0f,

		//back face
		-0.5f, +0.5f, +0.5f,		//4			
			2.0f / 3.0f, 0.0f,
		//	1.0f,
		-0.5f, +0.5f, -0.5f,		//5
			2.0f / 3.0f, 0.25f,
		//	1.0f,
		-0.5f, -0.5f, +0.5f,		//6
			1.0f / 3.0f, 0.0f,
		//	1.0f,
		-0.5f, -0.5f, -0.5f,		//7
			1.0f / 3.0f, 0.25f,
		//	1.0f,

		//Right face
	//	+0.5f, +0.5f, +0.5f,		//8 0 copy
	//		1.0f, 1.0f,
	//		0.0f,
	//	+0.5f, +0.5f, -0.5f,		//9 1 copy
	//		0.0f, 1.0f,
	//		0.0f,
		-0.5f, +0.5f, +0.5f,		//10 4 copy	(8)
			1.0f, 0.75f,
		//	0.0f,
		-0.5f, +0.5f, -0.5f,		//11 5 copy	(9)
			1.0f, 0.5f,
		//	0.0f,

		//left face
	//	+0.5f, -0.5f, +0.5f,		//12 2 copy
	//		1.0f, 1.0f,
	//		1.0f,
	//	+0.5f, -0.5f, -0.5f,		//13 3 copy
	//		0.0f, 1.0f,
	//		1.0f,
		-0.5f, -0.5f, +0.5f,		//14 6 copy	(10)
			0.0f, 0.75f,
		//	1.0f,
		-0.5f, -0.5f, -0.5f,		//15 7 copy	(11)
			0.0f, 0.5f,
		//	1.0f,

		//Top face
	//	+0.5f, +0.5f, +0.5f,		//16 0 copy
	//		1.0f, 1.0f,
	//		0.0f,
	//	+0.5f, -0.5f, +0.5f,		//17 2 copy
	//		1.0f, 0.0f,
	//		0.0f,
		-0.5f, +0.5f, +0.5f,		//18 4 copy	(12)
			2.0f / 3.0f, 1.0f,
		//	0.0f,
		-0.5f, -0.5f, +0.5f,		//19 6 copy	(13)
			1.0f / 3.0f, 1.0f,
		//	0.0f,

		//Bottom face
	//	+0.5f, +0.5f, -0.5f,		//20 1 copy
	//		1.0f, 1.0f,
	//		1.0f,
	//	+0.5f, -0.5f, -0.5f,		//21 3 copy
	//		1.0f, 0.0f,
	//		1.0f,
	//	-0.5f, +0.5f, -0.5f,		//22 5 copy
	//		0.0f, 1.0f,
	//		1.0f,
	//	-0.5f, -0.5f, -0.5f,		//23 7 copy
	//		0.0f, 0.0f,
	//		1.0f,
	};
	// Index data [EXAMPLE DATA]
	unsigned int cubeFaces[6 * 6] =
	{
		0, 2, 3,		//Front face
		0, 3, 1,		// x = 0.5f

		4, 5, 6,		//Back face
		5, 7, 6,		// x = -0.5f

		0, 1, 8,		//Right face
		1, 9, 8,		// y = 0.5f

		2, 10, 11, 		//Left face
		2, 11, 3, 		// y = -0.5f

		0, 13, 2,		//Top face
		0, 12, 13,		// z = 0.5f

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
	//ExplicitShader shader = ExplicitShader();
	TextureOnlyShader shader = TextureOnlyShader();
	//shader.bind();
	visi.setShader(&shader);

	//Vertex array object
	//GLvao vao = GLvao();//GLuint vao;
	//glGenVertexArrays(1, &vao);
	//vao.bind();//glBindVertexArray(vao);
	VERIFY("main");		//DEBUG
	if (!visi.load(cubeVerts, 5 * 14, cubeFaces, 6 * 6, &shader))
		LOGERROR("loading mesh failed");
		//std::cout << "loading mesh failed" << std::endl;


	//Vertex Attribute Array
	//shader.prep(visi, vao);
	VERIFY("main");		//DEBUG

	//Uniforms
	//TRY A PROJECTION MATRIX, MAYBE THAT'LL FIX THE "BACKWARDS" ISSUE!
	visi.shader->bind();

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
	cam0.setPosition(glm::vec3(0.0f, 0.0f, -1.0f));

	//wireframe?
	//glPolygonMode(GL_FRONT, GL_LINE);	//causes GL error
	//DEBUG, REPLACE WITH SIMPLE POLYGON
	


	//visi.setRotation(glm::vec3(0.0f, glm::pi<float>() / 6.0f, 0.0f));
	//visi.modRotation(glm::vec3(glm::pi<float>() / 6.0f, 0.0f, 0.0f));		//rotate a bit
	//visi.modPosition(glm::vec3(0.0f, 0.0f, -6.0f));	//move further away, too close with out projection matrix
	visi.setScale(glm::vec3(0.3f, 0.3f, 0.3f));		//scale down a bunch, to avoid overlapping cubes (10x10 setup)
	//draw
	while (!glfwWindowShouldClose(window))
	{
		//std::cout << "---Let's GOOO!---" << std::endl;			//DEBUG
		//something is wrong, error 1285 (out of memory?) line 190, I dunno, fix it please
		VERIFY("main");		//DEBUG

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		visi.modRotation(glm::vec3(1.0f, 1.0f, 1.0f), glm::pi<float>() * 0.005f);		//rotate a bit
		//visi.modScale(glm::vec3(0.9995f, 0.9995f, 0.9995f));		//scale down a bit
		//cam1.modPosition(glm::vec3(0.001f, 0.0002f, 0.0f));		//move camera up (a little) right (a lot)

		//button-stuff
		/*if (Input::active(cameraUp))
		{
			//TODO
			LOG("MOVE CAMERA UP!");
		}*/

		//TRANFORM PLAYGROUND CODE

		//SPINNING CUBES CODE//
		cam0.update(1.0f);	//argument should be delta time, but I can't be bothered atm
		
		//const GLfloat* mvpMatrix;
		//float x, y;
		//for (int i = 0; i < 10; i++)
		//{
		//	x = (i / 9.0f) * 3.0f - 1.5f;
		//	for (int j = 0; j < 10; j++)
		//	{
		//		y = (j / 9.0f) * 3.0f - 1.5f;
		//		visi.setPosition(glm::vec3(x, y, 0.0f));
		//		//const float* mvpMatrix = glm::value_ptr(*Cameraman::film() * *visi.getWorldMatrix());	//get matrix
		//		//glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, mvpMatrix);	//register changes (send to shader)
		//		//glDrawElements(GL_TRIANGLES, 3 * 12, GL_UNSIGNED_INT, 0);	//last argument is an offset into bound index buffer, not the index buffer itself!
		//		if (!visi.draw())
		//			LOGERROR("drawing failed");
		//			//std::cout << "drawing failed, shader probably not set!" << std::endl;
		//	}
		//}
		void putHypeBox(Visual* v, int x, int y, int depth);
		bool map[12][5] = { 
			{true,	true,	true,	true,	true}, 
			{false,	false,	true,	false,	false},
			{true,	true,	true,	true,	true}, 
			{false,	false,	false,	false,	true},
			{false,	false,	false,	true,	false},
			{true,	true,	true,	true,	true}, 
			{true,	true,	true,	true,	true}, 
			{false,	false,	true,	false,	true},
			{false,	false,	true,	true,	false},
			{true,	true,	true,	true,	true}, 
			{true,	false,	true,	false,	true},
			{true,	false,	false,	false,	true},
			//{true,	false,	false,	false,	false},
		};
		for(int i = 0; i < 12; i++)
			for (int j = 0; j < 5; j++)
				if(map[i][j])
					putHypeBox(&visi, i, j, 50);


		//std::cout << "---Hey, I made it!---" << std::endl;			//DEBUG
		//glDrawArrays(GL_TRIANGLES, 0, 3);
		glfwSwapBuffers(window);		//error here???
		VERIFY("main");			//DEBUG
		Input::resetKeys();
		glfwPollEvents();
		checkConsoleCommands(0.0f);
	}

	//vao needs to be deleted with glDeleteVertexArrays
}

glm::vec2 hexIndexToPos(glm::ivec2 i)
{
	static const float yMod = glm::sqrt(3.0f) / 2.0f;
	glm::vec2 pos(0.0f);
	pos.x = i.x + 0.5f * i.y;
	pos.x += (i.x / 3) * 0.5f;	//extra offset to separate letters
	pos.y = yMod * i.y;
	return pos;
}

//this is just for fun, delete later
void putHypeBox(Visual* v, int x, int y, int depth = 1)
{
	v->setPosition(glm::vec3(hexIndexToPos(glm::ivec2(x,y)), 0.0f));
	for (int i = 0; i < depth; i++)
	{
		if (!v->draw())
			LOGERROR("drawing " + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(0.0f) + " failed");
		v->modPosition(glm::vec3(0.0f, 0.0f, -5.0f));
	}
	return;
}