#ifndef RENDERBUFFER_HPP
#define RENDERBUFFER_HPP

////////////////////////
// GLEW
////////////////////////
#include <GL/glew.h>

////////////////////////
// STL
////////////////////////
#include <iostream>


namespace OpenGLEngine
{


/**
* \file renderBuffer.hpp
* \author Alexandre Ribard
* \date Feb 2016
*/

/** @addtogroup BUFFER_OBJECTS */
/*@{*/

/*!
*  \brief OpenGL Render Buffer Object Wrapper: \n
*		An RBO has a OpenGL ID \n
*		Similar to a Texture object but optimized for use as render targets \n
*
*	This wrapper takes care of the OpenGL directives to create an RBO
*
*/

class RBO {
public:
	///////////////////////////////////////////
	//	CONSTUCTOR & DESTRUCTOR
	///////////////////////////////////////////
	//! Constructor.
	/*!
	*	Generate + bind to current FBO
	*
	* \param size_t width : RBO width
	* \param size_t height : RBO height
	*/
	RBO(size_t width, size_t height)
	{
		// Create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
		glGenRenderbuffers(1, &ID);
		glBindRenderbuffer(GL_RENDERBUFFER, ID);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height); // Use a single renderbuffer object for both a depth AND stencil buffer.
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, ID); // Now actually attach it
		// Now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	/*!
	*  \brief Copy constructor: \n
	*
	* \param const RBO &rboSource : reference to a RBO
	*/
	RBO(RBO &rboSource)
	{
		this->ID = rboSource.ID;
	}



private:
	////////////////////
	//  RBO Data
	////////////////////
	//! OpenGL RBO ID
	/*! ID of the current RBO
	*/
	GLuint ID;

};

/*@}*/

}

#endif