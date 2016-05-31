#ifndef TEXTUREINTERFACE_HPP
#define TEXTUREINTERFACE_HPP

////////////////////////
// GLEW
////////////////////////
#include <GL/glew.h>

////////////////////////
// GLM
////////////////////////
#include <glm\glm.hpp>

////////////////////////
// STL
////////////////////////
#include <string>
#include <vector>

////////////////////////
// CUSTOM
////////////////////////
#include "shaderInterface.hpp"

namespace OpenGLEngine
{

/**
* \file textureInterface.hpp
* \author Alexandre Ribard
* \date Feb 2016
*/

/** @defgroup TEXTURES
*
* This module handles texture loading and linking
*
*/

/** @addtogroup TEXTURES */
/*@{*/

/*!
*  \brief Texture mother struct
*			ID, OpenGL texture refence: unsigned int \n
*			name, texture name: std::string \n
*			type, texture type (2D or cube-map): std::string \n
*			\n
*			virtual binding function to input shader location
*/
struct Texture {
	unsigned int ID; /**< ID, OpenGL texture refence: unsigned int */
	std::string name; /**< name, texture name: std::string */
	std::string type; /**< type, texture type (2D or cube-map): std::string */

	/*!
	*  \brief Default constructor: \n
	*
	*/
	explicit Texture()
	{
		name = "";
		type = "";
	}
	/*!
	*  \brief Copy constructor: \n
	*
	* \param const Texture &uSource : reference to a Texture
	*/
	Texture(Texture &tSource)
	{
		ID = tSource.ID;
		name = tSource.name;
		type = tSource.type;
	}

	/*!
	*  \brief Binds texture to input shader: \n
	*		Binds texture at input location in said shader \n
	*
	* \param GLuint locInShader: texture location in shader
	* \param Shader * shader: input shader to which texture needs binding
	* \return activate, retrieve and bind texture to input shader and specified location
	*/
	virtual void bindTexture(GLuint locInShader, Shader * shader)
	{
	}
};

/*!
*  \brief 2D Texture child struct : Texture
*			type, sampler2D \n
*/
struct Texture2D : Texture
{	
	/*!
	*  \brief Binds texture to input shader: \n
	*		Binds texture at input location in said shader \n
	*
	* \param GLuint locInShader: texture location in shader
	* \param Shader * shader: input shader to which texture needs binding
	* \return activate, retrieve and bind texture to input shader and specified location
	*/
	void bindTexture(GLuint locInShader, Shader * shader) override
	{
		// Active proper texture unit before binding
		glActiveTexture(GL_TEXTURE0 + locInShader);

		// Retrieve texture number : texture*
		GLuint id = ID;

		glBindTexture(GL_TEXTURE_2D, id);
		glUniform1i(glGetUniformLocation(shader->Program, name.c_str()), locInShader);
	}
};

/*!
*  \brief Cube Texture child struct : Texture
*			type, samplerCube \n
*/
struct TextureCube : Texture
{
	/*!
	*  \brief Binds texture to input shader: \n
	*		Binds texture at input location in said shader \n
	*
	* \param GLuint locInShader: texture location in shader
	* \param Shader * shader: input shader to which texture needs binding
	* \return activate, retrieve and bind texture to input shader and specified location
	*/
	void bindTexture(GLuint locInShader, Shader * shader) override
	{
		// Active proper texture unit before binding
		glActiveTexture(GL_TEXTURE0 + locInShader);

		// Retrieve texture number : texture*
		GLuint id = ID;

		glBindTexture(GL_TEXTURE_CUBE_MAP, id);
		glUniform1i(glGetUniformLocation(shader->Program, name.c_str()), locInShader);
	}
};



/*!
*  \brief Texture Client Wrapper: \n
*		Interface for a texture (both 2D and cubemap) handleing \n
*		Loads, generate textures
*	\note : wrapps SOIL and OpenGL functions. For thurther information, please refer to :
*/
namespace textureClient
{
	////////////////////
	//  Sampler2D
	////////////////////
	/*!
	*  \brief Generate texture from know data : \n
	*		Generate and bind texture
	*
	* \param std::vector<glm::vec3> * data: floating point rgb vector
	* \return generates, binds and create texture from input data
	*/
	unsigned int generateTexture(std::vector<glm::vec3> * data);
	/*!
	*  \brief Generate texture from know data : \n
	*		Generate and bind texture
	*
	* \param float * data: floating point array of dimension width x height
	* \param size_t width: Specifies the width of the texture image
	* \param size_t height: Specifies the height of the texture image
	* \return generates, binds and create texture from input data
	* \note default RGB
	*/
	unsigned int generateTexture(float * data, size_t width, size_t height);
	/*!
	*  \brief Generate texture from know data : \n
	*		Generate and bind texture \n
	*		cf: https://www.khronos.org/opengles/sdk/docs/man/xhtml/glTexImage2D.xml
	*
	* \param float * data: floating point array of dimension width x height
	* \param size_t width: Specifies the width of the texture image
	* \param size_t height: Specifies the height of the texture image
	* \param GLint internalFormat: internal format of the texture
	* \param GLenum format: format of the texel data (RGB, RG...)
	* \return generates, binds and create texture from input data
	*/
	unsigned int generateTexture(float * data, size_t width, size_t height, GLint internalFormat, GLenum format);
	/*!
	*  \brief Generate texture from know data : \n
	*		Generate and bind texture from path \n
	*		Supported type : cf SOIL_load_image
	*
	* \param const std::string path: path to texture image
	* \return generates, binds and create texture from input file
	*/
	unsigned int loadTexture(const std::string path);

	/*!
	*  \brief Generate attachment textures for render targets : \n
	*		Generate and bind new texture (GL_RGB as internal format)
	*
	* \param const size_t width: attachment width
	* \param const size_t height: attachment height
	* \param const bool depth: color or depth attachment
	* \return generates, binds texture attachment
	*/
	unsigned int generateAttachmentTexture(const size_t width, const size_t height, const bool depth);
	/*!
	*  \brief Generate floating point attachment textures for render targets : \n
	*		Generate and bind new floating point texture (GL_RGB16F as default internal format)
	*
	* \param const size_t width: attachment width
	* \param const size_t height: attachment height
	* \param const bool depth: color or depth attachment
	* \param unsigned int precision = GL_RGB16F internal texture format (aka precision)
	* \return generates, binds texture attachment
	*/
	unsigned int generateFloatingPointAttachmentTexture(const size_t width, const size_t height, const bool depth, unsigned int precision = GL_RGB16F);



	////////////////////
	//  ShaderCube
	////////////////////
	/*!
	*  \brief Generate cube map texture from know data : \n
	*		Generate and bind texture from path \n
	*		Supported type : cf SOIL_load_image
	*
	* \param const std::vector<std::string> * const textureFace: path to 6 faces image of cube map (order: (px,nx,py,ny,pz,nz)
	* \return generates, binds and create cubemap texture from input file
	* \note Face ordering should be:
	*		-* +X (right)
	*		-* -X (left)
	*		-* +Y (top)
	*		-* -Y (bottom)
	*		-* +Z (front) 
	*		-* -Z (back)
	*/
	unsigned int loadCubeMap(const std::vector<std::string> * const textureFaces);

	/*!
	*  \brief Irradiance map spherical harmonics coefficients commputation : \n
	*		Image Based Lighting: Every visible textel of the cubemap is a lightsource \n
	*		We compute the corresponding Irradiance Map's spherical harmonics coefficients
	*		=> we skip the precomupation of the Irradiance Map and use those coefficients of each shaded pixels
	*
	* \param double SH_COEFFS[9][3]: spherical coeeficients array 
	* \param const std::vector<std::string> * const textureFace: path to 6 faces image of cube map (order: (px,nx,py,ny,pz,nz)
	* \return computes corresponding 9 first SH coeffecients per color channel
	* \note Shperical Harmonics coefficients:
	*		L_{l,m}, 0<l<9 and 0<m<3 : 9 coefficents per color channel
	* \note Face ordering should be:
	*		-* +X (right)
	*		-* -X (left)
	*		-* +Y (top)
	*		-* -Y (bottom)
	*		-* +Z (front)
	*		-* -Z (back)
	*/
	void IBLDiffuse_Lambert_SHCoeffs(float(*SH_COEFFS)[9][3], const std::vector<std::string> * const textureFaces);


	GLuint IntegrateBRDF(size_t LUT_WIDTH, size_t LUT_HEIGHT);
}

/*@}*/


}

#endif