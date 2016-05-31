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


#define DEBUG_SAVE_GEN_DATA



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
	//glEnable(GL_CULL_FACE); // enable face culling
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
	OpenGLEngine::Shader pbrShader("pbr.vert", "pbr.frag");
	OpenGLEngine::Shader shadowMappingShader("shadowMapping.vert", "shadowMapping.frag");


	/////////////////////////////
	// UNIFORMS
	/////////////////////////////
	OpenGLEngine::f3vUniform vLight;
	vLight.name = "lightPos";
	vLight.value = glm::vec3(8 * 2, 7 * 2, -3 * 2);
	vLight.type = "f3v";

	/////////////////////////////
	// TEXTURES
	/////////////////////////////
	GLuint wallTexture = OpenGLEngine::textureClient::loadTexture("Resources/Textures/crakedpaint.jpg");
	OpenGLEngine::Texture2D tex_wall;
	tex_wall.ID = wallTexture;
	tex_wall.name = "wallTexture";
	tex_wall.type = "sampler2D";


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

	std::vector<OpenGLEngine::Texture *> shadowlMap_tex = { &tex_wall };
	OpenGLEngine::Material normalMap_material(&shadowlMap_tex, &std::vector<OpenGLEngine::Uniform *>(), &shadowMappingShader);

	/////////////////////////////
	// MESH
	/////////////////////////////
	OpenGLEngine::Mesh clumbsy_dragon(&mesh_geometry, &geometryPassMaterial);
	OpenGLEngine::Mesh standford_dragon_bunny(&mesh2_geometry, &geometryPassMaterial);
	OpenGLEngine::Mesh xyz_dragon(&mesh3_geometry, &geometryPassMaterial);


	OpenGLEngine::Mesh plane(&plane_geometry, &normalMap_material);


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
	scene.addMesh(&clumbsy_dragon);
	scene.addMesh(&standford_dragon_bunny);
	scene.addMesh(&xyz_dragon);
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

	
	////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////
	// Depth Map rendering
	// "Variance Shadow Maps // William Donnelly, Andrew Lauritzen"
	// "GPU Gems 3 - Chapter 8. Summed-Area Variance Shadow Maps // Andrew Lauritzen"
	// cf: http://www.punkuser.net/vsm/vsm_paper.pdf
	//	   http://http.developer.nvidia.com/GPUGems3/gpugems3_ch08.html
	////////////////////////
	// We render depth map from point of view of the light:
	//	=> r: depth
	//	   g: depth*depth + bias
	// NB: f(x,y) = depth + x*df/dx + y*df/dy
	// M2 = E(f²) = E(depth²) + E(x²)*(df/dx)² + E(y²)*(df/dy)² // E(x) = E(y) = E(xy) = 0
	//		E(depth²) = depth²
	//		E(x²) = sd_pixel² = (1/2)² = 1/4, pixel being a symetrical Gaussian distribution
	// bias = 0.25 * ((df/dx)² + (df/dy)²)
	//
	// Once the variancce shadow map is rendered, we can enable classical texture filtering
	// We perform a Bi-Lateral blur to average out the shadows
	OpenGLEngine::Shader shadowMapShader("depthShader.vert", "depthShader.frag");
	OpenGLEngine::Shader bilateralBlurShader("bilateralBlurShader.vert", "bilateralBlurShader.frag");

	////////////////////////
	// Initilalize Depth Map
	////////////////////////
	size_t ShadowMap_width = window.getWidth();
	size_t ShadowMap_height = window.getHeight();
	GLuint ShadowMap_textureID;
	glGenTextures(1, &ShadowMap_textureID);
	glBindTexture(GL_TEXTURE_2D, ShadowMap_textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glBindTexture(GL_TEXTURE_2D, 0);


	////////////////////////
	// two render pass:
	//		1° light depth map:
	//			=> r: depth
	//			   g: depth²
	//		2° bi-lateral blur on generated shadow map
	////////////////////////
	OpenGLEngine::FBO shadowMap_FBO;
	shadowMap_FBO.addDepthRenderTarget(GL_DEPTH_COMPONENT32F, ShadowMap_width, ShadowMap_height, GL_DEPTH_COMPONENT, GL_FLOAT);
	shadowMap_FBO.addColorRenderTarget(GL_RGB32F, ShadowMap_width, ShadowMap_height, GL_RGB, GL_FLOAT);
	shadowMap_FBO.setColorAttachments();

	OpenGLEngine::FBO biLateralBlur_FBO;
	biLateralBlur_FBO.addColorRenderTarget(GL_RGB32F, ShadowMap_width, ShadowMap_height, GL_RGB, GL_FLOAT);
	biLateralBlur_FBO.setColorAttachments();


	////////////////////////
	// 1st pass: render depth map
	////////////////////////
	shadowMap_FBO.bindFBO();

	// render from light position
	glm::vec3 cameraPos = camera.getCameraPosition();
	camera.moveTo(vLight.value);

	OpenGLEngine::f2vUniform uNearFarPlane;
	uNearFarPlane.name = "uNearFarPlane";
	std::pair<float, float> near_far = camera.getNearFarPlane();
	uNearFarPlane.value = glm::vec2(near_far.first, near_far.second);
	uNearFarPlane.type = "f2v";


	shadowMapShader.Use();
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	// Clear all relevant buffers
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	uNearFarPlane.linkUniform(&shadowMapShader);
	// draw scene (depth only)
	scene.drawMeshes(&camera, &window, &shadowMapShader);


	shadowMap_FBO.unbindFBO();
	
	////////////////////////
	// 2nd pass: blur depth map
	////////////////////////
	biLateralBlur_FBO.bindFBO();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	bilateralBlurShader.Use();

	// Active proper texture unit before binding
	shadowMap_FBO.bindTextureTargets();
	shadowMap_FBO.linkTextureTargets(&std::vector<std::string>{ "frag_depth" }, &bilateralBlurShader);

	// draw quad
	screenQuadGeometry.draw();

	biLateralBlur_FBO.unbindFBO();
	

	// Syncronyse
	glFinish();


	////////////////////////
	// Generate texture from FBO
	////////////////////////

	biLateralBlur_FBO.bindFBO();

	std::vector< float > shadowMap_rawData(3 * ShadowMap_width * ShadowMap_height);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glReadPixels(0, 0, ShadowMap_width, ShadowMap_height, GL_RGB, GL_FLOAT, shadowMap_rawData.data());

#ifdef DEBUG_SAVE_GEN_DATA
	std::vector< unsigned char > shadowMap_save(3 * ShadowMap_width * ShadowMap_height);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glReadPixels(0, 0, ShadowMap_width, ShadowMap_height, GL_RGB, GL_UNSIGNED_BYTE, shadowMap_save.data());
	std::string img_path = "Gen_Data/ShadowMap.bmp";
	SOIL_save_image(img_path.c_str(), SOIL_SAVE_TYPE_BMP, ShadowMap_width, ShadowMap_height, 3, shadowMap_save.data());
#endif

	glBindTexture(GL_TEXTURE_2D, ShadowMap_textureID);


	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, ShadowMap_width, ShadowMap_height, 0, GL_RGB, GL_FLOAT, shadowMap_rawData.data());

	glGenerateTextureMipmap(ShadowMap_textureID);

	glBindTexture(GL_TEXTURE_2D, 0);
	glFinish();

	biLateralBlur_FBO.unbindFBO();

	////////////////////////
	// Declare Texture uniform
	////////////////////////
	OpenGLEngine::Texture2D depthMap;
	depthMap.ID = ShadowMap_textureID;
	depthMap.name = "depthMap";
	depthMap.type = "sampler2D";


	OpenGLEngine::m4fUniform uLightSpaceMatrix;
	uLightSpaceMatrix.name = "uLightSpaceMatrix";
	near_far = camera.getNearFarPlane();
	uLightSpaceMatrix.value = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_far.first, near_far.second) * camera.getViewMatrix();
	uLightSpaceMatrix.type = "m4f";

	plane.addTexture(&depthMap);
	plane.addUniform(&uLightSpaceMatrix);

	// set the camera to its original position
	camera.moveTo(cameraPos);

	// add a cube representing the lights position
	OpenGLEngine::Geometry cube_geometry("CubeGeometry", 1, vLight.value);
	OpenGLEngine::Mesh light_cube(&cube_geometry, &OpenGLEngine::Material());
	scene.addMesh(&light_cube);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////








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