UIDesignTester
--------------

This is a test application an RmlUi-based interface for BZFlag.


Keybinds
--------
* `F12`: Quit
* `F5` or `Ctrl+R`: Reloads the RML and RCSS documents
* `` ` ``: Sandbox Mode (hides the HUD and render assets/sandbox/sandbox.rml)
* `1`: Main Menu (render assets/menus/main.rml)
* `2`: Options Menu (render assets/menus/options.rml)
* `3`: Host Menu (Server hosting) (render assets/menus/host.rml)
* `4`: Help Menu (render assets/menus/help.rml)
* `0`: No menu (just renders the HUD via assets/hud/hud.rml)
* `Tab`: Toggles the RmlUi debugger UI
* `q`: Sets the local player name, which will reflect in the HUD
* `w`: Sets the local player score to 1337
* `e`: Sets the local player score to a random value
* `r`: Adds a remote player with a random team
* `a`: Sets a random player kills to a random value
* `s`: Sets a random player deaths to a random value
* `d`: Sets a random player teamkills to a random value
* `f`: Sets a random player to have either GM or L flags

Building
--------

This assumes that CMake is installed.

Linux:

```shell
mkdir $HOME/code
cd $HOME/code
git clone https://github.com/Microsoft/vcpkg.git
./vcpkg/bootstrap-vcpkg.sh --disableMetrics
git clone https://github.com/blast007/UIDesignTester
cd UIDesignTester
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=$HOME/code/vcpkg/scripts/buildsystems/vcpkg.cmake
cd build
make -j$(nproc)
```

Windows:

```bat
mkdir %USERPROFILE%\code
cd /d %USERPROFILE%\code
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
bootstrap-vcpkg.bat -disableMetrics
vcpkg integrate install
cd ..
git clone https://github.com/blast007/UIDesignTester
cd UIDesignTester
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=%USERPROFILE%/code/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build
```

macOS:

```shell
mkdir $HOME/code
cd $HOME/code
git clone https://github.com/Microsoft/vcpkg.git
./vcpkg/bootstrap-vcpkg.sh --disableMetrics
git clone https://github.com/blast007/UIDesignTester
cd UIDesignTester
alias cmake=/Applications/CMake.app/Contents/bin/cmake
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=$HOME/code/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build
```


License
-------

The files created specifically for this test application are released under the MIT license (See LICENSE.md).

Some files, as listed under Credits, are released under a different license.


Credits
-------

https://opengameart.org/content/golden-ui-bigger-than-ever-edition (CC-BY-SA 3.0) by Buch (https://opengameart.org/users/buch)
* assets/images/ui_big_pieces.tga


https://github.com/BZFlag-Dev/bzflag (GNU LGPL 2.1, Copyright Tim Riker)
* assets/menus/title.tga
* assets/images/playericon.tga

https://github.com/derhass/HelloCube (no license specified)
* src/GLCube.cxx
* src/GLCube.h

See assets/fonts/LICENSE.txt
* assets/fonts/*

https://github.com/mikke89/RmlUi
```
MIT License

Copyright (c) 2008-2014 CodePoint Ltd, Shift Technology Ltd, and contributors
Copyright (c) 2019-2021 The RmlUi Team, and contributors

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```
* assets/basic.rcss
* assets/images/invader.tga
* assets/window.rcss
* assets/window.rml
* src/RmlUi\_Renderer\_GL3.cpp
* src/RmlUi\_Renderer\_GL3.h
