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

#include <MessageIdentifiers.h>
#include <RakNetTypes.h>
#include <OgreVector3.h>

//ctor---------------------
//out----------------------
ntMessage::ntMessage(uint32_t senderID, uint32_t msgFlag)
: fClientID(senderID) {
  fFlag = (MessageID)msgFlag;
  fStreamOut.Write(fFlag);
  fStreamOut.Write(fClientID);
}

//ctor--------------------
//in----------------------
ntMessage::ntMessage(Packet *packet)
: fPacket(packet) {
    fFlag = fPacket->data[0];
    fStreamIn = BitStream(fPacket->data, fPacket->length, false);
    fStreamIn.IgnoreBytes(sizeof(MessageID));

    fStreamIn.Read(fClientID);
}

//dtor-------------------
ntMessage::~ntMessage() {
    fStreamIn.Reset();
    fStreamOut.Reset();
}

//read
Vector3 ntMessage::readVector() {
    float x, y, z;
    fStreamIn.Read(x);
    fStreamIn.Read(y);
    fStreamIn.Read(z);

    return Vector3(x, y, z);
}

RakString ntMessage::readString() {
    RakString r;
    fStreamIn.Read(r);

    return r;
}

ntPlayer *ntMessage::readPlayer() {
    ntPlayer *p;
    fStreamIn.Read(*p);
    return p;
}

//write
void ntMessage::writeVector(Vector3 vector) {
    fStreamOut.Write((float)vector.x);
    fStreamOut.Write((float)vector.y);
    fStreamOut.Write((float)vector.z);
}

void ntMessage::writeVector(float x, float y, float z) {
    fStreamOut.Write(x);
    fStreamOut.Write(y);
    fStreamOut.Write(z);
}

void ntMessage::writeString(RakString string) {
    fStreamOut.Write(string);
}

void ntMessage::writePlayer(ntPlayer *player) {
    fStreamOut.Write(*player);
}
