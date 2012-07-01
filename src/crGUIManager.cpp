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
#include <CEGUISystem.h>
#include <RendererModules/Ogre/CEGUIOgreRenderer.h>

#include "crClient.h"
#include "crGUIManager.h"

crGUIManager::crGUIManager()
: fEscActive(false),
  fMouseLook(true) {
}

void crGUIManager::setup() {
    //setup
    fGUIRenderer = &CEGUI::OgreRenderer::bootstrapSystem();
    //configuration
    CEGUI::SchemeManager::getSingleton().create((CEGUI::utf8 *)"VanillaSkin.scheme");
    CEGUI::System::getSingleton().setDefaultMouseCursor("Vanilla-Images", "MouseArrow");
    CEGUI::MouseCursor::getSingleton().setImage("Vanilla-Images", "MouseArrow");
    CEGUI::MouseCursor::getSingleton().hide();
    CEGUI::FontManager::getSingleton().create("DejaVuSans-10.font");
    CEGUI::System::getSingleton().setDefaultFont("DejaVuSans-10");
    //load GUIs
    ///Esc Menu
    fEscRoot = CEGUI::WindowManager::getSingleton().loadWindowLayout("MainMenu.layout");
    CEGUI::System::getSingleton().setGUISheet( fEscRoot );
    fEscRoot->setVisible(false);
    fEscBtnExit = fEscRoot->getChildRecursive("Root/Background/btnExit");
    fEscBtnExit->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&crGUIManager::captureQuit,this));
    //get resolution from Window
    fHeight = crClient::getInstance().getWindowHeight();
    fWidth  = crClient::getInstance().getWindowWidth();
}

bool crGUIManager::captureQuit(const CEGUI::EventArgs& e) {
    crClient::getInstance().shutdown();
    return true;
}

bool crGUIManager::toggleEscapeMenu() {
    if(!fEscActive) {
        fEscRoot->setVisible(true);
        if(fMouseLook) {
            CEGUI::MouseCursor::getSingleton().show();
            CEGUI::MouseCursor::getSingleton().setPosition(CEGUI::Point(fWidth / 2, fHeight / 2));
        }
        fEscActive = true;
        return false;
    } else {
        fEscRoot->setVisible(false);
        fEscActive = false;
        if(fMouseLook) {
            CEGUI::MouseCursor::getSingleton().hide();
            return true;
        }
        return false;
    }
}

bool crGUIManager::toggleMouseCursor() {
    if(!fEscActive) {
        if(fMouseLook) {
            CEGUI::MouseCursor::getSingleton().show();
            CEGUI::MouseCursor::getSingleton().setPosition(CEGUI::Point(fWidth / 2, fHeight / 2));
            fMouseLook = false;
        } else {
            CEGUI::MouseCursor::getSingleton().hide();
            fMouseLook = true;
        }
    }
    return fMouseLook;
}
