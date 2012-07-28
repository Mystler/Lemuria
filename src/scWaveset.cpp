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
  fMesh(msh) {
    // initialize algorithm parameters
	fRippleSpeed = 0.3f;
	fDistance = 0.4f;
	fViscosity = 0.05f;
	fTime = 0.13f;
}

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

    fVertices1 = fVertices;
    fVertices2 = fVertices;

    //calculate Complexities
    //we assume that we have a rectangle with a homogeneous dispensation of the vertices
    Vector3 p1 = Vector3(fVertices[0], fVertices[1], fVertices[2]);
    Vector3 p2 = Vector3(fVertices[3], fVertices[4], fVertices[5]);
    Vector3 nextPoint = p2;
    Vector3 nextPointT = p2;
    Vector3 vec = p2 - p1;
    fXComplexity = 2;
    for(int i=2; i<fVertexCount/2; i++) {
        nextPoint = Vector3(fVertices[3*i], fVertices[3*i+1], fVertices[3*i+2]);
        nextPointT += vec;
        if(nextPointT == nextPoint)
            fXComplexity += 1;
        else
            break;
    }
    fYComplexity = fVertexCount/fXComplexity;

    calculateNormals();

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

void scWaveset::calculateNormals() {
    //first we need a temporary array
    Vector3 *vNormals = new Vector3[fVertexCount];
    // set all normals to zero
    for(int i=0; i < fVertexCount/3; i++)
        vNormals[i] = Vector3::ZERO;
    //now go through all faces and calculate the normals of them
    for(int i=0; i < fIndexCount/3; i++) {
        //get verteices of this face
        int ind1 = fFaces[3*i];
        int ind2 = fFaces[3*i+1];
        int ind3 = fFaces[3*i+2];
        //now get the positions of these vertices
        Vector3 vert1 = Vector3(fVertices[3*ind1], fVertices[3*ind1+1], fVertices[3*ind1+2]);
        Vector3 vert2 = Vector3(fVertices[3*ind2], fVertices[3*ind2+1], fVertices[3*ind2+2]);
        Vector3 vert3 = Vector3(fVertices[3*ind3], fVertices[3*ind3+1], fVertices[3*ind3+2]);
        //get the vectors, which define the face
        Vector3 vec1 = vert3 - vert2;
        Vector3 vec2 = vert1 - vert2;
        //get the normal and write it into the array
        Vector3 tn = vec1.crossProduct(vec2);
        vNormals[ind1] += tn;
        vNormals[ind2] += tn;
        vNormals[ind3] += tn;
    }
    //now normalize the normals
    for(int i=0; i < fVertexCount/3; i++) {
        Vector3 n = vNormals[i];
        n.normalise();
        fNormals[3*i] = n.x;
        fNormals[3*i+1] = n.y;
        fNormals[3*i+2] = n.z;
    }
}

void scWaveset::step() {
    double c = fRippleSpeed;
    double d = fDistance;
    double u = fViscosity;
    double t = fTime;

    float *tempBuffer = fVertices;
    Real v1 = (4.0f - 8.0f*c*c*t*t/(d*d))/(u*t+2);
    Real v2 = (u*t-2.0f)/(u*t+2.0f);
    Real v3 = (2.0f*c*c*t*t/(d*d))/(u*t+2);

    for(int y=1; y<fYComplexity; y++) {
        for(int x=1; x<fXComplexity; x++) {
            tempBuffer[3*x+1+y*fXComplexity] = v1*fVertices1[3*x+1+y*fXComplexity];
            tempBuffer[3*x+1+y*fXComplexity] += v2*fVertices2[3*x+1+y*fXComplexity];
            tempBuffer[3*x+1+y*fXComplexity] += v3*(fVertices1[3*x-2+y*fXComplexity]
                                                +fVertices1[3*x+4+y*fXComplexity]
                                                +fVertices1[3*x+1+(y-1)*fXComplexity]
                                                +fVertices1[3*x+1+(y+1)*fXComplexity]);
        }
    }
    fVertices2 = fVertices1;
    fVertices1 = fVertices;
    fVertices = tempBuffer;
    delete tempBuffer;
    calculateNormals();

    fVPosBuf->writeData(0, fVPosBuf->getSizeInBytes(), fVertices, true);

    fBind->setBinding(0, fVPosBuf);
}
