#ifndef SHADERINTERFACE_HPP
#define SHADERINTERFACE_HPP

////////////////////////
// GLEW
////////////////////////
#include <GL/glew.h>;

////////////////////////
// STL
////////////////////////
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>


namespace OpenGLEngine
{

/**
* \file shaderInterface.hpp
* \author Alexandre Ribard
* \date Feb 2016
*/

/** @defgroup SHADER
*
* This module handles shader loading and linking
*
*/

/** @addtogroup SHADER */
/*@{*/

/*!
*  \brief  Shader Wrapper: utility class for shader loading and linking. \n
*
*/

/*!
*	Handles loading external shaders and OpenGL bindings \n
*
*	\code{.cpp}
*			Shader ourShader("vertex_shader.vert", "frament_shader.frag"); 
*	\endcode
*/

class Shader
{
public :

	///////////////////////////////////////////
	//	CONSTUCTOR & DESTRUCTOR
	///////////////////////////////////////////
	/*!
	*  \brief Default Constructor: \n
	*		initialise the default shader:
	*			- two layouts: position and color
	*			- default uniforms (change of basis matricies)
	*			- ouptput color
	*
	* \return default shader is initialized and linked
	*
	*/
	Shader()
	{
		const GLchar* vShaderCode = "#version 330 core\n"
			"layout(location = 0) in vec3 position;\n"
			"layout(location = 1) in vec3 color;\n"

			"out vec3 ourColor;\n"

			"uniform mat4 modelMatrix;\n"
			"uniform mat4 viewMatrix;\n"
			"uniform mat4 projectionMatrix;\n"

			"void main()\n"
			"{"
			"	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position, 1.0f);\n"
			"	ourColor = color;\n"
			"}\n\0";

		const GLchar* fShaderCode = "#version 330 core\n"
			"in vec3 ourColor;\n"

			"out vec4 color;\n"

			"void main()\n"
			"{\n"
			"	color = vec4(1.0, 0.0, 0.0, 1.0f);\n"
			"}\n\0";


		// 2. Compile shaders
		GLuint vertex, fragment;
		GLint success;
		GLchar infoLog[512];
		// Vertex Shader
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, NULL);
		glCompileShader(vertex);
		// Print compile errors if any
		glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(vertex, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		}
		// Fragment Shader
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		glCompileShader(fragment);
		// Print compile errors if any
		glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(fragment, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		}
		// Shader Program
		this->Program = glCreateProgram();
		glAttachShader(this->Program, vertex);
		glAttachShader(this->Program, fragment);
		glLinkProgram(this->Program);
		// Print linking errors if any
		glGetProgramiv(this->Program, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(this->Program, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		}
		// Delete the shaders as they're linked into our program now and no longer necessery
		glDeleteShader(vertex);
		glDeleteShader(fragment);
	}

	///////////////////////////////////////////
	//	CONSTUCTOR & DESTRUCTOR
	///////////////////////////////////////////
	/*!
	*  \brief Constructor from known data: \n
	*		parses input files and links associated shader program
	*
	* \param const char * vertexPath : string representing to input vertex shader (must end in .vert)
	* \param const char * fragmentPath : string representing to input fragment shader (must end in .frag)
	* \return shader created, built and linked
	*
	*/
	Shader(const char* vertexPath, const char* fragmentPath)
	{
		// 1. Retrieve the vertex/fragment source code from filePath
		std::string vertexCode;
		std::string fragmentCode;
		std::ifstream vShaderFile;
		std::ifstream fShaderFile;
		// ensures ifstream objects can throw exceptions:
		vShaderFile.exceptions(std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::badbit);
		try
		{
			// Open files
			vShaderFile.open(vertexPath);
			fShaderFile.open(fragmentPath);
			std::stringstream vShaderStream, fShaderStream;
			// Read file's buffer contents into streams
			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();
			// close file handlers
			vShaderFile.close();
			fShaderFile.close();
			// Convert stream into string
			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();
		}
		catch (std::ifstream::failure e)
		{
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
		}
		const GLchar * vShaderCode = vertexCode.c_str();
		const GLchar * fShaderCode = fragmentCode.c_str();
		// 2. Compile shaders
		GLuint vertex, fragment;
		GLint success;
		GLchar infoLog[512];
		// Vertex Shader
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, NULL);
		glCompileShader(vertex);
		// Print compile errors if any
		glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(vertex, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		}
		// Fragment Shader
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		glCompileShader(fragment);
		// Print compile errors if any
		glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(fragment, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		}
		// Shader Program
		this->Program = glCreateProgram();
		glAttachShader(this->Program, vertex);
		glAttachShader(this->Program, fragment);
		glLinkProgram(this->Program);
		// Print linking errors if any
		glGetProgramiv(this->Program, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(this->Program, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		}
		// Delete the shaders as they're linked into our program now and no longer necessery
		glDeleteShader(vertex);
		glDeleteShader(fragment);
	}

	/*!
	*  \brief Copy Constructor : \n
	*		swaps shader Program ID
	*
	* \param Shader * shader : input shader
	* \return current shader now points to the same OpenGL shader ID
	*
	*/
	Shader(Shader * shader)
	{
		Program = shader->Program;
	}


	///////////////////////////////////////////
	//	GETTERS
	///////////////////////////////////////////


	///////////////////////////////////////////
	//	SETTERS
	///////////////////////////////////////////


	///////////////////////////////////////////
	//	UTILITY
	///////////////////////////////////////////
	/*!
	*	\brief Use shader program
	*
	* \note glUse associated shader programm
	*/
	void Use() 
	{ 
		glUseProgram(this->Program); 
	}



public:
	////////////////////
	//  Shader Data
	////////////////////
	//! Shader programa
	/*! OpenGL ID for this shader's programm
	*/
	GLuint Program;
};

/*@}*/


}

#endif