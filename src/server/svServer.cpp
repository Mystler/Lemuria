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

#include "svServer.h"

#include "MessageIdentifiers.h"
#include "RakPeerInterface.h"
#include "RakNetTypes.h"
#include "BitStream.h"

#define MAX_CLIENTS 4
#define SERVER_PORT 27015

enum GameMessages {
    NEW_CLIENT = ID_USER_PACKET_ENUM + 1,
    SPAWN_POSITION = ID_USER_PACKET_ENUM + 2,
    PLAYER_UPDATE = ID_USER_PACKET_ENUM + 3,
    PLAYERNAME = ID_USER_PACKET_ENUM + 4,
    DISCONNECT_PLAYER = ID_USER_PACKET_ENUM + 5
};

svServer::svServer() {
    peer = RakPeerInterface::GetInstance();
    SocketDescriptor *sockDesc = new SocketDescriptor(SERVER_PORT, 0);
    peer->Startup(MAX_CLIENTS, sockDesc, 1);
    printf("Starting the AkshahNet Lemuria server.\nWaiting for connections...\n");
    peer->SetMaximumIncomingConnections(MAX_CLIENTS);
}

svServer::~svServer() {
    RakPeerInterface::DestroyInstance(peer);
}

svServer &svServer::getInstance() {
    static svServer svInstance;
    return svInstance;
}

void svServer::receive() {
}

int main() {
    svServer &app = svServer::getInstance();
    app.receive();
}
