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

using namespace Ogre;

class crPlayer {
public:
    enum walkingFlags {
        kNoWalk,
        kWalkFore,
        kWalkBack,
        kWalkLeft,
        kWalkRight,
        kWalkFL,
        kWalkFR,
        kWalkBL,
        kWalkBR,
    };
    enum compFlags {
        kWalk = 1,
        kTurn = 2,
        kPosition = 4,
        kYaw = 8,
    };

    //ctors
    crPlayer(SceneManager *sceneMgr);
    crPlayer(SceneManager *sceneMgr, int clientID, Vector3 vector, float yaw);
    crPlayer(SceneManager *sceneMgr, int clientID, int walking, float turning, Vector3 vector, float yaw);

    uint32_t getClientID() { return fClientID; }
    uint32_t getWalking() { return fWalking; }
    void setTurning(float turning) { fTurning = turning; }
    float getTurning() { return fTurning; }
    float getYaw() { return fYaw; }
    Vector3 getPosition() { return fPosition; }
    void convertDirToFlag(bool avWalk, bool avWalkBack, bool avWalkLeft, bool avWalkRight);

    //functions
    void setSceneMgr(SceneManager *sceneMgr);
    void setToSavedPosition();
    void setToPosition(Vector3 position);
    void setToPosition(float x, float y, float z);
    uint32_t compare(crPlayer *player);
private:
    uint32_t fClientID, fWalking;
    float fTurning, fYaw;
    Vector3 fPosition;
    SceneNode* ndPlayer;
    String playerNode;
    stringstream playerNr;
    SceneManager *fSceneMgr;
};
#endif
