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
ntMessage::ntMessage(uint32_t clientID)
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


void ntMessage::setFlag(uint32_t flag) {
    ntFlag = (MessageID)flag;
    streamOut.Write(ntFlag);
    streamOut.Write(fClientID);
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
    uint32_t walking, turning;
    float x, y, z;
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

//write
void ntMessage::writePlayer(crPlayer *player) {
    streamOut.Write(player->getWalking());
    streamOut.Write(player->getTurning());
    streamOut.Write(player->getPosition().x);
    streamOut.Write(player->getPosition().y);
    streamOut.Write(player->getPosition().z);
    streamOut.Write(player->getYaw());
}

void ntMessage::writePlayerName(RakString name) {
    streamOut.Write(name);
}
