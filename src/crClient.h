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

#include <vector>

#include "MessageIdentifiers.h"
#include "RakPeerInterface.h"
#include "RakNetTypes.h"
#include "BitStream.h"

#define SERVER_PORT 27015

#include "phBullet.h"
#include "scObjectMgr.h"
#include "3rdparty/ogremax/OgreMaxScene.hpp"
#include "crPlayer.h"

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
private:
    crClient(void);
    virtual ~crClient(void);
    virtual void windowResized(RenderWindow *rw);
    virtual void windowClosed(RenderWindow *rw);
    virtual bool frameRenderingQueued(const FrameEvent &evt);
    virtual bool frameStarted(const FrameEvent &evt);
    virtual bool frameEnded(const FrameEvent &evt);
    virtual bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
    virtual bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
    virtual bool mouseMoved(const OIS::MouseEvent &arg);
    virtual bool keyPressed(const OIS::KeyEvent &arg);
    virtual bool keyReleased(const OIS::KeyEvent &arg);
    String mPluginsCfg;
    Root *mRoot;
    RenderWindow *mWindow;
    SceneManager *mSceneMgr;
    OIS::InputManager *mInputManager;
    OIS::Mouse *mMouse;
    OIS::Keyboard *mKeyboard;
    //Physics
    btRigidBody *phAvatar;
    bool btWireframe;
    //GUI
    CEGUI::Renderer *mGUIRenderer;
    //Sound
    OgreOggSound::OgreOggSoundManager *mSoundManager;
    //OIS Input
    bool kShiftDown;
    //AvatarMovement
    Real avWalkSpeed, avRotateSpeed, avJumpTime;
    bool avMouseLook;
    bool avWalk, avWalkBack, avWalkLeft, avWalkRight;
    bool avJump, avFly;
    //Scenes
    Camera *mCamera;
    Viewport *vp;
    OgreMax::OgreMaxScene scMgr;
    scObjectMgr scObjMgr;
    virtual void Scene01(void);
    //Multiplayer
    RakPeerInterface *ntPeer;
    Packet *ntPacket;
    SystemAddress ntServerAddress;
    BitStream bsOut;
    bool ntConnected;
    int ntClientID;
    Real ntPosSendTime, ntRotSendTime;
    Vector3 ntCurPos, ntCurDir;
    std::vector<crPlayer*> players;
};
#endif
