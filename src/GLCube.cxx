#include "GLCube.h"

#include <iostream>
#include <string.h>
#include <stdexcept>

// Makes use of some code from https://github.com/derhass/HelloCube which unfortunately does not list a license


GLCube::GLCube(int width, int height)
{
    const GLchar* vertexShaderSource = R"glsl(
        #version 150 core

        uniform mat4 modelView;
        uniform mat4 projection;

        in vec3 pos;
        in vec4 clr;

        out vec4 v_clr;

        void main()
        {
            v_clr = clr;
            gl_Position = projection * modelView * vec4(pos, 1.0);
        }
    )glsl";

    const GLchar* fragmentShaderSource = R"glsl(
        #version 150 core

        in vec4 v_clr;

        out vec4 color;

        void main()
        {
            color = v_clr;
        }
    )glsl";

    // Compile the shaders
    GLuint vertexShader = GLUtils::compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragmentShader = GLUtils::compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    // If either shader failed to compile, bail out
    if (!vertexShader || !fragmentShader)
    {
        std::cerr << "Exiting due to previous shader compilation failure(s)" << std::endl;
        GL_ERROR_DBG("shader compilation");
        throw std::invalid_argument("Shaders failed to compile");
    }

    // Create the program
    program = glCreateProgram();

    // Attach shaders
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    // hard code attribute indices
    glBindAttribLocation(program, 0, "pos");
    glBindAttribLocation(program, 1, "nrm");
    glBindAttribLocation(program, 2, "clr");
    glBindAttribLocation(program, 3, "tex");

    // Link the program
    glLinkProgram(program);
    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status != GL_TRUE)
    {
        char log[16384];
        glGetProgramInfoLog(program, sizeof(log), 0, log);
        std::cerr << "Program link failed:" << std::endl;
        std::cerr << log << std::endl;
        glDeleteProgram(program);
        program = 0;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    //glReleaseShaderCompiler();

    if (!program)
    {
        std::cerr << "Exiting due to program link failure" << std::endl;
        GL_ERROR_DBG("program link");
        throw std::invalid_argument("Program failed to link");
    }

    glUseProgram(program);
    glValidateProgram(program);

    GL_ERROR_DBG("attribute/uniform stuff");

    typedef struct {
        GLfloat pos[3]; /* 3D cartesian coordinates */
        GLubyte clr[4]; /* RGBA (8bit per channel is typically enough) */
    } Vertex;

    const Vertex cubeGeometry[] = {
        /*   X     Y     Z       R    G    B    A */
        /* front face */
        {{-1.0, -1.0,  1.0},  {255,   0,   0, 255}},
        {{ 1.0, -1.0,  1.0},  {192,   0,   0, 255}},
        {{-1.0,  1.0,  1.0},  {192,   0,   0, 255}},
        {{ 1.0,  1.0,  1.0},  {128,   0,   0, 255}},
        /* back face */
        {{ 1.0, -1.0, -1.0},  {  0, 255, 255, 255}},
        {{-1.0, -1.0, -1.0},  {  0, 192, 192, 255}},
        {{ 1.0,  1.0, -1.0},  {  0, 192, 192, 255}},
        {{-1.0,  1.0, -1.0},  {  0, 128, 128, 255}},
        /* left  face */
        {{-1.0, -1.0, -1.0},  {  0, 255,   0, 255}},
        {{-1.0, -1.0,  1.0},  {  0, 192,   0, 255}},
        {{-1.0,  1.0, -1.0},  {  0, 192,   0, 255}},
        {{-1.0,  1.0,  1.0},  {  0, 128,   0, 255}},
        /* right face */
        {{ 1.0, -1.0,  1.0},  {255,   0, 255, 255}},
        {{ 1.0, -1.0, -1.0},  {192,   0, 192, 255}},
        {{ 1.0,  1.0,  1.0},  {192,   0, 192, 255}},
        {{ 1.0,  1.0, -1.0},  {128,   0, 128, 255}},
        /* top face */
        {{-1.0,  1.0,  1.0},  {  0,   0, 255, 255}},
        {{ 1.0,  1.0,  1.0},  {  0,   0, 192, 255}},
        {{-1.0,  1.0, -1.0},  {  0,   0, 192, 255}},
        {{ 1.0,  1.0, -1.0},  {  0,   0, 128, 255}},
        /* bottom face */
        {{ 1.0, -1.0,  1.0},  {255, 255,   0, 255}},
        {{-1.0, -1.0,  1.0},  {192, 192,   0, 255}},
        {{ 1.0, -1.0, -1.0},  {192, 192,   0, 255}},
        {{-1.0, -1.0, -1.0},  {128, 128,   0, 255}},
    };

    /* use two triangles sharing an edge for each face */
    const GLushort cubeConnectivity[] = {
         0, 1, 2,  2, 1, 3,	/* front */
         4, 5, 6,  6, 5, 7,	/* back */
         8, 9,10, 10, 9,11,	/* left */
        12,13,14, 14,13,15,	/* right */
        16,17,18, 18,17,19,	/* top */
        20,21,22, 22,21,23	/* bottom */
    };

    glGenVertexArrays(1, &vao);
    GL_ERROR_DBG("vao generation");
    glBindVertexArray(vao);
    GL_ERROR_DBG("vao binding");

    glGenBuffers(2, vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeGeometry), cubeGeometry, GL_STATIC_DRAW);
    GL_ERROR_DBG("geometry vbo generation/binding");

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeConnectivity), cubeConnectivity, GL_STATIC_DRAW);
    GL_ERROR_DBG("connectivity vbo generation/binding");

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void*)(3*sizeof(GLfloat)));
    GL_ERROR_DBG("vertex attribution pointers");

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(2);
    GL_ERROR_DBG("enabling vertex attributes");

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    GL_ERROR_DBG("binding vertex buffers?");

    model = glm::mat4(1.0f);

    locProjection = glGetUniformLocation(program, "projection");
    locModelView = glGetUniformLocation(program, "modelView");

    GL_ERROR_DBG("cube initialization");

    resize(width, height);
    GL_ERROR_DBG("resizing");
}


GLCube::~GLCube()
{
    glBindVertexArray(0);
    if (vao)
    {
        glDeleteVertexArrays(1, &vao);
        vao = 0;
    }

    if (vbo[0] || vbo[1])
    {
        glDeleteBuffers(2, vbo);
        vbo[0] = vbo[1] = 0;
    }

    glDeleteProgram(program);
    program = 0;
}


void GLCube::resize(int _width, int _height)
{
    width = _width;
    height = _height;

    glViewport(0, 0, width, height);
    GL_ERROR_DBG("set GL viewport");
}


void GLCube::drawFrame(double abstime)
{
    glEnable(GL_DEPTH_TEST);
    GL_ERROR_DBG("enabling depth test");

    glUseProgram(program);
    GL_ERROR_DBG("draw - use program");

    static double lastTime = abstime;
    double timeDelta = abstime - lastTime;
    lastTime = abstime;

    model = glm::rotate(model, (float)(glm::half_pi<double>() * timeDelta), glm::vec3(0.8f, 0.6f, 0.1f));

    projection = glm::perspective(glm::radians(75.0f), (float)width / (float)height, 0.1f, 10.0f);
    view = glm::translate(glm::vec3(0.0f, 0.0f, -4.0f));

    /* combine model and view matrices to the modelView matrix our shader expects */
    glm::mat4 modelView = view * model;

    /* use the program and update the uniforms */
    glUniformMatrix4fv(locProjection, 1, GL_FALSE, glm::value_ptr(projection));
    GL_ERROR_DBG("draw - set location projection");
    glUniformMatrix4fv(locModelView, 1, GL_FALSE, glm::value_ptr(modelView));
    GL_ERROR_DBG("draw - set model view matrix");

    /* draw the cube */
    glBindVertexArray(vao);
    GL_ERROR_DBG("draw - bind vao");
    glDrawElements(GL_TRIANGLES, 6 * 6, GL_UNSIGNED_SHORT, (void*)0);
    GL_ERROR_DBG("draw - draw elements");

    /* "unbind" the VAO and the program. We do not have to do this.
    * OpenGL is a state machine. The last binings will stay effective
    * until we actively change them by binding something else. */
    glBindVertexArray(0);
    GL_ERROR_DBG("draw - unbind vertex array");

    glDisable(GL_DEPTH_TEST);
    GL_ERROR_DBG("disabling depth test");
}



