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

#ifndef _phBullet_h
#define _phBullet_h

#include "3rdparty/btogre/BtOgreExtras.h"

using namespace Ogre;

class phBullet {
public:
    static phBullet &getInstance();
    btDynamicsWorld *getWorld() { return btWorld; }
    BtOgre::DebugDrawer *getDbgDrawer() { return btDbgDrawer; }
    void initDebugDrawer(SceneManager *sceneMgr);
    void destroyPhysicals();
    btRigidBody *createPhysicalAvatar(SceneNode *avatarNode);
    void addBody(btRigidBody *body) { btBodies.push_back(body); }
    void addShape(btCollisionShape *shape) { btShapes.push_back(shape); }
private:
    phBullet();
    ~phBullet();
    btAxisSweep3 *btBroadphase;
    btDefaultCollisionConfiguration *btCollisionConfig;
    btCollisionDispatcher *btDispatcher;
    btSequentialImpulseConstraintSolver *btSolver;
    btDynamicsWorld *btWorld;
    BtOgre::DebugDrawer *btDbgDrawer;
    std::deque<btRigidBody*> btBodies;
    std::deque<btCollisionShape*> btShapes;
};
#endif
