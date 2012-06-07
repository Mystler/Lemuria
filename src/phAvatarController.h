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

#include <OgreVector3.h>

using namespace Ogre;

class btRigidBody;
class btTransform;

class phAvatarController {
public:
    phAvatarController(btRigidBody *body);
    ~phAvatarController();
    void move(float walkSpeed, Vector3 direction);
    void move(float walkSpeed, Vector3 direction, float yaw);
    void jump();
    bool avatarOnGround();

    virtual btTransform getTransform();
    virtual void setTransform(btTransform &xform);
    virtual Vector3 getPosition();
    virtual void setPosition(Vector3 pos);
    virtual void setYaw(float yaw);
    virtual Vector3 getDirection();
private:
    btRigidBody *fBody;
};
#endif
