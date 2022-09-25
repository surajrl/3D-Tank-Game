#ifndef MATRIX_H_
#define MATRIX_H_

#include <string>
#include <iostream>
#include <math.h>

#include "Vector.h"


// Forward Class Declaration
class Vector3f;

/* 4x4 Matrix Class */
class Matrix4x4 {
public:
	Matrix4x4();
	Matrix4x4(
			float v00,float v10,float v20,float v30,
			float v01,float v11,float v21,float v31,
			float v02,float v12,float v22,float v32,
			float v03,float v13,float v23,float v33);
	~Matrix4x4();

	void toIdentity();				// Creates identity matrix
	void set(Matrix4x4 matrix);		// Set matrix values

	float* getPtr();				// Return pointer to first value in matrix - Used when passing to OpenGL uniform

	static Matrix4x4 multiply(Matrix4x4 & lhs, Matrix4x4 & rhs);	// Multiply function

    Matrix4x4 inverse();
    Matrix4x4 transpose();
    float determinant();

	Matrix4x4 operator*(Matrix4x4 rhs);
    Matrix4x4 operator/(float scale);

	void print(std::string message = "");		// Print matrix

	void translate(float x, float y, float z);
	void rotate(float angle, float x, float y, float z);
	void scale(float x, float y, float z);

	void ortho(float left, float right, float bottom, float top, float zNear, float zFar);		// Orthographic projection matrix function 
	void perspective(float fovy, float aspect, float near, float far);							// Perspective projection matrix function

	void lookAt(Vector3f eye, Vector3f center, Vector3f up);	// LookAt function

private:
	float val[4][4];	// 2D Array containing values: accessed val[COLUMN][ROW]
};


#endif
