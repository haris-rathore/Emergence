#include "shader.hpp"

Shader::Shader(){
    const std::string vertex_source = R"(
    #version 330 core

    layout (location = 0) in vec2 aPos;
    uniform float scalar;
    uniform float xoffset;
    uniform float yoffset;
    void main(){
        vec4 c1 = vec4(scalar, 0, 0, 0);
        vec4 c2 = vec4(0, scalar, 0, 0);
        vec4 c3 = vec4(0, 0, 1, 0);
        vec4 c4 = vec4(xoffset, yoffset, 0, 1);
        mat4 transformation = mat4(c1, c2, c3, c4);
        gl_Position = transformation * vec4(aPos, 0.0, 1.0);
    }
    )";

    GLuint vertex_shader = createShader(vertex_source, GL_VERTEX_SHADER);

    const std::string fragment_source = R"(
    #version 330 core
    out vec4 frag_color;

    void main(){
        frag_color = vec4(1.0, 1.0, 1.0, 1.0);
    }
    )";
    
    GLuint fragment_shader = createShader(fragment_source, GL_FRAGMENT_SHADER);

    id = glCreateProgram();
    glAttachShader(id, vertex_shader);
    glAttachShader(id, fragment_shader);
    glLinkProgram(id);

    GLint success;
    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if(!success){
        GLint buffer_size;
        glGetProgramiv(id, GL_INFO_LOG_LENGTH, &buffer_size);
        char info_log[buffer_size];
        glGetProgramInfoLog(id, buffer_size, NULL, info_log);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << info_log << '\n';
    }
}

GLuint Shader::createShader(const std::string &source, GLenum type){
    const char *source_ptr = source.c_str();
    GLuint shader_id = glCreateShader(type);
    glShaderSource(shader_id, 1, &source_ptr, NULL);
    glCompileShader(shader_id);
    
    GLint success;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
    if(!success){
        GLint buffer_size;
        glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &buffer_size);
        char info_log[buffer_size];
        glGetShaderInfoLog(shader_id, buffer_size, NULL, info_log);
        std::cerr << "ERROR::SHADER::";
        if(type == GL_VERTEX_SHADER)
            std::cerr << "VERTEX::";
        else if(type == GL_FRAGMENT_SHADER)
            std::cerr << "FRAGMENT::";
        std::cerr << "COMPILATION_FAILED\n" << info_log << '\n';
    }
    return shader_id;
}

void Shader::use(){
    glUseProgram(id);
}

void Shader::setBool(const std::string &name, bool value){
    glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
}

void Shader::setInt(const std::string &name, int value){
    glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value){
    glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}