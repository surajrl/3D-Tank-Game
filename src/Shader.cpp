#include "Shader.h"


/* Load shaders from file function */
GLuint Shader::LoadFromFile(std::string vertexFile, std::string fragmentFile)
{
	// Read the Vertex Shader code from the file
	std::cout << "Loading "  << vertexFile << std::endl;
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertexFile.c_str(), std::ios::in);
	if(VertexShaderStream.is_open()){
		std::string Line = "";
		while(getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}else{
		std::cout << "Cannot open "  << vertexFile << "Please check input!" << std::endl;
		getchar();
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::cout << "Loading "  << fragmentFile << std::endl;
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragmentFile.c_str(), std::ios::in);
	if(FragmentShaderStream.is_open()){
		std::string Line = "";
		while(getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}else{
		std::cout << "Cannot open "  << fragmentFile << "Please check input!" << std::endl;
		getchar();
		return 0;
	}

    // Now use compile from src function
	return Shader::LoadFromSrc(VertexShaderCode, FragmentShaderCode);
}


/* Load shaders from src function */
GLuint Shader::LoadFromSrc(std::string vertexSrc, std::string fragmentSrc)
{
	// Create the shaders
	std::cout << vertexSrc << std::endl;
	std::cout << fragmentSrc << std::endl;

    // Variables used to check and debug shaders
	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	char const * VertexSourcePointer = vertexSrc.c_str();
	GLuint VertexShaderID   = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}


	// Compile Fragment Shader
	char const * FragmentSourcePointer = fragmentSrc.c_str();
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}


	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);


	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> ProgramErrorMessage(InfoLogLength+1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}
	printf("Shader Linked\n");


    // Clean up memory
	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	printf("Shader Program Compiled\n");

    // Return program ID
	return ProgramID;
}
