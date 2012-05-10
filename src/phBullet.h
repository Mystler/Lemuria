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
#include "3rdparty/btogre/BtOgreGP.h"
#include "3rdparty/btogre/BtOgrePG.h"

using namespace Ogre;

class phBullet {
public:
    std::deque<btRigidBody *> btBodies;
    std::deque<btCollisionShape *> btShapes;
    static phBullet &getInstance();
    btDynamicsWorld *getWorld();
    BtOgre::DebugDrawer *getDbgDrawer();
    void initDebugDrawer(SceneManager *sceneMgr);
    void destroyPhysicals(void);
    btRigidBody *createPhysicalAvatar(SceneNode *avatarNode);
    bool avatarOnGround(btRigidBody *avatar);
private:
    phBullet(void);
    virtual ~phBullet(void);
    btAxisSweep3 *btBroadphase;
    btDefaultCollisionConfiguration *btCollisionConfig;
    btCollisionDispatcher *btDispatcher;
    btSequentialImpulseConstraintSolver *btSolver;
    btDynamicsWorld *btWorld;
    BtOgre::DebugDrawer *btDbgDrawer;
};
#endif
