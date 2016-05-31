////////////////////////
// GLEW
////////////////////////
#include <GL\glew.h>

////////////////////////
// GLFW
////////////////////////
#include <GLFW\glfw3.h>

////////////////////////
// SOIL
////////////////////////
#include <SOIL\SOIL.h>

////////////////////////
// GLM
////////////////////////
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

////////////////////////
// OpenGL Engine
////////////////////////
#include <OpenGLEngine\windowInterface.hpp> // window wrapper (GLFW window instance)
#include <OpenGLEngine\cameraInterface.hpp> // camera wrapper
#include <OpenGLEngine\textureInterface.hpp> // texture wrapper
#include <OpenGLEngine\uniformInterface.hpp> // uniform wrapper
#include <OpenGLEngine\mesh.hpp> // mesh wrapper
#include <OpenGLEngine\scene.hpp> // scene manager
#include <OpenGLEngine\frameBuffer.hpp> // FBO wrapper
#include <OpenGLEngine\renderBuffer.hpp> // RBO wrapper


////////////////////////
// For most classes and wrappers:
//	- Memory managment is left for the user
//	- Classes are NOT intended to be thread safe
////////////////////////

////////////////////////
// STL
////////////////////////
#include <vector>
#include <time.h> 


////////////////////////
// UTILITIES
////////////////////////
#include "stopWatch.hpp"



int main(int argc, char ** argv)
{
	////////////////////////
	// 1°/ Window creation:
	//			- Create OpenGL Window
	//
	// => <OpenGLEngine\windowInterface.hpp>
	////////////////////////
	size_t WINDOW_WIDTH = OpenGLEngine::window::WINDOW_WIDTH; // default width
	size_t WINDOW_HEIGHT = OpenGLEngine::window::WINDOW_HEIGHT; // default height
	OpenGLEngine::window::Window window(WINDOW_WIDTH, WINDOW_HEIGHT, "My OpenGL Engine");

	////////////////////////
	// 2°/ Default Initialization Setup:
	//			- GLEW/GL specifications
	////////////////////////
	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	glewInit();
	// Define the viewport dimensions
	glViewport(0, 0, window.getWidth(), window.getHeight());

	// Setup OpenGL options
	// Depth-Testing
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// face culling
	//glEnable(GL_CULL_FACE); // enable face culling
	glCullFace(GL_BACK); //GL_BACK: Culls only the back faces.
	glFrontFace(GL_CCW);
	// Stencil-Buffer
	//glEnable(GL_STENCIL_TEST);
	//glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	//glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	// GL Clear Color specification
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	////////////////////////
	// 2°/ Camera creation:
	//			- Create a Camera
	// The camera is located at a given position in world space and is looking at a given point (in world space)
	// From specified FOV and aspect ratio, we build View (World to Camera space) Matrix and Projection (persepective divide) Matrix
	//
	// => <OpenGLEngine\cameraInterface.hpp>
	/////////////////////////
	glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 3.0f);
	glm::vec3 cameraFocus = glm::vec3(0.0f);
	float fov = 70.0;
	OpenGLEngine::camera::Camera camera(window.aspectRatio(), cameraPosition, cameraFocus, fov);


	////////////////////////
	// 3°/ Shader and Uniform loading
	//
	// The engine offers custom OpenGL wrapper classes for Shaders & Uniforms (Textures included) as well as Meshes
	// Memory managment is left to the user.
	// One way to do it is to use custom memory pools for each objects:
	//	std::vector<OpenGLEngine::Shader> or std::unordered_map<std::string, OpenGLEngine::Shader> shaderPool ... 
	// NB: dealocation for each object is done when destructor is called.
	//
	// => Shaders <OpenGLEngine\shaderInterface.hpp>
	// => Uniforms <OpenGLEngine\uniformInterface.hpp>
	// => Textures <OpenGLEngine\textureInterface.hpp>
	////////////////////////

	/////////////////////////////
	// SHADERS
	// NB: No support for tesselation nor geometry shaders in current build
	/////////////////////////////
	OpenGLEngine::Shader skyboxShader("skybox.vert", "skybox.frag");

	OpenGLEngine::Shader waterShader("water.vert", "water.frag");


	/////////////////////////////
	// UNIFORMS
	/////////////////////////////
	OpenGLEngine::fUniform xOffset;
	xOffset.name = "xOffset";
	xOffset.value = 0.2;
	xOffset.type = "f";


	/////////////////////////////
	// TEXTURES
	/////////////////////////////
	std::string texture_path = "Resources/Textures/orangemarble.jpg";
	//texture_path = "Resources/Textures/board.jpg";
	//texture_path = "Resources/Textures/naturalstone.jpg";
	//texture_path = "Resources/Textures/pavment.jpg";
	//texture_path = "Resources/Textures/stone.jpg";
	//texture_path = "Resources/Textures/wood.jpg";



	GLuint crateTexture = OpenGLEngine::textureClient::loadTexture(texture_path);
	OpenGLEngine::Texture2D tex_crate;
	tex_crate.ID = crateTexture;
	tex_crate.name = "wallTexture";
	tex_crate.type = "sampler2D";

	GLuint dUdVMap = OpenGLEngine::textureClient::loadTexture("Resources/Textures/water4DUDVorg.jpg");
	OpenGLEngine::Texture2D waterDUDV;
	waterDUDV.ID = dUdVMap;
	waterDUDV.name = "waterDUDV";
	waterDUDV.type = "sampler2D";

	GLuint NormalMap = OpenGLEngine::textureClient::loadTexture("Resources/Textures/water4DOT3.jpg");
	OpenGLEngine::Texture2D waterNormal;
	waterNormal.ID = NormalMap;
	waterNormal.name = "waterNormal";
	waterNormal.type = "sampler2D";


	/////////////////////////////
	// CUBEMAP
	/////////////////////////////
	// Load CubeMap & create OpenGL texture
	std::vector<std::string> textures_faces;
	std::string cube_mapPath = "Resources/Textures/SaintLazarusChurch/";

	textures_faces.push_back(cube_mapPath + "px.jpg");
	textures_faces.push_back(cube_mapPath + "nx.jpg");
	textures_faces.push_back(cube_mapPath + "py.jpg");
	textures_faces.push_back(cube_mapPath + "ny.jpg");
	textures_faces.push_back(cube_mapPath + "pz.jpg");
	textures_faces.push_back(cube_mapPath + "nz.jpg");
	GLuint cubeMap = OpenGLEngine::textureClient::loadCubeMap(&textures_faces);

	// custom utility texture class
	OpenGLEngine::TextureCube envMap;
	envMap.ID = cubeMap;
	envMap.name = "skybox";
	envMap.type = "samplerCube";


	////////////////////////
	// 3°/ Gemometry Setup
	//
	// The engine offers custom OpenGL wrapper classes for 3D Meshes.
	//		- A mesh is made of a Geometry (i.e: OpenGL VAO & VBO) and  a Material (associated Shader and Uniform & Textures).
	//		- A mesh is not nesisarilly rendered using Material specified at creation, but Shader,Uniform&Texture binding & linking is left to the user
	//		  (for such a case, be sure to only render the mesh gemometry)...
	//		- A mesh has a specified position in world space (=> Model to World Matrix)
	//
	// Memory managment is left to the user.
	// One way to do it is to use custom memory pools for each objects:
	//		std::unordered_map<std::string, OpenGLEngine::Geometry> geometryPool
	//		std::unordered_map<std::string, OpenGLEngine::Material> materialPool
	//		std::unordered_map<std::string, OpenGLEngine::Mesh> meshPool
	// NB: dealocation for each object is done when destructor is called.
	//
	// => <OpenGLEngine\mesh.hpp> (mesh)
	//		- #include <OpenGLEngine\modelGeometry.hpp>
	//		- #include <OpenGLEngine\modelMaterial.hpp>
	////////////////////////

	/////////////////////////////
	// GEOMETRY
	/////////////////////////////
	// Skybox
	OpenGLEngine::Geometry skybox_geometry("CubeGeometry", 90.0, glm::vec3(0.0, 0.0, 0.0));

	// Model
	glm::vec3 meshPos(0.0, 0.0, 0.0);
	OpenGLEngine::Geometry model_geometry("CubeGeometry", 2.0, meshPos);


	/////////////////////////////
	// MATERIAL
	/////////////////////////////
	// Skybox
	std::vector<OpenGLEngine::Texture *> skyboxTex = { &envMap };
	OpenGLEngine::Material skybox_material(&skyboxTex, &std::vector<OpenGLEngine::Uniform *>(), &skyboxShader);

	// Model
	std::vector<OpenGLEngine::Texture *> textureVec = { &tex_crate, &waterDUDV, &waterNormal, &envMap };
	std::vector<OpenGLEngine::Uniform *> uniformVec = { &xOffset };
	OpenGLEngine::Material model_material(&textureVec, &uniformVec, &waterShader);

	/////////////////////////////
	// MESH
	/////////////////////////////
	// Skybox
	OpenGLEngine::Mesh skybox(&skybox_geometry, &skybox_material);

	// Model
	OpenGLEngine::Mesh model(&model_geometry, &model_material);


	////////////////////////
	// 4°/ Scene Setup
	//
	// A scene is just a wrapper to render more effecitenly all the mesh present in the mesh
	//	(ie Matrix linking, controler managment and uniform binding & linking)
	//
	// Memory managment is left to the user. No copies are done
	//
	// => <OpenGLEngine\scene.hpp>
	////////////////////////
	OpenGLEngine::Scene scene;
	scene.addMesh(&model);


	// Create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
	OpenGLEngine::RBO renderBuffer(window.getWidth(), window.getHeight());



	////////////////////////
	// 5°/ Render loop
	//	The render loop is pretty straight forward:
	//		- Loop until window is closed (Esc Key)
	//		- Update key & controler events
	//		- Render scene
	//		- Swap screen buffer and display the window
	//
	////////////////////////

	stopWatch timer; // FPS counter

	// Render loop
	while (window.isOpen())
	{
		timer.start();

		////////////////////////
		//	- Update Events
		////////////////////////
		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		window.updateEvents();
		//window::mouse.inertia();
		window.getControler()->inertia();

		////////////////////////
		//	- Render
		////////////////////////
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);


		// Draw skybox first

		glDepthMask(GL_FALSE);// Remember to turn depth writing off

		// draw the cube inside out
		glFrontFace(GL_CW);
		scene.drawMesh(&skybox, &camera, &window);
		glFrontFace(GL_CCW);

		glDepthMask(GL_TRUE);



		glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		float timeValue = glfwGetTime();
		xOffset.updateValue((sin(0.5*timeValue) / 2) + 0.5);


		////////////////////
		// Render Object
		////////////////////
		// 1st render pass: draw object as normal and fill stencil buffer
		scene.drawMeshes(&camera, &window);

		// Optional
		// 2nd render pass: now draw slightly scaled versions of the objects, this time disabling stencil writing.
		// Because stencil buffer is now filled with several 1s. The parts of the buffer that are 1 are now not drawn, thus only drawing 
		// the objects' size differences, making it look like borders.
		//		scene.outlineMeshes(&stencilShader, &camera, &window);



		// Swap the screen buffers
		window.draw();

		// Syncronyse
		glFinish();


		timer.end();
		double render_time = timer.time();
		std::cout << "1F: " << 1000.0*render_time << "ms" << "," << "FPS: " << 1.0 / render_time << std::endl;
	}

	// Melete meshes
	// Properly de-allocate all resources once they've outlived their purpose
	// => done in mesh desctuctor

	window.isClosed();


	return 0;
}