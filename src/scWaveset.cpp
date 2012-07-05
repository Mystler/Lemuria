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

scWaveset::scWaveset(String name, MeshPtr msh)
: fMeshName(name),
  fMesh(msh) { }

void scWaveset::setup() {
    //-----------
    //get Vertex Data from Mesh
    //-----------
    SubMesh *sub = fMesh->getSubMesh(0);
    //get needed numbers
    fVertexCount = fMesh->sharedVertexData->vertexCount;
    fIndexCount = sub->indexData->indexCount;
    //alocate space
    fVertices = new float[3*fVertexCount];
    fNormals = new float[3*fVertexCount];
    fFaces = new unsigned long[3*fIndexCount];
    //fill Vertex Array
    const VertexElement *posElement = fMesh->sharedVertexData->vertexDeclaration->findElementBySemantic(VES_POSITION);
    HardwareVertexBufferSharedPtr vBuf = fMesh->sharedVertexData->vertexBufferBinding->getBuffer(posElement->getSource());
    unsigned char *vertices = static_cast<unsigned char*>(vBuf->lock(HardwareBuffer::HBL_READ_ONLY));
    Real *pReal;
    for(size_t i = 0; i < fVertexCount; i++, vertices += vBuf->getVertexSize()) {
        posElement->baseVertexPointerToElement(vertices, &pReal);

        fVertices[3*i] = (*pReal++);
        fVertices[3*i+1] = (*pReal++);
        fVertices[3*i+2] = (*pReal++);
        LogManager::getSingletonPtr()->logMessage("DEBUG: scWaveset Vertex x=" + StringConverter::toString(fVertices[3*i]) +
            " y=" + StringConverter::toString(fVertices[3*i+1]) + 
            " z=" + StringConverter::toString(fVertices[3*i+2]));//remove when it's working
    }
    vBuf->unlock();
    //fill Normals Array
    calculateNormals();//needs to be written
    //fill Index Array
    unsigned short *pShort;
    unsigned int *pInt;
    HardwareIndexBufferSharedPtr iBuf = sub->indexData->indexBuffer;
    bool use32bitindexes = (iBuf->getType() == Ogre::HardwareIndexBuffer::IT_32BIT);
    if(use32bitindexes)
        pInt = static_cast<unsigned int*>(iBuf->lock(HardwareBuffer::HBL_READ_ONLY));
    else
        pShort = static_cast<unsigned short*>(iBuf->lock(HardwareBuffer::HBL_READ_ONLY));
    for(size_t i = 0; i < fIndexCount; i++) {
        unsigned int vIndex = use32bitindexes ? *pInt++ : *pShort++;

        fFaces[i] = vIndex;
        LogManager::getSingletonPtr()->logMessage("DEBUG: scWaveset Vertex index =" + StringConverter::toString(fFaces[i]));//remove when it's working
    }
    iBuf->unlock();

    //------------
    //Now we have enough informations about the mesh and can create our waveset
    //------------
    fOldMesh = fMesh;//save the mesh for now, maybe we need it again
    fMesh = MeshManager::getSingleton().createManual(fMeshName, "General");
    sub = fMesh->createSubMesh();

    fMesh->sharedVertexData = new VertexData();
    fMesh->sharedVertexData->vertexCount = fVertexCount;

    VertexDeclaration *decl = fMesh->sharedVertexData->vertexDeclaration;
    //position Buffer
    size_t offset = 0;
    decl->addElement(0, offset, VET_FLOAT3, VES_POSITION);
    offset += VertexElement::getTypeSize(VET_FLOAT3);

    fVPosBuf = HardwareBufferManager::getSingleton().createVertexBuffer(
        offset, fMesh->sharedVertexData->vertexCount, HardwareBuffer::HBU_STATIC_WRITE_ONLY);

    fVPosBuf->writeData(0, fVPosBuf->getSizeInBytes(), fVertices, true);

    fBind = fMesh->sharedVertexData->vertexBufferBinding;
    fBind->setBinding(0, fVPosBuf);

    //normals Buffer
    offset = 0;
    decl->addElement(1, offset, VET_FLOAT3, VES_NORMAL);
    offset += VertexElement::getTypeSize(VET_FLOAT3);

    fVNormBuf = HardwareBufferManager::getSingleton().createVertexBuffer(
        offset, fMesh->sharedVertexData->vertexCount, HardwareBuffer::HBU_STATIC_WRITE_ONLY);

    fVNormBuf->writeData(0, fVNormBuf->getSizeInBytes(), fNormals, true);

    fBind->setBinding(1, fVNormBuf);

    //Index Buffer
    HardwareIndexBufferSharedPtr ibuf = HardwareBufferManager::getSingleton().createIndexBuffer(
        HardwareIndexBuffer::IT_16BIT,3, HardwareBuffer::HBU_STATIC_WRITE_ONLY);

    ibuf->writeData(0,ibuf->getSizeInBytes(),fFaces, true);

    sub->useSharedVertices = true;
    sub->indexData->indexBuffer = ibuf;
    sub->indexData->indexCount = fIndexCount;
    sub->indexData->indexStart = 0;

    fMesh->_setBounds(AxisAlignedBox(0,1,0,1,1,1));//this need some work

    fMesh->load();
}
