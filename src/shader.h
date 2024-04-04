#pragma once

#include <GL/glew.h>
#include <GL/gl.h>

// load vertex and fragment shaders from files and compile them into a program
GLuint LoadShaders(const char* vertexPath, const char* fragmentPath);
