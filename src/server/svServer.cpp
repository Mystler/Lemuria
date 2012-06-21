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

#include <vector>
#include <MessageIdentifiers.h>
#include <RakPeerInterface.h>
#include <RakNetTypes.h>
#include <BitStream.h>

#include "../shared/ntMessage.h"
#include "../shared/ntPlayer.h"

#define MAX_CLIENTS 4
#define SERVER_PORT 27015

enum GameMessages {
    NEW_CLIENT = ID_USER_PACKET_ENUM + 1,
    SPAWN_POSITION = ID_USER_PACKET_ENUM + 2,
    PLAYER_UPDATE = ID_USER_PACKET_ENUM + 3,
    PLAYERNAME = ID_USER_PACKET_ENUM + 4,
    DISCONNECT_PLAYER = ID_USER_PACKET_ENUM + 5,
    PLAYER_JUMP = ID_USER_PACKET_ENUM + 6
};

//helper struct
struct Client {
    uint32_t id;
    RakNetGUID guid;
    RakString name;
    bool offline;
    ntPlayer *player;

    Client(uint32_t passed_id, RakNetGUID passed_guid, bool passed_offline, ntPlayer *passed_player)
    : id(passed_id),
      guid(passed_guid),
      offline(passed_offline),
      player(passed_player) { }
};

svServer::~svServer() {
    RakPeerInterface::DestroyInstance(peer);
}

svServer &svServer::getInstance() {
    static svServer svInstance;
    return svInstance;
}

void svServer::init() {
    peer = RakPeerInterface::GetInstance();
    SocketDescriptor *sockDesc = new SocketDescriptor(SERVER_PORT, 0);
    peer->Startup(MAX_CLIENTS, sockDesc, 1);
    printf("Starting the AkshahNet Lemuria server.\nWaiting for connections...\n");
    peer->SetMaximumIncomingConnections(MAX_CLIENTS);
}

void svServer::receive() {
    Packet *packet;
    std::vector<Client> clients;
    while(true) {
        for (packet = peer->Receive(); packet; peer->DeallocatePacket(packet), packet = peer->Receive()) {
            ntMessage *inMsg = new ntMessage(packet);
            ntMessage *out;
            uint32_t client_id = 0;
            RakString client_name;

            switch(inMsg->getFlag()) {
                case ID_REMOTE_DISCONNECTION_NOTIFICATION:
                    printf("A client has disconnected.\n");
                    break;
                case ID_REMOTE_CONNECTION_LOST:
                    printf("A client has lost the connection.\n");
                    break;
                case ID_REMOTE_NEW_INCOMING_CONNECTION:
                    printf("A client has connected.\n");
                    break;
                case ID_NEW_INCOMING_CONNECTION: {
                    printf("New connection incoming.\n");
                    client_id = (int)clients.size();
                    ntPlayer *newPlayer = new ntPlayer(client_id, SPAWNPOS, 1.57f);
                    if(clients.size() > 0) {
                        printf("Sending new spawn position to other clients\n");
                        out = new ntMessage(client_id, NEW_CLIENT);
                        out->writePlayer(newPlayer);
                        for(size_t i = 0; i < clients.size(); i++) {
                            if(!clients[i].offline)
                                peer->Send(out->getStream(), HIGH_PRIORITY, RELIABLE_ORDERED, 0,
                                    peer->GetSystemAddressFromGuid(clients[i].guid), false);
                        }

                        printf("Sending other clients position to new client\n");
                        for(size_t i = 0; i < clients.size(); i++) {
                            if(!clients[i].offline) {
                                out = new ntMessage(i, NEW_CLIENT);
                                out->writePlayer(clients[i].player);
                                peer->Send(out->getStream(), HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
                            }
                        }
                    } else {
                        printf("No other clients online, no need to send data\n");
                    }

                    clients.push_back(Client(client_id, packet->guid, false, newPlayer));

                    out = new ntMessage(client_id, SPAWN_POSITION);
                    out->writePlayer(newPlayer);
                    peer->Send(out->getStream(), HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
                    break;
                }
                case ID_CONNECTION_LOST:
                    printf("A client lost the connection.\n");
                case ID_DISCONNECTION_NOTIFICATION: {
                    for(size_t i = 0; i < clients.size(); i++) {
                        if(clients[i].guid == packet->guid) {
                            client_id = clients[i].id;
                            client_name = clients[i].name;
                            clients[i].offline = true;
                            printf("Client %i (%s) disconnected.\n", client_id, client_name.C_String());
                            out = new ntMessage(client_id, DISCONNECT_PLAYER);
                            for(size_t j = 0; j < clients.size(); ++j) {
                                if(!clients[j].offline)
                                    peer->Send(out->getStream(), HIGH_PRIORITY, RELIABLE_ORDERED, 0,
                                        peer->GetSystemAddressFromGuid(clients[j].guid), false);
                            }
                        }
                    }
                    break;
                }
                case PLAYER_UPDATE: {
                    ntPlayer *player = inMsg->readPlayer();
                    client_id = inMsg->getClientID();
                    //printf("Client %i sent new player\n", client_id);
                    clients[client_id].player = player;
                    out = new ntMessage(client_id, PLAYER_UPDATE);
                    out->writePlayer(player);
                    for(size_t i = 0; i < clients.size(); i++) {
                        if(client_id != i && !clients[i].offline)
                            //printf("sending new player to %i\n", i);
                            peer->Send(out->getStream(), HIGH_PRIORITY, RELIABLE_ORDERED, 0,
                                peer->GetSystemAddressFromGuid(clients[i].guid), false);
                    }
                    break;
                }
                case PLAYER_JUMP:
                    //printf("A Client send a Jump Message\n");
                    out = new ntMessage(inMsg->getClientID(), PLAYER_JUMP);
                    for(size_t i = 0; i < clients.size(); i++) {
                        if(client_id != i && !clients[i].offline)
                            peer->Send(out->getStream(), MEDIUM_PRIORITY, RELIABLE_ORDERED, 0,
                                peer->GetSystemAddressFromGuid(clients[i].guid), false);
                    }
                    break;
                case PLAYERNAME:
                    client_id = inMsg->getClientID();
                    client_name = inMsg->readString();
                    clients[client_id].name = client_name;
                    printf("Received playername %s from client %i\n", client_name.C_String(), client_id);
                    break;
                default:
                    printf("Message with unknown identifier has arrived: %i\n", inMsg->getFlag());
                    break;
            }
            delete out;
        }
    }
}

int main() {
    svServer &app = svServer::getInstance();
    app.init();
    app.receive();
}
