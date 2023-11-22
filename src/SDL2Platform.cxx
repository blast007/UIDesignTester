#include "SDL2Platform.h"
#include <stdio.h>
#include <iostream>
#include <vector>
#ifdef _WIN32
#  include <windows.h>
#endif

///////////////////////////////////////////////////////////
// Platform
///////////////////////////////////////////////////////////

SDL2Platform::SDL2Platform() : audio(nullptr), joystick(nullptr)
{
    SDL_SetMainReady();
    if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_EVENTS | SDL_INIT_VIDEO) != 0)
    {
        std::cerr << "Error: There was an error initializing SDL2: " << SDL_GetError() << std::endl;
        exit(-1);
    }

    // SDL automatically starts text input when the video subsystem is initialized, so stop that
    SDL_StopTextInput();

    // Set the depth and stencil buffer precision
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
#ifdef DEBUG_RENDERING
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 4);
#else
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 1);
#endif

    // Enable double buffering
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

#ifdef _DEBUG
    // For debugging, set the start time of the program to be 184 days in the past to catch issues that may occur with long running programs
    startTime = SDL_GetPerformanceCounter() - (SDL_GetPerformanceFrequency() * 60 * 60 * 24 * 184);
#else
    startTime = SDL_GetPerformanceCounter();
#endif
}

SDL2Platform::~SDL2Platform()
{
    // Delete all the windows (Is this necessary?)
    for (auto window : windows)
        if (window != nullptr)
            delete window;

    // Shut down SDL2
    SDL_Quit();
}

BzfWindow* SDL2Platform::createWindow(int width, int height, BzfMonitor* monitor, int positionX, int positionY)
{
    SDL2Window* window = new SDL2Window(width, height, static_cast<SDL2Monitor*>(monitor), positionX, positionY);
    windows.push_back(window);
    return window;
}

BzfWindow* SDL2Platform::createWindow(BzfResolution resolution, BzfMonitor* monitor)
{
    SDL2Window* window = new SDL2Window(resolution, static_cast<SDL2Monitor*>(monitor));
    windows.push_back(window);
    return window;
}

BzfAudio* SDL2Platform::getAudio()
{
    if (audio == nullptr)
        audio = new SDL2Audio();
    return audio;
}

BzfJoystick* SDL2Platform::getJoystick()
{
    if (joystick == nullptr)
        joystick = new SDL2Joystick();
    return joystick;
}

bool SDL2Platform::isGameRunning() const
{
    for (auto window : windows)
    {
        if (window->shouldClose())
            return false;
    }

    return true;
}

double SDL2Platform::getGameTime() const
{
    return (double)(SDL_GetPerformanceCounter() - startTime) / SDL_GetPerformanceFrequency();
}

BzfMonitor* SDL2Platform::getPrimaryMonitor() const
{
    SDL2Monitor* monitor = new SDL2Monitor;
    monitor->displayIndex = 0;
    monitor->name = SDL_GetDisplayName(0);
    return monitor;
}

std::vector<BzfMonitor*> SDL2Platform::getMonitors() const
{
    std::vector<BzfMonitor*> monitors;
    int count = SDL_GetNumVideoDisplays();
    for (int i = 0; i < count; i++)
    {
        SDL2Monitor* monitor = new SDL2Monitor;
        monitor->displayIndex = i;
        monitor->name = SDL_GetDisplayName(i);
        monitors.push_back(monitor);
    }
    return monitors;
}

BzfResolution SDL2Platform::getCurrentResolution(BzfMonitor* monitor) const
{
    BzfResolution resolution;

    SDL_DisplayMode current;

    if (SDL_GetCurrentDisplayMode(static_cast<SDL2Monitor*>(monitor)->displayIndex, &current) == 0)
    {
        resolution.width = current.w;
        resolution.height = current.h;
        resolution.refreshRate = current.refresh_rate;
    }
    else
        resolution.width = resolution.height = resolution.refreshRate = 0;

    return resolution;
}

std::vector<BzfResolution> SDL2Platform::getResolutions(BzfMonitor* monitor) const
{
    std::vector<BzfResolution> resolutions;
    int displayIndex = (monitor != nullptr)?static_cast<SDL2Monitor*>(monitor)->displayIndex:0;
    int count = SDL_GetNumDisplayModes(displayIndex);
    SDL_DisplayMode mode;

    for (int i = 0; i < count; i++)
    {
        BzfResolution resolution;
        if (SDL_GetDisplayMode(displayIndex, i, &mode) == 0)
        {
            resolution.width = mode.w;
            resolution.height = mode.h;
            resolution.refreshRate = mode.refresh_rate;
            resolutions.push_back(resolution);
        }
    }

    return resolutions;
}

void SDL2Platform::GLSetVersion(BzfGLProfile profile, unsigned short majorVersion, unsigned short minorVersion) const
{
    // Set the OpenGL context profile
    if (profile == BzfGLCore)
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    else if (profile == BzfGLES)
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    else
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);

    // Set the OpenGL context requested version
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, majorVersion);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, minorVersion);
}

void SDL2Platform::GLSetRGBA(unsigned short red, unsigned short green, unsigned short blue, unsigned short alpha) const
{
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE,   red);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, green);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,  blue);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, alpha);
}

void SDL2Platform::pollEvents()
{
    if (SDL_PollEvent(NULL) == 1)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                for (auto window : windows)
                    window->requestClose();
            }
            else if (event.type == SDL_WINDOWEVENT)
            {
                auto window = getWindowFromSDLID(event.wheel.windowID);
                if (event.window.event == SDL_WINDOWEVENT_CLOSE)
                    window->requestClose();
                else if (event.window.event == SDL_WINDOWEVENT_RESIZED)
                {
                    for (auto resizeCallback : resizeCallbacks)
                        resizeCallback(this, window, event.window.data1, event.window.data2);
                }
                else if (event.window.event == SDL_WINDOWEVENT_MOVED)
                {
                    for (auto moveCallback : moveCallbacks)
                        moveCallback(this, window, event.window.data1, event.window.data2);
                }
            }
            else if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP)
            {
                if (keyCallbacks.size() > 0)
                {
                    BzfKey key = keyFromSDL(event.key.keysym.sym);
                    if (key == BZF_KEY_UNKNOWN)
                        continue;

                    BzfKeyAction action = (event.key.state == SDL_PRESSED)?BZF_KEY_PRESSED:BZF_KEY_RELEASED;
                    if (action == BZF_KEY_PRESSED && event.key.repeat != 0)
                        action = BZF_KEY_REPEATED;

                    for (auto keyCallback : keyCallbacks)
                        keyCallback(this, getWindowFromSDLID(event.key.windowID), key, action, modsFromSDL(event.key.keysym.mod));
                }
            }
            else if (event.type == SDL_JOYBUTTONDOWN || event.type == SDL_JOYBUTTONUP)
            {
                if (joystickButtonCallbacks.size() > 0)
                {
                    BzfJoyButton button;
                    // SDL starts their numbering at 0
                    switch(event.jbutton.button)
                    {
                    // *INDENT-OFF*
                    case 0: button = BZF_JOY_BUTTON_1; break;
                    case 1: button = BZF_JOY_BUTTON_2; break;
                    case 2: button = BZF_JOY_BUTTON_3; break;
                    case 3: button = BZF_JOY_BUTTON_4; break;
                    case 4: button = BZF_JOY_BUTTON_5; break;
                    case 5: button = BZF_JOY_BUTTON_6; break;
                    case 6: button = BZF_JOY_BUTTON_7; break;
                    case 7: button = BZF_JOY_BUTTON_8; break;
                    case 8: button = BZF_JOY_BUTTON_9; break;
                    case 9: button = BZF_JOY_BUTTON_10; break;
                    case 10: button = BZF_JOY_BUTTON_11; break;
                    case 11: button = BZF_JOY_BUTTON_12; break;
                    case 12: button = BZF_JOY_BUTTON_13; break;
                    case 13: button = BZF_JOY_BUTTON_14; break;
                    case 14: button = BZF_JOY_BUTTON_15; break;
                    case 15: button = BZF_JOY_BUTTON_16; break;
                    case 16: button = BZF_JOY_BUTTON_17; break;
                    case 17: button = BZF_JOY_BUTTON_18; break;
                    case 18: button = BZF_JOY_BUTTON_19; break;
                    case 19: button = BZF_JOY_BUTTON_20; break;
                    case 20: button = BZF_JOY_BUTTON_21; break;
                    case 21: button = BZF_JOY_BUTTON_22; break;
                    case 22: button = BZF_JOY_BUTTON_23; break;
                    case 23: button = BZF_JOY_BUTTON_24; break;
                    case 24: button = BZF_JOY_BUTTON_25; break;
                    case 25: button = BZF_JOY_BUTTON_26; break;
                    case 26: button = BZF_JOY_BUTTON_27; break;
                    case 27: button = BZF_JOY_BUTTON_28; break;
                    case 28: button = BZF_JOY_BUTTON_29; break;
                    case 29: button = BZF_JOY_BUTTON_30; break;
                    case 30: button = BZF_JOY_BUTTON_31; break;
                    case 31: button = BZF_JOY_BUTTON_32; break;
                    default: button = BZF_JOY_BUTTON_UNKNOWN; break;
                    // *INDENT-ON*
                    }
                    if (button == BZF_JOY_BUTTON_UNKNOWN)
                        continue;

                    for (auto joystickButtonCallback : joystickButtonCallbacks)
                        joystickButtonCallback(this, getWindowFromSDLID(event.key.windowID), button,
                                               (event.jbutton.state == SDL_PRESSED)?BZF_BUTTON_PRESSED:BZF_BUTTON_RELEASED);
                }
            }
            else if (event.type == SDL_JOYHATMOTION)
            {
                if (joystickHatCallbacks.size() > 0)
                {
                    BzfJoyHat hat;
                    // SDL starts their numbering at 0
                    switch(event.jhat.hat)
                    {
                    // *INDENT-OFF*
                    case 0: hat = BZF_JOY_HAT_1; break;
                    case 1: hat = BZF_JOY_HAT_2; break;
                    case 2: hat = BZF_JOY_HAT_3; break;
                    case 3: hat = BZF_JOY_HAT_4; break;
                    case 4: hat = BZF_JOY_HAT_5; break;
                    case 5: hat = BZF_JOY_HAT_6; break;
                    case 6: hat = BZF_JOY_HAT_7; break;
                    case 7: hat = BZF_JOY_HAT_8; break;
                    default: hat = BZF_JOY_HAT_UNKNOWN; break;
                    // *INDENT-ON*
                    }
                    if (hat == BZF_JOY_HAT_UNKNOWN)
                        continue;

                    BzfJoyHatDirection direction;
                    switch(event.jhat.value)
                    {
                    // *INDENT-OFF*
                    case SDL_HAT_LEFTUP: direction = BZF_JOY_HAT_LEFTUP; break;
                    case SDL_HAT_UP: direction = BZF_JOY_HAT_UP; break;
                    case SDL_HAT_RIGHTUP: direction = BZF_JOY_HAT_RIGHTUP; break;
                    case SDL_HAT_LEFT: direction = BZF_JOY_HAT_LEFT; break;
                    case SDL_HAT_RIGHT: direction = BZF_JOY_HAT_RIGHT; break;
                    case SDL_HAT_LEFTDOWN: direction = BZF_JOY_HAT_LEFTDOWN; break;
                    case SDL_HAT_DOWN: direction = BZF_JOY_HAT_DOWN; break;
                    case SDL_HAT_RIGHTDOWN: direction = BZF_JOY_HAT_RIGHTDOWN; break;
                    default: direction = BZF_JOY_HAT_CENTERED; break;
                    // *INDENT-ON*
                    }

                    for (auto joystickHatCallback : joystickHatCallbacks)
                        joystickHatCallback(this, getWindowFromSDLID(event.key.windowID), hat, direction);
                }
            }
            else if (event.type == SDL_TEXTINPUT)
            {
                if (SDL_strlen(event.text.text) == 0 || event.text.text[0] == '\n')
                    return;

                for (auto textCallback : textCallbacks)
                    textCallback(this, getWindowFromSDLID(event.text.windowID), event.text.text);
            }
            else if (event.type == SDL_MOUSEMOTION)
            {
                auto window = getWindowFromSDLID(event.motion.windowID);

    #ifndef _WIN32
                // For non-Windows platforms, we need to manually confine to the motion box
                if (window->getConfineMouse() == BZF_MOUSE_CONFINED_BOX)
                    window->checkMouseConfineBox(event.motion.x, event.motion.y);
    #endif

                for (auto cursorPosCallback : cursorPosCallbacks)
                    cursorPosCallback(this, window, event.motion.x, event.motion.y);
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP)
            {
                if (mouseButtonCallbacks.size() > 0)
                {
                    BzfMouseButton button;
                    // Unlike joystick buttons/hats, however, SDL starts their mouse button numbering at 1...
                    switch(event.button.button)
                    {
                    // *INDENT-OFF*
                    case 1: button = BZF_MOUSE_LEFT; break;
                    case 2: button = BZF_MOUSE_MIDDLE; break;
                    case 3: button = BZF_MOUSE_RIGHT; break;
                    case 4: button = BZF_MOUSE_4; break;
                    case 5: button = BZF_MOUSE_5; break;
                    case 6: button = BZF_MOUSE_6; break;
                    case 7: button = BZF_MOUSE_7; break;
                    case 8: button = BZF_MOUSE_8; break;
                    default: button = BZF_MOUSE_UNKNOWN; break;
                    // *INDENT-ON*
                    }
                    if (button == BZF_MOUSE_UNKNOWN)
                        continue;

                    for (auto mouseButtonCallback : mouseButtonCallbacks)
                        mouseButtonCallback(this, getWindowFromSDLID(event.button.windowID), button,
                                            (event.button.state == SDL_PRESSED)?BZF_BUTTON_PRESSED:BZF_BUTTON_RELEASED, modsFromSDL(SDL_GetModState()));
                }
            }
            else if (event.type == SDL_MOUSEWHEEL)
            {
                for (auto scrollCallback : scrollCallbacks)
                    // TODO: Take direction into account and reverse the values?
                    scrollCallback(this, getWindowFromSDLID(event.wheel.windowID), event.wheel.x, event.wheel.y);
            }
        }
    }
}

void SDL2Platform::startTextInput()
{
    SDL_StartTextInput();
}

void SDL2Platform::stopTextInput()
{
    SDL_StopTextInput();
}

bool SDL2Platform::isTextInput()
{
    return SDL_IsTextInputActive() == SDL_TRUE;
}

BzfKey SDL2Platform::keyFromSDL(SDL_Keycode key)
{
    switch (key)
    {
    // *INDENT-OFF*
    case SDLK_SPACE: return BZF_KEY_SPACE;
    case SDLK_QUOTE: return BZF_KEY_APOSTROPHE;
    case SDLK_COMMA: return BZF_KEY_COMMA;
    case SDLK_MINUS: return BZF_KEY_MINUS;
    case SDLK_PERIOD: return BZF_KEY_PERIOD;
    case SDLK_SLASH: return BZF_KEY_SLASH;
    case SDLK_0: return BZF_KEY_0;
    case SDLK_1: return BZF_KEY_1;
    case SDLK_2: return BZF_KEY_2;
    case SDLK_3: return BZF_KEY_3;
    case SDLK_4: return BZF_KEY_4;
    case SDLK_5: return BZF_KEY_5;
    case SDLK_6: return BZF_KEY_6;
    case SDLK_7: return BZF_KEY_7;
    case SDLK_8: return BZF_KEY_8;
    case SDLK_9: return BZF_KEY_9;
    case SDLK_SEMICOLON: return BZF_KEY_SEMICOLON;
    case SDLK_EQUALS: return BZF_KEY_EQUAL;
    case SDLK_a: return BZF_KEY_A;
    case SDLK_b: return BZF_KEY_B;
    case SDLK_c: return BZF_KEY_C;
    case SDLK_d: return BZF_KEY_D;
    case SDLK_e: return BZF_KEY_E;
    case SDLK_f: return BZF_KEY_F;
    case SDLK_g: return BZF_KEY_G;
    case SDLK_h: return BZF_KEY_H;
    case SDLK_i: return BZF_KEY_I;
    case SDLK_j: return BZF_KEY_J;
    case SDLK_k: return BZF_KEY_K;
    case SDLK_l: return BZF_KEY_L;
    case SDLK_m: return BZF_KEY_M;
    case SDLK_n: return BZF_KEY_N;
    case SDLK_o: return BZF_KEY_O;
    case SDLK_p: return BZF_KEY_P;
    case SDLK_q: return BZF_KEY_Q;
    case SDLK_r: return BZF_KEY_R;
    case SDLK_s: return BZF_KEY_S;
    case SDLK_t: return BZF_KEY_T;
    case SDLK_u: return BZF_KEY_U;
    case SDLK_v: return BZF_KEY_V;
    case SDLK_w: return BZF_KEY_W;
    case SDLK_x: return BZF_KEY_X;
    case SDLK_y: return BZF_KEY_Y;
    case SDLK_z: return BZF_KEY_Z;
    case SDLK_LEFTBRACKET: return BZF_KEY_LEFT_BRACKET;
    case SDLK_BACKSLASH: return BZF_KEY_BACKSLASH;
    case SDLK_RIGHTBRACKET: return BZF_KEY_RIGHT_BRACKET;
    case SDLK_BACKQUOTE: return BZF_KEY_GRAVE_ACCENT;
    //case SDLK_WORLD_1: return BZF_KEY_WORLD_1; // What are these?
    //case SDLK_WORLD_2: return BZF_KEY_WORLD_2; // What are these?
    case SDLK_ESCAPE: return BZF_KEY_ESCAPE;
    case SDLK_RETURN: return BZF_KEY_ENTER;
    case SDLK_TAB: return BZF_KEY_TAB;
    case SDLK_BACKSPACE: return BZF_KEY_BACKSPACE;
    case SDLK_INSERT: return BZF_KEY_INSERT;
    case SDLK_DELETE: return BZF_KEY_DELETE;
    case SDLK_RIGHT: return BZF_KEY_RIGHT;
    case SDLK_LEFT: return BZF_KEY_LEFT;
    case SDLK_DOWN: return BZF_KEY_DOWN;
    case SDLK_UP: return BZF_KEY_UP;
    case SDLK_PAGEUP: return BZF_KEY_PAGE_UP;
    case SDLK_PAGEDOWN: return BZF_KEY_PAGE_DOWN;
    case SDLK_HOME: return BZF_KEY_HOME;
    case SDLK_END: return BZF_KEY_END;
    case SDLK_PAUSE: return BZF_KEY_PAUSE;
    case SDLK_F1: return BZF_KEY_F1;
    case SDLK_F2: return BZF_KEY_F2;
    case SDLK_F3: return BZF_KEY_F3;
    case SDLK_F4: return BZF_KEY_F4;
    case SDLK_F5: return BZF_KEY_F5;
    case SDLK_F6: return BZF_KEY_F6;
    case SDLK_F7: return BZF_KEY_F7;
    case SDLK_F8: return BZF_KEY_F8;
    case SDLK_F9: return BZF_KEY_F9;
    case SDLK_F10: return BZF_KEY_F10;
    case SDLK_F11: return BZF_KEY_F11;
    case SDLK_F12: return BZF_KEY_F12;
    case SDLK_F13: return BZF_KEY_F13;
    case SDLK_F14: return BZF_KEY_F14;
    case SDLK_F15: return BZF_KEY_F15;
    case SDLK_F16: return BZF_KEY_F16;
    case SDLK_F17: return BZF_KEY_F17;
    case SDLK_F18: return BZF_KEY_F18;
    case SDLK_F19: return BZF_KEY_F19;
    case SDLK_F20: return BZF_KEY_F20;
    case SDLK_F21: return BZF_KEY_F21;
    case SDLK_F22: return BZF_KEY_F22;
    case SDLK_F23: return BZF_KEY_F23;
    case SDLK_F24: return BZF_KEY_F24;
    //case SDLK_F25: return BZF_KEY_F25; // SDL does not have F25, unlike GLFW
    case SDLK_KP_0: return BZF_KEY_KP_0;
    case SDLK_KP_1: return BZF_KEY_KP_1;
    case SDLK_KP_2: return BZF_KEY_KP_2;
    case SDLK_KP_3: return BZF_KEY_KP_3;
    case SDLK_KP_4: return BZF_KEY_KP_4;
    case SDLK_KP_5: return BZF_KEY_KP_5;
    case SDLK_KP_6: return BZF_KEY_KP_6;
    case SDLK_KP_7: return BZF_KEY_KP_7;
    case SDLK_KP_8: return BZF_KEY_KP_8;
    case SDLK_KP_9: return BZF_KEY_KP_9;
    case SDLK_KP_DECIMAL: return BZF_KEY_KP_DECIMAL;
    case SDLK_KP_DIVIDE: return BZF_KEY_KP_DIVIDE;
    case SDLK_KP_MULTIPLY: return BZF_KEY_KP_MULTIPLY;
    case SDLK_KP_MINUS: return BZF_KEY_KP_SUBTRACT;
    case SDLK_KP_PLUS: return BZF_KEY_KP_ADD;
    case SDLK_KP_ENTER: return BZF_KEY_KP_ENTER;
    case SDLK_KP_EQUALS: return BZF_KEY_KP_EQUAL;
    case SDLK_LSHIFT: return BZF_KEY_LEFT_SHIFT;
    case SDLK_LCTRL: return BZF_KEY_LEFT_CONTROL;
    case SDLK_LALT: return BZF_KEY_LEFT_ALT;
    case SDLK_LGUI: return BZF_KEY_LEFT_SUPER;
    case SDLK_RSHIFT: return BZF_KEY_RIGHT_SHIFT;
    case SDLK_RCTRL: return BZF_KEY_RIGHT_CONTROL;
    case SDLK_RALT: return BZF_KEY_RIGHT_ALT;
    case SDLK_RGUI: return BZF_KEY_RIGHT_SUPER;
    case SDLK_MENU: return BZF_KEY_MENU;
    default: return BZF_KEY_UNKNOWN;
    // *INDENT-ON*
    }
}

int SDL2Platform::modsFromSDL(int sdlMods)
{
    int mods = 0;
    if (sdlMods & KMOD_SHIFT)
        mods = mods | BZF_MOD_SHIFT;
    if (sdlMods & KMOD_CTRL)
        mods = mods | BZF_MOD_CTRL;
    if (sdlMods & KMOD_ALT)
        mods = mods | BZF_MOD_ALT;
    if (sdlMods & KMOD_GUI)
        mods = mods | BZF_MOD_SUPER;
    return mods;
}

SDL2Window* SDL2Platform::getWindowFromSDLID(Uint32 id)
{
    SDL_Window* win = SDL_GetWindowFromID(id);
    if (win == nullptr)
        return nullptr;
    return static_cast<SDL2Window*>(SDL_GetWindowData(win, "SDL2Window"));
}

///////////////////////////////////////////////////////////
// Window
///////////////////////////////////////////////////////////

SDL2Window::SDL2Window(int width, int height, SDL2Monitor* _monitor, int x, int y) : BzfWindow(), closeRequested(false),
    hasGamma(true), mouseConfinementMode(BZF_MOUSE_CONFINED_NONE)
{
    mouseBox[0][0] = 0;
    mouseBox[0][1] = 0;
    mouseBox[1][0] = 0;
    mouseBox[0][1] = 0;

    int displayCount = SDL_GetNumVideoDisplays();
    SDL2Monitor* monitor;
    if (_monitor == nullptr || _monitor->displayIndex >= displayCount)
    {
        // TODO: Handle this better
        if (_monitor != nullptr)
            std::cerr << "WARNING: Requested monitor index " << _monitor->displayIndex << " but only " << displayCount << " display"
                      << ((displayCount != 1)?"s are":" is") << " attached. Using primary display instead." << std::endl;
        monitor = new SDL2Monitor;
        monitor->displayIndex = 0;
        monitor->name = SDL_GetDisplayName(0);
    }
    else
        monitor = _monitor;

    if (x == -1)
        x = SDL_WINDOWPOS_CENTERED_DISPLAY(monitor->displayIndex);
    if (y == -1)
        y = SDL_WINDOWPOS_CENTERED_DISPLAY(monitor->displayIndex);

    window = SDL_CreateWindow("", x, y, width, height,
                              SDL_WINDOW_OPENGL|SDL_WINDOW_ALLOW_HIGHDPI|SDL_WINDOW_RESIZABLE);

    if (window == nullptr)
    {
        std::cerr << "Error: Unable to create window." << std::endl;
        exit(-1);
    }

    // Store a pointer to this SDL2Window inside the SDL window
    SDL_SetWindowData(window, "SDL2Window", this);

    glcontext = SDL_GL_CreateContext(window);

    // Init glad
    gladLoadGLLoader((GLADloadproc) SDL_GL_GetProcAddress);
}

// TODO: Handle refresh rate - Might have to start windowed and then set the display mode using SDL_SetWindowDisplayMode, and finally switching to fullscreen
SDL2Window::SDL2Window(BzfResolution resolution, SDL2Monitor* _monitor) : BzfWindow(), closeRequested(false),
    hasGamma(true), mouseConfinementMode(BZF_MOUSE_CONFINED_NONE)
{
    mouseBox[0][0] = 0;
    mouseBox[0][1] = 0;
    mouseBox[1][0] = 0;
    mouseBox[0][1] = 0;

    int displayCount = SDL_GetNumVideoDisplays();
    SDL2Monitor* monitor;
    if (_monitor == nullptr || _monitor->displayIndex >= displayCount)
    {
        // TODO: Handle this better
        if (_monitor != nullptr)
            std::cerr << "WARNING: Requested monitor index " << _monitor->displayIndex << " but only " << displayCount << " display"
                      << ((displayCount != 1)?"s are":" is") << " attached. Using primary display instead." << std::endl;
        monitor = new SDL2Monitor;
        monitor->displayIndex = 0;
        monitor->name = SDL_GetDisplayName(0);
    }
    else
        monitor = _monitor;

#ifdef __linux__
    // Work around an issue in Linux with Gnome 3 + Windows List extension: https://bugzilla.libsdl.org/show_bug.cgi?id=4990
    // Create windowed first, and then switch to fullscreen
    if (displayCount >= 2)
    {
        window = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED_DISPLAY(_monitor->displayIndex),
                                  SDL_WINDOWPOS_CENTERED_DISPLAY(_monitor->displayIndex), 640, 480, SDL_WINDOW_OPENGL|SDL_WINDOW_ALLOW_HIGHDPI);
        setFullscreen(resolution, _monitor);
    }
    else
    {
#endif
        window = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED_DISPLAY(_monitor->displayIndex),
                                  SDL_WINDOWPOS_CENTERED_DISPLAY(_monitor->displayIndex), resolution.width, resolution.height,
                                  SDL_WINDOW_OPENGL|SDL_WINDOW_ALLOW_HIGHDPI|SDL_WINDOW_FULLSCREEN);
#ifdef __linux__
    }
#endif

    if (window == nullptr)
    {
        std::cerr << "Error: Unable to create window." << std::endl;
        exit(-1);
    }

    // Store a pointer to this SDL2Window inside the SDL window
    SDL_SetWindowData(window, "SDL2Window", this);

    glcontext = SDL_GL_CreateContext(window);

    // Init glad
    gladLoadGLLoader((GLADloadproc) SDL_GL_GetProcAddress);
}

SDL2Window::~SDL2Window()
{
    // Delete the OpenGL context
    SDL_GL_DeleteContext(glcontext);

    // Destroy the SDL window
    SDL_DestroyWindow(window);
}



bool SDL2Window::isFullscreen() const
{
    return SDL_GetWindowFlags(window) & (SDL_WINDOW_FULLSCREEN | SDL_WINDOW_FULLSCREEN_DESKTOP);
}

bool SDL2Window::setVerticalSync(bool sync) const
{
    if (sync)
    {
        // Try to set adaptive vsync
        if (SDL_GL_SetSwapInterval(-1) == -1)
            // Try to set normal vsync
            if (SDL_GL_SetSwapInterval(1) == -1)
                return false;
    }
    else
        SDL_GL_SetSwapInterval(0);
    return true;
}

bool SDL2Window::getWindowSize(int &width, int &height) const
{
    // TODO: Check if this is the right function for both fullscreen and windowed
    SDL_GetWindowSize(window, &width, &height);
    return true;
}

bool SDL2Window::setWindowed(int width, int height, BzfMonitor* _monitor, int positionX, int positionY)
{
    int displayCount = SDL_GetNumVideoDisplays();
    SDL2Monitor* monitor;
    if (_monitor == nullptr || static_cast<SDL2Monitor*>(_monitor)->displayIndex >= displayCount)
    {
        // TODO: Handle this better
        if (_monitor != nullptr)
        {
            std::cerr << "WARNING: Requested monitor index " << static_cast<SDL2Monitor*>(_monitor)->displayIndex << " but only " <<
                      displayCount << " display" << ((displayCount != 1)?"s are":" is") << " attached. Using primary display instead." <<
                      std::endl;
            return false;
        }
        monitor = new SDL2Monitor;
        monitor->displayIndex = 0;
        monitor->name = SDL_GetDisplayName(0);
    }
    else
        monitor = static_cast<SDL2Monitor*>(_monitor);

    if (positionX == -1)
        positionX = SDL_WINDOWPOS_CENTERED_DISPLAY(monitor->displayIndex);
    if (positionY == -1)
        positionY = SDL_WINDOWPOS_CENTERED_DISPLAY(monitor->displayIndex);

    SDL_SetWindowFullscreen(window, 0);
    SDL_SetWindowSize(window, width, height);
    SDL_SetWindowPosition(window, positionX, positionY);

    return true;
}

bool SDL2Window::setFullscreen(BzfResolution resolution, BzfMonitor* _monitor)
{
    int displayCount = SDL_GetNumVideoDisplays();
    SDL2Monitor* monitor;
    if (_monitor == nullptr || static_cast<SDL2Monitor*>(_monitor)->displayIndex >= displayCount)
    {
        // TODO: Handle this better
        if (_monitor != nullptr)
        {
            std::cerr << "WARNING: Requested monitor index " << static_cast<SDL2Monitor*>(_monitor)->displayIndex << " but only " <<
                      displayCount << " display" << ((displayCount != 1)?"s are":" is") << " attached. Using primary display instead." <<
                      std::endl;
            return false;
        }
        monitor = new SDL2Monitor;
        monitor->displayIndex = 0;
        monitor->name = SDL_GetDisplayName(0);
    }
    else
        monitor = static_cast<SDL2Monitor*>(_monitor);

    // Find the closest valid display mode
    SDL_DisplayMode requested, closest;
    requested.w = resolution.width;
    requested.h = resolution.height;
    requested.refresh_rate = resolution.refreshRate;
    if (SDL_GetClosestDisplayMode(static_cast<SDL2Monitor*>(monitor)->displayIndex, &requested, &closest) == nullptr)
        return false;

    // Make the mode switch
    if (SDL_SetWindowDisplayMode(window, &closest) != 0)
    {
        std::cerr << "Unable to set display mode: " << SDL_GetError() << std::endl;
        return false;
    }

    if (SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN) != 0)
    {
        std::cerr << "Unable to set fullscreen mode: " << SDL_GetError() << std::endl;
        return false;
    }

    // TODO: Handle multiple monitors as I imagine this will use only the primary display

    return true;
}

void SDL2Window::iconify() const
{
    SDL_MinimizeWindow(window);
}

void SDL2Window::setMinSize(int width, int height)
{
    SDL_SetWindowMinimumSize(window, width, height);
}

void SDL2Window::setTitle(const char *title)
{
    SDL_SetWindowTitle(window, title);
}

// From: https://caedesnotes.wordpress.com/2015/04/13/how-to-integrate-your-sdl2-window-icon-or-any-image-into-your-executable/
void SDL2Window::setIcon(BzfIcon *icon)
{
    // these masks are needed to tell SDL_CreateRGBSurface(From)
    // to assume the data it gets is byte-wise RGB(A) data
    Uint32 rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    int shift = (icon->bytes_per_pixel == 3) ? 8 : 0;
    rmask = 0xff000000 >> shift;
    gmask = 0x00ff0000 >> shift;
    bmask = 0x0000ff00 >> shift;
    amask = 0x000000ff >> shift;
#else // little endian, like x86
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = (icon->bytes_per_pixel == 3) ? 0 : 0xff000000;
#endif

    SDL_Surface* surface = SDL_CreateRGBSurfaceFrom((void*)icon->pixel_data, icon->width, icon->height,
                           icon->bytes_per_pixel*8, icon->bytes_per_pixel*icon->width, rmask, gmask, bmask, amask);

    SDL_SetWindowIcon(window, surface);

    SDL_FreeSurface(surface);
}

void SDL2Window::setMouseRelative(bool relative)
{
    SDL_SetRelativeMouseMode(relative ? SDL_TRUE : SDL_FALSE);
}

void SDL2Window::setMousePosition(double x, double y)
{
    SDL_WarpMouseInWindow(window, (int)x, (int)y);
}

bool SDL2Window::supportsMouseConfinement()
{
    return true;
}

bool SDL2Window::setConfineMouse(BzfMouseConfinement mode, double x1, double y1, double x2, double y2)
{
    if (mode == BZF_MOUSE_CONFINED_WINDOW)
    {
        SDL_SetWindowGrab(window, SDL_TRUE);
#ifdef _WIN32
        ClipCursor(nullptr);
#endif
    }
    else if (mode == BZF_MOUSE_CONFINED_BOX)
    {
#ifndef _WIN32
        if (! SDL_GetWindowGrab(window))
            SDL_SetWindowGrab(window, SDL_TRUE);

        mouseBox[0][0] = (int)x1;
        mouseBox[0][1] = (int)y1;
        mouseBox[1][0] = (int)x2;
        mouseBox[1][1] = (int)y2;
#else
        int posx, posy;
        SDL_GetWindowPosition(window, &posx, &posy);

        // Store the boundary positions as rectangle
        RECT rect;
        rect.top = y1 + posy;
        rect.left = x1 + posx;
        rect.bottom = y2 + posy;
        rect.right = x2 + posx;

        // Restrict cursor to that rectangle
        ClipCursor(&rect);
#endif
    }
    else // mode == BZF_MOUSE_CONFINED_NONE
    {
        SDL_SetWindowGrab(window, SDL_FALSE);
#ifdef _WIN32
        ClipCursor(nullptr);
#endif
    }

    mouseConfinementMode = mode;

    return true;
}

BzfMouseConfinement SDL2Window::getConfineMouse()
{
    return mouseConfinementMode;
}

#ifndef _WIN32
void SDL2Window::checkMouseConfineBox(int mouseX, int mouseY)
{
    // Warp the mouse if it's out of the boundary
    if (mouseX < mouseBox[0][0])
        setMousePosition(mouseBox[0][0], mouseY);
    if (mouseX > mouseBox[1][0])
        setMousePosition(mouseBox[1][0], mouseY);
    if (mouseY < mouseBox[0][1])
        setMousePosition(mouseX, mouseBox[0][1]);
    if (mouseY > mouseBox[1][1])
        setMousePosition(mouseX, mouseBox[1][1]);
}
#endif

void SDL2Window::makeContextCurrent() const
{
    SDL_GL_MakeCurrent(window, glcontext);
}

void SDL2Window::swapBuffers() const
{
    SDL_GL_SwapWindow(window);

    // TODO: Include our workaround for an SDL2 vsync bug on macOS
}

void SDL2Window::setGamma(float gamma)
{
    if (SDL_SetWindowBrightness(window, gamma) == -1)
    {
        printf("Could not set Gamma: %s.\n", SDL_GetError());
        hasGamma = false;
    }
}

float SDL2Window::getGamma()
{
    return SDL_GetWindowBrightness(window);
}

bool SDL2Window::hasGammaControl() const
{
    return hasGamma;
}

void SDL2Window::requestClose()
{
    closeRequested = true;
}

bool SDL2Window::shouldClose() const
{
    return closeRequested;
}

///////////////////////////////////////////////////////////
// Audio
///////////////////////////////////////////////////////////

SDL2Audio::SDL2Audio() : dev(0)
{
    if (!(SDL_WasInit(SDL_INIT_AUDIO) != 0))
    {
        if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0)
        {
            std::cerr << "Error: There was an error initializing SDL2 audio: " << SDL_GetError() << std::endl;
            exit(-1);
        }
    }
}

SDL2Audio::~SDL2Audio()
{
    closeDevice();
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

std::vector<const char *> SDL2Audio::getAudioDevices()
{
    std::vector<const char *> devices;
    // Get audio playback devices
    int count = SDL_GetNumAudioDevices(0);
    for (int i = 0; i < count; ++i)
        devices.push_back(SDL_GetAudioDeviceName(i, 0));
    return devices;
}

bool SDL2Audio::openDevice(const char *name)
{
    SDL_AudioSpec desired, obtained;

    SDL_memset(&desired, 0, sizeof(desired));
    desired.freq = defaultAudioRate;
    desired.format = AUDIO_S16SYS;
    desired.channels = 2;
    desired.samples = 4096;
    desired.callback = &fillAudioWrapper;
    desired.userdata = (void*)this;

    dev = SDL_OpenAudioDevice(name, 0, &desired, &obtained, 0);

    if (dev == 0)
        return false;

    printf("SDL Audio Frequency: %d vs %d\n", desired.freq, obtained.freq);
    printf("SDL Audio Format:    %hu vs %hu\n", desired.format, obtained.format);

    audioOutputRate = obtained.freq;
    if (SDL_BuildAudioCVT
            (&convert,
             desired.format, desired.channels, obtained.freq,
             obtained.format, obtained.channels, obtained.freq) == -1)
    {
        /* Check that the convert was built */
        printf("Could not build converter for audio: %s\n", SDL_GetError());
        closeDevice();
        return false;
    }
    convert.len = (int)((double)obtained.size / convert.len_ratio);
    convert.buf = (Uint8 *)malloc(convert.len * convert.len_mult);

    // make an output buffer
    outputBufferEmpty = true;

    // ready to go
    audioReady = true;

    return true;
}

void            SDL2Audio::closeDevice()
{
    if (dev == 0)
        return;

    SDL_PauseAudioDevice(dev, 1);
    SDL_CloseAudioDevice(dev);

    // Stop Audio to avoid callback
    SDL_PauseAudioDevice(dev, 1);

    SDL_CloseAudioDevice(dev);
    free(convert.buf);
    convert.buf = 0;
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
    audioReady    = false;
}

void            SDL2Audio::startAudioCallback(bool (*proc)(void))
{
    userCallback = proc;
    // Stop sending silence and start calling audio callback
    SDL_PauseAudio(0);
}

void            SDL2Audio::writeSoundCommand(const void* cmd, int len)
{
    if (!audioReady) return;

    SDL_LockAudio();

    // Discard command if full
    if ((cmdFill + len) < 2048)
    {
        memcpy(&cmdQueue[cmdFill], cmd, len);
        // We should awake audioSleep - but game become unplayable
        // using here an SDL_CondSignal(wakeCond)
        cmdFill += len;
    }

    SDL_UnlockAudio();
}

bool            SDL2Audio::readSoundCommand(void* cmd, int len)
{
    bool result = false;

    if (cmdFill >= len)
    {
        memcpy(cmd, cmdQueue, len);
        // repack list of command waiting to be processed
        memmove(cmdQueue, &cmdQueue[len], cmdFill - len);
        cmdFill -= len;
        result = true;
    }
    return result;
}

int         SDL2Audio::getAudioOutputRate() const
{
    return audioOutputRate;
}

int         SDL2Audio::getAudioBufferChunkSize() const
{
    return convert.len >> 2;
}

void SDL2Audio::fillAudio (Uint8 * stream, int len)
{
    static int sampleToSend;  // next sample to send on output buffer
    if (outputBufferEmpty)
    {
        userCallback();
        sampleToSend = 0;
    }

    int transferSize = convert.len_cvt - sampleToSend;
    if (transferSize > len)
    {
        transferSize = len;
        outputBufferEmpty = false;
    }
    else
        outputBufferEmpty = true;

    // just copying into the soundBuffer is enough, SDL is looking for
    // something different from silence sample
    memcpy(stream, convert.buf + sampleToSend, transferSize);
    sampleToSend += transferSize;
}

void SDL2Audio::fillAudioWrapper (void * userdata, Uint8 * stream, int len)
{
    SDL2Audio * me = static_cast<SDL2Audio *>(userdata);
    me->fillAudio(stream, len);
}

void SDL2Audio::writeAudioFrames(const float* samples, int)
{
    int    audioBufferSize = convert.len >> 1;
    short *buffer   = (short *)convert.buf;

    for (int j = 0; j < audioBufferSize; j++)
    {
        if (samples[j] < -32767.0)
            buffer[j] = -32767;
        else if (samples[j] > 32767.0)
            buffer[j] = 32767;
        else
            buffer[j] = short(samples[j]);
    }
    SDL_ConvertAudio(&convert);
}


float*      SDL2Audio::doReadSound(const std::string &filename, int &numFrames,
                                   int &rate) const
{
    SDL_AudioSpec wav_spec;
    Uint32    wav_length;
    Uint8 *wav_buffer;
    int      ret;
    SDL_AudioCVT  wav_cvt;
    int16_t      *cvt16;
    int      i;

    float *data;
    rate  = defaultAudioRate;
    if (!SDL_LoadWAV(filename.c_str(), &wav_spec, &wav_buffer, &wav_length))
        return NULL;

    /* Build AudioCVT */
    ret = SDL_BuildAudioCVT(&wav_cvt,
                            wav_spec.format, wav_spec.channels, wav_spec.freq,
                            AUDIO_S16SYS, 2, audioOutputRate);
    /* Check that the convert was built */
    if (ret == -1)
    {
        printf("Could not build converter for Wav file %s: %s.\n",
               filename.c_str(), SDL_GetError());
        SDL_FreeWAV(wav_buffer);
        return NULL;
    }
    /* Setup for conversion */
    wav_cvt.buf = (Uint8*)malloc(wav_length * wav_cvt.len_mult);
    wav_cvt.len = wav_length;
    memcpy(wav_cvt.buf, wav_buffer, wav_length);
    /* And now we're ready to convert */
    SDL_ConvertAudio(&wav_cvt);
    numFrames = (int)(wav_length * wav_cvt.len_ratio / 4);
    cvt16     = (int16_t *)wav_cvt.buf;
    data      = new float[numFrames * 2];
    for (i = 0; i < numFrames * 2; i++)
        data[i] = cvt16[i];
    free(wav_cvt.buf);
    SDL_FreeWAV(wav_buffer);
    return data;
}

///////////////////////////////////////////////////////////
// Joystick / Game Controller
///////////////////////////////////////////////////////////

SDL2Joystick::SDL2Joystick() : device(nullptr), haptic(nullptr), rumbleSupported(false)
{
    if (!(SDL_WasInit(SDL_INIT_GAMECONTROLLER) != 0))
    {
        if (SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER) != 0)
        {
            std::cerr << "Error: There was an error initializing SDL2 game controller support: " << SDL_GetError() << std::endl;
            exit(-1);
        }
    }

    if (!(SDL_WasInit(SDL_INIT_HAPTIC) != 0))
    {
        if (SDL_InitSubSystem(SDL_INIT_HAPTIC) != 0)
            std::cerr << "Error: There was an error initializing SDL2 haptic support: " << SDL_GetError() << std::endl;
    }
}

SDL2Joystick::~SDL2Joystick()
{
    closeDevice();
    SDL_QuitSubSystem(SDL_INIT_GAMECONTROLLER|SDL_INIT_HAPTIC);
}

std::vector<BzfJoystickInfo*> SDL2Joystick::getJoysticks()
{
    std::vector<BzfJoystickInfo*> joysticks;

    if (!(SDL_WasInit(SDL_INIT_GAMECONTROLLER) != 0))
    {
        if (SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER) != 0)
        {
            std::cerr << "Error: There was an error initializing SDL2 joystick support: " << SDL_GetError() << std::endl;
            return joysticks;
        }
    }

    int numJoysticks = SDL_NumJoysticks();
    SDL_Joystick* sdljoy;
    for (int i = 0; i < numJoysticks; ++i)
    {
        BzfJoystickInfo* joystick = new BzfJoystickInfo;
        sdljoy = SDL_JoystickOpen(i);
        if (sdljoy != nullptr)
        {
            joystick->id = i;
            joystick->guid = new char[33];
            SDL_JoystickGetGUIDString(SDL_JoystickGetGUID(sdljoy), const_cast<char*>(joystick->guid), 33);
            joystick->name = SDL_JoystickNameForIndex(i);
            joystick->axes = SDL_JoystickNumAxes(sdljoy);
            joystick->hats = SDL_JoystickNumHats(sdljoy);
            joystick->buttons = SDL_JoystickNumButtons(sdljoy);
            joystick->isRumbleSupported = SDL_JoystickIsHaptic(sdljoy);
            joystick->isGameController = SDL_IsGameController(i) == SDL_TRUE;
            SDL_JoystickClose(sdljoy);
            joysticks.push_back(joystick);
        }
    }

    return joysticks;
}

bool SDL2Joystick::openDevice(int id)
{
    // Assume we don't have rumble feedback support
    rumbleSupported = true;

    // Close the current device if we already opened a joystick
    if (SDL_JoystickGetAttached(device))
    {
        if (haptic != nullptr) // TODO: Use SDL_HapticOpened?
            SDL_HapticClose(haptic);
        SDL_JoystickClose(device);
    }

    device = SDL_JoystickOpen(id);
    // See if we have a valid joystick device
    if (device != nullptr)
    {
        // Try setting up rumble feedback
        haptic = SDL_HapticOpenFromJoystick(device);
        if (haptic != nullptr)
        {
            if (SDL_HapticRumbleSupported(haptic) && SDL_HapticRumbleInit(haptic) == 0)
                rumbleSupported = true;
            else
            {
                SDL_HapticClose(haptic);
                haptic = nullptr;
            }
        }
        return true;
    }

    // Could not find this joystick
    return false;
}

void SDL2Joystick::closeDevice()
{
    if (haptic != nullptr)
    {
        SDL_HapticClose(haptic);
        haptic = nullptr;
    }
    if (SDL_JoystickGetAttached(device))
        SDL_JoystickClose(device);
}

const char *SDL2Joystick::getName()
{
    return SDL_JoystickName(device);
}

int SDL2Joystick::getNumAxes()
{
    return SDL_JoystickNumAxes(device);
}

int SDL2Joystick::getNumHats()
{
    return SDL_JoystickNumHats(device);
}

int SDL2Joystick::getNumButtons()
{
    return SDL_JoystickNumButtons(device);
}

bool SDL2Joystick::isRumbleSupported()
{
    return rumbleSupported;
}

void SDL2Joystick::rumble(float strength, unsigned int duration)
{
    if (rumbleSupported)
        SDL_HapticRumblePlay(haptic, strength, duration);
}

float SDL2Joystick::getAxis(int axis)
{
    return SDL_JoystickGetAxis(device, axis) / 32768.0f;
}
