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

#include "ntMessage.h"

//ctor---------------------
//out----------------------
ntMessage::ntMessage(int clientID)
: fClientID(clientID) {
}

//ctor--------------------
//in----------------------
ntMessage::ntMessage(Packet *packet)
: ntPacket(packet) {
    ntFlag = ntPacket->data[0];
    streamIn = BitStream(ntPacket->data, ntPacket->length, false);
    streamIn.IgnoreBytes(sizeof(MessageID));

    streamIn.Read(fClientID);
}

//dtor-------------------
ntMessage::~ntMessage() {
    streamIn.Reset();
    streamOut.Reset();
}

//read
Vector3 ntMessage::readVector() {
    float x, y, z;

    streamIn.Read(x);
    streamIn.Read(y);
    streamIn.Read(z);

    return Vector3(x, y, z);
}

crPlayer *ntMessage::readPlayer() {
    int walking;
    float turning, x, y, z;
    float yaw;
    crPlayer *player;

    streamIn.Read(walking);
    streamIn.Read(turning);
    streamIn.Read(x);
    streamIn.Read(y);
    streamIn.Read(z);
    streamIn.Read(yaw);

    player = new crPlayer(0, fClientID, walking, turning, Vector3(x, y, z), yaw);
    return player;
}

void ntMessage::setFlag(int flag) {
    ntFlag = (MessageID)flag;
    streamOut.Write(ntFlag);
    streamOut.Write(fClientID);
}

//write
void ntMessage::writePlayer(crPlayer *player) {
    streamOut.Write(player->fIsWalking);
    streamOut.Write(player->fIsTurning);
    streamOut.Write(player->fPosition.x);
    streamOut.Write(player->fPosition.y);
    streamOut.Write(player->fPosition.z);
    streamOut.Write(player->fYaw);
}

void ntMessage::writePlayerName(RakString name) {
    streamOut.Write(name);
}
