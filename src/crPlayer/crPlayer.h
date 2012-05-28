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

#ifndef _crPlayer_h
#define _crPlayer_h

#include <OgreVector3.h>
#include <OgreString.h>
#include <OgreSceneNode.h>
#include <OgreSceneManager.h>
#include "phAvatarController.h"

using namespace Ogre;

class crPlayer {
public:
    enum walkingFlags {
        kNoWalk = 0,
        kWalkForward = 1,
        kWalkBack = 2,
        kWalkLeft = 4,
        kWalkRight = 8,
        kRun = 16,
    };
    enum turningFlags {
        kNoTurn = 0,
        kTurnLeft = 1,
        kTurnRight = 2,
    };
    enum compFlags {
        kWalk = 1,
        kTurn = 2,
        kPosition = 4,
        kYaw = 8,
    };

    //ctors
    crPlayer(phAvatarController *ctrl);
    crPlayer(phAvatarController *ctrl, uint32_t clientID, Vector3 vector, float yaw);
    crPlayer(phAvatarController *ctrl, uint32_t clientID, uint32_t walking, uint32_t turning, Vector3 vector, float yaw);

    uint32_t getClientID() { return fClientID; }
    uint32_t getWalking() { return fWalking; }
    void setTurning(uint32_t turning) { fTurning = turning; }
    uint32_t getTurning() { return fTurning; }
    float getYaw() { return fYaw; }
    Vector3 getPosition() { return fPosition; }
    phAvatarController *getController() { return fAvCtrl; }
    void setController(phAvatarController *ctrl) { fAvCtrl = ctrl; }
    void setRunning() { fWalking |= kRun; }

    //functions
    void setToSavedPosition();
    void setToPosition(Vector3 position);
    void setToPosition(float x, float y, float z);
    void convertDirToFlag(bool avWalk, bool avWalkBack, bool avWalkLeft, bool avWalkRight);
    void convertRotToFlag(float rotSpeed);
    Vector3 getWalkDir();
    uint32_t compare(crPlayer *player);
private:
    phAvatarController *fAvCtrl;
    uint32_t fClientID, fWalking, fTurning;
    float fYaw;
    Vector3 fPosition;
    SceneNode* ndPlayer;
    String playerNode;
    stringstream playerNr;
    SceneManager *fSceneMgr;
};
#endif
