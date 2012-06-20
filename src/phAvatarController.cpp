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
#include "shared/ntPlayer.h"

phAvatarController::phAvatarController(btRigidBody *body)
: fBody(body) {
}

phAvatarController::~phAvatarController() {
    phBullet::getInstance().getWorld()->removeRigidBody(fBody);
    delete fBody;
}

void phAvatarController::move(float walkSpeed, Vector3 direction) {
    //fBody->setLinearVelocity(walkSpeed * BtOgre::Convert::toBullet(direction));
    fBody->applyCentralImpulse(walkSpeed * BtOgre::Convert::toBullet(direction));
}

void phAvatarController::move(float walkSpeed, Vector3 direction, float yaw) {
    //fBody->setLinearVelocity(walkSpeed * BtOgre::Convert::toBullet(direction));
    fBody->applyCentralImpulse(walkSpeed * BtOgre::Convert::toBullet(direction));
    setYaw(yaw);
}

void phAvatarController::jump() {
    if(!avatarOnGround())
        return;
    //btScalar magnitude = (1 / fBody->getInvMass());
    float jumpHeight = 0.7f;
    btScalar magnitude = (1 / fBody->getInvMass()) * sqrt(2.f * 9.81f * jumpHeight);
    fBody->applyCentralImpulse(btVector3(0, 1, 0) * magnitude);
    //fBody->setLinearVelocity(30 * btVector3(0, 1, 0));
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

void phAvatarController::setWalkingFlag(bool forward, bool backward, bool left, bool right, bool run) {
    fWalkingFlag = kNoWalk;
    if(forward)
        fWalkingFlag |= kWalkForward;
    if(backward)
        fWalkingFlag |= kWalkBack;
    if(left)
        fWalkingFlag |= kWalkLeft;
    if(right)
        fWalkingFlag |= kWalkRight;
    if(run)
        fWalkingFlag |= kRun;
}

void phAvatarController::setTurningFlag(float rotSpeed) {
    fTurningFlag = kNoTurn;
    if(rotSpeed < 0)
        fTurningFlag |= kTurnLeft;
    if(rotSpeed > 0)
        fTurningFlag |= kTurnRight;
}

void phAvatarController::getFlagsFromPlayer(ntPlayer *player) {
    fWalkingFlag = player->getWalking();
    fTurningFlag = player->getTurning();
}

btTransform phAvatarController::getTransform() {
    btTransform xform;
    fBody->getMotionState()->getWorldTransform(xform);
    return xform;
}

void phAvatarController::setTransform(btTransform &xform) {
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

float phAvatarController::getYaw() {
    btQuaternion rot = getTransform().getRotation();
    return rot.getAngle();
}

void phAvatarController::setYaw(float yaw) {
    btQuaternion rot = btQuaternion(btVector3(0, 1, 0), yaw);
    btTransform xform = getTransform();
    xform.setRotation(rot);
    setTransform(xform);
}

Vector3 phAvatarController::getDirection() {
    Quaternion rot = BtOgre::Convert::toOgre(getTransform().getRotation());
    Vector3 dir = rot.zAxis();
    return dir;
}

Vector3 phAvatarController::getWalkingDirection() {
    Vector3 frontDir = getDirection();
    Vector3 leftDir = Vector3(frontDir.z, 0, -frontDir.x);
    frontDir.normalise();
    leftDir.normalise();

    Vector3 dir = Vector3::ZERO;
    if(fWalkingFlag & kWalkForward)
        dir += frontDir;
    if(fWalkingFlag & kWalkBack)
        dir -= frontDir;
    if(fWalkingFlag & kWalkLeft)
        dir += leftDir;
    if(fWalkingFlag & kWalkRight)
        dir -= leftDir;
    if(dir != Vector3::ZERO)
        dir.normalise();

    return dir;
}

Vector3 phAvatarController::getFlymodeDirection(Vector3 camDirection) {
    Vector3 leftDir = Vector3(camDirection.z, 0, -camDirection.x);
    camDirection.normalise();
    leftDir.normalise();

    Vector3 dir = Vector3::ZERO;
    if(fWalkingFlag & kWalkForward)
        dir += camDirection;
    if(fWalkingFlag & kWalkBack)
        dir -= camDirection;
    if(fWalkingFlag & kWalkLeft)
        dir += leftDir;
    if(fWalkingFlag & kWalkRight)
        dir -= leftDir;
    if(dir != Vector3::ZERO)
        dir.normalise();

    return dir;
}
