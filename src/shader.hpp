#ifndef SHADER_H
#define SHADER_H

#include <epoxy/gl.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>


class Shader{
public:
    GLuint id;
    std::string readfile(const std::string &path);
    GLuint createShader(const std::string &source, GLenum type);
public:
    Shader();

    void use();
    void setBool(const std::string &name, bool value);
    void setInt(const std::string &name, int value);
    void setFloat(const std::string &name, float value);
    
};

#endif
