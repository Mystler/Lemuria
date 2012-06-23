/*==LICENSE==

This file is part of Lemuria.
Copyright (C) 2012 Florian Meißner

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

#ifndef _crClient_h
#define _crClient_h

#include <OgreRoot.h>
#include <OgreWindowEventUtilities.h>
#include <OgreException.h>
#include <OgreConfigFile.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreWindowEventUtilities.h>
#include <OgrePlugin.h>
#include <OgreCamera.h>
#include <OgreViewport.h>
#include <OgreEntity.h>
#include <OgreLight.h>
#include <OgreSubMesh.h>
#include <OgreSubEntity.h>

#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

#include <CEGUISystem.h>
#include <CEGUIWindow.h>
#include <CEGUIWindowManager.h>
#include <CEGUISchemeManager.h>
#include <CEGUIFontManager.h>
#include <RendererModules/Ogre/CEGUIOgreRenderer.h>

#include <OgreOggSound.h>

#include <map>

#include "MessageIdentifiers.h"
#include "RakPeerInterface.h"
#include "RakNetTypes.h"
#include "BitStream.h"

#define SERVER_PORT 27015

#include "shared/ntPlayer.h"
#include "shared/ntManager.h"
#include "phAvatarController.h"
#include "scObjectMgr.h"
#include "3rdparty/ogremax/OgreMaxScene.hpp"

using namespace Ogre;
using namespace RakNet;

class crClient : public WindowEventListener, public FrameListener, public OIS::MouseListener, public OIS::KeyListener {
public:
    static crClient &getInstance();
    bool init(void);
    SceneManager *getSceneManager();
    bool ntMultiplayer;
    String ntServerIP;
    RakString ntPlayerName;
    bool captureQuit(const CEGUI::EventArgs& e) {
        fShutDown = true;
        return true;
    }
private:
    crClient();
    ~crClient();
    void Scene01();
    void windowResized(RenderWindow *rw);
    void windowClosed(RenderWindow *rw);
    bool frameRenderingQueued(const FrameEvent &evt);
    bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
    bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
    bool mouseMoved(const OIS::MouseEvent &arg);
    bool keyPressed(const OIS::KeyEvent &arg);
    bool keyReleased(const OIS::KeyEvent &arg);

    //Rendering
    String mPluginsCfg;
    Root *mRoot;
    RenderWindow *mWindow;
    SceneManager *mSceneMgr;
    bool btWireframe;
    //GUI
    CEGUI::Renderer *mGUIRenderer;
    CEGUI::Window *fMMRoot;
    CEGUI::Window *fBtnExit;
    bool fMMActive;
    bool fShutDown;
    //Sound
    OgreOggSound::OgreOggSoundManager *mSoundManager;
    //OIS Input
    OIS::InputManager *mInputManager;
    OIS::Mouse *mMouse;
    OIS::Keyboard *mKeyboard;
    bool kShiftDown;
    //AvatarMovement
    phAvatarController *phAvatar;
    float avWalkSpeed, avRotateSpeed;
    bool avMouseLook;
    bool avWalk, avWalkBack, avWalkLeft, avWalkRight;
    bool avJump, avFly;
    //Scenes
    Camera *mCamera;
    Viewport *vp;
    OgreMax::OgreMaxScene scMgr;
    scObjectMgr scObjMgr;
    //Multiplayer
    ntManager *ntMgr;
    RakPeerInterface *ntPeer;
    Packet *ntPacket;
    SystemAddress ntServerAddress;
    BitStream bsOut;
    bool ntConnected;
    std::map<uint32_t, phAvatarController*> ntPlayers;
    ntPlayer* myPlayer;
    float rotate;
};
#endif
