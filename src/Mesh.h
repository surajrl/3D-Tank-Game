#ifndef MESH_H_
#define MESH_H_

#include <GL/glew.h>
#include <GL/glut.h>

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>

#include "Vector.h"
#include "Matrix.h"

/* Mesh Class Input and Rendering */
class Mesh {
public:
	Mesh() : AABBPositionBuffer(0), positionBuffer(0), normalBuffer(0), texcoordBuffer(0) {};
	~Mesh(){};

	// AABB Dimensions
	Vector3f min;
	Vector3f max;

	void transformAABB(Vector3f position, Vector3f scale);

	// AABB Positions in World Space
	Vector3f transformedMin;
	Vector3f transformedMax;

    void loadOBJ(std::string filename);
	void initBuffers();

    void draw(GLuint vertexPositionAttribute, GLuint vertexNormalAttribute = -1, GLuint vertexTexcordAttribute = -1 );	// Draws mesh
	void drawAABB(GLuint vertexPositionAttribute);																		// Draws AABB of mesh

private:
	
	// Face Structure
	struct Face
	{
		std::vector<unsigned int> position_index;
		std::vector<unsigned int> normal_index;
		std::vector<unsigned int> texturecoord_index;
	};

	std::vector<Vector3f> positions;	// Mesh positions
	std::vector<Vector3f> normals;		// Mesh normals
	std::vector<Vector2f> texcoords;	// Mesh texture coordinates
	std::vector<Face> faces;			// Mesh faces

	GLuint positionBuffer;	// OpenGL vertex position buffer
	GLuint normalBuffer;	// OpenGL vertex normal buffer
	GLuint texcoordBuffer;	// OpenGL vertex texture coordinates buffer

	GLuint AABBPositionBuffer;	// OpenGL vertex position buffer for AABB
};

#endif
