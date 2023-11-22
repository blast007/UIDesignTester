#include <iostream>

#include <fmt/core.h>


#include "PlatformFactory.h"
#include "bzicon.h"

#include "GLCube.h"


#define MESSAGE_LEN 1024



struct App {
    // Praise cube!
    GLCube* cube;
};
void resize_callback(BzfPlatform* /*platform*/, BzfWindow* window, int width, int height)
{
    window->makeContextCurrent();

    auto app = static_cast<App*>(window->getUserPointer());
    app->cube->resize(width, height);
}

int main()
{
    srandom(time(nullptr));



    // Get the platform factory
    BzfPlatform* platform = PlatformFactory::get();

    // Set OpenGL attributes
    platform->GLSetVersion(BzfGLCore, 3, 2);

    // Request 8 bits per channel
    platform->GLSetRGBA(8, 8, 8, 8);

    // Get monitors
    auto monitors = platform->getMonitors();

    // Calculate starting position
    int startX = 40;
    for (size_t i = 0; i < monitors.size() - 1; ++i) {
        auto resolution = platform->getCurrentResolution(monitors.at(i));
        startX += resolution.width;
    }

    // Create window
    BzfWindow* window = platform->createWindow(700, 550, nullptr, startX, 40);
    window->setMinSize(640, 480);
    window->setTitle("UITest");
    window->setIcon(&icon);

    // Make the OpenGL context of this window the current one
    window->makeContextCurrent();

    // Get the window dimensions
    int width, height;
    window->getWindowSize(width, height);

    // Create the app container
    App* app = new App();

    // Create and assign the GLCube instance
    app->cube = new GLCube(width, height);


    // Create and assign the UI instance
    app->ui = new UI(width, height, app->data);*/

    // Set user pointer for this window
    window->setUserPointer(app);

    // Callbacks
    using namespace std::placeholders;
    platform->addResizeCallback(resize_callback);



    auto pJoystick = platform->getJoystick();
    auto joysticks = pJoystick->getJoysticks();
    if (joysticks.size() > 0) {
        pJoystick->openDevice(joysticks.at(0)->id);
    }
    while (platform->isGameRunning())
    {
        platform->pollEvents();
        window->makeContextCurrent();
        auto application = static_cast<App*>(window->getUserPointer());

        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        GL_ERROR_DBG("set clear color");
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        GL_ERROR_DBG("clear buffers");

        application->cube->drawFrame(platform->getGameTime());

        window->swapBuffers();
    }

    // Cleanup
    auto application = static_cast<App*>(window->getUserPointer());
    delete application->cube;
    delete application;
    window->setUserPointer(nullptr);

    // Delete platform factory
    delete platform;
    platform = nullptr;

    // We out!
    return 0;
}
