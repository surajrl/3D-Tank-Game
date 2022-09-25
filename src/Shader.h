#ifndef SHADER_H_
#define SHADER_H_

#include <GL/glew.h>

#include <string>
#include <stdio.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <stdlib.h>

/* Handles input of Vertex and Fragment Shaders from file and src */
class Shader {
public:
	// Load shaders from file
	static GLuint LoadFromFile(std::string vertexFile, std::string fragmentFile);

	// Load shaders from src
	static GLuint LoadFromSrc(std::string vertexFile, std::string fragmentFile);
};

#endif
