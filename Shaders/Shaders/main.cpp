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
	// 1�/ Window creation:
	//			- Create OpenGL Window
	//
	// => <OpenGLEngine\windowInterface.hpp>
	////////////////////////
	size_t WINDOW_WIDTH = OpenGLEngine::window::WINDOW_WIDTH; // default width
	size_t WINDOW_HEIGHT = OpenGLEngine::window::WINDOW_HEIGHT; // default height
	OpenGLEngine::window::Window window(WINDOW_WIDTH, WINDOW_HEIGHT, "My OpenGL Engine");

	////////////////////////
	// 2�/ Default Initialization Setup:
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
	// Stencil-Buffer
	//glEnable(GL_STENCIL_TEST);
	//glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	//glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	// face culling
	//glEnable(GL_CULL_FACE); // enable face culling
	glCullFace(GL_BACK); //GL_BACK: Culls only the back faces.
	glFrontFace(GL_CCW);

	// GL Clear Color specification
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	////////////////////////
	// 2�/ Camera creation:
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
	// 3�/ Shader and Uniform loading
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
	OpenGLEngine::Shader pbrShader("pbr.vert", "pbr.frag");
	OpenGLEngine::Shader texturedShader("texture.vert", "texture.frag");


	/////////////////////////////
	// UNIFORMS
	/////////////////////////////
	OpenGLEngine::f3vUniform vLight;
	vLight.name = "lightPos";
	vLight.value = glm::vec3(0.0);
	vLight.type = "f3v";

	/////////////////////////////
	// TEXTURES
	/////////////////////////////
	GLuint wallTexture = OpenGLEngine::textureClient::loadTexture("Resources/Textures/stone.jpg");
	OpenGLEngine::Texture2D tex_wall;
	tex_wall.ID = wallTexture;
	tex_wall.name = "wallTexture";
	tex_wall.type = "sampler2D";


	////////////////////////
	// 3�/ Gemometry Setup
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
	float y_translate = -2.0;
	glm::vec3 meshPos = glm::vec3(0.0, -3.0 + y_translate, 0.0);
	OpenGLEngine::Geometry mesh_geometry("Resources/Models/clumsy-dragon.obj", meshPos, 5.0);

	OpenGLEngine::Geometry mesh2_geometry("Resources/Models/stanford-dragon.obj", meshPos, 1.0);
	mesh2_geometry.setWorldSpacePosition(glm::vec3(-5.5, -2.0 + y_translate, 0.0));

	OpenGLEngine::Geometry mesh3_geometry("Resources/Models/dragon-xyz-rgb-scan.obj", meshPos, 1.0);
	mesh3_geometry.setWorldSpacePosition(glm::vec3(0.5, -2.0 + y_translate, -8));

	OpenGLEngine::Geometry plane_geometry("PlaneGeometry", 40, glm::vec3(0.0, -2.0 + y_translate, 0.0));


	/////////////////////////////
	// MATERIAL
	/////////////////////////////
	std::vector<OpenGLEngine::Uniform *> uniformVec = { &vLight };
	OpenGLEngine::Material geometryPassMaterial(&std::vector<OpenGLEngine::Texture *>(), &uniformVec, &pbrShader);

	std::vector<OpenGLEngine::Texture *> textureVec = { &tex_wall };
	OpenGLEngine::Material textured_material(&textureVec, &std::vector<OpenGLEngine::Uniform *>(), &texturedShader);

	/////////////////////////////
	// MESH
	/////////////////////////////
	OpenGLEngine::Mesh clumbsy_dragon(&mesh_geometry, &geometryPassMaterial);
	OpenGLEngine::Mesh standford_dragon_bunny(&mesh2_geometry, &geometryPassMaterial);
	OpenGLEngine::Mesh xyz_dragon(&mesh3_geometry, &geometryPassMaterial);

	OpenGLEngine::Mesh plane(&plane_geometry, &textured_material);


	////////////////////////
	// 4�/ Scene Setup
	//
	// A scene is just a wrapper to render more effecitenly all the mesh present in the mesh
	//	(ie Matrix linking, controler managment and uniform binding & linking)
	//
	// Memory managment is left to the user. No copies are done
	//
	// => <OpenGLEngine\scene.hpp>
	////////////////////////
	OpenGLEngine::Scene scene;
	scene.addMesh(&clumbsy_dragon);
	scene.addMesh(&standford_dragon_bunny);
	scene.addMesh(&xyz_dragon);
	scene.addMesh(&plane);




	// Create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
	OpenGLEngine::RBO renderBuffer(window.getWidth(), window.getHeight());



	////////////////////////
	// 5�/ Render loop
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


		// Clear the colorbuffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);


		float timeValue = glfwGetTime();
		float costheta = cos(0.3*timeValue);
		float sintheta = sin(0.3*timeValue);
		float r = 3.0;
		glm::vec3 o(0.0, 1.0, 2.0);
		glm::vec3 lightPos = glm::vec3(o.x + r*costheta, o.y + r*sintheta, o.z);

		vLight.updateValue(lightPos);

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