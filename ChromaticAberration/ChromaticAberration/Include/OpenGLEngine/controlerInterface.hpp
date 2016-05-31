#ifndef CONTROLERINTERFACE_HPP
#define CONTROLERINTERFACE_HPP



////////////////////////
// STL
////////////////////////
#define _USE_MATH_DEFINES
#include <math.h>


namespace OpenGLEngine
{

/**
* \file controlerInterface.hpp
* \author Alexandre Ribard
* \date Feb 2016
*/

/** @defgroup CONTROLER
*
* This module handles controler managment
*
*/


/** @addtogroup CONTROLER */
/*@{*/

namespace controler
{
	/*!
	*  \brief Global controler specification:
	*			default parameters when creating a new controler: \n
	*			\n
	*			INERTIA_FACTOR, mouse inertial (resistance to any change in its state of motion): float in 0,1 \n
	*				- inertia of 0 : reaction stops after mouse movment
	*				- inertia of 1 : reaction continues forever after mouse movment
	*/
	// Controler specs
	const float INERTIA_FACTOR = 0.0;



	/*!
	*  \brief Controler Interface: \n
	*		Interface for a given controler (mouse, joystick) \n
	*		We recover the cursor's 2D position and movment across the screen as well as the scroll wheel \n
	*		We, then translate these info euler angles rotation angle aroound X (pitch) and Y (yaw) as well as a zoom factor (translation around Z) \n
	*		\n
	*	We adapt the mouse movement acroding to the screen's resolution. \n
	*	Inerta is handled: if toggled, the object will continue its previous mmovment regardless of user input. \n
	*	The transformation from controller movment and X,Y rotation and Z translation are handled as follows
	*		-# when the controler moves, we update it's relativement movement (X,Y pan and scroll).
	*		-# the pitch (angle of rotation on the X axis) is incremented by the controller relative pan on the X axis.
	*		-# the yaw (angle of rotation on the Y axis) is incremented by the controller relative pan on the Y axis.
	*		-# the Z translation factor is incremented by the relative scroll angle.
	*		-# if inertia is toggled, we increment both pitch and yaw (the X,Y rotation) by their previous amount factored by the inertia factor (0 <= inertia_factor <= 1.0)
	*/
	class Controler
	{
	public:
		///////////////////////////////////////////
		//	CONSTUCTOR & DESTRUCTOR
		///////////////////////////////////////////
		/*!
		*  \brief Constructor form known data: \n
		*		Sets controller parameters and initializes
		*
		* \param const size_t canvasWidth : canvas width in pixels
		* \param const size_t canvasHeight : canvas height in pixels
		* \param bool intertia = true : inertia toggle
		* \return sets controler parameters
		*
		* \note the canvas width and height do not nessiceraly have to be in pixels, as long as they are in the same unit as the controler position.
		*/
		Controler(const size_t canvasWidth, const size_t canvasHeight, bool intertia = true)
		{
			dX = 0.0;
			dY = 0.0;
			theta = 0.0;
			phi = 0.0;

			dScroll = 0.0;
			zoom = 0.0;

			old_x = 0.0;
			old_y = 0.0;

			drag = intertia;
			intertiaFactor = INERTIA_FACTOR;

			firstMovment = true;

			this->canvasWidth = canvasWidth;
			this->canvasHeight = canvasHeight;
		}


		///////////////////////////////////////////
		//	GETTERS
		///////////////////////////////////////////
		/*!
		*  \brief Returns the controler relativment movment in the X direction (screen space)
		* \return float representing the controler's movment vector in the X direction
		*/
		float getRelativeXMovement()
		{
			return dX;
		}
		/*!
		*  \brief Returns the controler relativment movment in the Y direction (screen space)
		* \return float representing the controler's movment vector in the Y direction
		*/
		float getRelativeYMovement()
		{
			return dY;
		}
		/*!
		*  \brief Returns pitch (rotation angle respective to the X axis) interpolated from the controller XY panning
		* \return float representing interpreted pitch (X-rotation) angle (in radian)
		*/
		float getXRotation()
		{
			return phi * M_PI / 180.0;
		}
		/*!
		*  \brief Returns yaw (rotation angle respective to the Y axis) interpolated from the controller XY panning
		* \return float representing interpreted yaw (Y-rotation) angle (in radian)
		*/
		float getYRotation()
		{
			return theta * M_PI / 180.0;
		}
		/*!
		*  \brief Returns translation factor respective to the Z axis interpolated from the controller scrolling
		* \return float representing interpreted zoom factor
		*/
		float getZoom()
		{
			return zoom;
		}


		///////////////////////////////////////////
		//	SETTERS
		///////////////////////////////////////////
		/*!
		*  \brief Toggles on/off inertia factor
		* \return if inertia was active, interia stops. Else inertia becomes active.
		*/
		bool toggleDrag()
		{
			drag != drag;
			return drag;
		}
		/*!
		*  \brief Sets interia to active
		* \return inertia is now active
		*/
		void setDrag()
		{
			drag = true;
		}
		/*!
		*  \brief Sets inertia to inactive
		* \return inertia is now inactive, previous motion stops.
		*/
		void stopDrag()
		{
			drag = false;
			dX = 0.0;
			dY = 0.0;
		}


		///////////////////////////////////////////
		//	UTILITY
		///////////////////////////////////////////
		/*!
		*  \brief Movment toggle: updates controller X,Y panning. \n
		* \param const float x : controler current X positon (in pixels)
		* \param const float y : controler current Y positon (in pixels)
		* \return computes relative movment (current_positonXY - previous_positionXY) and updates euler angles (pitch and yaw)
		*/
		void move(const float x, const float y)
		{
			if (firstMovment)
			{
				old_x = x;
				old_y = y;
				firstMovment = false;
				return;
			}


			//dX = (x - old_x) * 2.0 * M_PI / static_cast<float>(canvasWidth);
			//dY = (y - old_y) * 2.0 * M_PI / static_cast<float>(canvasHeight);

			dX = (x - old_x);
			dY = (y - old_y);

			theta += dX;
			phi += dY;
			old_x = x;
			old_y = y;
		}

		/*!
		*  \brief Zoom toggle: updates controller scroll. \n
		* \param const float delta : controler current scroll wheel rotation angle
		* \return updates zoom rotation
		*/
		void scroll(const float delta)
		{
			dScroll = delta;

			zoom += dScroll;
		}


		/*!
		*  \brief Apply inertia to euler angles and zoom \n
		* \param 
		* \return if inertia is toggled, we increment both pitch and yaw (X,Z rotations) by their previous amount factored by the inertia factor (0 <= inertia_factor <= 1.0)
		* \note zoom inertia is halved compared to X,Y rotation
		*/
		void inertia()
		{
			if (drag)
			{
				dX *= intertiaFactor;
				dY *= intertiaFactor;

				theta += dX;
				phi += dY;

				dScroll *= intertiaFactor / 2.0;

				zoom += dScroll;
			}
		}





	private:
		////////////////////
		//  Interpreted Data
		////////////////////
		//! pitch and yaw
		/*! interpreted pitch and yaw (respectively X and Y rotation angle) from controller XY-panning
		*/
		float theta, phi; /**< pitch and yaw in degree */

		//! zoom factor
		/*! interpreted zoom factor from controller XY-panning
		*/
		float zoom;

		//! scroll vector
		/*! current_scroll - previous_scroll
		*/
		float dScroll;
		//! x,y panning vector
		/*! current_pan[x,y] - previous _pan[x,y]
		*/
		float dX, dY; /**< in pixel */

		//! controler previous x,y screen space position (in pixel)
		/*! old_x : previous x position  \n
		*	old_y : previous y position
		*/
		float old_x, old_y; /**< in pixel */
		
		//! first movment toggle
		/*! temporal boundary handling: if the conroler just started moving, we need to update previous screen space position to the current one
		*/
		bool firstMovment;

		//! inertia toggle
		/*! if drag == true, inertia is applied to both yaw, pitch and zoom factor
		*/
		bool drag;
		//! inertia factor
		/*! float in [0,1] specifing the amount of inertia should be applied to euler angles and zoom factor
		*/
		float intertiaFactor;

		//! canvas width and height in pixels
		/*! will modulate euler angles and zoom acoriding to screen dimension
		*/
		size_t canvasWidth, canvasHeight;
		
	};

}

/*@}*/

}

#endif