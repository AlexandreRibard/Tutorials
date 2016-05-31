#ifndef MODELMATERIAL_HPP
#define MODELMATERIAL_HPP




////////////////////////
// STL
////////////////////////
#include <string>
#include <vector>
#include <unordered_map>

////////////////////////
// CUSTOM
////////////////////////
#include "shaderInterface.hpp"
#include "textureInterface.hpp"
#include "uniformInterface.hpp"


namespace OpenGLEngine
{

/**
* \file modelGeometry.hpp
* \author Alexandre Ribard
* \date Feb 2016
*/

/** @addtogroup MESH */
/*@{*/

/*!
*  \brief Mesh Material Wrapper: \n
*		The Material of a mesh is represented by a Shader, some Uniform variables and a list of Texture \n
*		We explicitely store a Shader, a vector of pointers to a Uniform and a vector of Texture pointers \n
*
*	This wrapper takes care of the OpenGL directives to bind the shader and link uniforms and textures to said shader \n
*	\note that uniforms and textures have to be declared ahead of time \n
*	=> this wrapper only takes pointers to existing Texture or Uniform \n
*	Of course one can swap/remove/update Texture and Uniform if need be
*
*/

/*!
*	\note the following convention for the texture binding: \n
*		The n-th texture in the input texture list is supposed to reference
*			\code{.cpp}
*				GL_TEXTUREi
*			\endcode
*		Such that the OpenGL texture buinding is the following:
*			\code{.cpp}
*				glActiveTexture(GL_TEXTURE0 + i)
*			\endcode
*/


/*!
*	\note the following convention for the uniform binding: \n
*			- The shader's location will be current in use Shader->program
*			- The uniform's location will be the Uniform.name
*			- The uniform's value will be  Uniform.value
*			\n
*			\code{.cpp}
*				GLint uniformLoc = glGetUniformLocation(ourShader->Program, uniformName.c_str());
*				glUniform1f(uniformLoc, uniformValue); 
*			\endcode
*/
class Material {
public:
	///////////////////////////////////////////
	//	CONSTUCTOR & DESTRUCTOR
	///////////////////////////////////////////
	/*!
	*  \brief Constructor form known data: \n
	*		Set'up internal data (realocate vector)
	*
	* \param const std::vector<Texture *> * const textureVec: array of previously allocated Texture
	* \param const std::vector<Uniform *> * const uniformVec: array of previously allocated Uniform
	* \param const Shader * const shader: Shader to be used when rendering the mesh
	* \return set up internal storage
	*/
	Material(const std::vector<Texture *> * const textureVec, const std::vector<Uniform *> * const uniformVec, const Shader * const shader);
	/*!
	*  \brief Deffault Constructor: \n
	*		No Shader, Uniforms or Textures
	*
	* \param
	* \return
	*/
	Material();

	///////////////////////////////////////////
	//	SETTERS
	///////////////////////////////////////////
	/*!
	*	\brief swaps Shader (rest stays unchanged)
	*
	* \param const Shader * const shader : new Sahder to be used for rendering
	* \return swaps Shader for the input one.
	*/
	void setShader(const Shader * const shader);
	/*!
	*	\brief swaps value of input Uniform
	*
	* \param Uniform * const u
	* \return looks for correspondding Uniform in internal uniform list. \n
	*		  if a Uniform with same name as input is found, swaps its value for new one.
	*/
	void updateUniform(Uniform * const u);

	/*!
	*	\brief adds a new Texture to the Texture list
	*
	* \param Texture * const t : new Texture to link to shader when rendering the mesh
	* \return adds input Texture to list of textures
	*	\note: this new Texture will be linked to the current Shader regardless of it's accually specified in the shader code \n
	*	\n
	*	\n
	*	\note the following convention for the texture binding: \n
	*		The n-th texture in the input texture list is supposed to reference
	*			\code{.cpp}
	*				GL_TEXTUREi
	*			\endcode
	*		Such that the OpenGL texture buinding is the following:
	*			\code{.cpp}
	*				glActiveTexture(GL_TEXTURE0 + i)
	*			\endcode
	*/
	void addTexture(Texture * const t);

	/*!
	*	\brief adds a new Uniform to the Uniform list
	*
	* \param Uniform * const u : new Uniform to link to shader when rendering the mesh
	* \return adds input Uniform to list of uniforms
	*	\note: this new Uniform will be linked to the current Shader regardless of it's accually specified in the shader code \n
	*	\n
	*	\n
	*	\note the following convention for the uniform binding: \n
	*			- The shader's location will be current in use Shader->program
	*			- The uniform's location will be the Uniform.name
	*			- The uniform's value will be  Uniform.value
	*			\n
	*			\code{.cpp}
	*				GLint uniformLoc = glGetUniformLocation(ourShader->Program, uniformName.c_str());
	*				glUniform1f(uniformLoc, uniformValue);
	*			\endcode
	*
	*/
	void addUniform(Uniform * const u);


	///////////////////////////////////////////
	//	GETTERS
	///////////////////////////////////////////
	/*!
	*	\brief returns current Shader
	*
	* \return returns a pointer to currently used Shader
	*/
	Shader * getShader();
	


	///////////////////////////////////////////
	//	UTILITY
	///////////////////////////////////////////
	//! Binds Textures and Uniform to current shader
	/*!
	/ \brief Binds uniforms to current shader : \n
	*			- Binds Uniforms (regardless of type)
	*			- Binds Textures (regarless of type)
	*
	* \param
	* \return links all uniforms (Uniform, Textures) to current shader the mesh \n
	*	- link Uniform
	*	- link Texture
	*/
	void bindMaterial();
	/*! Binds Textures and Uniform to input shader
	/ \brief Binds uniforms to input shader: \n
	*			- Binds Uniforms (regardless of type)
	*			- Binds Textures (regarless of type)
	*
	* \param
	* \return links all uniforms (Uniform, Textures) to input shader the mesh \n
	*	- link Uniform
	*	- link Texture
	*/
	void bindMaterial(Shader * shader);

	/*!
	*	\brief set texture to previous state
	*
	* \param 
	* \return unbinds textures
	*
	*/
	void unbindMaterial();

private:
	////////////////////
	//  Material Data
	////////////////////
	//! Shader data
	/*! Vertex and Fragment shader to be used for rendering
	*/
	Shader shader;

	//! Texture data
	/*! A list of previously allocated texture to be linked to shader when rendering
	*/
	std::vector<Texture *> textures;

	//! Uniform data
	/*!  A list of previously allocated uniforms to be linked to shader when rendering \n
	*	map storing every uniforms \n
	*	mapping uniform name to uniform \n
	*/
	std::unordered_map<std::string, Uniform *> uniforms;
};

/*@}*/
}

#endif