#ifndef SCENE_HPP
#define SCENE_HPP


////////////////////////
// GLM
////////////////////////
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

////////////////////////
// STL
////////////////////////
#include <vector>

////////////////////////
// CUSTOM
////////////////////////
#include "mesh.hpp"
#include "cameraInterface.hpp"
#include "windowInterface.hpp"


namespace OpenGLEngine
{


/**
* \file scene.hpp
* \author Alexandre Ribard
* \date Feb 2016
*/

/** @defgroup SCENE
*
* This module handles rendering and uniform updates and shader linking
*
*/

/** @addtogroup SCENE */
/*@{*/


/*!
*  \brief  Scene Wrapper: utility class for mesh rendering and uniform update. \n
*
*/

/*!
*	Handles rendering for all meshes added to the scene \n
*	\note For memory mangment issues, a scene does not copy explicitely the meshed or uniforms used.
*
*	\code{.cpp}
*		Mesh mesh(&geometry,&material);
*		// adding a mesh to the scene
*		Scene scene;
*		scene.addMesh(&mesh);
*		...
*		...
*		// drawing meshes
*		scene.drawMeshes(&camera, &window); // we need a camera and a window to project the meshes correctly
*	\endcode
*/
class Scene {
public:
	///////////////////////////////////////////
	//	CONSTUCTOR & DESTRUCTOR
	///////////////////////////////////////////
	/*!
	*  \brief Default Constructor: \n
	*		initialise an empty scene
	*
	* \return empty scene (no meshes created)
	*
	*/
	Scene();
	

	///////////////////////////////////////////
	//	GETTERS
	///////////////////////////////////////////


	///////////////////////////////////////////
	//	SETTERS
	///////////////////////////////////////////
	/*!
	*	\brief adds a Mesh to the scene
	*
	* \param Mesh * mesh : new Mesh to add to the scene
	* \return appends input Mesh to the render list
	*/
	void addMesh(Mesh * mesh);


	///////////////////////////////////////////
	//	UTILITY
	///////////////////////////////////////////
	/*!
	*	\brief render a single input mesh
	*
	* \param Mesh * mesh : input mesh to be rendered
	* \param camera::Camera * camera : camera filming the scene (will be used in projecting the mesh correctly)
	* \param window::Window * window : viewport window (will be used in projecting the mesh correctly)
	* \return draws the mesh on current viewport and using input camera specifications
	*
	* \note Make use of the stencil buffer (renders the mesh on the stencil buffer)
	*/
	void drawMesh(Mesh * mesh, camera::Camera * camera, window::Window * window);
	/*!
	*	\brief render all stored scene meshes
	*
	* \param camera::Camera * camera : camera filming the scene (will be used in projecting the mesh correctly)
	* \param window::Window * window : viewport window (will be used in projecting the mesh correctly)
	* \return draws all scene on current viewport and using input camera specifications
	*
	* \note Make use of the stencil buffer (renders all mesh on the stencil buffer)
	*/
	void drawMeshes(camera::Camera * camera, window::Window * window);
	/*!
	*	\brief render all stored scene meshes using input shader for every mesh
	*
	* \param camera::Camera * camera : camera filming the scene (will be used in projecting the mesh correctly)
	* \param window::Window * window : viewport window (will be used in projecting the mesh correctly)
	* \param Shader * shader : custom shader to use for all meshes
	* \return draws all scene on current viewport and using input camera specifications
	*
	* \note uses input Shader to draw the meshes and not their current Material
	*/
	void drawMeshes(camera::Camera * camera, window::Window * window, Shader * shader);

	/*!
	*	\brief Render the mesh outline (using the stencil buffer) \n
	*			The scene is re-rendered using the stencil buffer as a mask. \n
	*			Previous rendering (that updated the stencil buffer) is unafected
	*
	* \param Shader * stencilShader : shader to use when rendering the scene => outline aspect
	* \param camera::Camera * camera : camera filming the scene (will be used in projecting the mesh correctly)
	* \param window::Window * window : viewport window (will be used in projecting the mesh correctly)
	* \return draws the scene meshes slightly larger that current dimension but using stencil buffer as a mask (ie discarding previously rendered pixels)
	*/
	void outlineMeshes(Shader * stencilShader, camera::Camera * camera, window::Window * window);
	/*!
	*	\brief Binds default uniforms to input shader \n
	*			- glm::mat4 modelMatrix;
	*			- glm::mat4 cameraMovment;
	*			- glm::mat4 normalMatrix;
	*			- glm::mat4 viewMatrix;
	*			- glm::mat4 projectionMatrix;
	*
	* \param Shader * shader : shader that needs default uniforms
	* \param camera::Camera * camera : camera filming the scene (will be used in projecting the mesh correctly)
	* \param window::Window * window : viewport window (will be used in projecting the mesh correctly)
	* \return computes and links all transformation matricies to input shader
	*/
	void linkDefaultUniforms(Shader * shader, camera::Camera * camera, window::Window * window);


private:
	////////////////////
	//  Scene Data
	////////////////////
	//! Mesh data
	/*! A list of previously allocated meshes
	*/
	std::vector<Mesh *> meshes;



};

/*@}*/

}

#endif