#include "Vector.h"

Vector3f Vector3f::operator-(Vector3f rhs)
{
	Vector3f result;
	result.x = this->x - rhs.x;
	result.y = this->y - rhs.y;
	result.z = this->z - rhs.z;
	return result;
}


Vector3f Vector3f::operator+(Vector3f rhs)
{
	Vector3f result;
	result.x = this->x + rhs.x;
	result.y = this->y + rhs.y;
	result.z = this->z + rhs.z;
	return result;
}


Vector3f  Vector3f::operator/(float rhs)
{
	Vector3f result;
	result.x = this->x / rhs;
	result.y = this->y / rhs;
	result.z = this->z / rhs;
	return result;	
}


Vector3f  Vector3f::operator*(float rhs)
{
	Vector3f result;
	result.x = this->x * rhs;
	result.y = this->y * rhs;
	result.z = this->z * rhs;
	return result;	
}


float Vector3f::length()
{
	return sqrt(x*x + y*y + z*z);
}


Vector3f Vector3f::normalise(Vector3f v)
{
	return v / v.length();	
}

// Cross Product of Two 3D Vectors
Vector3f Vector3f::cross(Vector3f v1, Vector3f v2)
{
	Vector3f result;
	result.x = v1.y * v2.z - v2.y* v1.z;
	result.y = v1.z * v2.x - v2.z* v1.x;
	result.z = v1.x * v2.y - v2.x* v1.y;
	return result;	
}

// Dot Product of Two 3D Vectors
float Vector3f::dot(Vector3f v1, Vector3f v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}
