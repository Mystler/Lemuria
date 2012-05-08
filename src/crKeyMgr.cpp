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

#include "crClient.h"

bool crClient::keyPressed(const OIS::KeyEvent& arg)
{
    CEGUI::System::getSingleton().injectKeyDown(arg.key);
    CEGUI::System::getSingleton().injectChar(arg.text);
    
    switch (arg.key)
    {
        case OIS::KC_P:
            if(!btWireframe)
            {
                phBullet::getInstance().getDbgDrawer()->setDebugMode(1);
                btWireframe = true;
            }
            else
            {
                phBullet::getInstance().getDbgDrawer()->setDebugMode(0);
                btWireframe = false;
            }
            break;
        case OIS::KC_LSHIFT:
        case OIS::KC_RSHIFT:
            kShiftDown = true;
            break;
        case OIS::KC_W:
        case OIS::KC_UP:
            avWalk = true;
            break;
        case OIS::KC_S:
        case OIS::KC_DOWN:
            avWalkBack = true;
            break;
        case OIS::KC_A:
        case OIS::KC_LEFT:
            avWalkLeft = true;
            break;
        case OIS::KC_D:
        case OIS::KC_RIGHT:
            avWalkRight = true;
            break;
        case OIS::KC_SPACE:
            if(!avJump && !avFly) avJump = true;
            break;
        case OIS::KC_V:
            if(avFly) {avFly = false;}
            else {avFly = true;}
        default:
            break;
    }
    
    return true;
}
bool crClient::keyReleased(const OIS::KeyEvent& arg)
{
    CEGUI::System::getSingleton().injectKeyUp(arg.key);
    
    switch (arg.key)
    {
        case OIS::KC_LSHIFT:
        case OIS::KC_RSHIFT:
            kShiftDown = false;
            break;
        case OIS::KC_W:
        case OIS::KC_UP:
            avWalk = false;
            break;
        case OIS::KC_S:
        case OIS::KC_DOWN:
            avWalkBack = false;
            break;
        case OIS::KC_A:
        case OIS::KC_LEFT:
            avWalkLeft = false;
            break;
        case OIS::KC_D:
        case OIS::KC_RIGHT:
            avWalkRight = false;
            break;
        default:
            break;
    }
    
    return true;
}
