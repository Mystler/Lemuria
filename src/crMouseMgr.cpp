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

//function to convert OIS to CEGUI Mouse Button
CEGUI::MouseButton convertButton(OIS::MouseButtonID buttonID) {
    switch(buttonID) {
        case OIS::MB_Left:
            return CEGUI::LeftButton;
        case OIS::MB_Right:
            return CEGUI::RightButton;
        case OIS::MB_Middle:
            return CEGUI::MiddleButton;
        default:
            return CEGUI::LeftButton;
    }
}
bool crClient::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id) {
    CEGUI::System::getSingleton().injectMouseButtonDown(convertButton(id));

    switch(id) {
        case OIS::MB_Left:
            break;
        case OIS::MB_Right:
            if(!fMMActive) {
                if(avMouseLook) {
                    CEGUI::MouseCursor::getSingleton().show();
                    CEGUI::MouseCursor::getSingleton().setPosition(CEGUI::Point(arg.state.width / 2, arg.state.height / 2));
                    avMouseLook = false;
                } else {
                    CEGUI::MouseCursor::getSingleton().hide();
                    avMouseLook = true;
                }
            }
            break;
        default:
            break;
    }

    return true;
}
bool crClient::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id) {
    CEGUI::System::getSingleton().injectMouseButtonUp(convertButton(id));

    switch(id) {
        case OIS::MB_Left:
            break;
        default:
            break;
    }

    return true;
}
bool crClient::mouseMoved(const OIS::MouseEvent &arg) {
    CEGUI::System::getSingleton().injectMouseMove(arg.state.X.rel, arg.state.Y.rel);

    //Camera Control
    if(avMouseLook) {
        mCamera->yaw(Degree(-arg.state.X.rel * avRotateSpeed));
        rotate = arg.state.X.rel * avRotateSpeed;

        Vector3 camDirection = mCamera->getDirection();
        if(camDirection.y > 0.9f) {
            mCamera->setDirection(Vector3(camDirection.x, 0.89f, camDirection.z));
        } else
            if(camDirection.y < -0.9f) {
                mCamera->setDirection(Vector3(camDirection.x, -0.89f, camDirection.z));
            } else {
                mCamera->pitch(Degree(-arg.state.Y.rel * avRotateSpeed));
            }
    }

    return true;
}
