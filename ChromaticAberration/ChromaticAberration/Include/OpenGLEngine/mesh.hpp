#ifndef MESH_HPP
#define MESH_HPP



////////////////////////
// CUSTOM
////////////////////////
#include "modelGeometry.hpp"
#include "modelMaterial.hpp"

namespace OpenGLEngine
{
/**
* \file mesh.hpp
* \author Alexandre Ribard
* \date Feb 2016
*/

/** @defgroup MESH
*
* This module handles storage and rendering of a given mesh
*
*/

/** @addtogroup MESH */
/*@{*/

/*!
*  \brief  Mesh Wrapper: utility class for mesh handling. \n
*			A mesh contains a Geomety and a Material
*
*/

/*!
*	Custom class representing a renderable mesh. \n
*	The mesh is made up of a specified Material (Shader + enventual Textures & Uniforms) \n
*
*	\code{.cpp}
*		Geometry geomety();
*		Material material();
*		Mesh mesh(&geometry,&material);
*	\endcode
*/

class Mesh {
public:
	///////////////////////////////////////////
	//	CONSTUCTOR & DESTRUCTOR
	///////////////////////////////////////////
	//! Constructor.
	/*!
	* 
	* \param const Geometry * const geometry : mesh geometry data (VBO + array of Vertices<xyz,n,uv>)
	* \param const Material * const material : mesh shader-specefic information (Shader + Uniform)
	*/
	Mesh(const Geometry * const geometry, const Material * const material);
	//! Copy Constructor.
	/*!
	*
	* \param Mesh * mesh : pointer to a mesh<Geometry + Material>
	*/
	Mesh(Mesh * mesh);
	/*!
	*
	* \param const Mesh &mSource : reference to a mesh<Geometry + Material>
	*/
	Mesh(Mesh &mSource);
	//! Empty Constructor.
	/*!
	*
	* \return empty Geometry & Material (not void)
	*/
	Mesh();

	//! Destructor
	/*!
	*
	* \return dumps mesh geometric data (aka VBO)
	*/
	~Mesh();

	///////////////////////////////////////////
	//	GETTERS
	///////////////////////////////////////////
	//! returns Mesh world-space position (ie, local mesh frame in world space)
	/*!
	* \return glm::vec3, mesh local frame's world space poisiton
	*/
	glm::vec3 getWorldSpacePosition();
	//! returns Mesh material Shader
	/*!
	* \return Shader : material vertex and frament shader
	*/
	Shader getShader();
	//! returns a pointer to the Mesh's material Material
	/*!
	* \return Material * : material (shader + uniforms + textures)
	*/
	Material * getMaterial();
	//! returns a pointer to the Mesh's geometry Geometry
	/*!
	* \return Geometry * : material geometry data
	*/
	Geometry * getGeometry();

	///////////////////////////////////////////
	//	SETTERS
	///////////////////////////////////////////
	//! swaps Geometry (Material stays unchanged)
	/*!
	* \param const Geometry * const geometry : new geometry data (VBO + array of Vertices<xyz,n,uv>)
	* \return swaps Geometry for the input one.
	*/
	void setGeometry(const Geometry * const geometry);
	//! swaps Material (Geometry stays unchanged)
	/*!
	* \param const Material * const material : new material data (Shader + Uniform)
	* \return swaps Material for the input one.
	*/
	void setMaterial(const Material * const material);
	//! swaps local mesh frame's world space position (Geometry & Material stays the same)
	/*!
	* \param const glm::vec3 worldSpacePosition : new world space position
	* \return updates world space position
	*/
	void setWorldSpacePosition(const glm::vec3 worldSpacePosition);

	//! adds another Texture to the mesh's material
	/*!
	* \param Texture * texture : pointer to Texture
	* \return adds the texture to current material
	*/
	void addTexture(Texture * texture);
	//! adds another Uniform to the mesh's material
	/*!
	* \param Uniform * uniform : pointer to Uniform
	* \return adds the uniform to current material
	*/
	void addUniform(Uniform * uniform);


	///////////////////////////////////////////
	//	UTILITIES
	///////////////////////////////////////////
	//! Render the mesh using current material
	/*!
	/ \brief Render the mesh : \n
	*			- Binds material (ie, Texture & Uniforms to current material Shader)
	*			- Draws the mesh Geometry (ie VBO)
	*			- Unbinds current bound material
	*
	* \param 
	* \return renders the mesh, using current material \n
	*	- binds current material
	*	- draw geometry
	*	- unbind material
	*/
	void render();
	//! Render the mesh using specified shader
	/*!
	* \brief Render the mesh as above, but use specific Shader instead of the current one
	*
	* \param Shader * shader : shader to be used for rendering
	* \return renders the mesh, using current material with input Shader 
	*		  Note : Binds current Texture and Uniform to input Shader \n
	*	- binds current material (but swap current Shader with input)
	*	- draw geometry
	*	- unbind material
	*/
	void render(Shader * shader);
	//! Render only the geometry of the mesh
	void renderGeometry();

	//! Updates a Uniform of current Material
	/*!
	/ \brief Looks through current Material.
	*		 If a Uniform with the same name exists, update value
	*
	* \param Uniform * const uniform : Uniform to be updated
	* \return Updates the value of specified Uniform
	*		  Note : Is type inepentent : one can swap a float Uniform for a vec3 Uniform using this method
	*/
	void updateUniform(Uniform * const uniform);




private:
	////////////////////
	//  Mesh Data
	////////////////////
	//! Geomerty data
	/*! Mesh's Geometry (VBO for rendering + vector of position, normal, uv-coordinates)
	*/
	Geometry geometry;
	//! Material data
	/*! Mesh's Material (Shader, Textures and Uniform)
	*/
	Material material;
};


/*@}*/
}


#endif