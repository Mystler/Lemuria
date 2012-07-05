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

#ifndef _scWaveset_h
#define _scWaveset_h

#include "OgrePlatform.h"
#include "Ogre.h"

using namespace Ogre ;

class scWaveset {
public:
    scWaveset(String name, MeshPtr msh);
    void setup();
private:
    void calculateNormals();

    HardwareVertexBufferSharedPtr fVPosBuf;
    HardwareVertexBufferSharedPtr fVNormBuf;
    VertexBufferBinding *fBind;
    float *fVertices;
    float *fNormals;
    unsigned long *fFaces;
    int fVertexCount;
    int fIndexCount;
    String fMeshName;
    MeshPtr fMesh;
    MeshPtr fOldMesh;
} ;

#endif
