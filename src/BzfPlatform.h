#pragma once

#include "BzfKeys.h"

#include <vector>
#include <string>
#include <functional>

struct BzfResolution
{
    int width;
    int height;
    int refreshRate;
};

struct BzfMonitor
{
    std::string name;
};

class BzfWindow;
class BzfAudio;
class BzfJoystick;
struct BzfJoystickInfo;

typedef enum
{
    BZF_MOUSE_CONFINED_NONE = 0,
    BZF_MOUSE_CONFINED_WINDOW = 1,
    BZF_MOUSE_CONFINED_BOX = 2
} BzfMouseConfinement;

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

class BzfPlatform
{
public:
    BzfPlatform() {}
    virtual ~BzfPlatform() {}

    virtual BzfWindow* createWindow(int width, int height, BzfMonitor* monitor = nullptr, int positionX = -1,
                                    int positionY = -1) = 0;
    virtual BzfWindow* createWindow(BzfResolution resolution, BzfMonitor* monitor = nullptr) = 0;

    // Audio
    virtual BzfAudio* getAudio() = 0;

    // Joysticks / Game Controllers
    virtual BzfJoystick* getJoystick() = 0;

    virtual bool isGameRunning() const = 0;

    // Timers
    // TODO: Do we actually need this or is our TimeKeeper class enough?
    virtual double getGameTime() const = 0;

    // Monitors
    // When monitor is nullptr, assume the primary display
    virtual BzfMonitor* getPrimaryMonitor() const = 0;
    virtual std::vector<BzfMonitor*> getMonitors() const = 0;
    virtual BzfResolution getCurrentResolution(BzfMonitor* monitor = nullptr) const = 0;
    virtual std::vector<BzfResolution> getResolutions(BzfMonitor* monitor = nullptr) const = 0;

    // OpenGL Attributes
    // Set requested OpenGL profile and version
    virtual void GLSetVersion(BzfGLProfile profile, unsigned short majorVersion, unsigned short minorVersion) const = 0;
    // Set minumum color depth
    // TODO: Is there a reason to have this and not just hard-code some values?
    virtual void GLSetRGBA(unsigned short red, unsigned short green, unsigned short blue, unsigned short alpha) const = 0;

    // Events
    // This will poll for events and call any set callbacks
    virtual void pollEvents() = 0;

    // Add a callback for the window resize event
    // Callback arguments: BzfPlatform, BzfWindow, viewportWidth, viewportHeight
    void addResizeCallback(std::function<void(BzfPlatform*, BzfWindow*, int width, int height)> callback);
    // Add a callback for the window move event
    // Callback arguments: BzfPlatform, BzfWindow, positionX, positionY
    void addMoveCallback(std::function<void(BzfPlatform*, BzfWindow*, int x, int y)> callback);
    // Set the callback for key events which reference the physical key
    // Callback arguments: BzfPlatform, BzfWindow, scancode, action, mods
    void addKeyCallback(std::function<void(BzfPlatform*, BzfWindow*, BzfKey key, BzfKeyAction action, int mods)> callback);
    // Set the callback for text events which reference the unicode code points of the OS defined keyboard layout
    // Callback arguments: BzfPlatform, BzfWindow, unicode codepoint
    void addTextCallback(std::function<void(BzfPlatform*, BzfWindow*, char text[32])> callback);
    // Set the callback for mouse cursor movement
    // Callback arguments: BzfPlatform, BzfWindow, x position, y position (with positions being relative to the top left corner of the window area)
    void addCursorPosCallback(std::function<void(BzfPlatform*, BzfWindow*, double x, double y)> callback);
    // Set the callback for mouse button presses
    // Callback arguments: BzfPlatform, BzfWindow, button, action, mods
    void addMouseButtonCallback(
        std::function<void(BzfPlatform*,BzfWindow*,BzfMouseButton button, BzfButtonAction action, int mods)> callback);
    // Set the callback for the mouse wheel (or touchpad?) scrolling
    // Callback arguments: BzfPlatform, BzfWindow, x offset, y offset
    void addScrollCallback(std::function<void(BzfPlatform*,BzfWindow*, double x, double y)> callback);
    // Joystick / Game Controller events
    // Callback arguments: BzfPlatform, BzfEvent, button, state
    void addJoystickButtonCallback(
        std::function<void(BzfPlatform*, BzfWindow*, BzfJoyButton button, BzfButtonAction action)> callback);
    void addJoystickHatCallback(std::function<void(BzfPlatform*, BzfWindow*, BzfJoyHat hat, BzfJoyHatDirection direction)>
                                callback);

    virtual void startTextInput() = 0;
    virtual void stopTextInput() = 0;
    virtual bool isTextInput() = 0;

#ifdef USE_GLFW
public:
#else
protected:
#endif
    std::vector<std::function<void(BzfPlatform*,BzfWindow*,int,int)>> resizeCallbacks;
    std::vector<std::function<void(BzfPlatform*,BzfWindow*,int,int)>> moveCallbacks;
    std::vector<std::function<void(BzfPlatform*,BzfWindow*,BzfKey,BzfKeyAction,int)>> keyCallbacks;
    std::vector<std::function<void(BzfPlatform*,BzfWindow*,char[32])>> textCallbacks;
    std::vector<std::function<void(BzfPlatform*,BzfWindow*,double,double)>> cursorPosCallbacks;
    std::vector<std::function<void(BzfPlatform*,BzfWindow*,BzfMouseButton,BzfButtonAction,int)>> mouseButtonCallbacks;
    std::vector<std::function<void(BzfPlatform*,BzfWindow*,double,double)>> scrollCallbacks;
    std::vector<std::function<void(BzfPlatform*,BzfWindow*,BzfJoyButton,BzfButtonAction)>> joystickButtonCallbacks;
    std::vector<std::function<void(BzfPlatform*,BzfWindow*,BzfJoyHat,BzfJoyHatDirection)>> joystickHatCallbacks;
};

class BzfWindow
{
public:
    // The constructor will create a window of a desired size and initialize an OpenGL context.
    // Child classes will use the following format for their constructors:
    // ChildClassWindow(int width, int height, ChildClassMonitor* monitor = nullptr, int positionX = -1, int positionY = -1);
    // ChildClassWindow(BzfResolution resolution, ChildClassMonitor* monitor = nullptr);
    BzfWindow() : userPointer(nullptr) {};
    // Clean up and destroy the window
    virtual ~BzfWindow() {};

    // Fullscreen/Windowed
    virtual bool isFullscreen() const = 0;
    virtual bool setVerticalSync(bool sync) const = 0;
    virtual bool getWindowSize(int &width, int &height) const = 0;
    virtual bool setWindowed(int width, int height, BzfMonitor* monitor = nullptr, int x = -1, int y = -1) = 0;
    virtual bool setFullscreen(BzfResolution resolution, BzfMonitor* monitor = nullptr) = 0;
    virtual void iconify() const = 0;
    virtual void setMinSize(int width, int height) = 0;
    virtual void setTitle(const char *title) = 0;
    virtual void setIcon(BzfIcon *icon) = 0;

    // Mouse
    virtual void setMouseRelative(bool relative) = 0;
    // TODO: Stop using mouse warp/grab/confine in BZFlag and use relative position with a custom cursor
    virtual void setMousePosition(double x, double y) = 0;
    virtual bool supportsMouseConfinement() = 0;
    virtual bool setConfineMouse(BzfMouseConfinement mode, double x1 = 0, double y1 = 0, double x2 = 0, double y2 = 0) = 0;
    virtual BzfMouseConfinement getConfineMouse() = 0;

    // Drawing/context
    virtual void makeContextCurrent() const = 0;
    virtual void swapBuffers() const = 0;

    // Gamma control
    virtual void setGamma(float gamma) = 0;
    virtual float getGamma() = 0;
    virtual bool hasGammaControl() const = 0;

    // User pointer (for storing a reference to some object instance, tied to this window)
    void setUserPointer(void *pointer)
    {
        userPointer = pointer;
    }
    void* getUserPointer()
    {
        return userPointer;
    }
private:
    void* userPointer;
    BzfMouseConfinement mouseConfinementMode;
};

class BzfAudio
{
public:
    BzfAudio() {}
    virtual ~BzfAudio() {}

    virtual std::vector<const char *> getAudioDevices() = 0;
    virtual bool openDevice(const char *name) = 0;
    virtual void closeDevice() = 0;

    virtual void startAudioCallback(bool (*proc)(void)) = 0;
    virtual void writeSoundCommand(const void*, int) = 0;
    virtual bool readSoundCommand(void*, int) = 0;
    virtual int getAudioOutputRate() const = 0;
    virtual int getAudioBufferChunkSize() const = 0;
    virtual void writeAudioFrames(const float* samples, int numFrames) = 0;
    virtual float* doReadSound(const std::string& filename, int& numFrames, int& rate) const = 0;
};

struct BzfJoystickInfo
{
    int id;
    const char *guid;
    const char *name;
    int axes;
    int hats;
    int buttons;
    bool isRumbleSupported;
    // Is this controller in the Game Controller database?
    bool isGameController;
};

class BzfJoystick
{
public:
    BzfJoystick() {}
    virtual ~BzfJoystick() {}

    virtual std::vector<BzfJoystickInfo*> getJoysticks() = 0;

    virtual bool openDevice(int id) = 0;
    virtual void closeDevice() = 0;

    // Joystick information
    virtual const char *getName() = 0;
    virtual int getNumAxes() = 0;
    virtual int getNumHats() = 0;
    virtual int getNumButtons() = 0;

    // Rumble feedback
    virtual bool isRumbleSupported() = 0;
    virtual void rumble(float strength, unsigned int duration) = 0;

    // Ability to poll the axes. Returns a floating point value from -1.0 to 1.0
    virtual float getAxis(int axis) = 0;
};
