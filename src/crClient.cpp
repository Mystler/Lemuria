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
/*
    TODO: replace networking stuff with ntNetMgr
*/
#include "crClient.h"
#include "crGUIManager.h"
#include "shared/ntMessage.h"

#define kWalkSpeed 2
#define kRunSpeed 4

crClient::crClient()
    : mRoot(0),
      mWindow(0),
      mSceneMgr(0),
      mCamera(0) {
}

crClient::~crClient() {
    //unload Multiplayer
    if(ntMultiplayer) {
        RakPeerInterface::DestroyInstance(ntPeer);
    }

    //unload Sounds
    //mSoundManager->destroyAllSounds();

    //unload OgreMax
    scMgr.Destroy();

    //unload main OGRE stuff
    WindowEventUtilities::removeWindowEventListener(mWindow, this);
    windowClosed(mWindow);
    delete mRoot;
}

crClient &crClient::getInstance() {
    static crClient crInstance;
    return crInstance;
}

bool crClient::init(void) {
    //configure
#ifdef _DEBUG
    mPluginsCfg = "../cfg/plugins_d.cfg";
#else
    mPluginsCfg = "../cfg/plugins.cfg";
#endif

    mRoot = new Root(mPluginsCfg);

    if(!(mRoot->restoreConfig() || mRoot->showConfigDialog())) {
        return false;
    }

    mWindow = mRoot->initialise(true, "Project Lemuria");

    mSceneMgr = mRoot->createSceneManager("DefaultSceneManager");

    //load resources
    ResourceGroupManager::getSingleton().addResourceLocation("../dat/scene/material", "FileSystem");
    ResourceGroupManager::getSingleton().addResourceLocation("../dat/scene/texture", "FileSystem");
    ResourceGroupManager::getSingleton().addResourceLocation("../dat/scene/mesh", "FileSystem");
    ResourceGroupManager::getSingleton().addResourceLocation("../dat/scene", "FileSystem");
    ResourceGroupManager::getSingleton().addResourceLocation("../dat/gui/imagesets", "FileSystem");
    ResourceGroupManager::getSingleton().addResourceLocation("../dat/gui/looknfeel", "FileSystem");
    ResourceGroupManager::getSingleton().addResourceLocation("../dat/gui/schemes", "FileSystem");
    ResourceGroupManager::getSingleton().addResourceLocation("../dat/gui/layouts", "FileSystem");
    ResourceGroupManager::getSingleton().addResourceLocation("../dat/gui/fonts", "FileSystem");
    ResourceGroupManager::getSingleton().addResourceLocation("../dat/sfx", "FileSystem");
    TextureManager::getSingleton().setDefaultNumMipmaps(5);
    ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

    //read Multiplayer config
    ConfigFile cfgMultiplayer;
    cfgMultiplayer.load("../cfg/multiplayer.cfg");
    String cfgMultiOn = cfgMultiplayer.getSetting("Multiplayer", "", "0");
    if(cfgMultiOn == "1")
        ntMultiplayer = true;
    ntServerIP = cfgMultiplayer.getSetting("ServerIP", "", "127.0.0.1");
    ntPlayerName.Set(cfgMultiplayer.getSetting("Playername", "", "Guest").c_str());

    //connect to server if multiplayer is active
    if(ntMultiplayer) {
        LogManager::getSingletonPtr()->logMessage("MULTI: Active");
        ntMgr = new ntManager();
        ntPeer = ntMgr->connect(ntServerIP.c_str(), SERVER_PORT);
        LogManager::getSingletonPtr()->logMessage("MULTI: Connecting to server " + ntServerIP);
    }

    //initialize OIS
    LogManager::getSingletonPtr()->logMessage("*** Initializing OIS ***");
    OIS::ParamList pl;
    size_t windowHnd = 0;
    std::ostringstream windowHndStr;

    mWindow->getCustomAttribute("WINDOW", &windowHnd);
    windowHndStr << windowHnd;
    pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

    mInputManager = OIS::InputManager::createInputSystem(pl);

    mKeyboard = static_cast<OIS::Keyboard *>(mInputManager->createInputObject(OIS::OISKeyboard, true));
    mMouse = static_cast<OIS::Mouse *>(mInputManager->createInputObject(OIS::OISMouse, true));

    mMouse->setEventCallback(this);
    mKeyboard->setEventCallback(this);

    windowResized(mWindow);

    WindowEventUtilities::addWindowEventListener(mWindow, this);

    //setup Bullet Physics and Debug Drawer
    phBullet::getInstance().initDebugDrawer(mSceneMgr);
    phBullet::getInstance().getDbgDrawer()->setDebugMode(0);

    //Avatar settings
    avMouseLook = true;
    avRotateSpeed = 0.1f;

    //CEGUI setup
    guiMgr = new crGUIManager();

    //init OgreOggSound
    //mSoundManager = OgreOggSound::OgreOggSoundManager::getSingletonPtr();
    //mSoundManager->init();

    //let there be light
    Scene01();

    fShutDown = false;

    //add the Frame Listener
    mRoot->addFrameListener(this);

    //start rendering
    mRoot->startRendering();

    return true;
}

SceneManager *crClient::getSceneManager() {
    return mSceneMgr;
}

void crClient::Scene01() {
    //init Camera and Viewport
    mCamera = mSceneMgr->createCamera("PlayerCam");
    mCamera->setPosition(SPAWNPOS);
    mCamera->lookAt(Vector3::UNIT_X * 30);
    mCamera->setNearClipDistance(0.01f);

    Viewport *vp = mWindow->addViewport(mCamera);
    vp->setBackgroundColour(ColourValue(0, 0, 0));
    mCamera->setAspectRatio(Real(vp->getActualWidth()) / Real(vp->getActualHeight()));

    //create Avatar
    SceneNode *avatarNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("avatarNode");
    avatarNode->setPosition(SPAWNPOS);
    phAvatar = new phAvatarController(phBullet::getInstance().createPhysicalAvatar(avatarNode));

    scMgr.Load("test.scene", mWindow, 0, mSceneMgr);
    scObjMgr.processScene(mSceneMgr);

    //mSoundManager->createSound("Music", "music.ogg", false, true, true);
    //mSoundManager->getSound("Music")->setVolume(0.2f);
    //mSoundManager->getSound("Music")->play();
}

void crClient::windowResized(RenderWindow *rw) {
    uint32_t width, height, depth;
    int left, top;
    rw->getMetrics(width, height, depth, left, top);

    const OIS::MouseState &ms = mMouse->getMouseState();
    ms.width = width;
    ms.height = height;
}

void crClient::windowClosed(RenderWindow *rw) {
    if(rw == mWindow) {
        if(mInputManager) {
            mInputManager->destroyInputObject(mMouse);
            mInputManager->destroyInputObject(mKeyboard);

            OIS::InputManager::destroyInputSystem(mInputManager);
            mInputManager = 0;
        }
    }
}

bool crClient::frameRenderingQueued(const FrameEvent &evt) {
    if(mWindow->isClosed())
        return false;

    if(fShutDown == true)
        return false;

    rotate = 0;
    mKeyboard->capture();
    mMouse->capture();

    //Update OgreMax scene
    scMgr.Update(evt.timeSinceLastFrame);

    //Avatar Controls
    if(kShiftDown && !avWalkBack) {
        avWalkSpeed = kRunSpeed;
    } else {
        avWalkSpeed = kWalkSpeed;
    }

    Vector3 avWalkDir;
    phAvatar->setWalkingFlag(avWalk, avWalkBack, avWalkLeft, avWalkRight, kShiftDown);
    if(avFly) {
        avWalkDir = phAvatar->getFlymodeDirection(mCamera->getDirection());
        if(avWalk || avWalkBack || avWalkLeft || avWalkRight)
            mCamera->move(avWalkSpeed * avWalkDir * evt.timeSinceLastFrame);
        //do not send this
        phAvatar->setWalkingFlag(false, false, false, false, false);
    } else {
        phAvatar->setYaw(mCamera->getOrientation().getYaw().valueRadians() + Math::PI);
        if(phAvatar->getWalkingFlag() != phAvatarController::kNoWalk) {
            avWalkDir = phAvatar->getWalkingDirection();
            phAvatar->move(avWalkSpeed, avWalkDir);
        }
        Vector3 avCamPos = phAvatar->getPosition() + Vector3(0, 0.7f, 0);
        mCamera->setPosition(avCamPos);
    }

    if(avJump) {
        phAvatar->jump();
        if(ntMultiplayer)
            ntMgr->sendJumpMsg();
        avJump = false;
    }

    phAvatar->setTurningFlag(rotate);

    //Multiplayer
    if(ntMultiplayer) {
        for(ntPacket = ntPeer->Receive(); ntPacket; ntPeer->DeallocatePacket(ntPacket), ntPacket = ntPeer->Receive()) {
            ntMessage *inMsg;
            uint32_t ntNetClientID;
            Entity *entPlayer;
            SceneNode *ndPlayer;
            String playerEnt;
            String playerNode;
            stringstream playerNr;
            ntPlayer *player;
            phAvatarController *playerCtrl;
            Vector3 pos;

            inMsg = ntMgr->getMessage(ntPacket);

            switch(inMsg->getFlag()) {
                case ntManager::SPAWN_POSITION:
                    //get position and spawn
                    myPlayer = inMsg->readPlayer();
                    LogManager::getSingletonPtr()->logMessage("MULTI: Got client number " + StringConverter::toString(myPlayer->getClientID()));
                    LogManager::getSingletonPtr()->logMessage("MULTI: Spawn " + StringConverter::toString(pos.x) + ", " +
                            StringConverter::toString(pos.y) + ", " + StringConverter::toString(pos.z));
                    phAvatar->setPosition(myPlayer->getPosition());
                    phAvatar->setYaw(myPlayer->getYaw());
                    ntMgr->setMyClientID(inMsg->getClientID());
                    ntMgr->sendPlNameMsg(ntPlayerName);
                    break;
                case ID_CONNECTION_REQUEST_ACCEPTED:
                    ntConnected = true;
                    LogManager::getSingletonPtr()->logMessage("MULTI: Server accepted connection");
                    break;
                case ntManager::PLAYER_UPDATE:
                    ntNetClientID = inMsg->getClientID();
                    player = inMsg->readPlayer();
                    playerNode = "ndPlayer";
                    playerNr << ntNetClientID;
                    playerNode.append(playerNr.str());
                    if(mSceneMgr->hasSceneNode(playerNode)) {
                        playerCtrl = ntPlayers[ntNetClientID];
                        playerCtrl->getFlagsFromPlayer(player);
                        playerCtrl->setPosition(player->getPosition());
                        playerCtrl->setYaw(player->getYaw());
                    } else {
                        LogManager::getSingletonPtr()->logMessage("MULTI: Player node " + playerNode + " not found");
                    }
                    break;
                case ntManager::PLAYER_JUMP:
                    ntNetClientID = inMsg->getClientID();
                    playerNode = "ndPlayer";
                    playerNr << ntNetClientID;
                    playerNode.append(playerNr.str());
                    if(mSceneMgr->hasSceneNode(playerNode)) {
                        playerCtrl = ntPlayers[ntNetClientID];
                        playerCtrl->jump();
                    } else {
                        LogManager::getSingletonPtr()->logMessage("MULTI: Player node " + playerNode + " not found");
                    }
                    break;
                case ntManager::NEW_CLIENT:
                    ntNetClientID = inMsg->getClientID();
                    player = inMsg->readPlayer();
                    LogManager::getSingletonPtr()->logMessage("MULTI: New Client " +
                            StringConverter::toString(ntNetClientID) + " - " + StringConverter::toString(pos.x) +
                            ", " + StringConverter::toString(pos.y) + ", " + StringConverter::toString(pos.z));
                    playerEnt = "entPlayer";
                    playerNode = "ndPlayer";
                    playerNr << ntNetClientID;
                    playerNode.append(playerNr.str());
                    playerEnt.append(playerNr.str());
                    entPlayer = mSceneMgr->createEntity(playerEnt, "penguin.mesh");
                    ndPlayer = mSceneMgr->getRootSceneNode()->createChildSceneNode(playerNode);
                    ndPlayer->attachObject(entPlayer);
                    ndPlayer->scale(Vector3(0.035f, 0.035f, 0.035f));
                    //ndPlayer->setDirection(Vector3::UNIT_X);
                    playerCtrl = new phAvatarController(phBullet::getInstance().createPhysicalAvatar(ndPlayer));
                    playerCtrl->getFlagsFromPlayer(player);
                    playerCtrl->setPosition(player->getPosition());
                    playerCtrl->setYaw(player->getYaw());
                    ntPlayers[ntNetClientID] = playerCtrl;
                    break;
                case ntManager::DISCONNECT_PLAYER:
                    ntNetClientID = inMsg->getClientID();
                    LogManager::getSingletonPtr()->logMessage("MULTI: Client " +
                            StringConverter::toString(ntNetClientID) + "disconnected");
                    playerEnt = "entPlayer";
                    playerNode = "ndPlayer";
                    playerNr << ntNetClientID;
                    playerNode.append(playerNr.str());
                    playerEnt.append(playerNr.str());
                    ntPlayers.erase(ntNetClientID);
                    if(mSceneMgr->hasSceneNode(playerNode)) {
                        mSceneMgr->destroyEntity(playerEnt);
                        mSceneMgr->destroySceneNode(playerNode);
                    } else {
                        LogManager::getSingletonPtr()->logMessage("MULTI: Player node " + playerNode + " not found");
                    }
                    break;
                default:
                    LogManager::getSingletonPtr()->logMessage("MULTI: Got msg with ID " + ntPacket->data[0]);
                    break;
            }
            bsOut.Reset();
        }

        //kick client if no connection is established
        if(!ntConnected) {
            throw Exception(Exception::ERR_INVALID_STATE, "No connection to the server! Multiplayer is set", "multiplayer.cfg");
        }

        //check my Player for changes and send those
        if(phAvatar->getWalkingFlag() != myPlayer->getWalking() ||
            phAvatar->getTurningFlag() != myPlayer->getTurning()) {
            myPlayer->setWalking(phAvatar->getWalkingFlag());
            myPlayer->setTurning(phAvatar->getTurningFlag());
            myPlayer->setPosition(phAvatar->getPosition());
            myPlayer->setYaw(phAvatar->getYaw());
            ntMgr->sendPlayerMsg(myPlayer);
        }

        //move other players
        std::map<uint32_t, phAvatarController*>::iterator i;
        for(i = ntPlayers.begin(); i != ntPlayers.end(); i++) {
            uint32_t netId = i->first;
            phAvatarController *netAvCtrl = i->second;
            if(netAvCtrl->getWalkingFlag() != phAvatarController::kNoWalk) {
                float speed = kWalkSpeed;
                if(netAvCtrl->getWalkingFlag() & phAvatarController::kRun)
                    speed = kRunSpeed;
                netAvCtrl->move(speed, netAvCtrl->getWalkingDirection());
            }
            netAvCtrl->rotate(netAvCtrl->getTurningFlag());
        }
    }

    //Do the magic physics simulation
    phBullet::getInstance().getWorld()->stepSimulation(evt.timeSinceLastFrame, 10);
    phBullet::getInstance().getDbgDrawer()->step();

    return true;
}

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT)
#else
    int main(int argc, char *argv[])
#endif
    {
        crClient &app = crClient::getInstance();

        try {
            app.init();
        } catch(Exception &e) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
            MessageBox(NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
            std::cerr << "An exception has occured: " <<
                      e.getFullDescription().c_str() << std::endl;
#endif
        }

        return 0;
    }

#ifdef __cplusplus
}
#endif
