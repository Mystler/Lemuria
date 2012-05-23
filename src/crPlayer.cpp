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

#include "crPlayer.h"

//ctor-----------------------------------
//default--------------------------------
crPlayer::crPlayer(SceneManager *sceneMgr)
: fSceneMgr(sceneMgr),
  fClientID(0),
  fWalking(0),
  fTurning(0),
  fPosition(Vector3(0,0,0)),
  fYaw(0) {
    playerNode = "ndPlayer";
    playerNr << fClientID;
    playerNode.append(playerNr.str());
    if(fSceneMgr->hasSceneNode(playerNode)) {
        ndPlayer = fSceneMgr->getSceneNode(playerNode);
    }
}

//ctor---------------------------------
//direction only-----------------------
crPlayer::crPlayer(SceneManager *sceneMgr, int clientID, Vector3 vector, float yaw)
: fSceneMgr(sceneMgr),
  fClientID(clientID),
  fWalking(0),
  fTurning(0),
  fPosition(vector),
  fYaw(yaw) {
    playerNode = "ndPlayer";
    playerNr << fClientID;
    playerNode.append(playerNr.str());
    if(fSceneMgr->hasSceneNode(playerNode)) {
        ndPlayer = fSceneMgr->getSceneNode(playerNode);
    }
}

//ctor---------------------------------
//complete-----------------------------
crPlayer::crPlayer(SceneManager *sceneMgr, int clientID, int walking, float turning, Vector3 vector, float yaw)
: fSceneMgr(sceneMgr),
  fClientID(clientID),
  fWalking(walking),
  fTurning(turning),
  fPosition(vector),
  fYaw(yaw) {
    playerNode = "ndPlayer";
    playerNr << fClientID;
    playerNode.append(playerNr.str());
    if((fSceneMgr) && (fSceneMgr->hasSceneNode(playerNode))) {
        ndPlayer = fSceneMgr->getSceneNode(playerNode);
    }
}

void crPlayer::setSceneMgr(SceneManager *sceneMgr) {
    fSceneMgr = sceneMgr;
    if(fSceneMgr->hasSceneNode(playerNode))
        ndPlayer = fSceneMgr->getSceneNode(playerNode);
}

void crPlayer::setToSavedPosition() {
    ndPlayer->setPosition(fPosition);
    ndPlayer->setOrientation(Quaternion(Radian(fYaw), Vector3::UNIT_Y));
}

void crPlayer::setToPosition(Vector3 position) {
    ndPlayer->setPosition(position);
}

void crPlayer::setToPosition(float x, float y, float z) {
    ndPlayer->setPosition(Vector3(x, y, z));
}

void crPlayer::convertDirToFlag(bool avWalk, bool avWalkBack, bool avWalkLeft, bool avWalkRight) {
    if((avWalk) && (!avWalkBack)) {
        if((avWalkLeft) && (!avWalkRight))
            fWalking = kWalkFL;
        else if((avWalkRight) && (!avWalkLeft))
            fWalking = kWalkFR;
        else
            fWalking = kWalkFore;
    }
    else if((avWalkBack) && (!avWalk)) {
        if((avWalkLeft) && (!avWalkRight))
            fWalking = kWalkBL;
        else if((avWalkRight) && (!avWalkLeft))
            fWalking = kWalkBR;
        else
            fWalking = kWalkBack;
    }
    else {
        if((avWalkLeft) && (!avWalkRight))
            fWalking = kWalkLeft;
        else if((avWalkRight) && (!avWalkLeft))
            fWalking = kWalkRight;
        else
            fWalking = kNoWalk;
    }
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
