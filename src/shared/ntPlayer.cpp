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

#include "ntPlayer.h"

//ctor-----------------------------------
//default--------------------------------
ntPlayer::ntPlayer()
: fClientID(0),
  fWalking(0),
  fTurning(0),
  fPosition(Vector3(0,0,0)),
  fYaw(0) {
}

//ctor---------------------------------
//direction only-----------------------
ntPlayer::ntPlayer(uint32_t clientID, Vector3 vector, float yaw)
: fClientID(clientID),
  fWalking(0),
  fTurning(0),
  fPosition(vector),
  fYaw(yaw) {
}

//ctor---------------------------------
//complete-----------------------------
ntPlayer::ntPlayer(uint32_t clientID, uint32_t walking, uint32_t turning, Vector3 vector, float yaw)
: fClientID(clientID),
  fWalking(walking),
  fTurning(turning),
  fPosition(vector),
  fYaw(yaw) {
}

uint32_t ntPlayer::compare(ntPlayer *player) {
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

void ntPlayer::getPosition(float &x, float &y, float &z) {
    x = fPosition.x;
    y = fPosition.y;
    z = fPosition.z;
}
