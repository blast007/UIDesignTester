#include "UI.h"

#include "Models/HUDModel.h"
#include "Models/OptionsModel.h"

#include <iostream>

#include <RmlUi/Debugger.h>

UI::UI(int viewportWidth, int viewportHeight) : activeMenu(eMainMenu) {
    // Set up the system interface
    systemInterface = new UI_SystemInterface();
    Rml::SetSystemInterface(systemInterface);

    // Set up the OpenGL 3 render interface
    renderInterface = new RenderInterface_GL3();
    renderInterface->SetViewport(viewportWidth, viewportHeight);
    Rml::SetRenderInterface(renderInterface);

    // Initialize RmlUi
    Rml::Initialise();

    // Create a context
    context = Rml::CreateContext("default", Rml::Vector2i(viewportWidth, viewportHeight));

    // Initialize the debugger
    Rml::Debugger::Initialise(context);
    Rml::Debugger::SetVisible(false);

    // Add event listener
    //Rml::Factory::RegisterEventListenerInstancer(this);

    // Load some fonts
    loadFont("assets/fonts/LatoLatin-Regular.ttf");
    loadFont("assets/fonts/LatoLatin-Bold.ttf", false, Rml::Style::FontWeight::Bold);
    loadFont("assets/fonts/LatoLatin-Italic.ttf", false);
    loadFont("assets/fonts/LatoLatin-BoldItalic.ttf", false, Rml::Style::FontWeight::Bold);
    loadFont("assets/fonts/NotoEmoji-Regular.ttf", true);

    // Set up the font engine so we can calculate line heights
    fontEngine = Rml::GetFontEngineInterface();

    // Set up data models
    HUDModel::setupDataBinding(context, hudModel);
    OptionsModel::setupDataBinding(context, optionsModel);

    // Load the documents
    loadDocuments();
}

UI::~UI() {
    Rml::RemoveContext(context->GetName());
}

bool UI::isInputCaptured() const {
    // TODO: Return true if a menu is active or when typing a message

    return activeMenu != eNoMenu;
}

void UI::toggleDebugger() {
    Rml::Debugger::SetVisible(!Rml::Debugger::IsVisible());
}

void UI::update() {
    HUDModel::update(hudModel);
    OptionsModel::update(optionsModel);
}

void UI::render() {
    context->Update();
    renderInterface->BeginFrame();
    context->Render();
    renderInterface->EndFrame();
}

void UI::showMenu(Menu menu) {
    // Hide the current menu
    if (activeMenu == eMainMenu)
        hideDocument(mainMenu);
    else if (activeMenu == eOptionsMenu)
        hideDocument(optionsMenu);
    else if (activeMenu == eHostMenu)
        hideDocument(hostMenu);
    else if (activeMenu == eHelpMenu)
        hideDocument(helpMenu);
    else if (activeMenu == eServerMenu)
        hideDocument(serverMenu);
    else if (activeMenu == eSandbox)
        hideDocument(sandbox);

    // If we're leaving sandbox mode, show the HUD again.
    // Show it here so that the HUD is in the background.
    if (activeMenu == eSandbox && menu != eSandbox)
        showDocument(gameHUD);

    // Show the new menu
    if (menu == eMainMenu)
        showDocument(mainMenu);
    else if (menu == eOptionsMenu)
        showDocument(optionsMenu);
    else if (menu == eHostMenu)
        showDocument(hostMenu);
    else if (menu == eHelpMenu)
        showDocument(helpMenu);
    else if (menu == eServerMenu)
        showDocument(serverMenu);
    else if (menu == eSandbox) {
        showDocument(sandbox);
        // Hide the HUD when in sandbox mode
        hideDocument(gameHUD);
    }

    activeMenu = menu;
}

void UI::handleKey(SDL2Platform*, SDL2Window*, SDL_Keycode key, BzfKeyAction action, Uint16 mods) {
    // Reload RmlUi documents using F5 or Ctrl+R
    if (action == BZF_KEY_RELEASED && ((key == SDLK_F5) || (key == SDLK_r && mods & KMOD_CTRL))) {
        loadDocuments();

        return;
    }

    // The rest of the input should only be handled if we're capturing input in the UI
    if (!isInputCaptured())
        return;

    if (action == BZF_KEY_RELEASED) {
        if (activeMenu != eNoMenu) {
            // Trigger a back operation or closing of the menu using the Esc key
            if (key == SDLK_ESCAPE) {
                triggerMenuAction(MenuActionBack);
            }
            // Swich tabs
            else if (key == SDLK_F1 || key == SDLK_F4) {
                triggerMenuAction((key == SDLK_F1)?MenuActionPrevious:MenuActionNext);
            }
            else if (key == SDLK_UP || key == SDLK_DOWN) {
                triggerMenuAction((key == SDLK_UP)?MenuActionUp:MenuActionDown);
            }
            else if (key == SDLK_LEFT || key == SDLK_RIGHT) {
                triggerMenuAction((key == SDLK_LEFT)?MenuActionLeft:MenuActionRight);
            }
            else if (key == SDLK_RETURN || key == SDLK_KP_ENTER) {
                triggerMenuAction(MenuActionActivate);
            }
        }
    }
}

void UI::handleText(SDL2Platform *, SDL2Window *, char *text) {
    if (!isInputCaptured())
        return;

    context->ProcessTextInput(Rml::String(&text[0]));
}

void UI::handleMouseMove(SDL2Platform *, SDL2Window *, double x, double y, Uint16 mods) {
    context->ProcessMouseMove(int(x), int(y), convertModifier(mods));
}

void UI::handleMouseButton(SDL2Platform *, SDL2Window *, Uint8 button, BzfButtonAction action, Uint16 mods) {
    int rmlbutton;
    switch (button) {
        case SDL_BUTTON_LEFT: rmlbutton = 0; break;
        case SDL_BUTTON_RIGHT: rmlbutton = 1; break;
        case SDL_BUTTON_MIDDLE: rmlbutton = 2; break;
        default: rmlbutton = 3; break;
    }

    // TODO: Pass modifiers?
    if (action == BZF_BUTTON_PRESSED)
        context->ProcessMouseButtonDown(rmlbutton, convertModifier(mods));
    else
        context->ProcessMouseButtonUp(rmlbutton, convertModifier(mods));
}

void UI::handleMouseScroll(SDL2Platform *, SDL2Window *, double, double)
{
    if (!isInputCaptured())
        return;
}

void UI::handleResize(SDL2Platform *, SDL2Window *, int width, int height) {
    renderInterface->SetViewport(width, height);
    context->SetDimensions(Rml::Vector2i(width, height));
}

// This (re)loads the documents
void UI::loadDocuments() {
    // Clear the cache
    Rml::Factory::ClearStyleSheetCache();
    Rml::Factory::ClearTemplateCache();

    // (Re)load and show the HUD
    closeDocument(gameHUD);
    std::cout << "Loading HUD" << std::endl;
    gameHUD = context->LoadDocument("assets/hud/hud.rml");
    if (gameHUD != nullptr) {
        gameHUD->SetId("hud");
        // If we're not in the sandbox, show the HUD
        if (activeMenu != eSandbox)
            showDocument(gameHUD);
    }

    // (Re)load the menus and show the active menu, if any
    closeDocument(mainMenu);
    std::cout << "Loading main menu" << std::endl;
    mainMenu = context->LoadDocument("assets/menus/main.rml");
    if (activeMenu == eMainMenu)
        showDocument(mainMenu);

    closeDocument(optionsMenu);
    std::cout << "Loading options menu" << std::endl;
    optionsMenu = context->LoadDocument("assets/menus/options.rml");
    if (activeMenu == eOptionsMenu)
      showDocument(optionsMenu);

    closeDocument(hostMenu);
    std::cout << "Loading host menu" << std::endl;
    hostMenu = context->LoadDocument("assets/menus/host.rml");
    if (activeMenu == eHostMenu)
      showDocument(hostMenu);

    closeDocument(helpMenu);
    std::cout << "Loading help menu" << std::endl;
    helpMenu = context->LoadDocument("assets/menus/help.rml");
    if (activeMenu == eHelpMenu)
      showDocument(helpMenu);

    // The sandbox is just a document/directory to play around with to experiment with RmlUi
    closeDocument(sandbox);
    std::cout << "Loading sandbox" << std::endl;
    sandbox = context->LoadDocument("assets/sandbox/sandbox.rml");
    if (activeMenu == eSandbox)
        showDocument(sandbox);
}

void UI::closeDocument(Rml::ElementDocument *document) {
    if (document != nullptr)
        document->Close();
}

void UI::hideDocument(Rml::ElementDocument *document) {
    if (document != nullptr)
        document->Hide();
}

void UI::showDocument(Rml::ElementDocument *document) {
    if (document != nullptr)
        document->Show();
}

void UI::triggerMenuAction(MenuAction action) {
    if (action == MenuActionBack) {
        if (activeMenu == eMainMenu || activeMenu == eServerMenu) {
            showMenu(eNoMenu);
        }
        else {
            showMenu(eMainMenu);
        }
    }
}

bool UI::loadFont(Rml::String path, bool fallbackFace, Rml::Style::FontWeight weight) {
    bool success = Rml::LoadFontFace(path, fallbackFace, weight);
    if (!success)
        std::cerr << "Error loading font " << path << std::endl;
    return success;
}

int UI::convertModifier(Uint16 mods) {
    int r = 0;

    if (mods & KMOD_CTRL)
        r |= Rml::Input::KM_CTRL;

    if (mods & KMOD_SHIFT)
        r |= Rml::Input::KM_SHIFT;

    if (mods & KMOD_ALT)
        r |= Rml::Input::KM_ALT;

    if (mods & KMOD_NUM)
        r |= Rml::Input::KM_NUMLOCK;

    if (mods & KMOD_CAPS)
        r |= Rml::Input::KM_CAPSLOCK;

    return r;
}
