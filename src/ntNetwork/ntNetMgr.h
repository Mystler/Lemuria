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

#ifndef _ntNetMgr_h
#define _ntNetMgr_h

#include <stdint.h>

#include "RakNetTypes.h"

namespace RakNet {
    class RakPeerInterface;
    class RakString;
};

class ntMessage;
class ntPlayer;

using namespace RakNet;

class ntNetMgr {
public:
    ntNetMgr(const char *host, unsigned short port);
    ~ntNetMgr();
    RakPeerInterface *connect();
    ntMessage *getMessage(Packet *packet);
    void sendPlNameMsg(RakString name);
    void sendPlayerMsg(ntPlayer *player);
private:
    RakPeerInterface *ntPeer;
    SystemAddress ntServerAddress;
    const char *ntHost;
    unsigned short ntPort;
    uint32_t ntClientID;
};
#endif
