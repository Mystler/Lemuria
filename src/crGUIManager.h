/*==LICENSE==

This file is part of Lemuria.
Copyright (C) 2012 Florian Meiﬂner

Lemuria is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Lemuria is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Lemuria. If not, see <http://www.gnu.org/licenses/>.

*==LICENSE==*/

#ifndef _crGUIManager_h
#define _crGUIManager_h

#include <vector>

//Forward
namespace CEGUI {
    class Renderer;
    class Window;
    class EventArgs;
}

class crGUIManager {
public:
    //ctor
    crGUIManager();

    //functions
    void setup();
    void loadGlobalGUI();
    bool captureQuit(const CEGUI::EventArgs& e);
    bool toggleEscapeMenu(); //returns avMouseLook
    bool toggleMouseCursor(); //returns avMouseLook
private:
    CEGUI::Renderer *fGUIRenderer;
    CEGUI::Window *fEscRoot;
    std::vector<CEGUI::Window*> fGUIRoots;
    bool fEscActive;
    bool fMouseLook;
    int fWidth;
    int fHeight;
};
#endif
