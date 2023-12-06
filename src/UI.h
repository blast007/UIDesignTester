#pragma once

#include "UI_SystemInterface.h"
#include "RmlUi_Renderer_GL3.h"
#include "SDL2Platform.h"

#include <RmlUi/Core.h>


class UI {
public:
    UI(int viewportWidth, int viewportHeight);
    ~UI();

    // Is the UI capturing input?
    bool isInputCaptured() const;

    // Toggle debugger
    void toggleDebugger();

    // Update before rendering
    void update();

    // Render a frame
    void render();

    // Menus
    enum Menu { eNoMenu, eMainMenu, eOptionsMenu, eHostMenu, eHelpMenu, eServerMenu, eSandbox };
    void showMenu(Menu menu);

    //
    // Callbacks to handle events
    //

    // Keyboard
    void handleKey(SDL2Platform* platform, SDL2Window* window, SDL_Keycode key, BzfKeyAction action, Uint16 mods);
    void handleText(SDL2Platform* platform, SDL2Window* window, char text[32]);

    // Mouse
    void handleMouseMove(SDL2Platform* platform, SDL2Window* window, double x, double y, Uint16 mods);
    void handleMouseButton(SDL2Platform* platform, SDL2Window* window, Uint8 button, BzfButtonAction action, Uint16 mods);
    void handleMouseScroll(SDL2Platform* platform, SDL2Window* window, double x, double y);

    // Other events
    void handleResize(SDL2Platform* platform, SDL2Window* window, int width, int height);

private:
    UI_SystemInterface* systemInterface;
    RenderInterface_GL3* renderInterface;
    Rml::Context* context;
    Rml::FontEngineInterface* fontEngine;

    // Data models
    Rml::DataModelHandle hudModel;
    Rml::DataModelHandle optionsModel;

    // Documents
    Rml::ElementDocument* gameHUD = nullptr;
    Rml::ElementDocument* mainMenu = nullptr;
    Rml::ElementDocument* optionsMenu = nullptr;
    Rml::ElementDocument* hostMenu = nullptr;
    Rml::ElementDocument* helpMenu = nullptr;
    Rml::ElementDocument* serverMenu = nullptr;
    Rml::ElementDocument* sandbox = nullptr;

    // Currently active menu
    Menu activeMenu;

    //
    // Menu navigation
    //

    enum MenuAction {
        MenuActionUp = 0,
        MenuActionRight,
        MenuActionDown,
        MenuActionLeft,
        MenuActionActivate,
        MenuActionBack,
        MenuActionNext,
        MenuActionPrevious
    };

    void loadDocuments();
    void closeDocument(Rml::ElementDocument* document);
    void hideDocument(Rml::ElementDocument* document);
    void showDocument(Rml::ElementDocument* document);

    void triggerMenuAction(MenuAction action);

    bool loadFont(Rml::String path, bool fallbackFont = false, Rml::Style::FontWeight = Rml::Style::FontWeight::Auto);

    int convertModifier(Uint16 mods);
};
