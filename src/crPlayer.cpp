/*==LICENSE==

This file is part of Lemuria.
Copyright (C) 2012 Florian Mei�ner

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

#include "crPlayer.h"

//ctor-----------------------------------
//default--------------------------------
crPlayer::crPlayer(phAvatarController *ctrl)
: fAvCtrl(ctrl),
  fClientID(0),
  fWalking(0),
  fTurning(0),
  fPosition(Vector3(0,0,0)),
  fYaw(0) {
}

//ctor---------------------------------
//direction only-----------------------
crPlayer::crPlayer(phAvatarController *ctrl, uint32_t clientID, Vector3 vector, float yaw)
: fAvCtrl(ctrl),
  fClientID(clientID),
  fWalking(0),
  fTurning(0),
  fPosition(vector),
  fYaw(yaw) {
}

//ctor---------------------------------
//complete-----------------------------
crPlayer::crPlayer(phAvatarController *ctrl, uint32_t clientID, uint32_t walking, uint32_t turning, Vector3 vector, float yaw)
: fAvCtrl(ctrl),
  fClientID(clientID),
  fWalking(walking),
  fTurning(turning),
  fPosition(vector),
  fYaw(yaw) {
}

void crPlayer::setToSavedPosition() {
    if(fAvCtrl) {
        fAvCtrl->setPosition(fPosition);
        fAvCtrl->setYaw(fYaw);
    }
}

void crPlayer::setToPosition(Vector3 position) {
    if(fAvCtrl)
        fAvCtrl->setPosition(position);
}

void crPlayer::setToPosition(float x, float y, float z) {
    if(fAvCtrl)
        fAvCtrl->setPosition(Vector3(x, y, z));
}

void crPlayer::convertDirToFlag(bool avWalk, bool avWalkBack, bool avWalkLeft, bool avWalkRight) {
    fWalking = kNoWalk;
    if(avWalk)
        fWalking |= kWalkForward;
    if(avWalkBack)
        fWalking |= kWalkBack;
    if(avWalkLeft)
        fWalking |= kWalkLeft;
    if(avWalkRight)
        fWalking |= kWalkRight;
}

void crPlayer::convertRotToFlag(float rotSpeed) {
    fTurning = kNoTurn;
    if(rotSpeed < 0)
        fTurning |= kTurnLeft;
    if(rotSpeed > 0)
        fTurning |= kTurnRight;
}

Vector3 crPlayer::getWalkDir() {
    Quaternion rot = BtOgre::Convert::toOgre(fAvCtrl->getTransform().getRotation());
    Vector3 frontDir = rot.xAxis();
    Vector3 leftDir = Vector3(frontDir.z, 0, -frontDir.x);
    frontDir.normalise();
    leftDir.normalise();

    Vector3 dir = Vector3::ZERO;
    if((fWalking & kWalkForward) != 0)
        dir += frontDir;
    if((fWalking & kWalkBack) != 0)
        dir -= frontDir;
    if((fWalking & kWalkLeft) != 0)
        dir += leftDir;
    if((fWalking & kWalkRight) != 0)
        dir -= leftDir;
    if(dir != Vector3::ZERO)
        dir.normalise();

    return dir;
}

uint32_t crPlayer::compare(crPlayer *player) {
    uint32_t r = 0;
    if (player->getWalking() != fWalking)
        r |= kWalk;
    if (player->getTurning() != fTurning)
        r |= kTurn;
    if (player->getPosition() != fPosition)
        r |= kPosition;
    if (player->getYaw() != fYaw)
        r |= kYaw;

    return r;
}
