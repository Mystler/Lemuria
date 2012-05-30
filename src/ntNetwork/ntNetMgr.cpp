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

#include "ntNetMgr.h"

#include "MessageIdentifiers.h"
#include "RakPeerInterface.h"
#include "BitStream.h"

#include "ntMessage.h"
#include "ntPlayer.h"

using namespace Ogre;

enum GameMessages {
    NEW_CLIENT = ID_USER_PACKET_ENUM + 1,
    SPAWN_POSITION = ID_USER_PACKET_ENUM + 2,
    PLAYER_UPDATE = ID_USER_PACKET_ENUM + 3,
    PLAYERNAME = ID_USER_PACKET_ENUM + 4,
    DISCONNECT_PLAYER = ID_USER_PACKET_ENUM + 5
};

ntNetMgr::ntNetMgr(const char *host, unsigned short port)
: ntHost(host),
  ntPort(port) {
}

ntNetMgr::~ntNetMgr() {
    RakPeerInterface::DestroyInstance(ntPeer);
}

RakPeerInterface *ntNetMgr::connect() {
    ntPeer = RakPeerInterface::GetInstance();
    SocketDescriptor *ntSockDesc = new SocketDescriptor();
    ntPeer->Startup(1, ntSockDesc, 1);
    ntPeer->Connect(ntHost, ntPort,0,0);
    return ntPeer;
}

ntMessage *ntNetMgr::getMessage(Packet *packet) {
    ntMessage *r = new ntMessage(packet);
    ntClientID = r->getClientID();
    ntServerAddress = packet->systemAddress;

    return r;
}

void ntNetMgr::sendPlNameMsg(RakString name) {
	ntMessage *out = new ntMessage(ntClientID, PLAYERNAME);
    out->writeString(name);
    ntPeer->Send(&out->streamOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, ntServerAddress, false);
    delete out;
}

void ntNetMgr::sendPlayerMsg(ntPlayer *player) {
    ntMessage *out = new ntMessage(ntClientID, PLAYER_UPDATE);
    out->writePlayer(player);
    ntPeer->Send(&out->streamOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, ntServerAddress, false);
    delete out;
}
