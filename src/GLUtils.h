#pragma once

#include <glad/glad.h>
#include <iostream>
#include <string>

#define GL_ERROR_DBG(action) GLUtils::getGLError(action, __FILE__, __LINE__)

class GLUtils
{
    public:
        static GLuint compileShader(GLenum type, const GLchar* src);
        static GLenum getGLError(const char *action, const char *file = nullptr, const int line = 0);
    private:
        static std::string glErrorToString(GLenum error);
};
