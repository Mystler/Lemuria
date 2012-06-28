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

#include <OISEvents.h>

#include "crClient.h"
#include "crGUIManager.h"

crGUIManager::crGUIManager() {
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
    fEscActive = false;
}

bool crGUIManager::captureQuit(const CEGUI::EventArgs& e) {
    crClient::getInstance().requestShutDown();
    return true;
}

bool crGUIManager::toggleEscapeMenu() {
    if(!fEscActive) {
        fEscRoot->setVisible(true);
        CEGUI::MouseCursor::getSingleton().show();
        CEGUI::MouseCursor::getSingleton().setPosition(CEGUI::Point(0, 0));
        fEscActive = true;
        return false;
    } else {
        CEGUI::MouseCursor::getSingleton().hide();
        fEscRoot->setVisible(false);
        fEscActive = false;
        return true;
    }
}

bool crGUIManager::toggleMouseCursor(const OIS::MouseEvent arg, bool avMouseLook) {
    if(!fEscActive) {
        if(avMouseLook) {
            CEGUI::MouseCursor::getSingleton().show();
            CEGUI::MouseCursor::getSingleton().setPosition(CEGUI::Point(arg.state.width / 2, arg.state.height / 2));
            return false;
        } else {
            CEGUI::MouseCursor::getSingleton().hide();
            return true;
        }
    }
    return true;
}

void convertKey(const OIS::KeyCode upKeycode) {
    CEGUI::System::getSingleton().injectKeyUp(upKeycode);
}

void convertKey(const OIS::KeyCode downKeycode, unsigned int keyChar) {
    CEGUI::System::getSingleton().injectKeyDown(downKeycode);
    CEGUI::System::getSingleton().injectChar(keyChar);
}
