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


#ifndef _ntMessage_h
#define _ntMessage_h

#include <OgreVector3.h>

#include "MessageIdentifiers.h"
#include "RakPeerInterface.h"
#include "RakNetTypes.h"
#include "BitStream.h"

#include "crPlayer.h"

using namespace Ogre;
using namespace RakNet;

class ntMessage {
public:
    BitStream streamOut;

    //ctor
    ntMessage(int clientID);
    ntMessage(Packet *packet);
    //dtor
    ntMessage::~ntMessage();

    int getClientID() { return fClientID; }
    int getFlag() { return ntFlag; }
    void setFlag(int flag);

    //read
    Vector3 readVector();
    crPlayer *readPlayer();

    //write
    void writePlayerName(RakString name);
    void writePlayer(crPlayer *player);
private:
    BitStream streamIn;
    int fClientID;
    MessageID ntFlag;
    Packet *ntPacket;
};
#endif
