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

#include "ntManager.h"

#include "ntMessage.h"
#include "ntPlayer.h"

ntManager::~ntManager() {
    RakPeerInterface::DestroyInstance(fPeer);
}

RakPeerInterface *ntManager::connect(const char *host, unsigned short port) {
    fPeer = RakPeerInterface::GetInstance();
    SocketDescriptor *ntSockDesc = new SocketDescriptor();
    fPeer->Startup(1, ntSockDesc, 1);
    fPeer->Connect(host, port,0,0);
    return fPeer;
}

ntMessage *ntManager::getMessage(Packet *packet) {
    ntMessage *r = new ntMessage(packet);
    fClientID = r->getClientID();
    fServerAddress = packet->systemAddress;

    return r;
}

void ntManager::sendPlNameMsg(RakString name) {
    ntMessage *out = new ntMessage(fMyClientID, PLAYERNAME);
    out->writeString(name);
    fPeer->Send(out->getStream(), LOW_PRIORITY, RELIABLE_ORDERED, 0, fServerAddress, false);
    delete out;
}

void ntManager::sendPlayerMsg(ntPlayer *player) {
    ntMessage *out = new ntMessage(fMyClientID, PLAYER_UPDATE);
    out->writePlayer(player);
    fPeer->Send(out->getStream(), HIGH_PRIORITY, RELIABLE_ORDERED, 0, fServerAddress, false);
    delete out;
}

void ntManager::sendJumpMsg() {
    ntMessage *out = new ntMessage(fMyClientID, PLAYER_JUMP);
    fPeer->Send(out->getStream(), MEDIUM_PRIORITY, RELIABLE_ORDERED, 0, fServerAddress, false);
}
