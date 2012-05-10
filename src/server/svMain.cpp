/*==LICENSE==

This file is part of Lemuria.
Copyright (C) 2012 Florian Meißner

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

#include "MessageIdentifiers.h"
#include "RakPeerInterface.h"
#include "RakNetTypes.h"
#include "BitStream.h"

#include <string>
#include <iostream>
#include <vector>

#define MAX_CLIENTS 4
#define SERVER_PORT 27015

using namespace RakNet;

enum GameMessages {
    NEW_CLIENT = ID_USER_PACKET_ENUM + 1,
    SPAWN_POSITION = ID_USER_PACKET_ENUM + 2,
    POSITION_UPDATE = ID_USER_PACKET_ENUM + 3,
    DIRECTION_UPDATE = ID_USER_PACKET_ENUM + 4,
    PLAYERNAME = ID_USER_PACKET_ENUM + 5,
    DISCONNECT_PLAYER = ID_USER_PACKET_ENUM + 6
};

class Client {
public:
    int id;
    float x, y, z, yaw;
    RakNetGUID guid;
    RakString name;
    bool offline;

    Client(int passed_id) : id(passed_id) { }
};

int main(void) {
    RakPeerInterface *peer = RakPeerInterface::GetInstance();
    Packet *packet;

    SocketDescriptor *sockDesc = new SocketDescriptor(SERVER_PORT, 0);
    peer->Startup(MAX_CLIENTS, sockDesc, 1);

    printf("Starting the AkshahNet Lemuria server.\nWaiting for connections...\n");
    peer->SetMaximumIncomingConnections(MAX_CLIENTS);

    RakNet::BitStream bsOut;

    std::vector<Client> clients;

    while(1) {
        for(packet = peer->Receive(); packet; peer->DeallocatePacket(packet), packet = peer->Receive()) {
            RakNet::BitStream bsIn(packet->data, packet->length, false);
            bsIn.IgnoreBytes(sizeof(MessageID));

            /*std::cout << "Client List:\n";
            for(int i=0; i < (int)clients.size(); ++i)
            {
                std::cout << i << " - " << clients[i].guid.g << std::endl;
            }
            std::cout << "\n\nNew Packet from:"
                << packet->guid.g << std::endl;*/

            int client_id = 0;
            float x = 0, y = 0, z = 0, yaw = 0;
            RakString client_name;
            client_name.Clear();

            switch(packet->data[0]) {
                case ID_REMOTE_DISCONNECTION_NOTIFICATION:
                    printf("A client has disconnected.\n");
                    break;
                case ID_REMOTE_CONNECTION_LOST:
                    printf("A client has lost the connection.\n");
                    break;
                case ID_REMOTE_NEW_INCOMING_CONNECTION:
                    printf("A client has connected.\n");
                    break;
                case ID_NEW_INCOMING_CONNECTION:
                    printf("New conection incoming.\n");
                    client_id = (int)clients.size();
                    if((int)clients.size() > 0) {
                        std::cout << "Sending new spawn position to other clients\n";
                        bsOut.Reset();
                        bsOut.Write((MessageID)NEW_CLIENT);
                        bsOut.Write(client_id);
                        bsOut.Write(0.0f);
                        bsOut.Write(1.8f);
                        bsOut.Write(0.0f);
                        for(int i = 0; i < (int)clients.size(); ++i) {
                            if(!clients[i].offline) {
                                //std::cout << "  To: " << i << " - " << clients[i].guid.g << std::endl;
                                peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0,
                                           peer->GetSystemAddressFromGuid(clients[i].guid), false);
                            }
                        }
                        bsOut.Reset();

                        std::cout << "Sending other client positions to new client\n";
                        for(int i = 0; i < (int)clients.size(); ++i) {
                            if(!clients[i].offline) {
                                std::cout << "sending for " << i << std::endl;
                                bsOut.Reset();
                                bsOut.Write((MessageID)NEW_CLIENT);
                                bsOut.Write(i);
                                bsOut.Write(clients[i].x);
                                bsOut.Write(clients[i].y);
                                bsOut.Write(clients[i].z);
                                peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0,
                                           packet->systemAddress, false);
                            }
                        }
                    } else {
                        std::cout << "No other clients online, no need to send data\n";
                    }

                    clients.push_back(Client(client_id));
                    clients[client_id].guid = packet->guid;
                    clients[client_id].x = 0.0f;
                    clients[client_id].y = 1.8f;
                    clients[client_id].z = 0.0f;
                    clients[client_id].offline = false;

                    bsOut.Reset();
                    bsOut.Write((MessageID)SPAWN_POSITION);
                    bsOut.Write(client_id);
                    bsOut.Write(0.0f);
                    bsOut.Write(1.8f);
                    bsOut.Write(0.0f);
                    peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
                    bsOut.Reset();
                    break;
                case ID_CONNECTION_LOST:
                    printf("A client lost the connection.\n");
                case ID_DISCONNECTION_NOTIFICATION:
                    for(int i = 0; i < (int)clients.size(); ++i) {
                        if(clients[i].guid == packet->guid) {
                            client_id = clients[i].id;
                            client_name = clients[i].name;
                            clients[i].offline = true;
                            printf("Client %i (%s) disconnected.\n", client_id, client_name.C_String());
                            bsOut.Reset();
                            bsOut.Write((MessageID)DISCONNECT_PLAYER);
                            bsOut.Write(client_id);
                            for(int j = 0; j < (int)clients.size(); ++j) {
                                if(!clients[j].offline) {
                                    peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0,
                                               peer->GetSystemAddressFromGuid(clients[j].guid), false);
                                }
                            }
                            bsOut.Reset();
                        }
                    }
                    break;
                case POSITION_UPDATE:
                    bsIn.Read(client_id);
                    bsIn.Read(x);
                    bsIn.Read(y);
                    bsIn.Read(z);
                    //printf("Client %i sent new position %f,%f,%f\n", client_id, x, y, z);

                    clients[client_id].x = x;
                    clients[client_id].y = y;
                    clients[client_id].z = z;

                    //std::cout << "Sending new position to each client\n";
                    bsOut.Reset();
                    bsOut.Write((MessageID)POSITION_UPDATE);
                    bsOut.Write(client_id);
                    bsOut.Write(x);
                    bsOut.Write(y);
                    bsOut.Write(z);
                    for(int i = 0; i < (int)clients.size(); ++i) {
                        if(client_id != i && !clients[i].offline) {
                            //std::cout << "  To: " << i << " - " << clients[i].guid.g << std::endl;
                            peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0,
                                       peer->GetSystemAddressFromGuid(clients[i].guid), false);
                        }
                    }
                    bsOut.Reset();
                    break;
                case DIRECTION_UPDATE:
                    bsIn.Read(client_id);
                    bsIn.Read(yaw);
                    //printf("Client %i sent new direction %f\n", client_id, yaw);

                    clients[client_id].yaw = yaw;

                    //std::cout << "Sending new direction to each client\n";
                    bsOut.Reset();
                    bsOut.Write((MessageID)DIRECTION_UPDATE);
                    bsOut.Write(client_id);
                    bsOut.Write(yaw);
                    for(int i = 0; i < (int)clients.size(); ++i) {
                        if(client_id != i && !clients[i].offline) {
                            //std::cout << "  To: " << i << " - " << clients[i].guid.g << std::endl;
                            peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0,
                                       peer->GetSystemAddressFromGuid(clients[i].guid), false);
                        }
                    }
                    bsOut.Reset();
                    break;
                case PLAYERNAME:
                    bsIn.Read(client_id);
                    bsIn.Read(client_name);
                    clients[client_id].name = client_name;
                    printf("Recieved playername %s from client %i\n", client_name.C_String(), client_id);
                    break;
                default:
                    printf("Message with identifier %i has arrived.\n", packet->data[0]);
                    break;
            }
            bsOut.Reset();
        }
    }

    RakPeerInterface::DestroyInstance(peer);

    return 0;
}
