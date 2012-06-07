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
#include "shared/ntMessage.h"

#define kWalkSpeed 100
#define kRunSpeed 200

enum GameMessages {
    NEW_CLIENT = ID_USER_PACKET_ENUM + 1,
    SPAWN_POSITION = ID_USER_PACKET_ENUM + 2,
    PLAYER_UPDATE = ID_USER_PACKET_ENUM + 3,
    PLAYERNAME = ID_USER_PACKET_ENUM + 4,
    DISCONNECT_PLAYER = ID_USER_PACKET_ENUM + 5
};

crClient::crClient()
    : mRoot(0),
      mWindow(0),
      mSceneMgr(0),
      mCamera(0),
      mGUIRenderer(0) {
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
    mGUIRenderer = &CEGUI::OgreRenderer::bootstrapSystem();

    //configure CEGUI
    CEGUI::SchemeManager::getSingleton().create((CEGUI::utf8 *)"VanillaSkin.scheme");
    CEGUI::System::getSingleton().setDefaultMouseCursor("Vanilla-Images", "MouseArrow");
    CEGUI::MouseCursor::getSingleton().setImage("Vanilla-Images", "MouseArrow");
    CEGUI::MouseCursor::getSingleton().hide();
    CEGUI::FontManager::getSingleton().create("DejaVuSans-10.font");
    CEGUI::System::getSingleton().setDefaultFont("DejaVuSans-10");

    //init OgreOggSound
    //mSoundManager = OgreOggSound::OgreOggSoundManager::getSingletonPtr();
    //mSoundManager->init();

    //let there be light
    Scene01();

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
    mCamera->setPosition(Vector3(0, 1.8f, 0));
    mCamera->lookAt(Vector3::UNIT_X * 30);
    mCamera->setNearClipDistance(0.01f);

    Viewport *vp = mWindow->addViewport(mCamera);
    vp->setBackgroundColour(ColourValue(0, 0, 0));
    mCamera->setAspectRatio(Real(vp->getActualWidth()) / Real(vp->getActualHeight()));

    //create Avatar
    SceneNode *avatarNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("avatarNode");
    avatarNode->setPosition(Vector3(0, 1.8f, 0));
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

    mKeyboard->capture();
    mMouse->capture();

    if(mKeyboard->isKeyDown(OIS::KC_ESCAPE)) {
        if(ntMultiplayer)
            ntPeer->CloseConnection(ntServerAddress, true);
        return false;
    }

    //Update OgreMax scene
    scMgr.Update(evt.timeSinceLastFrame);

    //Avatar Controls
    if(kShiftDown && !avWalkBack) {
        avWalkSpeed = kRunSpeed;
    } else {
        avWalkSpeed = kWalkSpeed;
    }

    Vector3 avWalkDir;
    if(avFly) {
        avWalkDir = mCamera->getDirection();
    } else {
        avWalkDir = Vector3(mCamera->getDirection().x, 0, mCamera->getDirection().z);
    }
    Vector3 avWalkLeftDir = Vector3(mCamera->getDirection().z, 0, -mCamera->getDirection().x);
    avWalkDir.normalise();
    avWalkLeftDir.normalise();

    Vector3 direction = Vector3::ZERO;
    if(avWalk)
        direction += avWalkDir;
    if(avWalkBack)
        direction -= avWalkDir;
    if(avWalkLeft)
        direction += avWalkLeftDir;
    if(avWalkRight)
        direction -= avWalkLeftDir;
    if(direction != Vector3::ZERO)
        direction.normalise();

    if(avFly) {
        if(avWalk || avWalkBack || avWalkLeft || avWalkRight)
            mCamera->move(avWalkSpeed * direction * evt.timeSinceLastFrame);
    } else {
        phAvatar->move(avWalkSpeed * evt.timeSinceLastFrame, direction, mCamera->getOrientation().getYaw().valueRadians() + Math::PI);
        Vector3 avCamPos = phAvatar->getPosition() + Vector3(0, 0.7f, 0);
        mCamera->setPosition(avCamPos);
    }

    if(avJump) {
        phAvatar->jump();
        avJump = false;
    }

    //Multiplayer
    if(ntMultiplayer) {
        for(ntPacket = ntPeer->Receive(); ntPacket; ntPeer->DeallocatePacket(ntPacket), ntPacket = ntPeer->Receive()) {
            BitStream bsIn(ntPacket->data, ntPacket->length, false);
            bsIn.IgnoreBytes(sizeof(MessageID));

            ntMessage *inMsg = new ntMessage();
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
                case SPAWN_POSITION:
                    //get position and spawn
                    ntClientID = inMsg->getClientID();
                    pos = inMsg->readVector();
                    LogManager::getSingletonPtr()->logMessage("MULTI: Got client number " + StringConverter::toString(ntClientID));
                    LogManager::getSingletonPtr()->logMessage("MULTI: Spawn " + StringConverter::toString(pos.x) + ", " +
                            StringConverter::toString(pos.y) + ", " + StringConverter::toString(pos.z));
                    phAvatar->setPosition(pos);
                    ntMgr->sendPlNameMsg(ntPlayerName);
                    break;
                case ID_CONNECTION_REQUEST_ACCEPTED:
                    ntConnected = true;
                    LogManager::getSingletonPtr()->logMessage("MULTI: Server accepted connection");
                    break;
                case PLAYER_UPDATE:
                    ntNetClientID = inMsg->getClientID();
                    player = inMsg->readPlayer();
                    playerNode = "ndPlayer";
                    playerNr << ntNetClientID;
                    playerNode.append(playerNr.str());
                    if(mSceneMgr->hasSceneNode(playerNode)) {
                        playerCtrl = players[searchForPlayer(ntNetClientID)]->getController();
                        if(playerCtrl) //<--------isn't !playerCtrl?
                            player->setController(playerCtrl); //can be deleted when ntPlayer is sent
                        players[searchForPlayer(ntNetClientID)] = player;
                        player->setToSavedPosition();
                    } else {
                        LogManager::getSingletonPtr()->logMessage("MULTI: Player node " + playerNode + " not found");
                    }
                    break;
                case NEW_CLIENT:
                    ntNetClientID = inMsg->getClientID();
                    pos = inMsg->readVector();
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
                    ndPlayer->setDirection(Vector3::UNIT_X);
                    playerCtrl = new phAvatarController(phBullet::getInstance().createPhysicalAvatar(ndPlayer));
                    player = new ntPlayer(playerCtrl, ntNetClientID, pos, Math::PI);
                    player->setToSavedPosition();

                    players.push_back(player);
                    break;
                case DISCONNECT_PLAYER:
                    bsIn.Read(ntNetClientID);
                    LogManager::getSingletonPtr()->logMessage("MULTI: Client " +
                            StringConverter::toString(ntNetClientID) + "disconnected");
                    playerEnt = "entPlayer";
                    playerNode = "ndPlayer";
                    playerNr << ntNetClientID;
                    playerNode.append(playerNr.str());
                    playerEnt.append(playerNr.str());
                    delete players[searchForPlayer(ntNetClientID)]->getController();
                    players.erase(players.begin() + searchForPlayer(ntNetClientID));
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

        //check my Player for changes
        ntCurDir = mCamera->getRealDirection();
        float yaw = mCamera->getOrientation().getYaw().valueRadians() + Math::PI;
        ntPlayer *currPlayer = new ntPlayer(phAvatar, ntClientID, phAvatar->getPosition(), yaw);
        currPlayer->convertDirToFlag(avWalk, avWalkBack, avWalkLeft, avWalkRight);
        if(kShiftDown && !avWalkBack)
            currPlayer->setRunning();
        currPlayer->convertRotToFlag(rotate);
        uint32_t comp = 0;
        if(myPlayer)
            comp = currPlayer->compare(myPlayer);
        if(((comp & ntPlayer::kWalk) != 0) || ((comp & ntPlayer::kTurn) != 0)) {
            //there are interesting changes, so send the player
            ntMgr->sendPlayerMsg(currPlayer);
        }
        myPlayer = currPlayer;

        //move other players
        for(size_t i = 0; i < players.size(); i++) {
            ntPlayer *netPlayer = players[i];
            phAvatarController *netAvCtrl = netPlayer->getController();
            if(netPlayer->getWalking() != ntPlayer::kNoWalk) {
                float speed = kWalkSpeed;
                if((netPlayer->getWalking() & ntPlayer::kRun) != 0)
                    speed = kRunSpeed;
                netAvCtrl->move(speed * evt.timeSinceLastFrame, netPlayer->getWalkDir());
            }
            if(netPlayer->getTurning() != 0)
                netAvCtrl->setYaw(netPlayer->getYaw() + netPlayer->getTurning() + Math::PI);
        }

        //Do the magic physics simulation
        phBullet::getInstance().getWorld()->stepSimulation(evt.timeSinceLastFrame);
        phBullet::getInstance().getDbgDrawer()->step();
    }

    return true;
}

uint32_t crClient::searchForPlayer(uint32_t clientID) {
    uint32_t r = 0;
    for(size_t i = 0; i < players.size(); i++) {
        if(players[i]->getClientID() == clientID) {
            r = i;
            break;
        }
    }
    return r;
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
