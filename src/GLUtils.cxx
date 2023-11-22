#include "GLUtils.h"

GLuint GLUtils::compileShader(GLenum type, const GLchar* src)
{
    // Create a shader of the requested type
    GLuint shader = glCreateShader(type);

    // Assign the shader source
    glShaderSource(shader, 1, (const GLchar**)&src, nullptr);

    // Compile the shader
    glCompileShader(shader);

    // Get the compilation status
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

    // If the compilation failed, log the error and delete the shader
    if (status != GL_TRUE) {
        char log[16384];
        glGetShaderInfoLog(shader, sizeof(log), 0, log);
        std::string shaderType = "";
        switch(type) {
            case GL_VERTEX_SHADER:
                shaderType = "vertex";
                break;
            case GL_FRAGMENT_SHADER:
                shaderType = "fragment";
                break;
            case GL_GEOMETRY_SHADER:
                shaderType = "geometry";
                break;
            case GL_TESS_CONTROL_SHADER:
                shaderType = "tessellation control";
                break;
            case GL_TESS_EVALUATION_SHADER:
                shaderType = "tessellation evaluation";
                break;
            case GL_COMPUTE_SHADER:
                shaderType = "compute";
                break;
            default:
                shaderType = "unknown";
                break;
        }
        std::cerr << "Shader compilation failed for " << shaderType << " shader:" << std::endl;
        std::cerr << log << std::endl;
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

GLenum GLUtils::getGLError(const char *action, const char *file, const int line)
{
    GLenum error;
    GLenum lastError = GL_NO_ERROR;

    do {
        // Get the next GL error
        error = glGetError();

        if (error != GL_NO_ERROR)
        {
            lastError = error;
            if (file)
                std::cerr << "File " << file << ", ";
            if (line)
                std::cerr << "Line " << line << ", ";

            std::cerr << "OpenGL error " << glErrorToString(error) << " during " << action << std::endl;
        }
    } while (error != GL_NO_ERROR);

    return lastError;
}

std::string GLUtils::glErrorToString(GLenum error)
{
    switch(error) {
        case GL_NO_ERROR:
            return "GL_NO_ERROR";
        case GL_INVALID_ENUM:
            return "GL_INVALID_ENUM";
        case GL_INVALID_VALUE:
            return "GL_INVALID_VALUE";
        case GL_INVALID_OPERATION:
            return "GL_INVALID_OPERATION";
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            return "GL_INVALID_FRAMEBUFFER_OPERATION";
        case GL_OUT_OF_MEMORY:
            return "GL_OUT_OF_MEMORY";
        case GL_STACK_UNDERFLOW:
            return "GL_STACK_UNDERFLOW";
        case GL_STACK_OVERFLOW:
            return "GL_STACK_OVERFLOW";
        default:
            return "GL_UNKNOWN_ERROR";
    }
}
