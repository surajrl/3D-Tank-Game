#include "Matrix.h"

#define M_PI 3.14159265358979323846


Matrix4x4::Matrix4x4()
{
	toIdentity();
}


Matrix4x4::~Matrix4x4()
{

}


Matrix4x4::Matrix4x4(
			float v00,float v10,float v20,float v30,
			float v01,float v11,float v21,float v31,
			float v02,float v12,float v22,float v32,
			float v03,float v13,float v23,float v33)
{
	val[0][0] = v00;	val[1][0] = v10;	val[2][0] = v20;	val[3][0] = v30;
	val[0][1] = v01;	val[1][1] = v11;	val[2][1] = v21;	val[3][1] = v31;
	val[0][2] = v02;	val[1][2] = v12;	val[2][2] = v22;	val[3][2] = v32;
	val[0][3] = v03;	val[1][3] = v13;	val[2][3] = v23;	val[3][3] = v33;			
}


void Matrix4x4::toIdentity()
{
	val[0][0] = 1.0;	val[1][0] = 0.0;	val[2][0] = 0.0;	val[3][0] = 0.0;
	val[0][1] = 0.0;	val[1][1] = 1.0;	val[2][1] = 0.0;	val[3][1] = 0.0;
	val[0][2] = 0.0;	val[1][2] = 0.0;	val[2][2] = 1.0;	val[3][2] = 0.0;
	val[0][3] = 0.0;	val[1][3] = 0.0;	val[2][3] = 0.0;	val[3][3] = 1.0;	
}

// Print matrix with message to ideniify
void Matrix4x4::print(std::string message)
{
	if(!message.empty())
	{
		std::cout << message << "\n";
	}
	
	for(int row = 0; row < 4; row++)
	{
		for(int col = 0; col < 4; col++)
		{
			std::cout <<  this->val[col][row] << "\t";
		}
		std::cout << "\n";
	}
	std::cout << std::endl;
}

// Set matrix values using another matrix
void Matrix4x4::set(Matrix4x4 matrix)
{
	val[0][0] = matrix.val[0][0];	val[1][0] = matrix.val[1][0];	val[2][0] = matrix.val[2][0];	val[3][0] = matrix.val[3][0];
	val[0][1] = matrix.val[0][1];	val[1][1] = matrix.val[1][1];	val[2][1] = matrix.val[2][1];	val[3][1] = matrix.val[3][1];
	val[0][2] = matrix.val[0][2];	val[1][2] = matrix.val[1][2];	val[2][2] = matrix.val[2][2];	val[3][2] = matrix.val[3][2];
	val[0][3] = matrix.val[0][3];	val[1][3] = matrix.val[1][3];	val[2][3] = matrix.val[2][3];	val[3][3] = matrix.val[3][3];
}

// Multiply function usage
Matrix4x4 Matrix4x4::multiply(Matrix4x4 & lhs, Matrix4x4 & rhs)
{
	Matrix4x4 out;
	for(int row = 0; row < 4; row++)
	{
		for(int col = 0; col < 4; col++)
		{
			out.val[col][row] = 
				lhs.val[0][row] * rhs.val[col][0] + 
				lhs.val[1][row] * rhs.val[col][1] + 
				lhs.val[2][row] * rhs.val[col][2] + 
				lhs.val[3][row] * rhs.val[col][3] ;
		}
	}
	return out;
}


float * Matrix4x4::getPtr()
{
	return &val[0][0];
}


Matrix4x4 Matrix4x4::operator*(Matrix4x4 rhs)
{
	return Matrix4x4::multiply((*this), rhs);
}


Matrix4x4  Matrix4x4::operator/(float scale)
{
    Matrix4x4 out;
    
    for(int row = 0; row < 4; row++)
        for(int col = 0; col < 4; col++)
            out.val[col][row] = this->val[col][row]/scale;

    return out;
}

// Scale Function
void Matrix4x4::scale(float x, float y, float z)
{
    // Create Scale Matrix
	Matrix4x4 scaleMatrix;
	scaleMatrix.val[0][0] = x;
	scaleMatrix.val[1][1] = y;
	scaleMatrix.val[2][2] = z;	
	
    // Set this matrix as results of this multiplied by scale matrix     	
	set(Matrix4x4::multiply(*this, scaleMatrix));
}

// Translate Function
void Matrix4x4::translate(float x, float y, float z)
{
    // Create Translate Function
	Matrix4x4 translate;
	translate.val[3][0] = x;
	translate.val[3][1] = y;
	translate.val[3][2] = z;
    
    // Set this matrix as results of this multiplied by translate matrix     	
	set(Matrix4x4::multiply(*this, translate));
}

// Rotate Function
void Matrix4x4::rotate(float angle, float x, float y, float z)
{
	// Normalise axis
	float length = sqrt(x*x + y*y + z*z);
	x /= length;
	y /= length;
	z /= length;
	
	// Convert degrees to radians
	float rads = angle * (2.f * M_PI)/360.f;
	
	// Set up variables
	float c = cos(rads);
	float s = sin(rads);

	// Construct matrix
	Matrix4x4 rotateMatrix;
	rotateMatrix.val[0][0] = x*x*(1-c)+c;	rotateMatrix.val[1][0] = x*y*(1-c)-z*s;		rotateMatrix.val[2][0] = x*z*(1-c)+y*s;		rotateMatrix.val[3][0] = 0;
	rotateMatrix.val[0][1] = y*x*(1-c)+z*s;	rotateMatrix.val[1][1] = y*y*(1-c)+c;		rotateMatrix.val[2][1] = y*z*(1-c)-x*s;		rotateMatrix.val[3][1] = 0;
	rotateMatrix.val[0][2] = x*z*(1-c)-y*s;	rotateMatrix.val[1][2] = y*z*(1-c)+x*s;		rotateMatrix.val[2][2] = z*z*(1-c)+c;		rotateMatrix.val[3][2] = 0;
	rotateMatrix.val[0][3] = 0;				rotateMatrix.val[1][3] = 0;					rotateMatrix.val[2][3] = 0;					rotateMatrix.val[3][3] = 1;
	
    // Set this matrix as result of this multiplied by rotation     	
	set(Matrix4x4::multiply(*this, rotateMatrix));	
}


// Orthographic Projection Matrix Function 
void Matrix4x4::ortho(float left, float right, float bottom, float top, float zNear, float zFar)
{
	// Row 0
	this->val[0][0] = 2.f / (right - left);			
	this->val[1][0] = 0.0;		
	this->val[2][0] = 0.0;	
	this->val[3][0] = - (right + left) / (right - left);	

	// Row 1	
	this->val[0][1] = 0.0;
	this->val[1][1] = 2.f / (top - bottom);
	this->val[2][1] = 0.0;
	this->val[3][1] = - (top + bottom) / (top - bottom);

	// Row 2
	this->val[0][2] = 0.0;
	this->val[1][2] = 0.0;
	this->val[2][2] = - 2.f / (zFar - zNear);
	this->val[3][2] = - (zFar + zNear) / (zFar - zNear);

	// Row 3
	this->val[0][3] = 0.0;
	this->val[1][3] = 0.0;
	this->val[2][3] = 0.0;
	this->val[3][3] = 1.0;	
}

// Perspective Projection Matrix Function
void Matrix4x4::perspective(float fovy, float aspect, float zNear, float zFar)
{
	float rad = fovy * (2.f * M_PI) / 360.f;
	float range = tan(rad / 2.f) * zNear;	
	float left 	= -range * aspect;
	float right	= range * aspect;
	float bottom = - range;
	float top = range;

	// Row 0
	this->val[0][0] = (2.f * zNear) / (right - left);			
	this->val[1][0] = 0.0;		
	this->val[2][0] = (right + left)/(right - left);	
	this->val[3][0] = 0.0;	

	// Row 1	
	this->val[0][1] = 0.0;
	this->val[1][1] = (2.f * zNear) / (top - bottom);
	this->val[2][1] = (top + bottom)/(top - bottom);	
	this->val[3][1] = 0.0;

	// Row 2
	this->val[0][2] = 0.0;
	this->val[1][2] = 0.0;
	this->val[2][2] = - (zFar + zNear) /(zFar - zNear);	
	this->val[3][2] = - (2.f * zFar * zNear) / (zFar - zNear);

	// Row 3
	this->val[0][3] = 0.0;
	this->val[1][3] = 0.0;
	this->val[2][3] = - 1;
	this->val[3][3] = 0.0;	
}

// LookAt Function
void Matrix4x4::lookAt(Vector3f eye, Vector3f center, Vector3f up)
{
	Vector3f f = Vector3f::normalise(center - eye);
	Vector3f u = Vector3f::normalise(up);
	Vector3f s = Vector3f::normalise(Vector3f::cross(f, u));
	u = Vector3f::cross(s, f);
	
	// Row 0
	this->val[0][0] = s.x;			
	this->val[1][0] = s.y;		
	this->val[2][0] = s.z;	
	this->val[3][0] = -Vector3f::dot(s, eye);	

	// Row 1	
	this->val[0][1] = u.x;
	this->val[1][1] = u.y;
	this->val[2][1] = u.z;
	this->val[3][1] = -Vector3f::dot(u, eye);

	// Row 2
	this->val[0][2] = -f.x;
	this->val[1][2] = -f.y;
	this->val[2][2] = -f.z;	
	this->val[3][2] = Vector3f::dot(f, eye);

	// Row 3
	this->val[0][3] = 0.0;
	this->val[1][3] = 0.0;
	this->val[2][3] = 0.0;
	this->val[3][3] = 1.0;	
}


Matrix4x4 Matrix4x4::transpose()
{
    Matrix4x4 out;

    out.val[0][0] = this->val[0][0];    out.val[1][0] = this->val[0][1];    out.val[2][0] = this->val[0][2];    out.val[3][0] = this->val[0][3];
    out.val[0][1] = this->val[1][0];    out.val[1][1] = this->val[1][1];    out.val[2][1] = this->val[1][2];    out.val[3][1] = this->val[1][3];
    out.val[0][2] = this->val[2][0];    out.val[1][2] = this->val[2][1];    out.val[2][2] = this->val[2][2];    out.val[3][2] = this->val[2][3];
    out.val[0][3] = this->val[3][0];    out.val[1][3] = this->val[3][1];    out.val[2][3] = this->val[3][2];    out.val[3][3] = this->val[3][3];

    return out;
}
 

float Matrix4x4::determinant()
{
    return     
    this->val[0][0]*this->val[1][1]*this->val[2][2]*this->val[3][3] + this->val[0][0]*this->val[2][1]*this->val[3][2]*this->val[1][3] + this->val[0][0]*this->val[3][1]*this->val[1][2]*this->val[2][3] +
    this->val[1][0]*this->val[0][1]*this->val[3][2]*this->val[2][3] + this->val[1][0]*this->val[2][1]*this->val[0][2]*this->val[3][3] + this->val[1][0]*this->val[3][1]*this->val[2][2]*this->val[0][3] +
    this->val[2][0]*this->val[0][1]*this->val[1][2]*this->val[3][3] + this->val[2][0]*this->val[1][1]*this->val[3][2]*this->val[0][3] + this->val[2][0]*this->val[3][1]*this->val[0][2]*this->val[1][3] +
    this->val[3][0]*this->val[0][1]*this->val[2][2]*this->val[1][3] + this->val[3][0]*this->val[1][1]*this->val[0][2]*this->val[2][3] + this->val[3][0]*this->val[2][1]*this->val[1][2]*this->val[0][3] -
    this->val[0][0]*this->val[1][1]*this->val[3][2]*this->val[2][3] - this->val[0][0]*this->val[2][1]*this->val[1][2]*this->val[3][3] - this->val[0][0]*this->val[3][1]*this->val[2][2]*this->val[1][3] -
    this->val[1][0]*this->val[0][1]*this->val[2][2]*this->val[3][3] - this->val[1][0]*this->val[2][1]*this->val[3][2]*this->val[0][3] - this->val[1][0]*this->val[3][1]*this->val[0][2]*this->val[2][3] -
    this->val[2][0]*this->val[0][1]*this->val[3][2]*this->val[1][3] - this->val[2][0]*this->val[1][1]*this->val[0][2]*this->val[3][3] - this->val[2][0]*this->val[3][1]*this->val[1][2]*this->val[0][3] -
    this->val[3][0]*this->val[0][1]*this->val[1][2]*this->val[2][3] - this->val[3][0]*this->val[1][1]*this->val[2][2]*this->val[0][3] - this->val[3][0]*this->val[2][1]*this->val[0][2]*this->val[1][3];
}


Matrix4x4 Matrix4x4::inverse()
{
    #define M(col,row) this->val[col-1][row-1]
    #define M3(r1,c1,r2,c2,r3,c3)M(c1,r1)*M(c2,r2)*M(c3,r3)

    Matrix4x4 a;
    a.val[0][0] = M3(2,2,3,3,4,4) + M3(2,3,3,4,4,2) + M3(2,4,3,2,4,3) - M3(2,2,3,4,4,3) - M3(2,3,3,2,4,4) - M3(2,4,3,3,4,2);
    a.val[1][0] = M3(1,2,3,4,4,3) + M3(1,3,3,2,4,4) + M3(1,4,3,3,4,2) - M3(1,2,3,3,4,4) - M3(1,3,3,4,4,2) - M3(1,4,3,2,4,3);
    a.val[2][0] = M3(1,2,2,3,4,4) + M3(1,3,2,4,4,2) + M3(1,4,2,2,4,3) - M3(1,2,2,4,4,3) - M3(1,3,2,2,4,4) - M3(1,4,2,3,4,2);
    a.val[3][0] = M3(1,2,2,4,3,3) + M3(1,3,2,2,3,4) + M3(1,4,2,3,3,2) - M3(1,2,2,3,3,4) - M3(1,3,2,4,3,2) - M3(1,4,2,2,3,3);

    a.val[0][1] = M3(2,1,3,4,4,3) + M3(2,3,3,1,4,4) + M3(2,4,3,3,4,1) - M3(2,1,3,3,4,4) - M3(2,3,3,4,4,1) - M3(2,4,3,1,4,3);
    a.val[1][1] = M3(1,1,3,3,4,4) + M3(1,3,3,4,4,1) + M3(1,4,3,1,4,3) - M3(1,1,3,4,4,3) - M3(1,3,3,1,4,4) - M3(1,4,3,3,4,1);
    a.val[2][1] = M3(1,1,2,4,4,3) + M3(1,3,2,1,4,4) + M3(1,4,2,3,4,1) - M3(1,1,2,3,4,4) - M3(1,3,2,4,4,1) - M3(1,4,2,1,4,3);
    a.val[3][1] = M3(1,1,2,3,3,4) + M3(1,3,2,4,3,1) + M3(1,4,2,1,3,3) - M3(1,1,2,4,3,3) - M3(1,3,2,1,3,4) - M3(1,4,2,3,3,1);

    a.val[0][2] = M3(2,1,3,2,4,4) + M3(2,2,3,4,4,1) + M3(2,4,3,1,4,2) - M3(2,1,3,4,4,2) - M3(2,2,3,1,4,4) - M3(2,4,3,2,4,1);
    a.val[1][2] = M3(1,1,3,4,4,2) + M3(1,2,3,1,4,4) + M3(1,4,3,2,4,1) - M3(1,1,3,2,4,4) - M3(1,2,3,4,4,1) - M3(1,4,3,1,4,2);
    a.val[2][2] = M3(1,1,2,2,4,4) + M3(1,2,2,4,4,1) + M3(1,4,2,1,4,2) - M3(1,1,2,4,4,2) - M3(1,2,2,1,4,4) - M3(1,4,2,2,4,1);
    a.val[3][2] = M3(1,1,2,4,3,2) + M3(1,2,2,1,3,4) + M3(1,4,2,2,3,1) - M3(1,1,2,2,3,4) - M3(1,2,2,4,3,1) - M3(1,4,2,1,3,2);

    a.val[0][3] = M3(2,1,3,3,4,2) + M3(2,2,3,1,4,3) + M3(2,3,3,2,4,1) - M3(2,1,3,2,4,3) - M3(2,2,3,3,4,1) - M3(2,3,3,1,4,2);
    a.val[1][3] = M3(1,1,3,2,4,3) + M3(1,2,3,3,4,1) + M3(1,3,3,1,4,2) - M3(1,1,3,3,4,2) - M3(1,2,3,1,4,3) - M3(1,3,3,2,4,1);
    a.val[2][3] = M3(1,1,2,3,4,2) + M3(1,2,2,1,4,3) + M3(1,3,2,2,4,1) - M3(1,1,2,2,4,3) - M3(1,2,2,3,4,1) - M3(1,3,2,1,4,2);
    a.val[3][3] = M3(1,1,2,2,3,3) + M3(1,2,2,3,3,1) + M3(1,3,2,1,3,2) - M3(1,1,2,3,3,2) - M3(1,2,2,1,3,3) - M3(1,3,2,2,3,1);

    #undef M
    #undef M3
 
    return a/this->determinant();   
}















