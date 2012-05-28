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
#include "crPlayer.h"

#define MAX_CLIENTS 4
#define SERVER_PORT 27015

svServer::svServer() {
    peer = RakPeerInterface::GetInstance();
    sockDesc = new SocketDescriptor(SERVER_PORT, 0);
    peer->Startup(MAX_CLIENTS, sockDesc, 1);
    printf("Starting the AkshahNet Lemuria server.\nWaiting for connections...\n");
    peer->SetMaximumIncomingConnections(MAX_CLIENTS);
}

svServer::~svServer() {
    RakPeerInterface::DestroyInstance(peer);
}

void svServer::receive() {
}

svServer &svServer::getInstance() {
    static svServer svInstance;
    return svInstance;
}

int main() {
    svServer &app = svServer::getInstance();
    app.receive();
}
