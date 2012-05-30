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
ntMessage::ntMessage()
: fClientID(0),
  ntFlag(0) {
}

ntMessage::ntMessage(uint32_t clientID, uint32_t flag)
: fClientID(clientID) {
  ntFlag = (MessageID)flag;
  streamOut.Write(ntFlag);
  streamOut.Write(fClientID);
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
    Vector3 r = Vector3(x,y,z);

    return r;
}

RakString ntMessage::readString() {
    RakString r;
    streamIn.Read(r);

    return r;
}

ntPlayer *ntMessage::readPlayer() {
    ntPlayer *r;
    streamIn.Read(r);

    return r;
}

//write
void ntMessage::writeVector(Vector3 vector) {
    streamOut.Write(vector.x);
    streamOut.Write(vector.y);
    streamOut.Write(vector.z);
}

void ntMessage::writeVector(float x, float y, float z) {
    streamOut.Write(x);
    streamOut.Write(y);
    streamOut.Write(z);
}

void ntMessage::writeString(RakString string) {
    streamOut.Write(string);
}

void ntMessage::writePlayer(ntPlayer *player) {
    streamOut.Write(player);
}
