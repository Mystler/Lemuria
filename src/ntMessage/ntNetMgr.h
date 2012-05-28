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

#include "MessageIdentifiers.h"
#include "RakPeerInterface.h"
#include "RakNetTypes.h"
#include "BitStream.h"

using namespace RakNet;

class ntNetMgr {
public:
    ntNetMgr(const char *host, unsigned short port);
    ~ntNetMgr();
    void connect();
    void analyseIncoming(); //TODO
private:
    RakPeerInterface *ntPeer;
    Packet *ntPacket;
    SystemAddress ntServerAddress;
    const char *ntHost;
    unsigned short ntPort;
};
#endif
