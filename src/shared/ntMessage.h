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


#ifndef _ntMessage_h
#define _ntMessage_h

#include <BitStream.h>

#include "ntPlayer.h"

namespace RakNet {
    class RakString;
    struct Packet;
};

using namespace Ogre;
using namespace RakNet;

class ntMessage {
public:
    //ctor
    ntMessage(uint32_t senderID, uint32_t msgFlag); //outgoing
    ntMessage(Packet *packet); //incoming
    //dtor
    ~ntMessage();

    BitStream *getStream() { return &fStreamOut; }
    uint32_t getClientID() { return fClientID; }
    uint32_t getFlag() { return fFlag; }
    void setFlag(uint32_t flag);

    //read
    Vector3 readVector();
    RakString readString();
    ntPlayer *readPlayer();

    //write
    void writeVector(Vector3 vector);
    void writeVector(float x, float y, float z);
    void writeString(RakString string);
    void writePlayer(ntPlayer *player);
private:
    BitStream fStreamIn, fStreamOut;
    uint32_t fClientID;
    MessageID fFlag;
    Packet *fPacket;
};
#endif
