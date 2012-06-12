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

#ifndef _phAvatarController_h
#define _phAvatarController_h

#include "phBullet.h"
#include <OgreVector3.h>

using namespace Ogre;

class ntPlayer;

class phAvatarController {
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

    phAvatarController(btRigidBody *body);
    ~phAvatarController();

    //movement controls
    void move(float walkSpeed, Vector3 direction);
    void move(float walkSpeed, Vector3 direction, float yaw);
    void jump();
    bool avatarOnGround();

    //movement flags - local use
    uint32_t getWalkingFlag() { return fWalkingFlag; }
    void setWalkingFlag(bool forward, bool backward, bool left, bool right, bool run = false);
    uint32_t getTurningFlag() { return fTurningFlag; }
    void setTurningFlag(float rotSpeed);
    //movement flags - other players
    void getFlagsFromPlayer(ntPlayer *player);

    //physical information
    btTransform getTransform();
    void setTransform(btTransform &xform);
    Vector3 getPosition();
    void setPosition(Vector3 pos);
    float getYaw();
    void setYaw(float yaw);
    Vector3 getDirection();
    Vector3 getWalkingDirection();
    Vector3 getFlymodeDirection(Vector3 camDirection);
private:
    btRigidBody *fBody;
    uint32_t fWalkingFlag, fTurningFlag;
};
#endif
