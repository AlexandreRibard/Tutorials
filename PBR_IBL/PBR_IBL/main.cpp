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
//	glEnable(GL_CULL_FACE); // enable face culling
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
	OpenGLEngine::Shader skyboxShader("skybox.vert", "skybox.frag");


	/////////////////////////////
	// UNIFORMS
	/////////////////////////////
	OpenGLEngine::f3vUniform vLight;
	vLight.name = "lightPos";
	vLight.value = glm::vec3(0.0);
	vLight.type = "f3v";

	OpenGLEngine::iUniform importanceSampling;
	importanceSampling.name = "importanceSampling";
	importanceSampling.value = 0;
	importanceSampling.type = "i";


	/////////////////////////////
	// CUBEMAP
	/////////////////////////////
	// Load CubeMap & create OpenGL texture
	std::vector<std::string> textures_faces;
	std::string cube_mapPath = "Resources/Textures/Langholmen2/";
	//cube_mapPath = "Resources/Textures/NissiBeach/";
	//cube_mapPath = "Resources/Textures/SaintLazarusChurch/";

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

	/////////////////////////////
	// TEXTURES
	/////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////
	// BRDF DIffuse - Spherical Harmonics Computation:
	//	"An Efficient Representation for Irradiance Environment Maps // Ravi Ramamoorthi & Pat Hanrahan"
	// cf: https://cseweb.ucsd.edu/~ravir/papers/envmap/envmap.pdf
	////////////////////////
	float SH_COEFFS[9][3] = { 0 };

	// convol cubemap and compute SH9 coresponding coefficients
	OpenGLEngine::textureClient::IBLDiffuse_Lambert_SHCoeffs(&SH_COEFFS, &textures_faces);
	std::vector<glm::vec3> sh_Kernel;
	for (size_t i = 0; i < 9; i++)
	{
		glm::vec3 coeffs_rgb;
		coeffs_rgb.x = SH_COEFFS[i][0];
		coeffs_rgb.y = SH_COEFFS[i][1];
		coeffs_rgb.z = SH_COEFFS[i][2];

		sh_Kernel.push_back(coeffs_rgb);

	}

	// Pass them to custom uniform
	OpenGLEngine::af3vUniform sphericalHarmonics_Coeff;
	sphericalHarmonics_Coeff.name = "sphericalHarmonics_Coeff";
	sphericalHarmonics_Coeff.value = sh_Kernel;
	sphericalHarmonics_Coeff.type = "af3v";

	////////////////////////////////////////////////////////////////////////////////////////////////


	// Quad covering the whole screen
	OpenGLEngine::Geometry screenQuadGeometry("ScreenGeometry", 1.0, glm::vec3(0.0, 0.0, 0.0));


	////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////
	// ENVMAP CONVOLUTION
	// "Real Shading in Unreal Engine 4 // Brian Karis"
	// cf: https://de45xmedrsdbp.cloudfront.net/Resources/files/2013SiggraphPresentationsNotes-26915738.pdf
	////////////////////////
	// split-sum approximation: s = 1/N S_{k=1}^N L_i(lk) brdf(lk,v) cos(theta_lk) / p(lk,v)
	//							  ~ (1/N S_{k=1}^N L_i(lk) )  * (1/N S_{k=1}^N brdf(lk,v) cos(theta_lk) / p(lk,v) ) 
	// pre-compute first sum 1/N S_{k=1}^N L_i(lk) for different rougness values and store result in mip-map
	//	N.B: brdf being GGX distribution, the shading changes depending on the viewing angle. We thus assume that this angle is 0, n = v = r
	OpenGLEngine::Shader envMapConvolBRDFGenShader("envMapConvol.vert", "envMapConvol.frag");


	OpenGLEngine::fUniform uRoughness;
	uRoughness.name = "uRoughness";
	uRoughness.value = 0.0;
	uRoughness.type = "f";


	size_t cubemap_dim = 256;
	size_t width = 4 * cubemap_dim;
	size_t height = 3 * cubemap_dim;


	size_t max_mipmap_level = 9;

	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, max_mipmap_level - 1);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Store different blurred env map for each roughness level
	// as minmap level increase, roughness also increases as well

	glDisable(GL_DEPTH_TEST); // We don't care about depth information when rendering a single quad

	for (size_t i = 0; i < max_mipmap_level; i++)
	{
		OpenGLEngine::f2vUniform uInverseResolution;
		uInverseResolution.name = "uInverseResolution";
		uInverseResolution.value = glm::vec2(1.0 / float(width), 1.0 / float(height));
		uInverseResolution.type = "f2v";


		OpenGLEngine::FBO brdfEnvMapGenPassFBO;
		brdfEnvMapGenPassFBO.addColorRenderTarget(GL_RGB32F, width, height, GL_RGB, GL_FLOAT);
		brdfEnvMapGenPassFBO.setColorAttachments();

		brdfEnvMapGenPassFBO.bindFBO();

		// Clear all relevant buffers
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		envMapConvolBRDFGenShader.Use();

		float roughnessValue = static_cast<float>(i) / static_cast<float>(max_mipmap_level);
		uRoughness.updateValue(roughnessValue);
		uRoughness.linkUniform(&envMapConvolBRDFGenShader);
		uInverseResolution.linkUniform(&envMapConvolBRDFGenShader);
		envMap.bindTexture(1, &envMapConvolBRDFGenShader);


		// draw quad
		screenQuadGeometry.draw();

		// Syncronyse
		glFinish();

#ifdef DEBUG_SAVE_GEN_DATA
		std::vector< unsigned char > mipmap_save(3 * width * height);
		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, mipmap_save.data());
		std::string img_path = "Gen_Data/mipmap" + std::to_string(i) + ".bmp";
		SOIL_save_image(img_path.c_str(), SOIL_SAVE_TYPE_BMP, width, height, 3, mipmap_save.data());
#endif

		std::vector< float > mipmap(3 * width * height);
		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glReadPixels(0, 0, width, height, GL_RGB, GL_FLOAT, mipmap.data());
		

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, i, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, mipmap.data());
		glBindTexture(GL_TEXTURE_2D, 0);


		width = std::max(static_cast<size_t>(1), static_cast<size_t>(width / 2));
		height = std::max(static_cast<size_t>(1), static_cast<size_t>(height / 2));

		glFinish();
		brdfEnvMapGenPassFBO.unbindFBO();
	}

	glEnable(GL_DEPTH_TEST); // reset depth testing

	OpenGLEngine::Texture2D EnvBRDF1stSum;
	EnvBRDF1stSum.ID = textureID;
	EnvBRDF1stSum.name = "IBLequirectangularEnvMap";
	EnvBRDF1stSum.type = "sampler2D";

	////////////////////////////////////////////////////////////////////////////////////////////////



	////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////
	// BRDF LUT Generation
	// "Real Shading in Unreal Engine 4 // Brian Karis"
	// cf: https://de45xmedrsdbp.cloudfront.net/Resources/files/2013SiggraphPresentationsNotes-26915738.pdf
	////////////////////////
	// split-sum approximation: s = 1/N S_{k=1}^N L_i(lk) brdf(lk,v) cos(theta_lk) / p(lk,v)
	//							  ~ (1/N S_{k=1}^N L_i(lk) )  * (1/N S_{k=1}^N brdf(lk,v) cos(theta_lk) / p(lk,v) ) 
	// pre-compute second sum (1/N S_{k=1}^N brdf(lk,v) cos(theta_lk) / p(lk,v) ) for different rougness values & cos(theta_v) and store result in mip-map
	OpenGLEngine::Shader brdfLUTGenShader("brdfLUT.vert", "brdfLUT.frag");


	////////////////////////
	// Initilalize LUT
	////////////////////////
	size_t LUTwidth = 256;
	size_t LUTheight = 256;
	GLuint LUTtextureID;
	glGenTextures(1, &LUTtextureID);
	glBindTexture(GL_TEXTURE_2D, LUTtextureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glBindTexture(GL_TEXTURE_2D, 0);

	OpenGLEngine::f2vUniform uInverseResolution;
	uInverseResolution.name = "uInverseResolution";
	uInverseResolution.value = glm::vec2(1.0 / float(LUTwidth), 1.0 / float(LUTheight));
	uInverseResolution.type = "f2v";

	////////////////////////
	// Render BRDF-IBL sampling LUT (2nd sum)
	////////////////////////
	glDisable(GL_DEPTH_TEST); // We don't care about depth information when rendering a single quad


	OpenGLEngine::FBO brdfLUTGenPassFBO;
	brdfLUTGenPassFBO.addColorRenderTarget(GL_RG32F, LUTwidth, LUTheight, GL_RG, GL_FLOAT);
	brdfLUTGenPassFBO.setColorAttachments();

	brdfLUTGenPassFBO.bindFBO();

	brdfLUTGenShader.Use();

	uInverseResolution.linkUniform(&brdfLUTGenShader);

	// draw quad
	screenQuadGeometry.draw();

	glEnable(GL_DEPTH_TEST); // reset depth testing


	// Syncronyse
	glFinish();


	////////////////////////
	// Build texture from FBO
	////////////////////////

	std::vector< float > mipmap(2 * LUTwidth * LUTheight);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glReadPixels(0, 0, LUTwidth, LUTheight, GL_RG, GL_FLOAT, mipmap.data());

#ifdef DEBUG_SAVE_GEN_DATA
	std::vector< unsigned char > mipmap_save(2 * LUTwidth * LUTheight);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glReadPixels(0, 0, LUTwidth, LUTheight, GL_RG, GL_UNSIGNED_BYTE, mipmap_save.data());
	std::string img_path = "Gen_Data/2ndSum.bmp";
	SOIL_save_image(img_path.c_str(), SOIL_SAVE_TYPE_BMP, LUTwidth, LUTheight, 2, mipmap_save.data());
#endif

	glBindTexture(GL_TEXTURE_2D, LUTtextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, LUTwidth, LUTheight, 0, GL_RG, GL_FLOAT, mipmap.data());
	glBindTexture(GL_TEXTURE_2D, 0);
	glFinish();
	brdfLUTGenPassFBO.unbindFBO();



	OpenGLEngine::Texture2D EnvBRDF2ndSum;
	EnvBRDF2ndSum.ID = LUTtextureID;
	EnvBRDF2ndSum.name = "IntegrateBRDF";
	EnvBRDF2ndSum.type = "sampler2D";
	////////////////////////////////////////////////////////////////////////////////////////////////




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
	mesh2_geometry.setWorldSpacePosition(glm::vec3(-5.5, -2.5 + y_translate, 3.0));

	OpenGLEngine::Geometry mesh3_geometry("Resources/Models/dragon-xyz-rgb-scan.obj", meshPos, 1.0);
	mesh3_geometry.setWorldSpacePosition(glm::vec3(0.5, -2.0 + y_translate, -8));

	OpenGLEngine::Geometry plane_geometry("PlaneGeometry", 20.0, glm::vec3(0.0, -2.0 + y_translate, 0.0));


	/////////////////////////////
	// MATERIAL
	/////////////////////////////
	std::vector<OpenGLEngine::Uniform *> uniformVec = { &sphericalHarmonics_Coeff, &importanceSampling };
	std::vector<OpenGLEngine::Texture *> textureVec = { &envMap, &EnvBRDF2ndSum, &EnvBRDF1stSum };
	OpenGLEngine::Material pbrPassMaterial(&textureVec, &uniformVec, &pbrShader);

	/////////////////////////////
	// MESH
	/////////////////////////////
	// PBR-Rendering
	OpenGLEngine::Mesh clumbsy_dragon(&mesh_geometry, &pbrPassMaterial);
	OpenGLEngine::Mesh standford_dragon(&mesh2_geometry, &pbrPassMaterial);
	OpenGLEngine::Mesh xyz_dragon(&mesh3_geometry, &pbrPassMaterial);
	OpenGLEngine::Mesh plane(&plane_geometry, &pbrPassMaterial);


	// Append uniforms afterwards
	// Fresnel
	OpenGLEngine::f3vUniform goldF0;
	goldF0.name = "uF_0";
	goldF0.value = glm::vec3(1.0, 0.71, 0.29); // gold
	goldF0.type = "f3v";

	OpenGLEngine::f3vUniform copperF0;
	copperF0.name = "uF_0";
	copperF0.value = glm::vec3(0.95, 0.64, 0.54); // copper
	copperF0.type = "f3v";

	OpenGLEngine::f3vUniform brassF0;
	brassF0.name = "uF_0";
	brassF0.value = glm::vec3(250.0 / 255.0, 230.0 / 255.0, 150.0 / 255.0); // brass
	brassF0.type = "f3v";

	OpenGLEngine::f3vUniform silverF0;
	silverF0.name = "uF_0";
	silverF0.value = glm::vec3(0.95, 0.93, 0.88); // silver
	silverF0.type = "f3v";

	clumbsy_dragon.getMaterial()->addUniform(&goldF0);
	standford_dragon.getMaterial()->addUniform(&copperF0);
	xyz_dragon.getMaterial()->addUniform(&silverF0);
	plane.getMaterial()->addUniform(&brassF0);

	// Roughness
	OpenGLEngine::fUniform rough;
	rough.name = "uRoughness";
	rough.value = 0.8;
	rough.type = "f";

	OpenGLEngine::fUniform normal;
	normal.name = "uRoughness";
	normal.value = 0.25;
	normal.type = "f";

	OpenGLEngine::fUniform smooth;
	smooth.name = "uRoughness";
	smooth.value = 0.1;
	smooth.type = "f";

	clumbsy_dragon.getMaterial()->addUniform(&smooth);
	standford_dragon.getMaterial()->addUniform(&normal);
	xyz_dragon.getMaterial()->addUniform(&rough);
	plane.getMaterial()->addUniform(&rough);

	// uMaterialMetalness
	OpenGLEngine::fUniform metal;
	metal.name = "uMaterialMetalness";
	metal.value = 0.9;
	metal.type = "f";

	OpenGLEngine::fUniform dielectric;
	dielectric.name = "uMaterialMetalness";
	dielectric.value = 0.2;
	dielectric.type = "f";

	clumbsy_dragon.getMaterial()->addUniform(&metal);
	standford_dragon.getMaterial()->addUniform(&metal);
	xyz_dragon.getMaterial()->addUniform(&metal);
	plane.getMaterial()->addUniform(&dielectric);


	// Skybox
	OpenGLEngine::Geometry skybox_geometry("CubeGeometry", 90.0, glm::vec3(0.0, 0.0, 0.0));
	std::vector<OpenGLEngine::Texture *> skyboxTex = { &envMap };
	OpenGLEngine::Material skybox_material(&skyboxTex, &std::vector<OpenGLEngine::Uniform *>(), &skyboxShader);
	OpenGLEngine::Mesh skybox(&skybox_geometry, &skybox_material);


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
	scene.addMesh(&standford_dragon); 
	scene.addMesh(&xyz_dragon);
	scene.addMesh(&plane);


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
		
		pbrShader.Use();
		float costheta = cos(0.3*timeValue);
		float sintheta = sin(0.3*timeValue);
		float r = 3.0;
		glm::vec3 o(0.0, 1.0, 2.0);
		glm::vec3 lightPos = glm::vec3(o.x + r*costheta, o.y + r*sintheta, o.z);

		vLight.updateValue(lightPos);
		vLight.linkUniform(&pbrShader);

		// 1st render pass: draw object as normal and fill stencil buffer
		scene.drawMeshes(&camera, &window);

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