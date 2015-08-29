#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <time.h>
#include <stdarg.h>

#include "camera.h"

void glfw_error_callback(int error, const char* description);
void glfw_window_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

bool enable_log = true;
bool keys[1024];
bool mouse_keys[12];

class Log {
public:
    const char* logfile;

    Log(const char* file, ...) {
        logfile = file;
        restart_gl_log();
    }

    bool restart_gl_log() {
        if(enable_log) {
            FILE* file = fopen(logfile, "w");
            if(!file) {
                fprintf(stderr, "ERROR::LOG::COULD_NOT_OPEN_LOG_FILE_FOR_WRITING\n");
                return false;
            }
            time_t now = time(NULL);
            char* date = ctime(&now);
            fprintf(file, "%s started at local time %s\n", logfile, date);
            fclose(file);
        }
        return true;
    }

    bool gl_log(const char* message, ...) {
        if(enable_log) {
            va_list argptr;
            FILE* file = fopen(logfile, "a");
            if(!file) {
                fprintf(stderr, "ERROR::LOG::COULD_NOT_OPEN_LOG_FILE_FOR_APPENDING\n");
                return false;
            }
            va_start(argptr, message);
            vfprintf(file, message, argptr);
            va_end(argptr);
            fclose(file);
        }
        return true;
    }
};

Log logger("gl.log");

class Shell {
public:
    GLuint screenWidth;
    GLuint screenHeight;
    GLFWwindow* window = NULL;
    const GLubyte* renderer = NULL;
    const GLubyte* version = NULL;
    const char* title;

    Shell(bool fullscreen, int major_version, int minor_version, int antialiasing) {
        logger.gl_log("INFO::INITIALIZATION::STARTING_GLFW3_INIT\n");
        if(!glfwInit()) {
            fprintf(stderr, "ERROR::INITIALIZATION::COULD_NOT_INIT_GLFW3\n");
        }
        logger.gl_log("INFO::GLFW::MAJOR::%i\n", major_version);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major_version);
        logger.gl_log("INFO::GLFW::MINOR::%i\n", minor_version);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor_version);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
        logger.gl_log("INFO::GLFW::ANTI_ALIASING::%i\n", antialiasing);
        glfwWindowHint(GLFW_SAMPLES, antialiasing);

        logger.gl_log("INFO::GLFW::CREATING_WINDOW\n");
        title = "MAIN-WINDOW";
        if(fullscreen) {
            const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
            screenWidth = mode->width;
            screenHeight = mode->height;
            this->window = glfwCreateWindow(screenWidth, screenHeight, title, glfwGetPrimaryMonitor(), 0);
        } else {
            screenHeight = 600;
            screenWidth = 800;
            this->window = glfwCreateWindow(screenWidth, screenHeight, title, NULL, NULL);
        }

        if(!this->window) {
            fprintf(stderr, "ERROR::INITIALIZATION::COULD_NOT_OPEN_WINDOW_WITH_GLFW3\n");
            glfwTerminate();
        }
        glfwMakeContextCurrent(this->window);

        glewExperimental = GL_TRUE;
        glewInit();
        renderer = glGetString(GL_RENDERER);
        version = glGetString(GL_VERSION);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        logger.gl_log("INFO::INITIALIZATION::DONE\n\n");
        logger.gl_log("INFO::RENDERER: %s\n", renderer);
        logger.gl_log("INFO::SUPPORTED_VERSION: %s\n", version);

        log_gl_params();
    }

    double update_fps_counter(GLFWwindow* window) {
        static double previous_seconds_update = glfwGetTime();
        static double previous_seconds = glfwGetTime();
        static int frame_count;
        double current_seconds = glfwGetTime();
        double elapsed_seconds_update = current_seconds - previous_seconds_update;
        double elapsed_seconds = current_seconds - previous_seconds;
        previous_seconds = current_seconds;
        if(elapsed_seconds_update > 0.25) {
            previous_seconds_update = current_seconds;
            double fps = (double)frame_count / elapsed_seconds_update;
            char tmp[128];
            sprintf(tmp, "%s: %.2f frames per second", title, fps);
            glfwSetWindowTitle(window, tmp);
            frame_count = 0;
        }
        frame_count++;
        return elapsed_seconds;
    }

    void log_gl_params() {
        GLenum params[] = {
            GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS,
            GL_MAX_CUBE_MAP_TEXTURE_SIZE,
            GL_MAX_DRAW_BUFFERS,
            GL_MAX_FRAGMENT_UNIFORM_COMPONENTS,
            GL_MAX_TEXTURE_IMAGE_UNITS,
            GL_MAX_TEXTURE_SIZE,
            GL_MAX_VARYING_FLOATS,
            GL_MAX_VERTEX_ATTRIBS,
            GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS,
            GL_MAX_VERTEX_UNIFORM_COMPONENTS,
            GL_MAX_VIEWPORT_DIMS,
            GL_STEREO,
        };
        const char* names[] = {
            "GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS",
            "GL_MAX_CUBE_MAP_TEXTURE_SIZE",
            "GL_MAX_DRAW_BUFFERS",
            "GL_MAX_FRAGMENT_UNIFORM_COMPONENTS",
            "GL_MAX_TEXTURE_IMAGE_UNITS",
            "GL_MAX_TEXTURE_SIZE",
            "GL_MAX_VARYING_FLOATS",
            "GL_MAX_VERTEX_ATTRIBS",
            "GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS",
            "GL_MAX_VERTEX_UNIFORM_COMPONENTS",
            "GL_MAX_VIEWPORT_DIMS",
            "GL_STEREO",
        };
        logger.gl_log("INFO::GL_CONTEXT_PARAMS:\n");
        for(int i = 0; i < 10; i++) {
            int v = 0;
            glGetIntegerv(params[i], &v);
            logger.gl_log("INFO::%s::%i\n", names[i], v);
        }
        int v[2];
        v[0] = v[1] = 0;
        glGetIntegerv(params[10], v);
        logger.gl_log("INFO::%s::%i_%i\n", names[10], v[0], v[1]);
        unsigned char s = 0;
        glGetBooleanv(params[11], &s);
        logger.gl_log("INFO::%s::%u\n", names[11], (unsigned int)s);
    }
};

Shell shell(true, 4, 1, 16);

void glfw_error_callback(int error, const char* description) {
    logger.gl_log("ERROR::GLFW::CODE:%i_%s\n", error, description);
}

void glfw_window_size_callback(GLFWwindow* window, int width, int height) {
    shell.screenWidth = width;
    shell.screenHeight = height;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    if(key >= 0 && key < 1024) {
        if(action == GLFW_PRESS)
            keys[key] = true;
        else if (action == GLFW_RELEASE)
            keys[key] = false;
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if(button >= 0 && button < 12) {
        if(action == GLFW_PRESS)
            mouse_keys[button] = true;
        else if (action == GLFW_RELEASE)
            mouse_keys[button] = false;
    }
}

bool firstMouse = true;
float lastX = 0.0f, lastY = 0.0f;
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if(firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;
    camera.ProcessMouseMovement(xoffset, yoffset);
}

const char* GL_type_to_string(GLenum type) {
    switch(type) {
        case GL_BOOL:
            return "bool";
        case GL_INT:
            return "int";
        case GL_FLOAT:
            return "float";
        case GL_FLOAT_VEC2:
            return "vec2";
        case GL_FLOAT_VEC3:
            return "vec3";
        case GL_FLOAT_VEC4:
            return "vec4";
        case GL_FLOAT_MAT2:
            return "mat2";
        case GL_FLOAT_MAT3:
            return "mat3";
        case GL_FLOAT_MAT4:
            return "mat4";
        case GL_SAMPLER_2D:
            return "sampler2D";
        case GL_SAMPLER_3D:
            return "sampler3D";
        case GL_SAMPLER_CUBE:
            return "samplerCube";
        case GL_SAMPLER_2D_SHADOW:
            return "sampler2DShadow";
        default:
            break;
    }
    return "other";
}
