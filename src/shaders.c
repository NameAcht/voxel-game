#include <stdio.h>
#include <stdlib.h>
#include <glad/glad.h>
#include <string.h>
#include <unistd.h>

char* readFileFromPath(const char* path) {
	FILE* f = fopen(path, "rb");
	if (!f) {
		printf("Shader source file not found at %s\n", path);
		return 0;
	}
	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	rewind(f);
	char* str = malloc(fsize + 1);
	if (!str) {
		printf("Out of memory.\n");
		return 0;
	}
	fread(str, fsize + 1, 1, f);
	fclose(f);
	str[fsize] = 0;
	return str;
}
unsigned int compileVertexShader(const char* vertexShaderSource) {
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	int  success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s", infoLog);
	}
	return vertexShader;
}
unsigned int compileFragmentShader(const char* fragmentShaderSource) {
	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	int  success;
	char infoLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s", infoLog);
	}
	return fragmentShader;
}
unsigned int buildShaderProgram(const char* vertFileName, const char* fragFileName) {
	// vertex shader
	char vertPath[1024];
	char* _ = getcwd(vertPath, 1024);
	strcat(vertPath, "\\shaders\\");
	strcat(vertPath, vertFileName);

	// fragment shader
	char fragPath[1024];
	_ = getcwd(fragPath, 1024);
	strcat(fragPath, "\\shaders\\");
	strcat(fragPath, fragFileName);
	
	char* vertSource = readFileFromPath(vertPath);
	char* fragSource = readFileFromPath(fragPath);

	unsigned int vertShader = compileVertexShader(vertSource);
	unsigned int fragShader = compileFragmentShader(fragSource);

	unsigned int program;
	program = glCreateProgram();

	// link shaders and program
	glAttachShader(program, vertShader);
	glAttachShader(program, fragShader);
	glLinkProgram(program);

	// delete shader
	glDeleteShader(vertShader);
	glDeleteShader(fragShader);

	return program;
}