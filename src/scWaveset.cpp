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

#include "scWaveset.h"

scWaveset::scWaveset(String name) {
    MeshPtr msh = MeshManager::getSingleton().createManual(name, "General");
    SubMesh *sub = msh->createSubMesh();

    vertices[0] = 0; vertices[1] = 1; vertices[2] = 0;
    vertices[3] = 1; vertices[4] = 1; vertices[5] = 0;
    vertices[6] = 0; vertices[7] = 1; vertices[8] = 1;

    unsigned short faces[3] = {
        0,1,2,
    };

    msh->sharedVertexData = new VertexData();
    msh->sharedVertexData->vertexCount = 3;

    VertexDeclaration *decl = msh->sharedVertexData->vertexDeclaration;
    size_t offset = 0;

    decl->addElement(0, offset, VET_FLOAT3, VES_POSITION);
    offset += VertexElement::getTypeSize(VET_FLOAT3);

    vbuf = HardwareBufferManager::getSingleton().createVertexBuffer(
        offset, msh->sharedVertexData->vertexCount, HardwareBuffer::HBU_STATIC_WRITE_ONLY);

    vbuf->writeData(0, vbuf->getSizeInBytes(), vertices, true);

    bind = msh->sharedVertexData->vertexBufferBinding;
    bind->setBinding(0, vbuf);

    HardwareIndexBufferSharedPtr ibuf = HardwareBufferManager::getSingleton().createIndexBuffer(
        HardwareIndexBuffer::IT_16BIT,3, HardwareBuffer::HBU_STATIC_WRITE_ONLY);

    ibuf->writeData(0,ibuf->getSizeInBytes(),faces, true);

    sub->useSharedVertices = true;
    sub->indexData->indexBuffer = ibuf;
    sub->indexData->indexCount = 3;
    sub->indexData->indexStart = 0;

    msh->_setBounds(AxisAlignedBox(0,1,0,1,1,1));

    msh->load();
}
