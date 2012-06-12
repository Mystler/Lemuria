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

#ifndef _ntPlayer_h
#define _ntPlayer_h

#include <OgreVector3.h>

using namespace Ogre;

class ntPlayer {
public:
    enum compFlags {
        kWalk = 1,
        kTurn = 2,
        kPosition = 4,
        kYaw = 8,
    };

    //ctors
    ntPlayer();
    ntPlayer(uint32_t clientID, Vector3 vector, float yaw);
    ntPlayer(uint32_t clientID, uint32_t walking, uint32_t turning, Vector3 vector, float yaw);

    void setClientID(uint32_t id) { fClientID = id; }
    uint32_t getClientID() { return fClientID; }
    void setWalking(uint32_t flag) { fWalking = flag; }
    uint32_t getWalking() { return fWalking; }
    void setTurning(uint32_t flag) { fTurning = flag; }
    uint32_t getTurning() { return fTurning; }
    void setYaw(float yaw) { fYaw = yaw; }
    float getYaw() { return fYaw; }
    void setPosition(Vector3 pos) { fPosition = pos; }
    Vector3 getPosition() { return fPosition; }

    uint32_t compare(ntPlayer *player);
private:
    uint32_t fClientID, fTurning, fWalking;
    float fYaw;
    Vector3 fPosition;
};
#endif
