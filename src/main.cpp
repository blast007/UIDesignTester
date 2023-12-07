#include "PlatformFactory.h"
#include "bzicon.h"
#include "GLCube.h"
#include "UI.h"

#include "Models/HUDModel.h"
#include <fmt/format.h>

#include <iostream>
#include <stdlib.h>
#include <time.h>

#define MESSAGE_LEN 1024


SDL2Window* window;
GLCube* cube;
UI* ui;

int player_index = 0;

void handleKey(SDL2Platform* /*platform*/, SDL2Window* /*window*/, SDL_Keycode key, BzfKeyAction action, Uint16 mods)
{
    //printf("Key event %d (%s), action %d and modifiers %d\n", key, getKeyName(key), action, mods);

    if (action == BZF_KEY_PRESSED && key == SDLK_F12) {
        window->requestClose();
    }
    else if (action == BZF_KEY_RELEASED && !(mods & KMOD_CTRL) && !(mods & KMOD_SHIFT) && !(mods & KMOD_ALT))
    {
        if (key == SDLK_BACKQUOTE) {
            ui->showMenu(UI::eSandbox);
        }
        else if (key == SDLK_1) {
            ui->showMenu(UI::eMainMenu);
        }
        else if (key == SDLK_2) {
            ui->showMenu(UI::eOptionsMenu);
        }
        else if (key == SDLK_3) {
            ui->showMenu(UI::eHostMenu);
        }
        else if (key == SDLK_4) {
            ui->showMenu(UI::eHelpMenu);
        }
        else if (key == SDLK_5) {
            ui->showMenu(UI::eServerMenu);
        }
        else if (key == SDLK_0) {
            ui->showMenu(UI::eNoMenu);
        }
        else if (key == SDLK_TAB) {
            ui->toggleDebugger();
        }
        else if (key == SDLK_q) {
            HUDModel::setLocalPlayerName("Somebody");
        }
        else if (key == SDLK_w) {
            HUDModel::setLocalPlayerScore(1337);
        }
        else if (key == SDLK_e) {
            HUDModel::setLocalPlayerScore((rand() % 200) - 100);
        }
        else if (key == SDLK_r) {
            HUDModel::addRemotePlayer(player_index, fmt::format("Player {}", player_index+1), TeamColor(RogueTeam + (rand() % 4)));
            player_index++;
        }
        else if (key == SDLK_a) {
            if (player_index > 0)
                HUDModel::setRemotePlayerKills(rand() % player_index, rand() % 100);
        }
        else if (key == SDLK_s) {
            if (player_index > 0)
                HUDModel::setRemotePlayerDeaths(rand() % player_index, rand() % 40);
        }
        else if (key == SDLK_d) {
            if (player_index > 0)
                HUDModel::setRemotePlayerTeamkills(rand() % player_index, rand() % 10);
        }
        else if (key == SDLK_f) {
            if (player_index > 0)
                HUDModel::setRemotePlayerFlag(rand() % player_index, (rand() % 2 == 1)?"GM":"L");
        }
    }
}

void handleResize(SDL2Platform* /*platform*/, SDL2Window* window, int width, int height)
{
    window->makeContextCurrent();
    cube->resize(width, height);
}

int main()
{
    // Seed some randomness
    srand(time(nullptr));

    // Get the platform factory
    SDL2Platform* platform = PlatformFactory::get();

    // Set OpenGL attributes
    platform->GLSetVersion(BzfGLCore, 3, 2);

    // Request 8 bits per channel
    platform->GLSetRGBA(8, 8, 8, 8);

    // Get monitors
    auto monitors = platform->getMonitors();

    // Calculate starting position
    int startX = 40;
    for (size_t i = 0; i < monitors.size() - 1; ++i) {
        auto resolution = platform->getCurrentResolution(i);
        startX += resolution.width;
    }

    // Create window
    window = platform->createWindow(700, 550, -1, startX, 40);
    window->setMinSize(640, 480);
    window->setTitle("UI Design Tester");
    window->setIcon(&icon);

    // Make the OpenGL context of this window the current one
    window->makeContextCurrent();

    // Get the window dimensions
    int width, height;
    window->getWindowSize(width, height);

    // Create and assign the GLCube instance
    cube = new GLCube(width, height);

    // Set up the UI
    ui = new UI(width, height);

    // Add some sample data
    HUDModel::setLocalPlayerName("Somebody");
    HUDModel::addRemotePlayer(0, "Ready Player One", RedTeam);
    player_index++;

    // Callbacks
    platform->addKeyCallback(handleKey);
    platform->addResizeCallback(handleResize);

    using namespace std::placeholders;
    platform->addKeyCallback(std::bind(&UI::handleKey, ui, _1, _2, _3, _4, _5));
    platform->addTextCallback(std::bind(&UI::handleText, ui, _1, _2, _3));

    platform->addCursorPosCallback(std::bind(&UI::handleMouseMove, ui, _1, _2, _3, _4, _5));
    platform->addMouseButtonCallback(std::bind(&UI::handleMouseButton, ui, _1, _2, _3, _4, _5));
    platform->addScrollCallback(std::bind(&UI::handleMouseScroll, ui, _1, _2, _3, _4));

    //platform->addJoystickHatCallback(std::bind(&UI::handleJoystickHat, ui, _1, _2, _3, _4));
    //platform->addJoystickButtonCallback(std::bind(&UI::handleJoystickButton, ui, _1, _2, _3, _4));

    platform->addResizeCallback(std::bind(&UI::handleResize, ui, _1, _2, _3, _4));

    while (platform->isGameRunning())
    {
        // Poll events
        platform->pollEvents();

        // Make this window the current context
        window->makeContextCurrent();

        // Clear the screen
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        GL_ERROR_DBG("set clear color");
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        GL_ERROR_DBG("clear buffers");

        // Update the UI
        ui->update();

        // Draw stuff
        cube->drawFrame(platform->getGameTime());
        ui->render();

        // Throw the new frame at the screen
        window->swapBuffers();
    }

    // Cleanup
    delete cube;
    delete ui;
    window->setUserPointer(nullptr);

    // Delete platform factory
    delete platform;
    platform = nullptr;

    // We out!
    return 0;
}
