#ifndef GLOBALINTERFACE_HPP
#define GLOBALINTERFACE_HPP



////////////////////////
// GLFW
////////////////////////
#include <GLFW/glfw3.h>

////////////////////////
// CUSTOM
////////////////////////
#include "controlerInterface.hpp"

namespace OpenGLEngine
{

/**
* \file renderBuffer.hpp
* \author Alexandre Ribard
* \date Feb 2016
*/

/** @defgroup GLOBAL
*
* This module handles global variables or functions shared throughout the project \n
* - Mainly controler and key callbacks
*
*/
/** @addtogroup GLOBAL */
/*@{*/

/*!
*  \brief Global variables & functions used throught the projects
*		- controler
*		- key callbacks
*		
*	Redefines GLFW key calls backs to encorporate our controler handling \n
*	Stores our curently used controler\n
*
*	\note Messy, true. But haven't found a better way to overload GLFW key call backs
*
*/

namespace global
{
	///////////////////////////////////////////
	//	CONTROLERS
	///////////////////////////////////////////
	/*!
	*	\brief Global controler
	*		Currently, only mouse is handles correctly \n
	*		Joysticks might work, but stability not guaranteed \n
	*/
	// Mouse controler
	controler::Controler mouse = controler::Controler(0, 0);


	///////////////////////////////////////////
	//	KEY CALLBACKS
	///////////////////////////////////////////

	/*!
	*	\brief key callback
	*		Is called whenever a key is pressed/released while the GLFW window context is active \n
	*		Handles callbacks: \n
	*			- ESCAPE : close window
	*	\param GLFWwindow* window : GLFW window instance
	*	\pram for the rest, cf http://www.glfw.org/docs/latest/input.html
	*	\return : key press callbacks, depends on what key is presses
	*/
	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
	{
		// a key is pressed
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GL_TRUE);
	}

	/*!
	*	\brief mouse callback
	*		Is called whenever a moves in the GLFW window \n
	*		Handles callbacks: \n
	*			- left button pressed + moving : set Controler drag and update the Controler internal position
	*	\param GLFWwindow* window : GLFW window instance
	*	\pram for the rest, cf http://www.glfw.org/docs/latest/input.html
	*	\return : mouse moves callbacks
	*/
	void mouse_callback(GLFWwindow* window, double xpos, double ypos)
	{
		// the mouse is moving
		// check which button is pressed
		int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
		if (state == GLFW_PRESS)
		{
			// left button is pressed while the mouse moves => update controler
			mouse.setDrag();
			mouse.move(xpos, ypos);
		}
	}
	/*!
	*	\brief mouse button callback
	*		Is called whenever the mouse is clicked in the GLFW window \n
	*		Handles callbacks: \n
	*			- right button pressed : stop Controler drag
	*	\param GLFWwindow* window : GLFW window instance
	*	\pram for the rest, cf http://www.glfw.org/docs/latest/input.html
	*	\return : mouse click callbacks
	*/
	void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
	{
		if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
		{
			// toggle mouse drag
			mouse.stopDrag();
		}
	}
	/*!
	*	\brief mouse scroll callback
	*		Is called whenever the mouse scrolles in the GLFW window \n
	*		set Controler scroll offset: \n
	*	\param GLFWwindow* window : GLFW window instance
	*	\pram for the rest, cf http://www.glfw.org/docs/latest/input.html
	*	\return : mouse scroll callbacks
	*/
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
	{
		float delta = yoffset;
		mouse.scroll(delta);

	}


}


/*@}*/

}

#endif