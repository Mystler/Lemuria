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

#ifndef _svServer_h
#define _svServer_h

#include "MessageIdentifiers.h"
#include "RakPeerInterface.h"
#include "RakNetTypes.h"
#include "BitStream.h"

#include <string>
#include <iostream>
#include <vector>

using namespace RakNet;

class svServer {
public:
    //ctor
    svServer();

    //dtor
    ~svServer();

    static svServer &getInstance();
    void receive();

    enum GameMessages {
    NEW_CLIENT = ID_USER_PACKET_ENUM + 1,
    SPAWN_POSITION = ID_USER_PACKET_ENUM + 2,
    PLAYER_UPDATE = ID_USER_PACKET_ENUM + 3,
    PLAYERNAME = ID_USER_PACKET_ENUM + 4,
    DISCONNECT_PLAYER = ID_USER_PACKET_ENUM + 5
    };
private:
    RakPeerInterface *peer;
    Packet *packet;
    SocketDescriptor *sockDesc;
};
#endif
