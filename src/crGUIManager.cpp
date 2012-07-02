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

#include <CEGUI.h>
#include <RendererModules/Ogre/CEGUIOgreRenderer.h>

#include "crClient.h"
#include "crGUIManager.h"

using namespace CEGUI;

enum guiRootNames {
    kEscMenu = 0,
};

crGUIManager::crGUIManager()
: fEscActive(false),
  fMouseLook(true) {
}

void crGUIManager::setup() {
    //setup
    fGUIRenderer = &OgreRenderer::bootstrapSystem();
    //configuration
    SchemeManager::getSingleton().create((utf8 *)"VanillaSkin.scheme");
    System::getSingleton().setDefaultMouseCursor("Vanilla-Images", "MouseArrow");
    MouseCursor::getSingleton().setImage("Vanilla-Images", "MouseArrow");
    MouseCursor::getSingleton().hide();
    CEGUI::FontManager::getSingleton().create("DejaVuSans-10.font");
    System::getSingleton().setDefaultFont("DejaVuSans-10");

    //get resolution from Window
    fHeight = crClient::getInstance().getWindowHeight();
    fWidth  = crClient::getInstance().getWindowWidth();
}

void crGUIManager::loadGlobalGUI() {
    //------------
    //Esc Menu
    //------------
    if(fGUIRoots.size() <= kEscMenu)
        fGUIRoots.resize(kEscMenu + 1);
    fGUIRoots[kEscMenu] = WindowManager::getSingleton().loadWindowLayout("MainMenu.layout");
    System::getSingleton().setGUISheet( fGUIRoots[kEscMenu] );
    fGUIRoots[kEscMenu]->setVisible(false);
    //register quit button for events
    fGUIRoots[kEscMenu]->getChildRecursive("Root/Background/btnExit")->subscribeEvent(PushButton::EventClicked,Event::Subscriber(&crGUIManager::captureQuit,this));
}

bool crGUIManager::captureQuit(const EventArgs& e) {
    crClient::getInstance().shutdown();
    return true;
}

bool crGUIManager::toggleEscapeMenu() {
    if(!fEscActive) {
        fGUIRoots[kEscMenu]->setVisible(true);
        if(fMouseLook) {
            MouseCursor::getSingleton().show();
            MouseCursor::getSingleton().setPosition(Point(fWidth / 2, fHeight / 2));
        }
        fEscActive = true;
        return false;
    } else {
        fGUIRoots[kEscMenu]->setVisible(false);
        fEscActive = false;
        if(fMouseLook) {
            MouseCursor::getSingleton().hide();
            return true;
        }
        return false;
    }
}

bool crGUIManager::toggleMouseCursor() {
    if(!fEscActive) {
        if(fMouseLook) {
            MouseCursor::getSingleton().show();
            MouseCursor::getSingleton().setPosition(Point(fWidth / 2, fHeight / 2));
            fMouseLook = false;
        } else {
            MouseCursor::getSingleton().hide();
            fMouseLook = true;
        }
    }
    return fMouseLook;
}
