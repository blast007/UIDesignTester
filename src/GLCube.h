#pragma once

#include "GLUtils.h"

#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/transform.hpp>

class GLCube
{
    public:
        GLCube(int width, int height);
        ~GLCube();
        void resize(int width, int height);
        void drawFrame(double abstime);
    private:

        int width, height;

        GLuint program;

        GLint locProjection;
        GLint locModelView;

        GLuint vbo[2], vao;
        glm::mat4 model;

        /*  the gloabal transformation matrices */
        glm::mat4 projection;
        glm::mat4 view;
};
