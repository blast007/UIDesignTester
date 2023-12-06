#pragma once

#define SDL_MAIN_HANDLED

#include <glad/glad.h>
#include <SDL2/SDL.h>

#include <string>
#include <vector>
#include <map>
#include <functional>

struct BzfResolution
{
    int width;
    int height;
    int refreshRate;
};

typedef enum
{
    BZF_MOUSE_CONFINED_NONE = 0,
    BZF_MOUSE_CONFINED_WINDOW = 1,
    BZF_MOUSE_CONFINED_BOX = 2
} BzfMouseConfinement;

// Key actions
typedef enum
{
    BZF_KEY_RELEASED = 0,
    BZF_KEY_PRESSED,
    BZF_KEY_REPEATED
} BzfKeyAction;

// Button actions
typedef enum
{
    BZF_BUTTON_RELEASED = 0,
    BZF_BUTTON_PRESSED
} BzfButtonAction;

typedef enum bzfglprofile
{
    BzfGLCompatibility,
    BzfGLCore,
    BzfGLES
} BzfGLProfile;

#define BZF_ICON_SIZE 64

struct BzfIcon
{
    int    width;
    int    height;
    int    bytes_per_pixel; /* 2:RGB16, 3:RGB, 4:RGBA */
    unsigned char  pixel_data[BZF_ICON_SIZE * BZF_ICON_SIZE * 4 + 1];
};

class SDL2Window;
//class SDL2Audio;
//class SDL2Joystick;

class SDL2Platform
{
public:
    SDL2Platform();
    ~SDL2Platform();

    SDL2Window* createWindow(int width, int height, int monitor = -1, int positionX = -1, int positionY = -1);
    SDL2Window* createWindow(BzfResolution resolution, int monitor = -1);

    // Audio
    //SDL2Audio* getAudio();

    // Joysticks / Game Controllers
    //SDL2Joystick* getJoystick();

    bool isGameRunning() const;

    // Timers
    double getGameTime() const;

    // Monitors
    std::pair<int, std::string> getPrimaryMonitor() const;
    std::map<int, std::string> getMonitors() const;
    BzfResolution getCurrentResolution(int monitor) const;
    std::vector<BzfResolution> getResolutions(int monitor) const;

    // OpenGL Attributes
    // Set requested OpenGL profile and version
    void GLSetVersion(BzfGLProfile profile, unsigned short majorVersion, unsigned short minorVersion) const;
    // Set minumum color depth
    void GLSetRGBA(unsigned short red, unsigned short green, unsigned short blue, unsigned short alpha) const;

    // Events
    // This will poll for events and call any set callbacks
    void pollEvents();

    // Add a callback for the window resize event
    // Callback arguments: SDL2Platform, SDL2Window, viewportWidth, viewportHeight
    void addResizeCallback(std::function<void(SDL2Platform*, SDL2Window*, int width, int height)> callback);
    // Add a callback for the window move event
    // Callback arguments: SDL2Platform, SDL2Window, positionX, positionY
    void addMoveCallback(std::function<void(SDL2Platform*, SDL2Window*, int x, int y)> callback);
    // Set the callback for key events which reference the physical key
    // Callback arguments: SDL2Platform, SDL2Window, SDL_Keycode, BzfKeyAction, Uint16
    void addKeyCallback(std::function<void(SDL2Platform*, SDL2Window*, SDL_Keycode key, BzfKeyAction action, Uint16 mods)> callback);
    // Set the callback for text events which reference the unicode code points of the OS defined keyboard layout
    // Callback arguments: SDL2Platform, SDL2Window, unicode codepoint
    void addTextCallback(std::function<void(SDL2Platform*, SDL2Window*, char text[32])> callback);
    // Set the callback for mouse cursor movement
    // Callback arguments: SDL2Platform, SDL2Window, x position, y position (with positions being relative to the top left corner of the window area)
    void addCursorPosCallback(std::function<void(SDL2Platform*, SDL2Window*, double x, double y, Uint16 mods)> callback);
    // Set the callback for mouse button presses
    // Callback arguments: SDL2Platform, SDL2Window, button, action, mods
    void addMouseButtonCallback(
        std::function<void(SDL2Platform*, SDL2Window*, Uint8 button, BzfButtonAction action, Uint16 mods)> callback);
    // Set the callback for the mouse wheel (or touchpad?) scrolling
    // Callback arguments: SDL2Platform, SDL2Window, x offset, y offset
    void addScrollCallback(std::function<void(SDL2Platform*, SDL2Window*, double x, double y)> callback);
    // Joystick / Game Controller events
    // Callback arguments: SDL2Platform, SDL2Event, button, state
    /*void addJoystickButtonCallback(
        std::function<void(SDL2Platform*, SDL2Window*, Uint8 button, BzfButtonAction action)> callback);*/

    void startTextInput();
    void stopTextInput();
    bool isTextInput();

protected:
    std::vector<std::function<void(SDL2Platform*,SDL2Window*,int,int)>> resizeCallbacks;
    std::vector<std::function<void(SDL2Platform*,SDL2Window*,int,int)>> moveCallbacks;
    std::vector<std::function<void(SDL2Platform*,SDL2Window*,SDL_Keycode,BzfKeyAction,Uint16)>> keyCallbacks;
    std::vector<std::function<void(SDL2Platform*,SDL2Window*,char[32])>> textCallbacks;
    std::vector<std::function<void(SDL2Platform*,SDL2Window*,double,double,Uint16)>> cursorPosCallbacks;
    std::vector<std::function<void(SDL2Platform*,SDL2Window*,Uint8,BzfButtonAction,Uint16)>> mouseButtonCallbacks;
    std::vector<std::function<void(SDL2Platform*,SDL2Window*,double,double)>> scrollCallbacks;
    //std::vector<std::function<void(SDL2Platform*,SDL2Window*,Uint8,BzfButtonAction)>> joystickButtonCallbacks;

private:
    std::vector<SDL2Window*> windows;
    //SDL2Audio *audio;
    //SDL2Joystick *joystick;
    Uint64 startTime;

    //BzfKey keyFromSDL(SDL_Keycode key);
    //int modsFromSDL(int sdlMods);
    SDL2Window* getWindowFromSDLID(Uint32 id);
};

class SDL2Window
{
public:
    SDL2Window(int width, int height, int monitor = -1, int positionX = -1, int positionY = -1);
    SDL2Window(BzfResolution resolution, int monitor = -1);
    ~SDL2Window();

    // Fullscreen/Windowed
    bool isFullscreen() const;
    bool setVerticalSync(bool sync) const;
    bool getWindowSize(int &width, int &height) const;
    bool setWindowed(int width, int height, int monitor = -1, int x = -1, int y = -1);
    bool setFullscreen(BzfResolution resolution, int monitor = -1);
    void iconify() const;
    void setMinSize(int width, int height);
    void setTitle(const char *title);
    void setIcon(BzfIcon *icon);

    // Mouse
    void setMouseRelative(bool relative);
    void setMousePosition(double x, double y);
    bool supportsMouseConfinement();
    bool setConfineMouse(BzfMouseConfinement mode, double x1 = 0, double y1 = 0, double x2 = 0, double y2 = 0);
    BzfMouseConfinement getConfineMouse();
#ifndef _WIN32
    void checkMouseConfineBox(int mouseX, int mouseY);
#endif

    // Drawing/context
    void makeContextCurrent() const;
    void swapBuffers() const;

    // Gamma control
    void setGamma(float gamma);
    float getGamma();
    bool hasGammaControl() const;

    // User pointer (for storing a reference to some object instance, tied to this window)
    void setUserPointer(void *pointer)
    {
        userPointer = pointer;
    }
    void* getUserPointer()
    {
        return userPointer;
    }

    // SDL2 window specific methods
    void requestClose();
    bool shouldClose() const;

private:
    SDL_Window* window;
    SDL_GLContext glcontext;

    void* userPointer;

    bool closeRequested;
    bool hasGamma;

    BzfMouseConfinement mouseConfinementMode;
    int mouseBox[2][2];
};

/*class SDL2Audio
{
public:
    SDL2Audio();
    ~SDL2Audio();

    std::vector<const char *> getAudioDevices();
    bool openDevice(const char *name);
    void closeDevice();

    void startAudioCallback(bool (*proc)(void));
    void writeSoundCommand(const void*, int);
    bool readSoundCommand(void*, int);
    int getAudioOutputRate() const;
    int getAudioBufferChunkSize() const;
    void writeAudioFrames(const float* samples, int numFrames);
    float* doReadSound(const std::string& filename, int& numFrames, int& rate) const;

private:
    void fillAudio(Uint8 *, int);
    static void  fillAudioWrapper(void *userdata, Uint8 *stream, int len);;

    SDL_AudioDeviceID dev;

    bool audioReady;
    static const int defaultAudioRate=22050;
    int audioOutputRate;

    bool outputBufferEmpty;

    char cmdQueue[2048]; // space to save temporary command
    int cmdFill;   // from 0 to cmdFill

    bool (*userCallback)(void);
    SDL_AudioCVT convert;
};*/

/*class SDL2Joystick
{
public:
    SDL2Joystick();
    virtual ~SDL2Joystick();

    std::vector<JoystickInfo*> getJoysticks();

    bool openDevice(int id);
    void closeDevice();

    // Joystick information
    const char *getName();
    int getNumAxes();
    int getNumHats();
    int getNumButtons();
    bool isRumbleSupported();

    // Rumble feedback
    void rumble(float strength, unsigned int duration);

    float getAxis(int axis);
private:
    SDL_Joystick *device;
    SDL_Haptic *haptic;
    bool rumbleSupported;
};*/
