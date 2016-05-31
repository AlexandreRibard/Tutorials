#ifndef FRAMEBUFFER_HPP
#define FRAMEBUFFER_HPP


////////////////////////
// GLEW
////////////////////////
#include <GL/glew.h>

////////////////////////
// STL
////////////////////////
#include <vector>

////////////////////////
// CUSTOM
////////////////////////
#include "shaderInterface.hpp"


namespace OpenGLEngine
{


/**
* \file frameBuffer.hpp
* \author Alexandre Ribard
* \date Feb 2016
*/

/** @defgroup MESH
*
* This module handles FBO and RBO wrapping
*
*/

/** @addtogroup BUFFER_OBJECTS */
/*@{*/


/*!
*  \brief OpenGL Frame Buffer Object Wrapper: \n
*		An FBO has a OpenGL ID, render tagerts (depth and color) and as well as some color attachments \n
*		We explicitely store a Shader, a vector of pointers to a Uniform and a vector of Texture pointers \n
*
*	This wrapper takes care of the OpenGL directives to create an FBO, set texture attachments and render targets
*
*/

/*!
*	Ex: Setting up G-Buffer \n
*			\code{.cpp}
*				// gen + bind FBO
*				FBO geometryBufferPassFBO; // declare FBO
*				// gen + bind depth render target + set depth attachment
*				geometryBufferPassFBO.addDepthRenderTarget(internalFormat, width, height, format, type); // adds a depth render target
*
*				// gen + bind color render target + set color attachment
*				// multiple color attachment supported
*				geometryBufferPassFBO.addColorRenderTarget(GL_RGBA32F, window::WINDOW_WIDTH, window::WINDOW_HEIGHT, GL_RGBA, GL_FLOAT);
*				geometryBufferPassFBO.addColorRenderTarget(GL_RGB32F, window::WINDOW_WIDTH, window::WINDOW_HEIGHT, GL_RGB, GL_FLOAT);
*				geometryBufferPassFBO.addColorRenderTarget(GL_RGB32F, window::WINDOW_WIDTH, window::WINDOW_HEIGHT, GL_RGB, GL_FLOAT);
*				// glDrawBuffers
*				geometryBufferPassFBO.setColorAttachments();
*
*				// 	Create a renderbuffer object for depth and stencil attachment
*				
*				...
*				...
*				
*				// Multi-Pass Rendering
*				// => first pass, write to GBuffer
*				geometryBufferPassFBO.bindFBO(); // bind GBuffer
*				
*				// Render Scene
*				...
*
*				geometryBufferPassFBO.unbindFBO(); // undinb GBuffer
*
*				// => second pass, recover texture attachment and pass them to shader
*				// Active proper texture unit before binding
*				geometryBufferPassFBO.bindTextureTargets();
*				// link them to shader
*				geometryBufferPassFBO.linkTextureTargets(&std::vector<std::string>{ "G_PositionDepth" , "G_Normal" , "G_Color" }, &screenShader);
*
*			\endcode
*/

class FBO {
public:
	///////////////////////////////////////////
	//	CONSTUCTOR & DESTRUCTOR
	///////////////////////////////////////////
	/*!
	*  \brief Deffault Constructor: \n
	*		Gen and Bind FBO
	*
	*/
	FBO();
	/*!
	*  \brief Copy constructor: \n
	*
	* \param const FBO &fboSource : reference to a FBO
	*/
	FBO(FBO &fboSource);


	///////////////////////////////////////////
	//	GETTERS
	///////////////////////////////////////////
	/*!
	*	\brief returns the ith attached color render target ID
	*
	* \param size_t i : index of attached color render target
	* \return returns associated OpenGL ID of given color render target
	*/
	GLuint getColorRenderTarget(size_t i);
	/*!
	*	\brief returns the attached depth render target ID
	*
	* \return returns associated OpenGL ID of depth render target
	*/
	GLuint getDepthRenderTarget();
	/*!
	*	\brief returns the ith attached color texture ID
	*
	* \param size_t i : index of attached color texture
	* \return returns associated OpenGL ID of given texture in which each render targets is saved
	*/
	GLuint getColorAttachment(size_t i);

	///////////////////////////////////////////
	//	SETTERS
	///////////////////////////////////////////
	/*!
	*	\brief adds a color render target:\n
	*		- Generate texture
	*		- Attach it to current FBO
	*
	* \param GLint internalFormat : Specifies the number of color components in the texture (https://www.opengl.org/sdk/docs/man/html/glTexImage2D.xhtml)
	* \param size_t width : Specifies the width of the texture image (https://www.opengl.org/sdk/docs/man/html/glTexImage2D.xhtml)
	* \param size_t height : Specifies the height of the texture image (https://www.opengl.org/sdk/docs/man/html/glTexImage2D.xhtml)
	* \param size_t format : Specifies the format of the pixel data (https://www.opengl.org/sdk/docs/man/html/glTexImage2D.xhtml)
	* \param size_t type : Specifies the data type of the pixel data (https://www.opengl.org/sdk/docs/man/html/glTexImage2D.xhtml)
	* \return returns associated OpenGL ID of given color render target
	*/
	void addColorRenderTarget(GLint internalFormat, size_t width, size_t height, GLenum format, GLenum type);
	/*!
	*	\brief Adds a depth render target:\n
	*		- Generate texture
	*		- Attach it to current FBO
	*
	* \param GLint internalFormat : Specifies the number of color components in the texture (https://www.opengl.org/sdk/docs/man/html/glTexImage2D.xhtml)
	* \param size_t width : Specifies the width of the texture image (https://www.opengl.org/sdk/docs/man/html/glTexImage2D.xhtml)
	* \param size_t height : Specifies the height of the texture image (https://www.opengl.org/sdk/docs/man/html/glTexImage2D.xhtml)
	* \param size_t format : Specifies the format of the pixel data (https://www.opengl.org/sdk/docs/man/html/glTexImage2D.xhtml)
	* \param size_t type : Specifies the data type of the pixel data (https://www.opengl.org/sdk/docs/man/html/glTexImage2D.xhtml)
	* \return returns associated OpenGL ID of given depth render target
	*/
	void addDepthRenderTarget(GLint internalFormat, size_t width, size_t height, GLenum format, GLenum type);
	/*!
	*	\brief Sets color attachments.\n
	*		   Links shader outputs to FBO render targets (color atachments) \n
	*		   https://www.opengl.org/sdk/docs/man/html/glDrawBuffers.xhtml
	*
	* \return  Defines an array of buffers into which outputs from the fragment shader data will be written.
	*/
	void setColorAttachments();


	///////////////////////////////////////////
	//	UTILITY
	///////////////////////////////////////////
	/*!
	*	\brief Binds FBO \n
	*		https://www.opengl.org/sdk/docs/man/html/glBindFramebuffer.xhtml
	*
	* \return  binds the FBO
	*/
	void bindFBO();
	/*!
	*	\brief Unbinds FBO \n
	*		https://www.opengl.org/sdk/docs/man/html/glBindFramebuffer.xhtml
	*
	* \return  unbinds the FBO
	*/
	void unbindFBO();

	/*!
	*	\brief Binds all color attachments\n
	*
	* \return  binds all texture render targets
	*/
	void bindTextureTargets();
	/*!
	*	\brief Links all color atachments to input Shader\n
	*
	* \param std::vector<std::string> * texNames : names used in input shader for all render targets of current FBO
	* \param Shader * shader : shader to wich the texture attachement of current FBO need to be bound
	* \return  Links ith color attachment to texNames[i] string name in input shader
	*/
	void linkTextureTargets(std::vector<std::string> * texNames, Shader * shader);

private:
	////////////////////
	//  FBO Data
	////////////////////
	//! OpenGL FBO ID
	/*! ID of the current FBO
	*/
	GLuint ID;
	//! color render targets
	/*! array of textures to be rendered into
	*/
	std::vector< GLuint > color_RenderTargets;
	//! depth render target
	/*! buffer to render depth information
	*/
	GLuint depth_RenderTarget;

	//! color attachments
	/*! textures in wich to render each color render targets
	*/
	std::vector<GLuint> color_attachments;

};


/*@}*/

}

#endif