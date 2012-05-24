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

#include "phAvatarController.h"

phAvatarController::phAvatarController(btRigidBody *body)
: fBody(body) {
}

phAvatarController::~phAvatarController() {
    delete fBody;
}

void phAvatarController::move(float walkSpeed, Vector3 direction) {
    fBody->setLinearVelocity(walkSpeed * BtOgre::Convert::toBullet(direction));
}

void phAvatarController::move(float walkSpeed, Vector3 direction, float yaw) {
    fBody->setLinearVelocity(walkSpeed * BtOgre::Convert::toBullet(direction));
    setYaw(yaw);
}

void phAvatarController::jump() {
    if(!avatarOnGround())
        return;
    btScalar magnitude = (1 / fBody->getInvMass()) * 16;
    fBody->applyCentralImpulse(btVector3(0, 1, 0) * magnitude);
}

bool phAvatarController::avatarOnGround() {
    btVector3 avPos = BtOgre::Convert::toBullet(getPosition());
    btVector3 avToGround = avPos + btVector3(0, -1.1f, 0);
    btDynamicsWorld::ClosestRayResultCallback groundRay(avPos, avToGround);
    phBullet::getInstance().getWorld()->rayTest(avPos, avToGround, groundRay);
    if(groundRay.hasHit()) {
        return true;
    }
    return false;
}

btTransform phAvatarController::getTransform() {
    btTransform xform;
    fBody->getMotionState()->getWorldTransform(xform);
    return xform;
}

void phAvatarController::setTransform(btTransform xform) {
    fBody->getMotionState()->setWorldTransform(xform);
    fBody->setCenterOfMassTransform(xform);
}

Vector3 phAvatarController::getPosition() {
    return BtOgre::Convert::toOgre(getTransform().getOrigin());
}

void phAvatarController::setPosition(Vector3 pos) {
    btTransform xform = getTransform();
    xform.setOrigin(BtOgre::Convert::toBullet(pos));
    setTransform(xform);
}

void phAvatarController::setYaw(float yaw) {
    btQuaternion rot = btQuaternion(btVector3(0, 1, 0), yaw);
    btTransform xform = getTransform();
    xform.setRotation(rot);
    setTransform(xform);
}
