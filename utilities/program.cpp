
#include <GL/glew.h>

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <program.h>


Program::Program (const GLchar* vertexShaderPath,
		const GLchar* fragmentShaderPath,
		const GLchar* geometryShaderPath)
{
	std::string vertexShaderCode, fragmentShaderCode, geometryShaderCode;
	GLuint vertexShaderH, fragmentShaderH, geometryShaderH;

	vertexShaderCode = readFile(vertexShaderPath);
	fragmentShaderCode = readFile(fragmentShaderPath);
	if (geometryShaderPath) {
		geometryShaderCode = readFile(geometryShaderPath);
	}

	const GLchar* vShaderCode = vertexShaderCode.c_str();
	const GLchar* fShaderCode = fragmentShaderCode.c_str();
	const GLchar* gShaderCode = (geometryShaderPath) ? geometryShaderCode.c_str() : nullptr;


	// Compile shaders
	GLuint vertex, fragment, geometry = GL_INVALID_INDEX;
	vertex = compileShader(GL_VERTEX_SHADER, vShaderCode);
	fragment = compileShader(GL_FRAGMENT_SHADER, fShaderCode);
	if (gShaderCode) {
		geometry = compileShader(GL_GEOMETRY_SHADER, gShaderCode);
	}

	// Link Shaders
	this->id = glCreateProgram();
	linkShaders(vertex, fragment, geometry);


	// Delete the shaders as they're linked into our program now and no longer necessery
	glDeleteShader(vertex);
	glDeleteShader(fragment);
	if (geometry != GL_INVALID_INDEX) {
		glDeleteShader(geometry);
	}
}



std::string
Program::readFile(const GLchar* fileName)
{

	std::ifstream file;
	std::stringstream buffer;
	std::string shaderCode;

	file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try {

		file.open(fileName);

		buffer << file.rdbuf();
		shaderCode = buffer.str();

		file.close();

	}

	catch (std::ifstream::failure &e) {

		std::cout << "Error when reading file: " << fileName << std::endl;
		std::cout << "ERROR: " << e.what() << std::endl;

		file.close();
	}

	return shaderCode;
}


GLuint
Program::compileShader(GLuint shaderType, const GLchar* shaderCode)
{

	GLuint shaderHandler;
	GLint success;
	GLchar infoLog[512];


	shaderHandler = glCreateShader(shaderType);
	glShaderSource(shaderHandler, 1, &shaderCode, NULL);
	glCompileShader(shaderHandler);


	glGetShaderiv(shaderHandler, GL_COMPILE_STATUS, &success);
	if(!success)
	{
		glGetShaderInfoLog(shaderHandler, 512, NULL, infoLog);

		// Ternary if to identify the currect type of vertex.
		std::string name = (shaderType == GL_VERTEX_SHADER) ? "Vertex Shader" :
				(shaderType == GL_FRAGMENT_SHADER) ? "Fragment Shader" : "Geometry Shader";


		std::cout << "Error while compiling " << name << ":" << std::endl;
		std::cout << infoLog << std::endl;
	}


	return shaderHandler;
}


void
Program::linkShaders(GLuint vertexShaderH, GLuint fragmentShaderH, GLuint geometryShaderH)
{


	GLint success;
	GLchar infoLog[1024];

	glAttachShader(this->id, vertexShaderH);
	glAttachShader(this->id, fragmentShaderH);
	if (geometryShaderH != GL_INVALID_INDEX) {
		glAttachShader(this->id, geometryShaderH);
	}

	// Link shaders to the program
	glLinkProgram(this->id);

	glGetProgramiv(this->id, GL_LINK_STATUS, &success);
	if(!success)
	{
		glGetProgramInfoLog(this->id, 1024, NULL, infoLog);
		std::cout << "Error while Linking: " << std::endl;
		std::cout << infoLog << std::endl;
	}

}



