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
    //Variables
    int fIsWalking;
    int fIsTurning;
    Vector3 fPosition;
    float fYaw;

    enum walkingFlags {
        kNoWalk,
        kWalkFore,
        kWalkBack,
        kWalkLeft,
        kWalkRight,
    };
    enum turningFalgs {
        kNoTurn,
        kTurnLeft,
        kTurnRight,
    };

    //ctors
    crPlayer(SceneManager* sceneMgr);
    crPlayer(SceneManager* sceneMgr, int clientID, Vector3 vector, float yaw);
    crPlayer(SceneManager* sceneMgr, int clientID, int walking, int turning, Vector3 vector, float yaw);

    int getClientID() { return fClientID; }

    //functions
    void setToSavedPosition();
    void setToPosition(Vector3 position);
    void setToPosition(float x, float y, float z);
private:
    int fClientID;
    SceneNode* ndPlayer;
    String playerNode;
    stringstream playerNr;
    SceneManager* fSceneMgr;
};
#endif
