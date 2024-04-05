#include "shader.h"
#include <sstream>
#include <fstream>
#include <iostream>

// load a single shader
GLuint LoadSingleShader(GLuint type, const char* path)
{
    printf("loading shader %s\n", path);
    GLuint shader = glCreateShader(type);
    if(!shader) return 0;

    std::ifstream file(path);
    if(!file)
    {
        printf("couldln't open shader %s\n", path);
        return 0;
    }

    std::stringstream code;
    code << file.rdbuf();
    std::string codestr = code.str();

    const char* pcode = codestr.c_str();
    glShaderSource(shader, 1, &pcode, 0);

    glCompileShader(shader);
    int  success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        const char* typestr;
        if(type == GL_VERTEX_SHADER) typestr = "vertex";
        if(type == GL_FRAGMENT_SHADER) typestr = "fragment";
        std::cout << "ERROR: could not compile " << typestr << " shader.\n" << infoLog << std::endl;
    }

    // std::cout << codestr.c_str() << std::endl;
    return shader;
}

GLuint LoadShaders(const char *vertexPath, const char *fragmentPath)
{
    GLuint VertID = LoadSingleShader(GL_VERTEX_SHADER, vertexPath);
    GLuint FragID = LoadSingleShader(GL_FRAGMENT_SHADER, fragmentPath);

    GLuint program = glCreateProgram();
    glAttachShader(program, VertID);
    glAttachShader(program, FragID);
    glLinkProgram(program);


    int  success;
    char infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cout << "ERROR: could not link shader program.\n" << infoLog << std::endl;
        glDeleteProgram(program);
        program = 0;
    }

    glDeleteShader(VertID);
    glDeleteShader(FragID);

    return program;
}
