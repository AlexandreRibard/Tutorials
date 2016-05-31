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

float lerp(float a, float b, float w)
{
	return a + w*(b - a);
}


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
	// Stencil-Buffer
	//glEnable(GL_STENCIL_TEST);
	//glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	//glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	// face culling
	glEnable(GL_CULL_FACE); // enable face culling
	glCullFace(GL_BACK); //GL_BACK: Culls only the back faces.
	glFrontFace(GL_CCW);

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
	OpenGLEngine::Shader geometryPassShader("geometryPass.vert", "geometryPass.frag");
	OpenGLEngine::Shader ssaoShader("ssao.vert", "ssao.frag");
	OpenGLEngine::Shader blurPassShader("blur.vert", "blur.frag");


	/////////////////////////////
	// UNIFORMS
	/////////////////////////////
	// SSAO Sampling Kernel: http://john-chapman-graphics.blogspot.fr/2013/01/ssao-tutorial.html
	//		=> used for sampling depth buffer for AO computation (https://en.wikipedia.org/wiki/Screen_space_ambient_occlusion)
	//	Instead of using a spherical sample kernel, we use a normal-oriented hemisphere
	
	// sampling direction in hemisphere oriented in the z direction, coalesed around z axis
	std::vector<glm::vec3> ssaoKernel;
	// seed srand 
	srand(time(NULL));
	size_t kernelSize = 16;
	for (size_t i = 0; i < kernelSize; ++i)
	{
		// random points in unit sphere
		float rndX = ((float)rand() / (RAND_MAX));
		float rndY = ((float)rand() / (RAND_MAX));
		float rndZ = ((float)rand() / (RAND_MAX));

		// unit hemisphere (z-direction)
		glm::vec3 sample(2.0*rndX - 1.0, 2.0*rndY - 1.0,rndZ);
		sample = glm::normalize(sample);

		// uniform scale in hemisphere
		sample *= ((float)rand() / (RAND_MAX));

		// we group points near z-direction.
		// we want points to be more distributed around z-axis and sparse as thurther away you get from axis
		float scale = static_cast<float>(i) / static_cast<float>(kernelSize);
		scale = lerp(0.1, 1.0, scale*scale);
		sample *= scale;

		ssaoKernel.push_back(sample);
	}
	// create uniform (to be passed to shader when rendering)
	OpenGLEngine::af3vUniform samples;
	samples.name = "samples";
	samples.value = ssaoKernel;
	samples.type = "af3v";


	/////////////////////////////
	// TEXTURES
	/////////////////////////////
	// noise texture
	// will be used for tilting sample kernel
	// => reduces banding effects
	std::vector<glm::vec3> ssaoNoise;
	for (size_t i = 0; i < 16; i++)
	{	
		// random orientation around z axis
		float rndX = ((float)rand() / (RAND_MAX));
		float rndY = ((float)rand() / (RAND_MAX));
		glm::vec3 noise(2.0*rndX - 1.0, 2.0*rndY - 1.0, 0.0f);

		ssaoNoise.push_back(noise);
	}
	// generate texture (to be used down the line)
	GLuint noiseTexture = OpenGLEngine::textureClient::generateTexture(&ssaoNoise);
	// texture uniform
	OpenGLEngine::Texture2D tex_noise;
	tex_noise.ID = noiseTexture;
	tex_noise.name = "noiseTexture";
	tex_noise.type = "sampler2D";

	// noise scale (repectively to window width)
	OpenGLEngine::f2vUniform uNoiseScale;
	uNoiseScale.name = "uNoiseScale";
	uNoiseScale.value = glm::vec2(window.getWidth() / sqrt(ssaoNoise.size()), window.getHeight() / sqrt(ssaoNoise.size()));
	uNoiseScale.type = "f2v";



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
	float y_translate = -2.0;
	glm::vec3 meshPos(2.0, 0.0 + y_translate, 0.0);
	OpenGLEngine::Geometry cube_geometry("CubeGeometry", 2.0, meshPos);

	meshPos = glm::vec3(0.0, -3.0 + y_translate, 0.0);
	OpenGLEngine::Geometry mesh_geometry("Resources/Models/clumsy-dragon.obj", meshPos, 5.0);

	OpenGLEngine::Geometry mesh2_geometry("Resources/Models/stanford-dragon.obj", meshPos, 1.0);
	mesh2_geometry.setWorldSpacePosition(glm::vec3(-5.5, -2.0 + y_translate, 0.0));

	OpenGLEngine::Geometry plane_geometry("PlaneGeometry", 20.0, glm::vec3(0.0, -2.0 + y_translate, 0.0));


	/////////////////////////////
	// MATERIAL
	/////////////////////////////
	OpenGLEngine::Material geometryPassMaterial(&std::vector<OpenGLEngine::Texture *>(), &std::vector<OpenGLEngine::Uniform *>(), &geometryPassShader);

	/////////////////////////////
	// MESH
	/////////////////////////////
	// G-Buffer
	OpenGLEngine::Mesh cube(&cube_geometry, &geometryPassMaterial);
	OpenGLEngine::Mesh clumbsy_dragon(&mesh_geometry, &geometryPassMaterial);
	OpenGLEngine::Mesh standford_dragon_bunny(&mesh2_geometry, &geometryPassMaterial);
	OpenGLEngine::Mesh plane(&plane_geometry, &geometryPassMaterial);


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
	scene.addMesh(&cube);
	scene.addMesh(&clumbsy_dragon);
	scene.addMesh(&standford_dragon_bunny);
	scene.addMesh(&plane);


	////////////////////////
	// 4°/ FBO Setup
	//
	// For multiple pass rendering, the engine offers FBO, RBO wrappers
	//
	// => <OpenGLEngine\frameBuffer.hpp>
	//    <OpenGLEngine\renderBuffer.hpp>
	////////////////////////
	OpenGLEngine::Geometry screenQuadGeometry("ScreenGeometry", 1.0, glm::vec3(0.0, 0.0, 0.0));

	///////////////////
	// G-Buffer
	//	- Position & Depth
	//	- Normal
	//	- Color
	///////////////////
	OpenGLEngine::FBO geometryBufferPassFBO;
	geometryBufferPassFBO.addDepthRenderTarget(GL_DEPTH_COMPONENT32F, window.getWidth(), window.getHeight(), GL_DEPTH_COMPONENT, GL_FLOAT);

	geometryBufferPassFBO.addColorRenderTarget(GL_RGBA32F, window.getWidth(), window.getHeight(), GL_RGBA, GL_FLOAT); 	// position & depth
	geometryBufferPassFBO.addColorRenderTarget(GL_RGB32F, window.getWidth(), window.getHeight(), GL_RGB, GL_FLOAT);		// normal
	geometryBufferPassFBO.addColorRenderTarget(GL_RGB32F, window.getWidth(), window.getHeight(), GL_RGB, GL_FLOAT);		// color
	geometryBufferPassFBO.setColorAttachments();

	///////////////////
	// Blurr pass
	///////////////////
	OpenGLEngine::FBO finalPassFBO;
	finalPassFBO.addDepthRenderTarget(GL_DEPTH_COMPONENT32F, window.getWidth(), window.getHeight(), GL_DEPTH_COMPONENT, GL_FLOAT);
	finalPassFBO.addColorRenderTarget(GL_RGBA32F, window.getWidth(), window.getHeight(), GL_RGBA, GL_FLOAT); // postprod frame
	finalPassFBO.setColorAttachments();


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
		// Multi-Pass rendering:
		// 1° G-Buffer Pass
		// 3° SSAO Pass
		// 2° Blur Pass

		// => G-Buffer Pass
		geometryBufferPassFBO.bindFBO();

		// Clear the colorbuffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);


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

		geometryBufferPassFBO.unbindFBO();

		// => SSAO pass:
		// sample G-Buffer and render scene to quad spaning the whole window
		// Clear all relevant buffers
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST); // We don't care about depth information when rendering a single quad
		
		finalPassFBO.bindFBO();


		ssaoShader.Use();
		// Pass G-Buffer to render target
		geometryBufferPassFBO.bindTextureTargets();
		geometryBufferPassFBO.linkTextureTargets(&std::vector<std::string>{ "G_PositionDepth" , "G_Normal" , "G_Color" }, &ssaoShader);

		// use different shader that current associated material, for this pass
		samples.linkUniform(&ssaoShader);

		// Bind & link uniforms
		uNoiseScale.linkUniform(&ssaoShader);
		tex_noise.bindTexture(3, &ssaoShader);
		scene.linkDefaultUniforms(&ssaoShader, &camera, &window);

		screenQuadGeometry.draw();

		finalPassFBO.unbindFBO();

		// => Blur Pass
		// Bi-Lateral blur or simple Gaussian blur
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		blurPassShader.Use();

		// Active proper texture unit before binding
		finalPassFBO.bindTextureTargets();
		finalPassFBO.linkTextureTargets(&std::vector<std::string>{ "screenTexture" }, &blurPassShader);

		screenQuadGeometry.draw();






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