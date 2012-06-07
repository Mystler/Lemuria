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

#include "scObjectMgr.h"
#include "crClient.h"
#include "phBullet.h"

#include "3rdparty/btogre/BtOgreGP.h"
#include <OgreEntity.h>

/*
This class can process the current scene of the core client.
The nodes are filtered by their names and then handled like this:
--- "pho_": Create physical collision
--- "spn_<Type>_": Spawn object of type
------ Types are: <none yet>
*/

void scObjectMgr::processScene(SceneManager *sceneMgr) {
    //iterate through the Nodes
    SceneNode *scRootNode = sceneMgr->getRootSceneNode();
    Node::ChildNodeIterator itSceneObject = scRootNode->getChildIterator();
    while(itSceneObject.hasMoreElements()) {
        //get Node
        Node *node = itSceneObject.getNext();

        //get the Name
        String scObjectName = node->getName();
        if(StringUtil::startsWith(scObjectName, "pho_")) {
            //create Physics
            Entity *ent = sceneMgr->getEntity(scObjectName);
            Vector3 pos = node->getPosition();
            Quaternion rot = node->getOrientation();

            BtOgre::StaticMeshToShapeConverter conv(ent);
            btCollisionShape *shape = conv.createTrimesh();
            btDefaultMotionState *state = new btDefaultMotionState(btTransform(BtOgre::Convert::toBullet(rot),
                    BtOgre::Convert::toBullet(pos)));
            btRigidBody *body = new btRigidBody(0, state, shape, btVector3(0, 0, 0));
            phBullet::getInstance().getWorld()->addRigidBody(body);
            phBullet::getInstance().addShape(shape);
            phBullet::getInstance().addBody(body);
        }
    }
}
