#ifndef TEXTURE_H_
#define TEXTURE_H_

#include <GL/glew.h>
#include <GL/glut.h>

#include <string>
#include <iostream> 
#include <assert.h>
#include <vector>
#include <fstream>
#include <sstream>


class Texture {
public:
    static GLuint LoadBMP(std::string filename);
    static bool LoadBMP(std::string filename, int & width, int & height, char * &data);
     
private:    
	// Converts a four-character array to an integer, using little-endian form
	static int toInt(const char* bytes) {
		return (int)(((unsigned char)bytes[3] << 24) |
					 ((unsigned char)bytes[2] << 16) |
					 ((unsigned char)bytes[1] << 8) |
					 (unsigned char)bytes[0]);
	}
	
	// Converts a two-character array to a short, using little-endian form
	static short toShort(const char* bytes) {
		return (short)(((unsigned char)bytes[1] << 8) |
					   (unsigned char)bytes[0]);
	}
	
	// Reads the next four bytes as an integer, using little-endian form
	static int readInt(std::ifstream &input) {
		char buffer[4];
		input.read(buffer, 4);
		return toInt(buffer);
	}
	
	// Reads the next two bytes as a short, using little-endian form
	static short readShort(std::ifstream &input) {
		char buffer[2];
		input.read(buffer, 2);
		return toShort(buffer);
	}
};
	
// Just like auto_ptr, but for arrays
template<class T> class auto_array {
private:
	T* array;
	mutable bool isReleased;

public:
	explicit auto_array(T* array_ = NULL) :
		array(array_), isReleased(false) {
	}
		
	auto_array(const auto_array<T> &aarray) {
		array = aarray.array;
		isReleased = aarray.isReleased;
		aarray.isReleased = true;
	}
		
	~auto_array() {
		if (!isReleased && array != NULL) {
			delete[] array;
		}
	}
		
	T* get() const {
		return array;
	}
	
	T &operator*() const {
		return *array;
	}
	
	void operator=(const auto_array<T> &aarray) {
		if (!isReleased && array != NULL) {
			delete[] array;
		}
		array = aarray.array;
		isReleased = aarray.isReleased;
		aarray.isReleased = true;
	}
		
	T* operator->() const {
		return array;
	}
		
	T* release() {
		isReleased = true;
		return array;
	}
		
	void reset(T* array_ = NULL) {
		if (!isReleased && array != NULL) {
			delete[] array;
		}
		array = array_;
	}
	
	T* operator+(int i) {
		return array + i;
	}
	
	T &operator[](int i) {
		return array[i];
	}
};


#endif
