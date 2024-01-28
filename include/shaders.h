#pragma once
unsigned int buildShaderProgram(const char* vertexPath, const char* fragmentPath);
unsigned int compileVertexShader(const char* vertexShaderSource);
unsigned int compileFragmentShader(const char* fragmentShaderSource);