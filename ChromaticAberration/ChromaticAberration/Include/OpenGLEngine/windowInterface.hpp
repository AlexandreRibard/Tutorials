#ifndef WINDOWINTERFACE_HPP
#define WINDOWINTERFACE_HPP

////////////////////////
// GLEW
////////////////////////
#include <GL/glew.h>;

////////////////////////
// GLFW
////////////////////////
#include <GLFW/glfw3.h>

////////////////////////
// STL
////////////////////////
#include <string>
#include <algorithm>

////////////////////////
// CUSTOM
////////////////////////
#include "controlerInterface.hpp"

namespace OpenGLEngine
{

/**
* \file windowInterface.hpp
* \author Alexandre Ribard
* \date Feb 2016
*/

/** @defgroup WINDOW
*
* This module handles window handling
*
*/

/** @addtogroup WINDOW */
/*@{*/

/*!
*  \brief  Window Wrapper: utility class for window creating and handleing. \n
*		   Handles glfw calls \n
*
*/


namespace window
{
	/*!
	*  \brief Global window dimensions:
	*			default parameters when creating a new window: \n
	*			\n
	*			WINDOW_WIDTH, window width in pixels: size_t \n
	*			WINDOW_HEIGHT, window height in pixels: size_t \n
	*/
	//const size_t WINDOW_WIDTH = 1280, WINDOW_HEIGHT = 720; // 720p
	const size_t WINDOW_WIDTH = 1920, WINDOW_HEIGHT = 1080; // 1080p : 1920 x 1080


	/*!
	*  \brief Window Interface: \n
	*		rendering viewport \n
	*/
	/*!
	*
	*	\code{.cpp}
	*			// Create OpenGL Window
	*			window::Window window(window::WINDOW_WIDTH, window::WINDOW_HEIGHT, "My Window!");
	*			...
	*			...
	*			// Game loop
	*			while (window.isOpen())
	*			{
	*				// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
	*				window.updateEvents();
	*				...
	*				...
	*				// Swap the screen buffers
	*				window.draw();
	*			}
	*			...
	*			...
	*			window.isClosed();
	*	\endcode
	*/
	class Window
	{
	public:
		///////////////////////////////////////////
		//	CONSTUCTOR & DESTRUCTOR
		///////////////////////////////////////////
		/*!
		*  \brief Default Constructor: \n
		*		Sets up viewport
		*
		* \param const size_t width = WINDOW_WIDTH: viewport width
		* \param const size_t height = WINDOW_HEIGHT: viewport height
		* \param const std::string name = "": viewport winodw name
		* \return creates and sets up glfw viewport window
		*/
		Window(const size_t width = WINDOW_WIDTH, const size_t height = WINDOW_HEIGHT, const std::string name = "");

		///////////////////////////////////////////
		//	GETTERS
		///////////////////////////////////////////
		/*!
		*  \brief Returns current controler (controler is global, cf global.hpp for thruther info) \n
		* \return controler::Controler * : current global controler
		*/
		controler::Controler * getControler();
		/*!
		*  \brief Returns in use glfw window object \n
		* \return GLFWwindow * : coresponding glfw window object
		*/
		GLFWwindow * getWindow();
		/*!
		*  \brief Returns current aspect ratio (width/height)\n
		* \return float: coresponding aspect ratio
		*/
		float aspectRatio();
		/*!
		*  \brief Returns current window width \n
		* \return size_t: coresponding width
		*/
		size_t getWidth();
		/*!
		*  \brief Returns current window height \n
		* \return size_t: coresponding height
		*/
		size_t getHeight();

		///////////////////////////////////////////
		//	SETTERS
		///////////////////////////////////////////

		///////////////////////////////////////////
		//	UTILITY
		///////////////////////////////////////////
		/*!
		*  \brief Checks is window is open or closed\n
		* \return bool : true if window is closed, false else
		*/
		bool isClosed();
		/*!
		*  \brief Checks is window is open or closed\n
		* \return bool : true if window is open, false else
		*/
		bool isOpen();
		/*!
		*  \brief polls and updates events\n
		*		Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions \n
		*		c.f: global.hpp for callbacks
		* \return poll events and call corresponding callbacks
		*/
		void updateEvents();
		/*!
		*  \brief draws on viewport rendered image \n
		* \return Swap the screen buffers
		*/
		void draw();
		/*!
		*  \brief closes viewport \n
		* \return terminate GLFW (clears any resources allocated by GLFW).
		*/
		void close();





	private:
		// Window dimensions
		size_t width, height;
		std::string name;

		// GLFW window object
		GLFWwindow * glfw_window;

	};

}

/*@}*/

}
#endif