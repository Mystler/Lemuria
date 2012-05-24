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

#include "phBullet.h"

phBullet::phBullet() {
    //init Physics World
    btBroadphase = new btAxisSweep3(btVector3(-10000, -10000, -10000),
                                    btVector3(10000, 10000, 10000), 1024);
    btCollisionConfig = new btDefaultCollisionConfiguration();
    btDispatcher = new btCollisionDispatcher(btCollisionConfig);
    btSolver = new btSequentialImpulseConstraintSolver();
    btWorld = new btDiscreteDynamicsWorld(btDispatcher, btBroadphase, btSolver, btCollisionConfig);
    btWorld->setGravity(btVector3(0, -46.9f, 0));
}

phBullet::~phBullet() {
    //clean up
    destroyPhysicals();
    delete btWorld;
    delete btSolver;
    delete btDispatcher;
    delete btCollisionConfig;
    delete btBroadphase;
}

phBullet &phBullet::getInstance() {
    static phBullet btInstance;
    return btInstance;
}

void phBullet::initDebugDrawer(SceneManager *sceneMgr) {
    //init Debug Drawer
    btDbgDrawer = new BtOgre::DebugDrawer(sceneMgr->getRootSceneNode(), btWorld);
    btWorld->setDebugDrawer(btDbgDrawer);
}

void phBullet::destroyPhysicals() {
    std::deque<btRigidBody *>::iterator itBody = btBodies.begin();
    while(btBodies.end() != itBody) {
        delete *itBody;
        ++itBody;
    }
    std::deque<btCollisionShape *>::iterator itShape = btShapes.begin();
    while(btShapes.end() != itShape) {
        delete *itShape;
        ++itShape;
    }
    btBodies.clear();
    btShapes.clear();
}

btRigidBody *phBullet::createPhysicalAvatar(SceneNode *avatarNode) {
    //calculate Avatar Shape
    btScalar width = 0.4f;
    btScalar height = 1.8f;
    btVector3 spherePositions[2];
    btScalar sphereRadii[2];
    sphereRadii[0] = width;
    sphereRadii[1] = width;
    spherePositions[0] = btVector3(0, (height / 2 - width), 0);
    spherePositions[1] = btVector3(0, (-height / 2 + width), 0);
    btCollisionShape *avatarShape = new btMultiSphereShape(&spherePositions[0], &sphereRadii[0], 2);

    btScalar mass = 75;
    btVector3 inertia;
    avatarShape->calculateLocalInertia(mass, inertia);

    //create Avatar physics
    BtOgre::RigidBodyState *avatarState = new BtOgre::RigidBodyState(avatarNode);
    btRigidBody *avatarBody = new btRigidBody(mass, avatarState, avatarShape, inertia);
    avatarBody->setSleepingThresholds(0, 0);
    avatarBody->setAngularFactor(0);
    btWorld->addRigidBody(avatarBody);
    btShapes.push_back(avatarShape);
    btBodies.push_back(avatarBody);

    return avatarBody;
}
