#ifndef MODELGEOMETRY_HPP
#define MODELGEOMETRY_HPP



////////////////////////
// GLM
////////////////////////
#include <glm\glm.hpp>

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
*  \brief Vertex struct:
*			Contains all necessary information for rendering: \n
*			\n
*			Position, vertex position: 3D point (x,y,z) \n
*			Normal, vertex normal: 3D vector (x,y,z) \n
*			TexCoord, vertex texture coordinates: 2D vector (u,v) \n
*/
struct Vertex {
	glm::vec3 Position; /**< Position, vertex position: 3D point glm::vec3(x,y,z) */
	glm::vec3 Normal;   /**< Normal, vertex normal: 3D vector glm::vec3(x,y,z) */
	glm::vec2 TexCoords; /**< TexCoord, vertex texture coordinates: 2D vector glm::vec2(u,v)*/

	// Tangeant & Bi-Tangeant vector
	glm::vec3 Tangeant;
	glm::vec3 BiTangeant;
};



/*!
*  \brief Mesh Geometry Wrapper: \n
*		The geometrical data of a mesh is represented by a set of 3D points, normals and texture coordinates \n
*		These data are grouped in an array of Vertex (pos,normal,uv) \n
*		
*	This wrapper takes care of the OpenGL direction to allocate and fill the corresponding mesh VBO and VAO \n
*	Loads a mesh either from a set of vertices or a file \n
*	Calls the needed OpenGL directives to draw and dump the mesh \n
*
*/


class Geometry {
public:
	///////////////////////////////////////////
	//	CONSTUCTOR & DESTRUCTOR
	///////////////////////////////////////////
	/*!
	*  \brief Constructor form known data: \n
	*		Create VAO and VBO from an array of Vertex
	*
	* \param const std::vector<Vertex> * const verticesVec : array of Vertex [(x,y,z),(nx,ny,nz),(u,v)]
	* \param const glm::vec3 worldSpacePosition : mesh's referntial world space position (default value 0,0,0)
	* \return creates corresponding VAO and VBO
	*/
	Geometry(const std::vector<Vertex> * const verticesVec, const glm::vec3 worldSpacePosition = glm::vec3(0, 0, 0));
	/*!
	*  \brief Constructor from file: \n
	*		Parses an input .obj file and create corresponding mesh \n
	*		Builds VAO and VBO
	*
	* \param const std::string filenmane : .obj file see <https://en.wikipedia.org/wiki/Wavefront_.obj_file>
	* \param const glm::vec3 worldSpacePosition : mesh's referntial world space position (default value 0,0,0)
	* \param const float scale : geometry scaling factor. All vertices x,z,y position will be multiplied by scale
	* \return builds the geomety from file and binds corresponding VAO and VBO
	*/
	Geometry(const std::string filenmane, const glm::vec3 worldSpacePosition = glm::vec3(0, 0, 0), const float scale = 1.0);
	/*!
	*  \brief Constructs specific geometry: \n
	*		Builds a specified geometry \n
	*		Builds VAO and VBO \n
	*		Supported geometry:
	*			- Cube
	*			- Plane
	*			- Screen_Plane (aka Plane without normal)
	*		
	*
	* \param const std::string geometryType : one of the supported type (CubeGeometry, PlaneGeometry,...)
	* \param const float scale : geometry scaling factor. All vertices x,z,y position will be multiplied by scale
	* \param const glm::vec3 worldSpacePosition : mesh's referntial world space position (default value 0,0,0)
	* \return builds the geomety from file and binds corresponding VAO and VBO
	*/
	Geometry(const std::string geometryType, const float scale = 1.0, const glm::vec3 worldSpacePosition = glm::vec3(0, 0, 0));
	/*!
	*  \brief Defalut Constructor : \n
	*		Empty Mesh \n
	*		VAO and VBO are not build
	*
	* \param 
	* \return sets mesh world frame position to (0,0,0)
	*/
	Geometry();
	/*!
	*  \brief Constructor form known data: \n
	*		Create VAO and VBO from an array of float \n
	*			- Awaited vector structure : [x1, y1, z1, n1x, n2y, n3z, u1, v2, x2, y2, ... ]
	*
	* \param const std::vector<float> * const dataVec : raw data array of [(x,y,z),(nx,ny,nz),(u,v), ... ]
	* \param const glm::vec3 worldSpacePosition : mesh's referntial world space position (default value 0,0,0)
	* \return creates corresponding VAO and VBO
	*/
	Geometry(const std::vector<float> * const dataVec, const glm::vec3 worldSpacePosition = glm::vec3(0, 0, 0), const float scale = 1.0);
	/*!
	*  \brief Copy constructor: \n
	*
	* \param const Geometry &gSource : reference to a Geometry
	*/
	Geometry(Geometry &gSource);

	///////////////////////////////////////////
	//	BUILD KNOWN GEOMETRY
	///////////////////////////////////////////
	/*!
	*  \brief Supported geometry:
	*			- Cube
	*			- Plane
	*			- Screen_Plane (aka Plane without normal)
	*/
	/*!
	*  \brief Constructs a Cube of specific dimension
	*
	*
	* \param const float size: desired scale (=> cube object space coordinates will be scalled accordingly
	* \param const glm::vec3 worldSpacePosition : mesh's referntial world space position (default value 0,0,0)
	* \return builds the cube geomety, scales it and binds corresponding VAO and VBO
	*/
	void loadCubeGeometry(const float size, const glm::vec3 worldSpacePosition = glm::vec3(0, 0, 0));
	/*!
	*  \brief Constructs a Pane of specific dimension
	*
	*
	* \param const float size: desired scale (=> cube object space coordinates will be scalled accordingly
	* \param const glm::vec3 worldSpacePosition : mesh's referntial world space position (default value 0,0,0)
	* \return builds the cube geomety, scales it and binds corresponding VAO and VBO
	*/
	void loadPlaneGeometry(const float size, const glm::vec3 worldSpacePosition = glm::vec3(0, 0, 0));
	/*!
	*  \brief Constructs a Screen_Plane (Plane without normals) of specific dimension
	*
	*
	* \param const float size: desired scale (=> cube object space coordinates will be scalled accordingly
	* \param const glm::vec3 worldSpacePosition : mesh's referntial world space position (default value 0,0,0)
	* \return builds the cube geomety, scales it and binds corresponding VAO and VBO
	*/
	void loadScreenPlaneGeometry(const float size, const glm::vec3 worldSpacePosition = glm::vec3(0, 0, 0));


	///////////////////////////////////////////
	//	GETTERS
	///////////////////////////////////////////
	/*!
	*  \brief Returns the mesh's world-space position
	* \return glm::vec3 representing the mesh's world-space position
	*/
	glm::vec3 getWorldSpacePosition();
	/*!
	*  \brief Returns the mesh's raw geometric data (vector of Vertices)
	* \return std::vector<Vertex> * pointer to the mesh's vertices data (x,y,z, nx,ny,nz, u,v)
	*/
	std::vector<Vertex> * getGeometricData();
	/*!
	*  \brief Returns the mesh's corresponding VAO
	* \return GLuint mesh VAO index (generated by OpenGL allocation call)
	*/
	GLuint getVAO();
	/*!
	*  \brief Returns the mesh's corresponding VBO
	* \return GLuint mesh VBO index (generated by OpenGL allocation call)
	*/
	GLuint getVBO();


	///////////////////////////////////////////
	//	SETTERS
	///////////////////////////////////////////
	/*!
	*  \brief Sets the mesh's world-space position
	* \param const glm::vec3 worldSpacePosition : inupt mesh's world-space position
	* \return
	*/
	void setWorldSpacePosition(const glm::vec3 worldSpacePosition);


	///////////////////////////////////////////
	//	UTILITY
	///////////////////////////////////////////
	/*!
	*  \brief Renders the mesh using input shader
	* \param const Shader * const shader : inupt shader to use for rendering \link shaderInterface.hpp for details on the shaderWrapper
	* \return calls the OpenGL specific functions to render the corresponing VOA, linking it with the specified textures
	*/
	void draw();

	/*!
	*  \brief Dumps the mesh
	* \param
	* \return dumps VAO and VBO
	*/
	void dealocate();

	/*!
	*  \brief Computes Tangeant and Bit-Tangeant vectors for every vertex
	* \param
	* \return dumps VAO and VBO
	* \note Computes an approximation of the tangeant & bi-tangeant vectors by deriving them from the vartiation in texture coordinates
	*		"Computing Tangent Space Basis Vectors for an Arbitrary Mesh (Lengyel’s Method)" by Eric Lengyel
	*		C.f: http://www.terathon.com/code/tangent.html for further information
	*/
	void computeTangeant_BiTangeant();


private:
	////////////////////
	//  Mesh Data
	////////////////////
	//! worldSpacePosition
	/*! mesh's local frame position in world space
	*/
	glm::vec3 worldSpacePosition;
	//! vertices
	/*! mesh's geometrical data (array of Vertices) \n
	*	Note that the vertices are ordered as follows:
	*		< f0:{v0,v1,2} , f1:{v0,v1,v2} , ... > \n
	*		Three consecutive vertices make up a face. (vertices are repeated)
	*/
	std::vector<Vertex> vertices;


	////////////////////
	// OpenGL Render data
	////////////////////
	//! VAO, VBO
	/*! Corresonding OPENGL mesh data: VBO and VAO. \n
	*	this data will be used for rendering the mesh
	*/
	GLuint VAO, VBO;

	/*!
	*  \brief Builds a OpenGL specific representation
	* \return VBO and VAO of the corresponding mesh
	*/
	void setupMesh();
};


/*@}*/
}


#endif