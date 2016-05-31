#ifndef UNIFORMINTERFACE_HPP
#define UNIFORMINTERFACE_HPP


////////////////////////
// GLM
////////////////////////
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

////////////////////////
// STL
////////////////////////
#include <string>
#include <typeinfo>  // operator typeid
#include <type_traits> // is_same
#include <vector>


////////////////////////
// CUSTOM
////////////////////////
#include "shaderInterface.hpp"

namespace OpenGLEngine
{

/**
* \file uniformInterface.hpp
* \author Alexandre Ribard
* \date Feb 2016
*/

/** @defgroup UNIFORMS
*
* This module handles uniform shader variables (creation, handeling, update and linking)
*
*/

/** @addtogroup TEXTURE */
/*@{*/

/*!
*  \brief Uniform mother struct \n
*			The meshes needs to support uniform variables for rendering
*			In order to support custom uniforms types, we define a mother classe template on the specific type \n
*			\n
*			name, uniform name: std::string \n
*			type, uniform type (float, vector2,3f...): std::string \n
*			\n
*			virtual uniform linking to specified shader \n
*			virtual update uniform value
*/
struct Uniform
{
	std::string name; /**< name, uniform name: std::string */
	std::string type; /**< name, uniform type: std::string */

	/*!
	*  \brief Default constructor: \n
	*
	*/
	explicit Uniform()
	{
		name = "";
		type = "";
	}
	/*!
	*  \brief Copy constructor: \n
	*
	* \param const Uniform &uSource : reference to a Uniform
	*/
	Uniform(Uniform &uSource)
	{
		name = uSource.name;
		type = uSource.type;
	}

	/*!
	*  \brief Links uniform to input shader \n
	*	\note Retrieve uniform location from uniform name before binding \n
	*			=> please ensure that uniform has same name as in shader
	*
	* \param Shader * shader: input shader to which uniforms needs to be linked
	* \return retreive uniform location and link it to input shader
	*/
	virtual void linkUniform(const Shader * const ourShader)
	{
	};

	/*!
	*  \brief Updates uniform value \n
	*
	* \return hard update (does not re-link texture)
	*/
	virtual void updateValue()
	{};

};


/*!
*  \brief fUniform : float Uniform
*			type, float \n
*/
struct fUniform : Uniform
{
	float value; /**< value, uniform value: float */
	
	/*!
	*  \brief Default constructor: \n
	*
	*/
	explicit fUniform() : Uniform() {
		type = "f";
	}
	/*!
	*  \brief Copy constructor: \n
	*
	* \param const fUniform &uSource : reference to a fUniform
	*/
	fUniform(fUniform &uSource)
	{
		name = uSource.name;
		type = uSource.type;
		value = uSource.value;
	}

	/*!
	*  \brief Links uniform to input shader \n
	*	\note Retrieve uniform location from uniform name before binding \n
	*			=> please ensure that uniform has same name as in shader
	*
	* \param Shader * shader: input shader to which uniforms needs to be linked
	* \return retreive uniform location and link it to input shader
	*/
	void linkUniform(const Shader * const ourShader) override
	{
		std::string uniformName = this->name;
		float uniformValue = this->value;

		GLint uniformLoc = glGetUniformLocation(ourShader->Program, uniformName.c_str());
		glUniform1f(uniformLoc, uniformValue);
	}
	
	/*!
	*  \brief Updates uniform value \n
	*
	* \param const float f: new uniform value
	* \return hard update (does not re-link texture)
	*/
	void updateValue(const float f)
	{
		value = f;
	}
};

/*!
*  \brief iUniform : int Uniform
*			type, int \n
*/
struct iUniform : Uniform
{
	int value; /**< value, uniform value: int */

	/*!
	*  \brief Default constructor: \n
	*
	*/
	explicit iUniform() : Uniform() {
		type = "i";
	}
	/*!
	*  \brief Copy constructor: \n
	*
	* \param const iUniform &uSource : reference to a iUniform
	*/
	iUniform(iUniform &uSource)
	{
		name = uSource.name;
		type = uSource.type;
		value = uSource.value;
	}

	/*!
	*  \brief Links uniform to input shader \n
	*	\note Retrieve uniform location from uniform name before binding \n
	*			=> please ensure that uniform has same name as in shader
	*
	* \param Shader * shader: input shader to which uniforms needs to be linked
	* \return retreive uniform location and link it to input shader
	*/
	void linkUniform(const Shader * const ourShader) override
	{
		std::string uniformName = this->name;
		int uniformValue = this->value;

		GLint uniformLoc = glGetUniformLocation(ourShader->Program, uniformName.c_str());
		glUniform1i(uniformLoc, uniformValue);
	}

	/*!
	*  \brief Updates uniform value \n
	*
	* \param const int i: new uniform value
	* \return hard update (does not re-link texture)
	*/
	void updateValue(const int i)
	{
		value = i;
	}
};



/*!
*  \brief f2vUniform : float2 Uniform
*			type, glm::vec2 \n
*/
struct f2vUniform : Uniform
{
	glm::vec2 value; /**< value, uniform value: glm::vec2 */

	/*!
	*  \brief Default constructor: \n
	*
	*/
	explicit f2vUniform() : Uniform() {
		type = "f2v";
	}
	/*!
	*  \brief Copy constructor: \n
	*
	* \param const f2vUniform &uSource : reference to a fUniform
	*/
	f2vUniform(f2vUniform &uSource)
	{
		name = uSource.name;
		type = uSource.type;
		value = uSource.value;
	}

	/*!
	*  \brief Links uniform to input shader \n
	*	\note Retrieve uniform location from uniform name before binding \n
	*			=> please ensure that uniform has same name as in shader
	*
	* \param Shader * shader: input shader to which uniforms needs to be linked
	* \return retreive uniform location and link it to input shader
	*/
	void linkUniform(const Shader * const ourShader) override
	{
		std::string uniformName = this->name;
		glm::vec2 uniformValue = this->value;

		GLint uniformLoc = glGetUniformLocation(ourShader->Program, uniformName.c_str());
		glUniform2fv(uniformLoc, 1, glm::value_ptr(uniformValue));
	}
	
	/*!
	*  \brief Updates uniform value \n
	*
	* \param const float glm::vec2 f2v: new uniform value
	* \return hard update (does not re-link texture)
	*/
	void updateValue(const glm::vec2 f2v)
	{
		value = f2v;
	}
};

/*!
*  \brief f3vUniform : float3 Uniform
*			type, glm::vec3 \n
*/
struct f3vUniform : Uniform
{
	glm::vec3 value; /**< value, uniform value: glm::vec3 */

	/*!
	*  \brief Default constructor: \n
	*
	*/
	explicit f3vUniform() : Uniform() {
		type = "f3v";
	}
	/*!
	*  \brief Copy constructor: \n
	*
	* \param const f3vUniform &uSource : reference to a fUniform
	*/
	f3vUniform(f3vUniform &uSource)
	{
		name = uSource.name;
		type = uSource.type;
		value = uSource.value;
	}

	/*!
	*  \brief Links uniform to input shader \n
	*	\note Retrieve uniform location from uniform name before binding \n
	*			=> please ensure that uniform has same name as in shader
	*
	* \param Shader * shader: input shader to which uniforms needs to be linked
	* \return retreive uniform location and link it to input shader
	*/
	void linkUniform(const Shader * const ourShader) override
	{
		std::string uniformName = this->name;
		glm::vec3 uniformValue = this->value;

		GLint uniformLoc = glGetUniformLocation(ourShader->Program, uniformName.c_str());
		glUniform3fv(uniformLoc, 1, glm::value_ptr(uniformValue));
	}

	/*!
	*  \brief Updates uniform value \n
	*
	* \param const glm::vec3 f3v: new uniform value
	* \return hard update (does not re-link texture)
	*/
	void updateValue(const glm::vec3 f3v)
	{
		value = f3v;
	}
};

/*!
*  \brief m4fUniform : 4x4 floating point matrix Uniform
*			type, glm::mat4 \n
*/
struct m4fUniform : Uniform
{
	glm::mat4 value; /**< value, uniform value: glm::mat4 */

	/*!
	*  \brief Default constructor: \n
	*
	*/
	explicit m4fUniform() : Uniform() {
		type = "m4f";
	}
	/*!
	*  \brief Copy constructor: \n
	*
	* \param const m4fUniform &uSource : reference to a m4fUniform
	*/
	m4fUniform(m4fUniform &uSource)
	{
		name = uSource.name;
		type = uSource.type;
		value = uSource.value;
	}

	/*!
	*  \brief Links uniform to input shader \n
	*	\note Retrieve uniform location from uniform name before binding \n
	*			=> please ensure that uniform has same name as in shader
	*
	* \param Shader * shader: input shader to which uniforms needs to be linked
	* \return retreive uniform location and link it to input shader
	*/
	void linkUniform(const Shader * const ourShader) override
	{
		std::string uniformName = this->name;

		GLint uniformLoc = glGetUniformLocation(ourShader->Program, uniformName.c_str());
		glUniformMatrix4fv(uniformLoc, 1, GL_FALSE, glm::value_ptr(value));

	}

	/*!
	*  \brief Updates uniform value \n
	*
	* \param glm::mat4 const * m4f: new uniform value
	* \return hard update (does not re-link texture)
	*/
	void updateValue(glm::mat4 const * m4f)
	{
		value = *m4f;
	}

};




/*!
*  \brief af3vUniform : array of float3 Uniform
*			type, std::vector<glm::vec3> \n
*/
struct af3vUniform : Uniform
{
	std::vector<glm::vec3> value; /**< value, uniform value: std::vector<glm::vec3> */

	/*!
	*  \brief Default constructor: \n
	*
	*/
	explicit af3vUniform() : Uniform() {
		type = "af3v";
	}
	/*!
	*  \brief Copy constructor: \n
	*
	* \param const af3vUniform &uSource : reference to a af3vUniform
	*/
	af3vUniform(af3vUniform &uSource)
	{
		name = uSource.name;
		type = uSource.type;
		value = uSource.value;
	}

	/*!
	*  \brief Links uniform to input shader \n
	*	\note Retrieve uniform location from uniform name before binding \n
	*			=> please ensure that uniform has same name as in shader
	*
	* \param Shader * shader: input shader to which uniforms needs to be linked
	* \return retreive uniform location and link it to input shader
	*/
	void linkUniform(const Shader * const ourShader) override
	{
		std::string uniformName = this->name;


		for (GLuint i = 0; i < value.size(); ++i)
		{
			GLint uniformLoc = glGetUniformLocation(ourShader->Program, (uniformName + "[" + std::to_string(i) + "]").c_str());
			glUniform3fv(uniformLoc, 1, &(this->value)[i][0]);
		}
	}

	/*!
	*  \brief Updates uniform value \n
	*
	* \param std::vector<glm::vec3> const * av2f: new uniform value
	* \return hard update (does not re-link texture)
	*/
	void updateValue(std::vector<glm::vec3> const * av2f)
	{
		value = *av2f;
	}
	
};

/*@}*/

}

#endif