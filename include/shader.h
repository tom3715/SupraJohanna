#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
using namespace std;

#include <GL/glew.h>
#include "shell.h"

Log shader_logger("shader.log");

class Shader {
public:
    GLuint Program;

    Shader(const GLchar* vertexPath, const GLchar* fragmentPath) {
        // 1. Retrieve shader code from source
        string vertexCode;
        string fragmentCode;
        try {
            ifstream vShaderFile(vertexPath);
            ifstream fShaderFile(fragmentPath);
            stringstream vShaderStream, fShaderStream;
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            vShaderFile.close();
            fShaderFile.close();
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        } catch(exception e) {
            printf("ERROR::SHADER::COULD_NOT_READ_FROM_SOURCE\n");
        }
        const GLchar* vShaderCode = vertexCode.c_str();
        const GLchar* fShaderCode = fragmentCode.c_str();

        // 2. Compile shaders
        GLuint vertex, fragment;
        GLint success;
        GLchar infoLog[2048];
        // Vertex
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
        if(!success) {
            glGetShaderInfoLog(vertex, 2048, NULL, infoLog);
            printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n");
            shader_logger.gl_log("ERROR::SHADER::%u::LOG:\n%s\n", vertex, infoLog);
        }
        // Fragment
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
        if(!success) {
            glGetShaderInfoLog(fragment, 2048, NULL, infoLog);
            printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n");
            shader_logger.gl_log("ERROR::SHADER::%u::LOG:\n%s\n", vertex, infoLog);
        }

        // 3. Shader Program
        this->Program = glCreateProgram();
        glAttachShader(this->Program, vertex);
        glAttachShader(this->Program, fragment);
        glLinkProgram(this->Program);
        glGetProgramiv(this->Program, GL_LINK_STATUS, &success);
        if(!success) {
            glGetProgramInfoLog(this->Program, 2048, NULL, infoLog);
            printf("ERROR::SHADER::PROGRAM::LINKING_FAILED\n");
            shader_logger.gl_log("ERROR::SHADER::%u::LOG:\n%s\n", this->Program, infoLog);
        }

        // 4. Delete Shaders
        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }

    void Use() {
        glUseProgram(this->Program);
    }

    void print_all() {
        shader_logger.gl_log("\nINFO::SHADER::%i::\n", this->Program);
        int params;
        glGetProgramiv(this->Program, GL_LINK_STATUS, &params);
        shader_logger.gl_log("GL_LINK_STATUS = %i\n", params);

        glGetProgramiv(this->Program, GL_ATTACHED_SHADERS, &params);
        shader_logger.gl_log("GL_ATTACHED_SHADERS = %i\n", params);

        glGetProgramiv(this->Program, GL_ACTIVE_ATTRIBUTES, &params);
        shader_logger.gl_log("GL_ACTIVE_ATTRIBUTES = %i\n", params);

        for(GLuint i = 0; i < (GLuint)params; i++) {
            char name[64];
            int actual_length = 0;
            int size = 0;
            GLenum type;
            glGetActiveAttrib(this->Program, i, 64, &actual_length, &size, &type, name);
            if(size > 1) {
                for(int j = 0; j < size; j++) {
                    char long_name[64];
                    sprintf(long_name, "%s[%i]", name, j);
                    int location = glGetAttribLocation(this->Program, long_name);
                    shader_logger.gl_log(" %i) type:%s name:%s location:%i\n", i, GL_type_to_string(type), long_name, location);
                }
            } else {
                int location = glGetAttribLocation(this->Program, name);
                shader_logger.gl_log(" %i) type:%s name:%s location:%i\n", i, GL_type_to_string(type), name, location);
            }
        }

        glGetProgramiv(this->Program, GL_ACTIVE_UNIFORMS, &params);
        shader_logger.gl_log("GL_ACTIVE_UNIFORMS = %i\n", params);
        for(GLuint i = 0; i < (GLuint)params; i++) {
            char name[64];
            int actual_length = 0;
            int size = 0;
            GLenum type;
            glGetActiveUniform(this->Program, i, 64, &actual_length, &size, &type, name);
            if(size > 1) {
                for(int j = 0; j < size; j++) {
                    char long_name[64];
                    sprintf(long_name, "%s[%i]", name, j);
                    int location = glGetUniformLocation(this->Program, long_name);
                    shader_logger.gl_log(" %i) type:%s name:%s location:%i\n", i, GL_type_to_string(type), long_name, location);
                }
            } else {
                int location = glGetAttribLocation(this->Program, name);
                shader_logger.gl_log(" %i) type:%s name:%s location:%i\n", i, GL_type_to_string(type), name, location);
            }
        }
    }
};
