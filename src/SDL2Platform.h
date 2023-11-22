#pragma once

#include "BzfPlatform.h"

#define SDL_MAIN_HANDLED

#include <glad/glad.h>
#include <SDL2/SDL.h>

#include <vector>

struct SDL2Monitor : public BzfMonitor
{
    int displayIndex;
};

class SDL2Window;
class SDL2Audio;
class SDL2Joystick;

class SDL2Platform : public BzfPlatform
{
public:
    SDL2Platform();
    ~SDL2Platform();

    BzfWindow* createWindow(int width, int height, BzfMonitor* monitor = nullptr, int positionX = -1, int positionY = -1);
    BzfWindow* createWindow(BzfResolution resolution, BzfMonitor* monitor = nullptr);

    // Audio
    BzfAudio* getAudio();

    // Joysticks / Game Controllers
    BzfJoystick* getJoystick();

    bool isGameRunning() const;

    // Timers
    double getGameTime() const;

    // Monitors
    BzfMonitor* getPrimaryMonitor() const;
    std::vector<BzfMonitor*> getMonitors() const;
    BzfResolution getCurrentResolution(BzfMonitor* monitor = nullptr) const;
    std::vector<BzfResolution> getResolutions(BzfMonitor* monitor = nullptr) const;

    // OpenGL Attributes
    // Set requested OpenGL profile and version
    void GLSetVersion(BzfGLProfile profile, unsigned short majorVersion, unsigned short minorVersion) const;
    // Set minumum color depth
    void GLSetRGBA(unsigned short red, unsigned short green, unsigned short blue, unsigned short alpha) const;

    // Events
    // This will poll for events and call any set callbacks
    void pollEvents();

    void startTextInput();
    void stopTextInput();
    bool isTextInput();

private:
    std::vector<SDL2Window*> windows;
    SDL2Audio *audio;
    SDL2Joystick *joystick;
    Uint64 startTime;

    BzfKey keyFromSDL(SDL_Keycode key);
    int modsFromSDL(int sdlMods);
    SDL2Window* getWindowFromSDLID(Uint32 id);
};

class SDL2Window : public BzfWindow
{
public:
    SDL2Window(int width, int height, SDL2Monitor* monitor = nullptr, int positionX = -1, int positionY = -1);
    SDL2Window(BzfResolution resolution, SDL2Monitor* monitor = nullptr);
    ~SDL2Window();

    // Fullscreen/Windowed
    bool isFullscreen() const;
    bool setVerticalSync(bool sync) const;
    bool getWindowSize(int &width, int &height) const;
    bool setWindowed(int width, int height, BzfMonitor* monitor = nullptr, int x = -1, int y = -1);
    bool setFullscreen(BzfResolution resolution, BzfMonitor* monitor = nullptr);
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

    // SDL2 window specific methods
    void requestClose();
    bool shouldClose() const;

private:
    SDL_Window* window;
    SDL_GLContext glcontext;
    bool closeRequested;
    bool hasGamma;

    BzfMouseConfinement mouseConfinementMode;
    int mouseBox[2][2];
};

class SDL2Audio : public BzfAudio
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
};

class SDL2Joystick : public BzfJoystick
{
public:
    SDL2Joystick();
    virtual ~SDL2Joystick();

    std::vector<BzfJoystickInfo*> getJoysticks();

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
};
