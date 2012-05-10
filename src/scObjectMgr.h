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

#ifndef _scObjectMgr_h
#define _scObjectMgr_h

#include <OgreSceneManager.h>

using namespace Ogre;

//For more information, see the cpp file.

class scObjectMgr {
public:
    scObjectMgr(void);
    virtual ~scObjectMgr(void);
    void processScene(SceneManager *sceneMgr);
};
#endif
