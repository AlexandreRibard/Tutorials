#ifndef CAMERAINTERFACE_HPP
#define CAMERAINTERFACE_HPP

////////////////////////
// GLFW
////////////////////////
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>


////////////////////////
// STL
////////////////////////
#include <string>

namespace OpenGLEngine
{

/**
* \file cameraInterface.hpp
* \author Alexandre Ribard
* \date Feb 2016
*/

/** @defgroup CAMERA
*
* This module handles camera managment
*
*/

/** @addtogroup CAMERA */
/*@{*/

namespace camera
{

	/*!
	*  \brief Global camera specification:
	*			default parameters when creating a new camera: \n
	*			\n
	*			CAMERA_POSITION, camera position in world space: 3D point (x,y,z) \n
	*			CAMERA_FOCUS, camera focus position in world space: 3D point (x,y,z) \n
	*			CAMERA_FOV, camera field of view: extent of the observable world that is seen at any given moment \n
	*/
	// Camera specs
	const glm::vec3 CAMERA_POSITION = glm::vec3(0.0f, 0.0f, 3.0f); /**< CAMERA_POSITION, 3D point glm::vec3(x,y,z) */
	const glm::vec3 CAMERA_FOCUS = glm::vec3(0.0f, 0.0f, 0.0f); /**< CAMERA_FOCUS, 3D point glm::vec3(x,y,z) */
	const float CAMERA_FOV = 45.0; /**< CAMERA_FOV, degree floating point value */
	/*!
	*	\brief Global viewing frustum : OpenGL, near and far clipping distance 
	*		NEAR_PLANE, minimal distance to the camera. All objects closer than that distance to the camera will be clipped \n
	*		FAR_PLANE, maximal distance to the camera. All objects farther than that distance to the camera will be clipped \n
	*/
	// Viewing frustum
	const float NEAR_PLANE = 1.0;
	const float FAR_PLANE = 100.0;

	/*!
	*  \brief Camera Interface: \n
	*		Interface for a camera \n
	*		Builds view matrix for camera parameters \n
	*		\n
	*		- We consider the camera as a pinhole system \n
	*		\n
	*	A camera is located at a given position (in world space). \n
	*	It's looking at a given focus point (again in world space). \n
	*	It has a given FOV. \n
	*	=> from these 3 parameters, we build the view matrix (transformation matrix from world space to camera view space), world to camera (view) space.
	*/
	class Camera
	{
	public:
		///////////////////////////////////////////
		//	CONSTUCTOR & DESTRUCTOR
		///////////////////////////////////////////
		/*!
		*  \brief Constructor form known data: \n
		*		Sets camera parameters and compute view matrix
		*
		* \param float aspect_ratio : camera aspect ration : screen_width/screen_height
		* \param glm::vec3 pos = CAMERA_POSITION : camera world space position
		* \param glm::vec3 focus = CAMERA_FOCUS : camera focus point (3D point were the camera is aiming or looking at)
		* \param float fov = CAMERA_FOV : camera field of view in degree : extent of the observable world that is seen at any given moment
		* \param std::pair<float,float> viewingFrustum : <near_plane, far_plane> : closest and farthest clipping distance to the camrea position
		* \return sets camera parameters and computes the corresponding view matrix.
		*
		* \note The camera FOV can be recovered if one knows both, the camera focal lenght, the sensor size and its focus distance: \n
		*		fov = 2 * atan(h * (do - f) / (2 * do * f) )  \n
		*		were \n
		*			- f  : camera focal length
		*			- h  : sensor size
		*			- do : focus distance
		*		or <=> fov = 2 * atan( frame_size/(f * 2 * (m +1) ) \n
		*		were m : maginfication = f / (d0 - f) \n
		*/
		Camera(float aspect_ratio = 1.0, glm::vec3 pos = CAMERA_POSITION, glm::vec3 focus = CAMERA_FOCUS, float fov = CAMERA_FOV, std::pair<float, float> viewingFrustum = std::make_pair(NEAR_PLANE, FAR_PLANE));
		


		///////////////////////////////////////////
		//	GETTERS
		///////////////////////////////////////////
		/*!
		*  \brief Returns the internal camera local frame (the frame is orthonormal) in the X direction
		* \return glm::vec3 representing the camera's X direction local frame vector
		* \note the corresponding vector is in world frame
		*/
		glm::vec3 getXAxis();
		/*!
		*  \brief Returns the internal camera local frame (the frame is orthonormal) in the Y direction
		* \return glm::vec3 representing the camera's Y direction local frame vector
		* \note the corresponding vector is in world frame
		*/
		glm::vec3 getYAxis();
		/*!
		*  \brief Returns the internal camera local frame (the frame is orthonormal) in the Z direction
		* \return glm::vec3 representing the camera's Z direction local frame vector
		* \note the corresponding vector is in world frame
		*/
		glm::vec3 getZAxis();


		/*!
		*  \brief Returns the camera's world-space position
		* \return glm::vec3 representing the camera's world-space position
		*/
		glm::vec3 getCameraPosition();
		/*!
		*  \brief Returns the camera's focusing world-space position
		* \return glm::vec3 representing the point in world space that the camera is currently focusing
		*/
		glm::vec3 getCameraFocus();
		/*!
		*  \brief Returns the camera's FOV (radian or degree)
		* \param bool rad = true: specify FOV in radian or in degree
		* \return float current camera FOV (in radian)
		*/
		float getCameraFOV(bool rad = true);
		/*!
		*  \brief Returns current view matrix (transformation matrix from world space to camera view space) \n
		*		  This matrix is build from the current FOV, camera positon and focus point.
		* \return glm::mat4 current view matrix transformation
		*/
		glm::mat4 getViewMatrix();
		/*!
		*  \brief Returns current projection matrix (transformation view space to clip space) \n
		*		  This matrix is build from the current FOV, near and far plane and a given aspect ratio.
		* \return glm::mat4 current projection transformation
		*/
		glm::mat4 getProjectionMatrix();

		/*!
		*  \brief Returns current near and far plane \n
		* \return std::pair<float,float> : respectively current near and far plane
		*/
		std::pair<float,float> getNearFarPlane();

		///////////////////////////////////////////
		//	SETTERS
		///////////////////////////////////////////
		/*!
		*  \brief Sets FOV \n
		* \param float fov : new FOV value
		* \return Updates current camera FOV.
		*/
		void setFOV(float fov);
		/*!
		*  \brief Sets near and far clipping distance \n
		* \param std::pair<float,float> viewingFrustum : pair of new near and far plane value
		* \return Updates current near and far plane.
		*/
		void setViewFurstum(std::pair<float, float> viewingFrustum);
		/*!
		*  \brief Sets aspect_ratio \n
		* \param float aspect_ratio : new aspect ratio (screen_width/screen_height) value
		* \return Updates current camera aspect ratio.
		*/
		void setAspectRatio(float aspect_ratio);
		/*!
		*  \brief Sets camera postion \n
		* \param glm::vec3 pos : new camera position (in world space) value
		* \return Updates current camera world space position.
		*/
		void setPositon(glm::vec3 pos);
		/*!
		*  \brief Sets camera focus point \n
		* \param glm::vec3 focus : new camera focusing position (in world space) value
		* \return Updates current camera focus position in world space position.
		*/
		void setFocus(glm::vec3 focus);
		/*!
		*  \brief Sets camera viewMatrix \n
		* \param glm::vec3 viewMatrix : new camera viewMatrix
		* \return Updates current camera viewMatrix
		*/
		void setViewMatrix(glm::mat4 viewMatrix);


		///////////////////////////////////////////
		//	UTILITY
		///////////////////////////////////////////
		/*!
		*  \brief Builds view matrix from current positon, fov and input focus point. \n
		*		Algorithm : \n
		*			=> we build the view matrix, which transforms world space to camera view space. \n
		*			   for this we need to compute the camera local frame\n
		*			\n
		*			We procced using a Gramm-Schmitt process to build the frame of reference
		*			-# The camera is resting at a given 3D point and is focusing at a given point \n
		*			   => cameraPos, cameraFocus\n
		*			-# We first compute the direction it's pointing at : z_localFrame \n
		*			-# given an arbytrary upward facing direction, we compute an orthogonal vector to z_localFrame : x_localFrame \n
		*			-# cross product of z_localFrame and x_localFrame to construct y_localFrame
		*
		*	For more info, http://www.3dgep.com/understanding-the-view-matrix/ and http://glm.g-truc.net/0.9.2/api/a00245.html \n
		*
		* \param glm::vec3 focus : camera focus point
		* \return builds view matrix by applying a lookAt transoformation
		*/
		void lookAt(glm::vec3 focus);



		/*!
		*  \brief Builds persepective projection from current near, far plane, fov and input aspect ratio. \n
		*		Builds a perspective projection matrix based on a field of view. \n
		*	For more info, http://glm.g-truc.net/0.9.4/api/a00151.html#ga283629a5ac7fb9037795435daf22560f \n
		*
		* \param float aspectRatio : camera aspect ratio
		* \return builds projection matrix by applying a perspective transformation
		*/
		void perspective(float aspectRatio);
		/*!
		*  \brief Builds persepective projection input aspect ratio and current camera parameters, returns computed projectionMatrix. \n
		*		Builds a perspective projection matrix based on a field of view. \n
		*
		* \param float aspectRatio : camera aspect ratio
		* \return builds and returns projection matrix by applying a perspective transformation
		*/
		glm::mat4 perspectiveProjection(float aspectRatio);

		/*!
		*  \brief Moves the camera to specified location. \n
		*		Recomputes viewMatrix. \n
		*
		* \param glm::vec3 position new camera position
		* \return moves the camera to specified location and re-compute the view matrix
		*/
		void moveTo(glm::vec3 position);



	private:
		////////////////////
		//  Camera Data
		////////////////////
		//! viewing frustum
		/*! near and farthest clipping distance
		*/
		float near_plane, far_plane;

		//! screen aspect ratio : screen_width/screen_height
		/*! ratio of the screen's height and width
		*/
		float aspect_ratio;

		//! cameraPos
		/*! camera's local frame position in world space
		*/
		glm::vec3 cameraPos;
		//! cameraFocus
		/*! camera's focusing position in world space
		*/
		glm::vec3 cameraFocus;

		//! internal camera local frame (the frame is orthonormal)
		glm::vec3 xAxis; /**< xAxis vector */
		glm::vec3 yAxis; /**< yAxis vector */
		glm::vec3 zAxis; /**< zAxis vector */

		//! view matrix
		/*! transformation matrix from world space to camera view space
		*/
		glm::mat4 viewMatrix;

		//! projection matrix
		/*! transformation matrix from eye spaceinto clip space (perspective projection)
		*/
		glm::mat4 projectionMatrix;

		//! camera FOV
		/*! camera field of view (extent of the observable world that is seen at any given moment) in degree 
		*/
		float fov;
	};

}

/*@}*/

}

#endif