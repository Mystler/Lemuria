/*
 * OgreMax Sample Viewer and Scene Loader - Ogre3D-based viewer and code for loading and displaying .scene files
 * Copyright 2012 AND Entertainment
 *
 * This code is available under the OgreMax Free License:
 *   -You may use this code for any purpose, commercial or non-commercial.
 *   -If distributing derived works (that use this source code) in binary or source code form,
 *    you must give the following credit in your work's end-user documentation:
 *        "Portions of this work provided by OgreMax (www.ogremax.com)"
 *
 * AND Entertainment assumes no responsibility for any harm caused by using this code.
 *
 * The OgreMax Sample Viewer and Scene Loader were released at www.ogremax.com
 */


//Includes---------------------------------------------------------------------
#include "OgreMaxUtilities.hpp"
#include "OgreMaxScene.hpp"
#include <OgreEntity.h>
#include <OgreSubEntity.h>
#include <OgrePlatform.h>
#include <limits.h>
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#include <wtypes.h>
#undef min
#endif

using namespace OgreMax;
using namespace OgreMax::Types;


//Implementation---------------------------------------------------------------
void OgreMaxUtilities::LoadXmlDocument(const Ogre::String &fileName, TiXmlDocument &document, const Ogre::String &resourceGroupName) {
    //Open the file
    Ogre::DataStreamPtr stream = Ogre::ResourceGroupManager::getSingleton().openResource(fileName, resourceGroupName);
    if(stream.isNull()) {
        Ogre::StringUtil::StrStreamType errorMessage;
        errorMessage << "Could not open XML file: " << fileName;

        OGRE_EXCEPT
        (
            Ogre::Exception::ERR_FILE_NOT_FOUND,
            errorMessage.str(),
            "OgreMaxUtilities::LoadXmlDocument"
        );
    }

    //Get the file contents
    Ogre::String data = stream->getAsString();

    //Parse the XML document
    document.Parse(data.c_str());
    stream.setNull();
    if(document.Error()) {
        Ogre::StringUtil::StrStreamType errorMessage;
        errorMessage << "There was an error with the XML file: " << fileName;

        OGRE_EXCEPT
        (
            Ogre::Exception::ERR_INVALID_STATE,
            errorMessage.str(),
            "OgreMaxUtilities::LoadXmlDocument"
        );
    }
}

void OgreMaxUtilities::LoadXYZ(const TiXmlElement *objectElement, Ogre::Vector3 &xyz) {
    xyz.x = GetRealAttribute(objectElement, "x", 0);
    xyz.y = GetRealAttribute(objectElement, "y", 0);
    xyz.z = GetRealAttribute(objectElement, "z", 0);
}

Ogre::Vector3 OgreMaxUtilities::LoadXYZ(const TiXmlElement *objectElement) {
    Ogre::Vector3 xyz;
    LoadXYZ(objectElement, xyz);
    return xyz;
}

Ogre::ColourValue OgreMaxUtilities::LoadColor(const TiXmlElement *objectElement) {
    Ogre::ColourValue color;
    color.r = GetRealAttribute(objectElement, "r", 0);
    color.g = GetRealAttribute(objectElement, "g", 0);
    color.b = GetRealAttribute(objectElement, "b", 0);
    color.a = GetRealAttribute(objectElement, "a", 1);
    return color;
}

Ogre::Plane OgreMaxUtilities::LoadPlane(const TiXmlElement *objectElement) {
    Ogre::Plane plane;
    plane.normal.x = GetRealAttribute(objectElement, "x", 0);
    plane.normal.y = GetRealAttribute(objectElement, "y", 0);
    plane.normal.z = GetRealAttribute(objectElement, "z", 0);
    plane.d = GetRealAttribute(objectElement, "d", 0);
    return plane;
}

Ogre::Quaternion OgreMaxUtilities::LoadRotation(const TiXmlElement *objectElement) {
    Ogre::Quaternion rotation = Ogre::Quaternion::IDENTITY;

    if(objectElement->Attribute("qx") != 0) {
        //The rotation is specified as a quaternion
        rotation.x = GetRealAttribute(objectElement, "qx", 0);
        rotation.y = GetRealAttribute(objectElement, "qy", 0);
        rotation.z = GetRealAttribute(objectElement, "qz", 0);
        rotation.w = GetRealAttribute(objectElement, "qw", 0);

        if(rotation == Ogre::Quaternion::ZERO)
            rotation = Ogre::Quaternion::IDENTITY;
    } else
        if(objectElement->Attribute("axisX") != 0) {
            //The rotation is specified as an axis and angle
            Ogre::Real angle = GetRealAttribute(objectElement, "angle", 0);

            Ogre::Vector3 axis;
            axis.x = GetRealAttribute(objectElement, "axisX", 0);
            axis.y = GetRealAttribute(objectElement, "axisY", 0);
            axis.z = GetRealAttribute(objectElement, "axisZ", 0);

            //Convert the angle and axis into the rotation quaternion
            rotation.FromAngleAxis(Ogre::Radian(angle), axis);
        } else
            if(objectElement->Attribute("angleX") != 0) {
                //Assume the rotation is specified as three Euler angles
                Ogre::Vector3 euler;
                euler.x = GetRealAttribute(objectElement, "angleX", 0);
                euler.y = GetRealAttribute(objectElement, "angleY", 0);
                euler.z = GetRealAttribute(objectElement, "angleZ", 0);
                Ogre::String order = GetStringAttribute(objectElement, "order");

                //Convert Euler angles to a matrix
                Ogre::Matrix3 rotationMatrix;
                if(order.length() < 2)
                    rotationMatrix.FromEulerAnglesXYZ(Ogre::Radian(euler.x), Ogre::Radian(euler.y), Ogre::Radian(euler.z));
                else {
                    if(order[0] == 'x') {
                        if(order[1] == 'y')
                            rotationMatrix.FromEulerAnglesXYZ(Ogre::Radian(euler.x), Ogre::Radian(euler.y), Ogre::Radian(euler.z));
                        else
                            rotationMatrix.FromEulerAnglesXZY(Ogre::Radian(euler.x), Ogre::Radian(euler.y), Ogre::Radian(euler.z));
                    } else
                        if(order[0] == 'y') {
                            if(order[1] == 'x')
                                rotationMatrix.FromEulerAnglesYXZ(Ogre::Radian(euler.x), Ogre::Radian(euler.y), Ogre::Radian(euler.z));
                            else
                                rotationMatrix.FromEulerAnglesYZX(Ogre::Radian(euler.x), Ogre::Radian(euler.y), Ogre::Radian(euler.z));
                        } else {
                            if(order[1] == 'x')
                                rotationMatrix.FromEulerAnglesZXY(Ogre::Radian(euler.x), Ogre::Radian(euler.y), Ogre::Radian(euler.z));
                            else
                                rotationMatrix.FromEulerAnglesZYX(Ogre::Radian(euler.x), Ogre::Radian(euler.y), Ogre::Radian(euler.z));
                        }
                }

                //Convert the matrix into the rotation quaternion
                rotation.FromRotationMatrix(rotationMatrix);
            }

    return rotation;
}

Ogre::FloatRect OgreMaxUtilities::LoadFloatRectangle(const TiXmlElement *objectElement) {
    Ogre::FloatRect rect;
    rect.left = GetRealAttribute(objectElement, "left", 0);
    rect.top = GetRealAttribute(objectElement, "top", 0);
    rect.right = GetRealAttribute(objectElement, "right", 0);
    rect.bottom = GetRealAttribute(objectElement, "bottom", 0);
    return rect;
}

void OgreMaxUtilities::LoadBufferUsage(const TiXmlElement *objectElement, Ogre::HardwareBuffer::Usage &usage, bool &shadowed) {
    Ogre::String usageText = GetStringAttribute(objectElement, "usage");
    usage = usageText.empty() ? Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY : ParseHardwareBufferUsage(usageText);

    shadowed = GetBoolAttribute(objectElement, "useShadow", true);
}

void OgreMaxUtilities::LoadBoundingVolume(const TiXmlElement *objectElement, BoundingVolume &volume) {
    Ogre::String type = OgreMaxUtilities::GetStringAttribute(objectElement, "type");
    volume.type = ParseBoundingVolumeType(type);
    volume.radius = GetRealAttribute(objectElement, "radius", 0);
    volume.size.x = GetRealAttribute(objectElement, "height", 0);
    int faceCount = GetIntAttribute(objectElement, "faceCount", 0);
    if(faceCount > 0)
        volume.meshFaces.resize(faceCount);

    //Parse child elements
    Ogre::String elementName;
    const TiXmlElement *childElement = 0;
    while((childElement = IterateChildElements(objectElement, childElement))) {
        elementName = childElement->Value();

        if(elementName == "size")
            volume.size = LoadXYZ(childElement);
        else
            if(elementName == "faces")
                LoadBoundingVolumeFaces(childElement, faceCount, volume.meshFaces);
    }
}

void OgreMaxUtilities::LoadBoundingVolumeFaces(const TiXmlElement *objectElement, int faceCount, std::vector<BoundingVolume::Face>& faces) {
    //Parse child elements, treating them all as faces
    int faceIndex = 0;
    const TiXmlElement *childElement = 0;
    while((childElement = IterateChildElements(objectElement, childElement)) && faceIndex < faceCount) {
        BoundingVolume::Face &face = faces[faceIndex++];

        //Load the vertices
        int vertexIndex = 0;
        const TiXmlElement *vertexElement = 0;
        while((vertexElement = IterateChildElements(childElement, vertexElement)) && vertexIndex < 3)
            LoadXYZ(vertexElement, face.vertex[vertexIndex++]);
    }
}

bool OgreMaxUtilities::ParseSceneManager(const Ogre::String &sceneManager, Ogre::SceneType &sceneType) {
    sceneType = (Ogre::SceneType)0;

    Ogre::String sceneManagerLower = sceneManager;
    Ogre::StringUtil::toLowerCase(sceneManagerLower);

    if(sceneManagerLower == "generic")
        sceneType = Ogre::ST_GENERIC;
    else
        if(sceneManagerLower == "exteriorclose")
            sceneType = Ogre::ST_EXTERIOR_CLOSE;
        else
            if(sceneManagerLower == "exteriorfar")
                sceneType = Ogre::ST_EXTERIOR_FAR;
            else
                if(sceneManagerLower == "exteriorrealfar")
                    sceneType = Ogre::ST_EXTERIOR_REAL_FAR;
                else
                    if(sceneManagerLower == "interior")
                        sceneType = Ogre::ST_INTERIOR;

    return sceneType != (Ogre::SceneType)0;
}

bool OgreMaxUtilities::ParseBool(const Ogre::String &value) {
    Ogre::String valueLower = value;
    Ogre::StringUtil::toLowerCase(valueLower);
    if(valueLower.empty() || valueLower == "false" || valueLower == "no" || value == "0")
        return false;
    else
        return true;
}

Ogre::SkeletonAnimationBlendMode OgreMaxUtilities::ParseSkeletonAnimationBlendMode(const Ogre::String &mode) {
    Ogre::String modeLower = mode;
    Ogre::StringUtil::toLowerCase(modeLower);

    if(modeLower.empty() || modeLower == "average")
        return Ogre::ANIMBLEND_AVERAGE;
    else
        if(modeLower == "cumulative")
            return Ogre::ANIMBLEND_CUMULATIVE;

    Ogre::StringUtil::StrStreamType errorMessage;
    errorMessage << "Invalid skeleton animation blend mode specified: " << mode;

    OGRE_EXCEPT
    (
        Ogre::Exception::ERR_INVALIDPARAMS,
        errorMessage.str(),
        "OgreMaxUtilities::ParseSkeletonAnimationBlendMode"
    );
}

Ogre::Light::LightTypes OgreMaxUtilities::ParseLightType(const Ogre::String &type) {
    Ogre::String typeLower = type;
    Ogre::StringUtil::toLowerCase(typeLower);

    if(typeLower == "point")
        return Ogre::Light::LT_POINT;
    else
        if(typeLower == "directional")
            return Ogre::Light::LT_DIRECTIONAL;
        else
            if(typeLower == "spot")
                return Ogre::Light::LT_SPOTLIGHT;

    Ogre::StringUtil::StrStreamType errorMessage;
    errorMessage << "Invalid light type specified: " << type;

    OGRE_EXCEPT
    (
        Ogre::Exception::ERR_INVALIDPARAMS,
        errorMessage.str(),
        "OgreMaxUtilities::ParseLightType"
    );
}

Ogre::ProjectionType OgreMaxUtilities::ParseProjectionType(const Ogre::String &type) {
    Ogre::String typeLower = type;
    Ogre::StringUtil::toLowerCase(typeLower);

    if(typeLower == "perspective")
        return Ogre::PT_PERSPECTIVE;
    else
        if(typeLower == "orthographic")
            return Ogre::PT_ORTHOGRAPHIC;

    Ogre::StringUtil::StrStreamType errorMessage;
    errorMessage << "Invalid projection type specified: " << type;

    OGRE_EXCEPT
    (
        Ogre::Exception::ERR_INVALIDPARAMS,
        errorMessage.str(),
        "OgreMaxUtilities::ParseProjectionType"
    );
}

Ogre::BillboardType OgreMaxUtilities::ParseBillboardType(const Ogre::String &type) {
    Ogre::String typeLower = type;
    Ogre::StringUtil::toLowerCase(typeLower);

    if(typeLower == "point")
        return Ogre::BBT_POINT;
    else
        if(typeLower == "orientedcommon")
            return Ogre::BBT_ORIENTED_COMMON;
        else
            if(typeLower == "orientedself")
                return Ogre::BBT_ORIENTED_SELF;
            else
                if(typeLower == "perpendicularcommon")
                    return Ogre::BBT_PERPENDICULAR_COMMON;
                else
                    if(typeLower == "perpendicularself")
                        return Ogre::BBT_PERPENDICULAR_SELF;

    Ogre::StringUtil::StrStreamType errorMessage;
    errorMessage << "Invalid billboard type specified: " << type;

    OGRE_EXCEPT
    (
        Ogre::Exception::ERR_INVALIDPARAMS,
        errorMessage.str(),
        "OgreMaxUtilities::ParseBillboardType"
    );
}

Ogre::BillboardOrigin OgreMaxUtilities::ParseBillboardOrigin(const Ogre::String &origin) {
    Ogre::String originLower = origin;
    Ogre::StringUtil::toLowerCase(originLower);

    if(originLower == "topleft")
        return Ogre::BBO_TOP_LEFT;
    else
        if(originLower == "topcenter")
            return Ogre::BBO_TOP_CENTER;
        else
            if(originLower == "topright")
                return Ogre::BBO_TOP_RIGHT;
            else
                if(originLower == "centerleft")
                    return Ogre::BBO_CENTER_LEFT;
                else
                    if(originLower == "center")
                        return Ogre::BBO_CENTER;
                    else
                        if(originLower == "centerright")
                            return Ogre::BBO_CENTER_RIGHT;
                        else
                            if(originLower == "bottomleft")
                                return Ogre::BBO_BOTTOM_LEFT;
                            else
                                if(originLower == "bottomcenter")
                                    return Ogre::BBO_BOTTOM_CENTER;
                                else
                                    if(originLower == "bottomright")
                                        return Ogre::BBO_BOTTOM_RIGHT;

    Ogre::StringUtil::StrStreamType errorMessage;
    errorMessage << "Invalid billboard origin specified: " << origin;

    OGRE_EXCEPT
    (
        Ogre::Exception::ERR_INVALIDPARAMS,
        errorMessage.str(),
        "OgreMaxUtilities::ParseBillboardOrigin"
    );
}

Ogre::BillboardRotationType OgreMaxUtilities::ParseBillboardRotationType(const Ogre::String &rotationType) {
    Ogre::String rotationTypeLower = rotationType;
    Ogre::StringUtil::toLowerCase(rotationTypeLower);

    if(rotationTypeLower == "vertex")
        return Ogre::BBR_VERTEX;
    else
        if(rotationTypeLower == "texcoord")
            return Ogre::BBR_TEXCOORD;

    Ogre::StringUtil::StrStreamType errorMessage;
    errorMessage << "Invalid billboard rotation type specified: " << rotationType;

    OGRE_EXCEPT
    (
        Ogre::Exception::ERR_INVALIDPARAMS,
        errorMessage.str(),
        "OgreMaxUtilities::ParseBillboardRotationType"
    );
}

Ogre::FogMode OgreMaxUtilities::ParseFogMode(const Ogre::String &mode) {
    Ogre::String modeLower = mode;
    Ogre::StringUtil::toLowerCase(modeLower);

    if(modeLower == "exp")
        return Ogre::FOG_EXP;
    else
        if(modeLower == "exp2")
            return Ogre::FOG_EXP2;
        else
            if(modeLower == "linear")
                return Ogre::FOG_LINEAR;
            else
                return Ogre::FOG_NONE;

    Ogre::StringUtil::StrStreamType errorMessage;
    errorMessage << "Invalid fog mode specified: " << mode;

    OGRE_EXCEPT
    (
        Ogre::Exception::ERR_INVALIDPARAMS,
        errorMessage.str(),
        "OgreMaxUtilities::ParseFogMode"
    );
}

Ogre::HardwareBuffer::Usage OgreMaxUtilities::ParseHardwareBufferUsage(const Ogre::String &usage) {
    Ogre::String usageLower = usage;
    Ogre::StringUtil::toLowerCase(usageLower);

    if(usageLower == "static")
        return Ogre::HardwareBuffer::HBU_STATIC;
    else
        if(usageLower == "dynamic")
            return Ogre::HardwareBuffer::HBU_DYNAMIC;
        else
            if(usageLower == "writeonly")
                return Ogre::HardwareBuffer::HBU_WRITE_ONLY;
            else
                if(usageLower == "staticwriteonly")
                    return Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY;
                else
                    if(usageLower == "dynamicwriteonly")
                        return Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY;

    Ogre::StringUtil::StrStreamType errorMessage;
    errorMessage << "Invalid hardware buffer usage specified: " << usage;

    OGRE_EXCEPT
    (
        Ogre::Exception::ERR_INVALIDPARAMS,
        errorMessage.str(),
        "OgreMaxUtilities::ParseHardwareBufferUsage"
    );
}

Ogre::Node::TransformSpace OgreMaxUtilities::ParseTransformSpace(const Ogre::String &space) {
    Ogre::String spaceLower = space;
    Ogre::StringUtil::toLowerCase(spaceLower);

    if(spaceLower == "local")
        return Ogre::Node::TS_LOCAL;
    else
        if(spaceLower == "parent")
            return Ogre::Node::TS_PARENT;
        else
            if(spaceLower == "world")
                return Ogre::Node::TS_WORLD;

    Ogre::StringUtil::StrStreamType errorMessage;
    errorMessage << "Invalid transform space specified: " << space;

    OGRE_EXCEPT
    (
        Ogre::Exception::ERR_INVALIDPARAMS,
        errorMessage.str(),
        "OgreMaxUtilities::ParseTransformSpace"
    );
}

BoundingVolume::Type OgreMaxUtilities::ParseBoundingVolumeType(const Ogre::String &type) {
    Ogre::String typeLower = type;
    Ogre::StringUtil::toLowerCase(typeLower);

    if(typeLower == "sphere")
        return BoundingVolume::SPHERE;
    else
        if(typeLower == "box")
            return BoundingVolume::BOX;
        else
            if(typeLower == "cylinder")
                return BoundingVolume::CYLINDER;
            else
                if(typeLower == "capsule")
                    return BoundingVolume::CAPSULE;
                else
                    if(typeLower == "mesh")
                        return BoundingVolume::MESH;

    Ogre::StringUtil::StrStreamType errorMessage;
    errorMessage << "Invalid bounding volume type specified: " << type;

    OGRE_EXCEPT
    (
        Ogre::Exception::ERR_INVALIDPARAMS,
        errorMessage.str(),
        "OgreMaxUtilities::ParseBoundingVolumeType"
    );
}

void OgreMaxUtilities::LoadCustomParameters(const TiXmlElement *objectElement, std::vector<CustomParameter>& customParameters) {
    customParameters.resize(GetElementCount(objectElement, "customParameter"));

    size_t customParameterIndex = 0;
    const TiXmlElement *childElement = 0;
    while((childElement = IterateChildElements(objectElement, childElement))) {
        customParameters[customParameterIndex].id = (size_t)GetIntAttribute(childElement, "id", 0);
        customParameters[customParameterIndex].value = GetVector4Attributes(childElement);

        customParameterIndex++;
    }
}

void OgreMaxUtilities::LoadUserDataReference(const TiXmlElement *objectElement, Ogre::String &userDataReference) {
    userDataReference = GetStringAttribute(objectElement, "id");
}

void OgreMaxUtilities::LoadSubentities(const TiXmlElement *objectElement, std::vector<EntityParameters::Subentity>& subentities) {
    subentities.resize(GetElementCount(objectElement, "subentity"));

    const TiXmlElement *childElement = 0;
    while((childElement = IterateChildElements(objectElement, childElement))) {
        int index = GetIntAttribute(childElement, "index", 0);
        subentities[index].materialName = GetStringAttribute(childElement, "materialName");
    }
}

void OgreMaxUtilities::LoadManualLods(const TiXmlElement *objectElement, std::vector<Types::EntityParameters::ManualLod>& manualLods) {
    manualLods.resize(GetElementCount(objectElement, "manualLod"));

    const TiXmlElement *childElement = 0;
    while((childElement = IterateChildElements(objectElement, childElement))) {
        int index = GetIntAttribute(childElement, "index", 0);

        const TiXmlElement *subentitiesElement = childElement->FirstChildElement("subentities");
        if(subentitiesElement != 0)
            LoadSubentities(subentitiesElement, manualLods[index].subentities);
    }
}

void OgreMaxUtilities::LoadNoteTracks(const TiXmlElement *objectElement, std::vector<NoteTrack>& noteTracks) {
    //Allocate note tracks
    size_t noteTrackCount = GetChildElementCount(objectElement, "noteTrack");
    noteTracks.resize(noteTrackCount);

    //Parse child elements
    size_t noteTrackIndex = 0;
    Ogre::String elementName;
    const TiXmlElement *childElement = 0;
    while((childElement = IterateChildElements(objectElement, childElement))) {
        elementName = childElement->Value();

        if(elementName == "noteTrack")
            LoadNoteTrack(childElement, noteTracks[noteTrackIndex++]);
    }
}

void OgreMaxUtilities::LoadNoteTrack(const TiXmlElement *objectElement, NoteTrack &noteTrack) {
    //Track name
    noteTrack.name = OgreMaxUtilities::GetStringAttribute(objectElement, "name");

    //Allocate notes
    size_t noteCount = OgreMaxUtilities::GetChildElementCount(objectElement, "note");
    noteTrack.notes.resize(noteCount);

    //Load notes
    size_t noteIndex = 0;
    Ogre::String elementName;
    const TiXmlElement *childElement = 0;
    while((childElement = IterateChildElements(objectElement, childElement))) {
        elementName = childElement->Value();

        if(elementName == "note")
            LoadNote(childElement, noteTrack.notes[noteIndex++]);
    }
}

void OgreMaxUtilities::LoadNote(const TiXmlElement *objectElement, Note &note) {
    //Note time
    note.time = GetRealAttribute(objectElement, "time", 0);

    //Note text
    GetChildText(objectElement, note.text);
}

Ogre::ShadowTechnique OgreMaxUtilities::ParseShadowTechnique(const Ogre::String &technique) {
    Ogre::String techniqueLower = technique;
    Ogre::StringUtil::toLowerCase(techniqueLower);

    if(techniqueLower == "none")
        return Ogre::SHADOWTYPE_NONE;
    else
        if(techniqueLower == "stencilmodulative")
            return Ogre::SHADOWTYPE_STENCIL_MODULATIVE;
        else
            if(techniqueLower == "stenciladditive")
                return Ogre::SHADOWTYPE_STENCIL_ADDITIVE;
            else
                if(techniqueLower == "texturemodulative")
                    return Ogre::SHADOWTYPE_TEXTURE_MODULATIVE;
                else
                    if(techniqueLower == "textureadditive")
                        return Ogre::SHADOWTYPE_TEXTURE_ADDITIVE;
                    else
                        if(techniqueLower == "texturemodulativeintegrated")
                            return Ogre::SHADOWTYPE_TEXTURE_MODULATIVE_INTEGRATED;
                        else
                            if(techniqueLower == "textureadditiveintegrated")
                                return Ogre::SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED;

    Ogre::StringUtil::StrStreamType errorMessage;
    errorMessage << "Invalid shadow technique specified: " << technique;

    OGRE_EXCEPT
    (
        Ogre::Exception::ERR_INVALIDPARAMS,
        errorMessage.str(),
        "OgreMaxUtilities::ParseShadowTechnique"
    );
}

UpAxis OgreMaxUtilities::ParseUpAxis(const Ogre::String &upAxis) {
    Ogre::String upAxisLower = upAxis;
    Ogre::StringUtil::toLowerCase(upAxisLower);

    if(upAxisLower == "y")
        return UP_AXIS_Y;
    else
        if(upAxisLower == "z")
            return UP_AXIS_Z;

    Ogre::StringUtil::StrStreamType errorMessage;
    errorMessage << "Invalid up axis specified: " << upAxis;

    OGRE_EXCEPT
    (
        Ogre::Exception::ERR_INVALIDPARAMS,
        errorMessage.str(),
        "OgreMaxUtilities::ParseUpAxis"
    );
}

Ogre::Animation::InterpolationMode OgreMaxUtilities::ParseAnimationInterpolationMode(const Ogre::String &mode) {
    Ogre::String modeLower = mode;
    Ogre::StringUtil::toLowerCase(modeLower);

    if(modeLower == "linear")
        return Ogre::Animation::IM_LINEAR;
    else
        if(modeLower == "spline")
            return Ogre::Animation::IM_SPLINE;

    Ogre::StringUtil::StrStreamType errorMessage;
    errorMessage << "Invalid animation interpolation mode specified: " << mode;

    OGRE_EXCEPT
    (
        Ogre::Exception::ERR_INVALIDPARAMS,
        errorMessage.str(),
        "OgreMaxUtilities::ParseAnimationInterpolationMode"
    );
}

Ogre::Animation::RotationInterpolationMode OgreMaxUtilities::ParseAnimationRotationInterpolationMode(const Ogre::String &mode) {
    Ogre::String modeLower = mode;
    Ogre::StringUtil::toLowerCase(modeLower);

    if(modeLower == "linear")
        return Ogre::Animation::RIM_LINEAR;
    else
        if(modeLower == "spherical")
            return Ogre::Animation::RIM_SPHERICAL;

    Ogre::StringUtil::StrStreamType errorMessage;
    errorMessage << "Invalid animation rotation interpolation mode specified: " << mode;

    OGRE_EXCEPT
    (
        Ogre::Exception::ERR_INVALIDPARAMS,
        errorMessage.str(),
        "OgreMaxUtilities::ParseAnimationRotationInterpolationMode"
    );
}

NodeVisibility OgreMaxUtilities::ParseNodeVisibility(const Ogre::String &visibility) {
    Ogre::String visibilityLower = visibility;
    Ogre::StringUtil::toLowerCase(visibilityLower);

    if(visibilityLower == "visible")
        return NODE_VISIBLE;
    else
        if(visibilityLower == "hidden")
            return NODE_HIDDEN;
        else
            if(visibilityLower == "treevisible")
                return NODE_TREE_VISIBLE;
            else
                if(visibilityLower == "treehidden")
                    return NODE_TREE_HIDDEN;
                else
                    return NODE_VISIBILITY_DEFAULT;
}

ObjectVisibility OgreMaxUtilities::ParseObjectVisibility(const Ogre::String &visibility) {
    if(visibility.empty())
        return OBJECT_VISIBILITY_DEFAULT;
    else
        return ParseBool(visibility) ? OBJECT_VISIBLE : OBJECT_HIDDEN;
}

Ogre::uint8 OgreMaxUtilities::ParseRenderQueue(const Ogre::String &renderQueue) {
    static std::map<Ogre::String, Ogre::uint8> nameToNumber;
    if(nameToNumber.empty()) {
        nameToNumber["background"] = Ogre::RENDER_QUEUE_BACKGROUND;
        nameToNumber["skiesearly"] = Ogre::RENDER_QUEUE_SKIES_EARLY;
        nameToNumber["queue1"] = Ogre::RENDER_QUEUE_1;
        nameToNumber["queue2"] = Ogre::RENDER_QUEUE_2;
        nameToNumber["worldgeometry1"] = Ogre::RENDER_QUEUE_WORLD_GEOMETRY_1;
        nameToNumber["queue3"] = Ogre::RENDER_QUEUE_3;
        nameToNumber["queue4"] = Ogre::RENDER_QUEUE_4;
        nameToNumber["main"] = Ogre::RENDER_QUEUE_MAIN;
        nameToNumber["queue6"] = Ogre::RENDER_QUEUE_6;
        nameToNumber["queue7"] = Ogre::RENDER_QUEUE_7;
        nameToNumber["worldgeometry2"] = Ogre::RENDER_QUEUE_WORLD_GEOMETRY_2;
        nameToNumber["queue8"] = Ogre::RENDER_QUEUE_8;
        nameToNumber["queue9"] = Ogre::RENDER_QUEUE_9;
        nameToNumber["skieslate"] = Ogre::RENDER_QUEUE_SKIES_LATE;
        nameToNumber["overlay"] = Ogre::RENDER_QUEUE_OVERLAY;
        nameToNumber["max"] = Ogre::RENDER_QUEUE_MAX;
    }

    if(renderQueue.empty())
        return Ogre::RENDER_QUEUE_MAIN;
    else
        if(AllDigits(renderQueue))
            return (Ogre::uint8)Ogre::StringConverter::parseUnsignedInt(renderQueue);
        else {
            //The render queue name, lowercase
            Ogre::String renderQueueLower;

            //Get the offset that comes after the +, if any
            Ogre::uint8 offset = 0;
            size_t plusFoundAt = renderQueue.find('+');
            if(plusFoundAt != Ogre::String::npos) {
                //Parse the number
                Ogre::String offsetText = renderQueue.substr(plusFoundAt + 1);
                Ogre::StringUtil::trim(offsetText);

                offset = (Ogre::uint8)Ogre::StringConverter::parseUnsignedInt(offsetText);

                //Remove the "+offset" substring from the render queue name
                renderQueueLower = renderQueue.substr(0, plusFoundAt);
                Ogre::StringUtil::trim(renderQueueLower);
            } else
                renderQueueLower = renderQueue;
            Ogre::StringUtil::toLowerCase(renderQueueLower);

            //Look up the render queue and return it
            std::map<Ogre::String, Ogre::uint8>::iterator item = nameToNumber.find(renderQueueLower);
            if(item != nameToNumber.end()) {
                //Don't let the render queue exceed the maximum
                return std::min((Ogre::uint8)(item->second + offset), (Ogre::uint8)Ogre::RENDER_QUEUE_MAX);
            } else {
                Ogre::StringUtil::StrStreamType errorMessage;
                errorMessage << "Invalid render queue specified: " << renderQueue;

                OGRE_EXCEPT
                (
                    Ogre::Exception::ERR_INVALIDPARAMS,
                    errorMessage.str(),
                    "OgreMaxUtilities::ParseRenderQueue"
                );
            }
        }
}

Ogre::PixelFormat OgreMaxUtilities::ParsePixelFormat(const Ogre::String &pixelFormat) {
    static bool initialized = false;
    static std::map<Ogre::String, Ogre::PixelFormat> nameToFormat;
    if(!initialized) {
        nameToFormat["L8"] = Ogre::PF_L8;
        nameToFormat["L16"] = Ogre::PF_L16;
        nameToFormat["A8"] = Ogre::PF_A8;
        nameToFormat["A4L4"] = Ogre::PF_A4L4;
        nameToFormat["BYTE_LA"] = Ogre::PF_BYTE_LA;
        nameToFormat["R5G6B5"] = Ogre::PF_R5G6B5;
        nameToFormat["B5G6R5"] = Ogre::PF_B5G6R5;
        nameToFormat["R3G3B2"] = Ogre::PF_R3G3B2;
        nameToFormat["A4R4G4B4"] = Ogre::PF_A4R4G4B4;
        nameToFormat["A1R5G5B5"] = Ogre::PF_A1R5G5B5;
        nameToFormat["R8G8B8"] = Ogre::PF_R8G8B8;
        nameToFormat["B8G8R8"] = Ogre::PF_B8G8R8;
        nameToFormat["A8R8G8B8"] = Ogre::PF_A8R8G8B8;
        nameToFormat["A8B8G8R8"] = Ogre::PF_A8B8G8R8;
        nameToFormat["B8G8R8A8"] = Ogre::PF_B8G8R8A8;
        nameToFormat["R8G8B8A8"] = Ogre::PF_R8G8B8A8;
        nameToFormat["X8R8G8B8"] = Ogre::PF_X8R8G8B8;
        nameToFormat["X8B8G8R8"] = Ogre::PF_X8B8G8R8;
        nameToFormat["A2R10G10B10"] = Ogre::PF_A2R10G10B10;
        nameToFormat["A2B10G10R10"] = Ogre::PF_A2B10G10R10;
        nameToFormat["FLOAT16_R"] = Ogre::PF_FLOAT16_R;
        nameToFormat["FLOAT16_RGB"] = Ogre::PF_FLOAT16_RGB;
        nameToFormat["FLOAT16_RGBA"] = Ogre::PF_FLOAT16_RGBA;
        nameToFormat["FLOAT32_R"] = Ogre::PF_FLOAT32_R;
        nameToFormat["FLOAT32_RGB"] = Ogre::PF_FLOAT32_RGB;
        nameToFormat["FLOAT32_RGBA"] = Ogre::PF_FLOAT32_RGBA;
        nameToFormat["FLOAT16_GR"] = Ogre::PF_FLOAT16_GR;
        nameToFormat["FLOAT32_GR"] = Ogre::PF_FLOAT32_GR;
        nameToFormat["DEPTH"] = Ogre::PF_DEPTH;
        nameToFormat["SHORT_RGBA"] = Ogre::PF_SHORT_RGBA;
        nameToFormat["SHORT_GR"] = Ogre::PF_SHORT_GR;
        nameToFormat["SHORT_RGB"] = Ogre::PF_SHORT_RGB;

        initialized = true;
    }

    std::map<Ogre::String, Ogre::PixelFormat>::iterator format = nameToFormat.find(pixelFormat);
    if(format != nameToFormat.end())
        return format->second;

    Ogre::StringUtil::StrStreamType errorMessage;
    errorMessage << "Invalid pixel format specified: " << pixelFormat;

    OGRE_EXCEPT
    (
        Ogre::Exception::ERR_INVALIDPARAMS,
        errorMessage.str(),
        "OgreMaxUtilities::ParsePixelFormat"
    );
}

Ogre::TextureType OgreMaxUtilities::ParseTextureType(const Ogre::String &textureType) {
    Ogre::String textureTypeLower = textureType;
    Ogre::StringUtil::toLowerCase(textureTypeLower);

    if(textureTypeLower == "1d")
        return Ogre::TEX_TYPE_1D;
    else
        if(textureTypeLower == "2d")
            return Ogre::TEX_TYPE_2D;
        else
            if(textureTypeLower == "3d")
                return Ogre::TEX_TYPE_3D;
            else
                if(textureTypeLower == "cubic")
                    return Ogre::TEX_TYPE_CUBE_MAP;

    Ogre::StringUtil::StrStreamType errorMessage;
    errorMessage << "Invalid texture type specified: " << textureType;

    OGRE_EXCEPT
    (
        Ogre::Exception::ERR_INVALIDPARAMS,
        errorMessage.str(),
        "OgreMaxUtilities::ParseTextureType"
    );
}

void OgreMaxUtilities::LoadClipping(const TiXmlElement *objectElement, Ogre::Real &nearClip, Ogre::Real &farClip) {
    nearClip = GetRealAttribute(objectElement, "near", 1);
    farClip = GetRealAttribute(objectElement, "far", OgreMaxScene::DEFAULT_MAX_DISTANCE);
}

void OgreMaxUtilities::GetChildText(const TiXmlElement *xmlElement, Ogre::String &text) {
    //Get the first element
    const TiXmlNode *childNode = xmlElement->FirstChild();
    while(childNode != 0) {
        if(childNode->Type() == TiXmlNode::TINYXML_TEXT) {
            const TiXmlText *textNode = childNode->ToText();
            if(textNode != 0) {
                text = textNode->Value();
                break;
            }
        }
        childNode = xmlElement->NextSibling();
    }
}

Ogre::String OgreMaxUtilities::GetStringAttribute(const TiXmlElement *xmlElement, const char *name) {
    const char *value = xmlElement->Attribute(name);
    if(value != 0)
        return value;
    else
        return Ogre::StringUtil::BLANK;
}

Ogre::String OgreMaxUtilities::GetStringAttribute(const TiXmlElement *xmlElement, const char *name, const char *defaultValue) {
    Ogre::String value = GetStringAttribute(xmlElement, name);
    return value.empty() ? defaultValue : value;
}

Ogre::Real OgreMaxUtilities::GetRealAttribute(const TiXmlElement *xmlElement, const char *name, Ogre::Real defaultValue) {
    Ogre::String value = GetStringAttribute(xmlElement, name);
    return value.empty() ? defaultValue : Ogre::StringConverter::parseReal(value);
}

int OgreMaxUtilities::GetIntAttribute(const TiXmlElement *xmlElement, const char *name, int defaultValue) {
    Ogre::String value = GetStringAttribute(xmlElement, name);
    return value.empty() ? defaultValue : Ogre::StringConverter::parseInt(value);
}

int OgreMaxUtilities::GetUIntAttribute(const TiXmlElement *xmlElement, const char *name, unsigned int defaultValue) {
    Ogre::String value = GetStringAttribute(xmlElement, name);
    return value.empty() ? defaultValue : Ogre::StringConverter::parseUnsignedInt(value);
}

bool OgreMaxUtilities::GetBoolAttribute(const TiXmlElement *xmlElement, const char *name, bool defaultValue) {
    Ogre::String value = GetStringAttribute(xmlElement, name);
    return value.empty() ? defaultValue : ParseBool(value);
}

Ogre::Plane OgreMaxUtilities::GetPlaneAttributes(const TiXmlElement *xmlElement, Ogre::Real defaultX, Ogre::Real defaultY, Ogre::Real defaultZ, Ogre::Real defaultD) {
    Ogre::Plane plane;
    plane.normal.x = GetRealAttribute(xmlElement, "planeX", defaultX);
    plane.normal.y = GetRealAttribute(xmlElement, "planeY", defaultY);
    plane.normal.z = GetRealAttribute(xmlElement, "planeZ", defaultZ);
    plane.d = GetRealAttribute(xmlElement, "planeD", defaultD);
    return plane;
}

Ogre::Vector4 OgreMaxUtilities::GetVector4Attributes(const TiXmlElement *xmlElement) {
    Ogre::Vector4 v4;
    v4.x = GetRealAttribute(xmlElement, "x", 0);
    v4.y = GetRealAttribute(xmlElement, "y", 0);
    v4.z = GetRealAttribute(xmlElement, "z", 0);
    v4.w = GetRealAttribute(xmlElement, "w", 0);
    return v4;
}

Ogre::PixelFormat OgreMaxUtilities::GetPixelFormatAttribute(const TiXmlElement *xmlElement, const char *name, Ogre::PixelFormat defaultValue) {
    Ogre::String value = GetStringAttribute(xmlElement, name);
    return value.empty() ? defaultValue : ParsePixelFormat(value);
}

Ogre::TextureType OgreMaxUtilities::GetTextureTypeAttribute(const TiXmlElement *xmlElement, const char *name, Ogre::TextureType defaultValue) {
    Ogre::String value = GetStringAttribute(xmlElement, name);
    return value.empty() ? defaultValue : ParseTextureType(value);
}

NodeVisibility OgreMaxUtilities::GetNodeVisibilityAttribute(const TiXmlElement *xmlElement, const char *name) {
    Ogre::String visibility = OgreMaxUtilities::GetStringAttribute(xmlElement, name);
    return ParseNodeVisibility(visibility);
}

ObjectVisibility OgreMaxUtilities::GetObjectVisibilityAttribute(const TiXmlElement *xmlElement, const char *name) {
    Ogre::String visibility = OgreMaxUtilities::GetStringAttribute(xmlElement, name);
    return ParseObjectVisibility(visibility);
}

size_t OgreMaxUtilities::GetElementCount(const TiXmlElement *xmlElement, const Ogre::String &elementName) {
    size_t count = 0;

    //Check name
    if(elementName == xmlElement->Value())
        count++;

    //Recurse into children
    const TiXmlElement *childElement = 0;
    while((childElement = IterateChildElements(xmlElement, childElement)))
        count += GetElementCount(childElement, elementName);

    return count;
}

size_t OgreMaxUtilities::GetChildElementCount(const TiXmlElement *xmlElement, const Ogre::String &elementName) {
    size_t count = 0;

    //Check children
    const TiXmlElement *childElement = 0;
    while((childElement = IterateChildElements(xmlElement, childElement))) {
        if(elementName == childElement->Value())
            count++;
    }

    return count;
}

const TiXmlElement *OgreMaxUtilities::IterateChildElements(const TiXmlElement *xmlElement, const TiXmlElement *childElement) {
    if(xmlElement != 0) {
        if(childElement == 0)
            childElement = xmlElement->FirstChildElement();
        else
            childElement = childElement->NextSiblingElement();

        return childElement;
    }

    return 0;
}

bool OgreMaxUtilities::AllDigits(const Ogre::String &text) {
    for(size_t i = 0; i < text.length(); i++) {
        if(!isdigit(text[i]))
            return false;
    }

    return true;
}

bool OgreMaxUtilities::IsPowerOfTwo(unsigned int value) {
    return (value & (value - 1)) == 0;
}

unsigned int OgreMaxUtilities::NextLargestPowerOfTwo(unsigned int value) {
    //From Ice/IceUtils.c in the ODE physics library
    value |= (value >> 1);
    value |= (value >> 2);
    value |= (value >> 4);
    value |= (value >> 8);
    value |= (value >> 16);
    return value + 1;
}

unsigned int OgreMaxUtilities::NextSmallestPowerOfTwo(unsigned int value) {
    if(!IsPowerOfTwo(value)) {
        //Not a power of 2. Round value up to the next power of 2
        value = NextLargestPowerOfTwo(value);
    }

    //The value is a power of 2. Shift downward to get the next smallest power of 2
    return value >> 1;
}

void OgreMaxUtilities::SetNodeVisibility(Ogre::SceneNode *node, NodeVisibility visibility) {
    switch(visibility) {
        case NODE_VISIBLE:
            node->setVisible(true, false);
            break;
        case NODE_HIDDEN:
            node->setVisible(false, false);
            break;
        case NODE_TREE_VISIBLE:
            node->setVisible(true, true);
            break;
        case NODE_TREE_HIDDEN:
            node->setVisible(false, true);
            break;
        default:
            break;
    }
}

void OgreMaxUtilities::SetCustomParameters(Ogre::Renderable *renderable, const std::vector<CustomParameter>& customParameters) {
    for(size_t customParameterIndex = 0; customParameterIndex < customParameters.size(); customParameterIndex++)
        renderable->setCustomParameter(customParameters[customParameterIndex].id, customParameters[customParameterIndex].value);
}

void OgreMaxUtilities::SetCustomParameters(Ogre::Entity *entity, const std::vector<CustomParameter>& customParameters) {
    for(unsigned int subentityIndex = 0; subentityIndex < entity->getNumSubEntities(); subentityIndex++)
        SetCustomParameters(entity->getSubEntity(subentityIndex), customParameters);
}

void OgreMaxUtilities::SetIdentityInitialState(Ogre::SceneNode *node) {
    //Get the current state
    Ogre::Vector3 position = node->getPosition();
    Ogre::Quaternion orientation = node->getOrientation();
    Ogre::Vector3 scale = node->getScale();

    //Set the initial state to be at identity
    node->setPosition(Ogre::Vector3::ZERO);
    node->setOrientation(Ogre::Quaternion::IDENTITY);
    node->setScale(Ogre::Vector3::UNIT_SCALE);
    node->setInitialState();

    //Set the current state so the node is in the correct position if the node has
    //animations that are initially disabled
    node->setPosition(position);
    node->setOrientation(orientation);
    node->setScale(scale);
}

void OgreMaxUtilities::CreateMovablePlaneName(Ogre::String &createdName, const Ogre::String &baseName) {
    createdName = baseName;
    createdName += "_movable";
}

Ogre::Entity *OgreMaxUtilities::CreateEntity
(
    Ogre::SceneManager *sceneManager,
    const Ogre::String &entityName,
    const Ogre::String &meshFile,
    std::vector<EntityParameters::Subentity>& subentities
) {
    Ogre::Entity *entity = sceneManager->createEntity(entityName, meshFile);

    //Set subentity materials
    size_t subentityCount = std::min(subentities.size(), (size_t)entity->getNumSubEntities());
    for(size_t subentityIndex = 0; subentityIndex < subentityCount; subentityIndex++) {
        Ogre::SubEntity *subentity = entity->getSubEntity((unsigned int)subentityIndex);
        subentity->setMaterialName(subentities[subentityIndex].materialName);
    }

    return entity;
}

void OgreMaxUtilities::SetSubentities(Ogre::Entity *entity, const std::vector<EntityParameters::Subentity>& subentities) {
    size_t subentityCount = std::min(subentities.size(), (size_t)entity->getNumSubEntities());
    for(size_t subentityIndex = 0; subentityIndex < subentityCount; subentityIndex++) {
        Ogre::SubEntity *subentity = entity->getSubEntity((unsigned int)subentityIndex);
        if(!subentities[subentityIndex].materialName.empty())
            subentity->setMaterialName(subentities[subentityIndex].materialName);
    }
}

void OgreMaxUtilities::SetManualLods(Ogre::Entity *entity, const std::vector<EntityParameters::ManualLod>& manualLods) {
    size_t manualLodCount = std::min(manualLods.size(), (size_t)entity->getNumManualLodLevels());
    for(size_t manualLodIndex = 0; manualLodIndex < manualLodCount; manualLodIndex++) {
        Ogre::Entity *manualLod = entity->getManualLodLevel(manualLodIndex);
        SetSubentities(manualLod, manualLods[manualLodIndex].subentities);
    }
}

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
#include <CoreFoundation/CoreFoundation.h>

// This function will locate the path to our application on OS X,
// unlike windows you can not rely on the curent working directory
// for locating your configuration files and resources.
Ogre::String OgreMaxUtilities::GetMacBundlePath() {
    char path[1024];
    CFBundleRef mainBundle = CFBundleGetMainBundle();
    assert(mainBundle);

    CFURLRef mainBundleURL = CFBundleCopyBundleURL(mainBundle);
    assert(mainBundleURL);

    CFStringRef cfStringRef = CFURLCopyFileSystemPath(mainBundleURL, kCFURLPOSIXPathStyle);
    assert(cfStringRef);

    CFStringGetCString(cfStringRef, path, 1024, kCFStringEncodingASCII);

    CFRelease(mainBundleURL);
    CFRelease(cfStringRef);

    return Ogre::String(path);
}

#endif

Ogre::String OgreMaxUtilities::GetApplicationResourcesPath() {
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
    return GetMacBundlePath() + "/Contents/Resources/";
#else
    return GetApplicationDirectory();
#endif
}

Ogre::String OgreMaxUtilities::GetApplicationDirectory() {
    Ogre::String path;

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    const DWORD maxLength = MAX_PATH + 1;
    char pathChars[maxLength];
    GetModuleFileNameA(GetModuleHandle(0), pathChars, maxLength);
    Ogre::String baseName;
    Ogre::StringUtil::splitFilename(pathChars, baseName, path);
#else
    const unsigned int maxLength = PATH_MAX + 1;
    char pathChars[maxLength];
    getcwd(pathChars, maxLength);
    path = pathChars;
#endif

    EnsureTrailingPathSeparator(path);

    return path;
}

bool OgreMaxUtilities::IsSeparator(char c) {
    return c == '\\' || c == '/';
}

void OgreMaxUtilities::EnsureTrailingPathSeparator(Ogre::String &path) {
    if(path.length() > 0) {
        char lastChar = path[path.size() - 1];
        if(!IsSeparator(lastChar))
            path += "/";
    }
}

void OgreMaxUtilities::MakeFullPath(Ogre::String &path) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    DWORD maxLength = MAX_PATH + 1;

    std::vector<char> fullPath;
    fullPath.resize(maxLength);

    char *filePath;
    DWORD result = GetFullPathNameA(path.c_str(), maxLength, &fullPath[0], &filePath);
    if(result > maxLength) {
        fullPath.resize(result);
        result = GetFullPathNameA(path.c_str(), result, &fullPath[0], &filePath);
    }

    path = &fullPath[0];
#endif
}

bool OgreMaxUtilities::IsAbsolutePath(const Ogre::String &path) {
    if(path.empty())
        return false;
    else
        if(path.length() > 1) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
            if(path.length() == 1)
                return IsSeparator(path[0]);
            else
                return path[1] == ':' || (IsSeparator(path[0]) && IsSeparator(path[1]));
#else
            return IsSeparator(path[0]);
#endif
        } else
            return false;
}

Ogre::String OgreMaxUtilities::JoinPath(const Ogre::String &path1, const Ogre::String &path2) {
    //If path2 is an absolute path, it supercedes path1
    if(IsAbsolutePath(path2))
        return path2;

    //Path2 is not absolute
    Ogre::String joined(path1);
    EnsureTrailingPathSeparator(joined);

    //Skip leading separators in path2
    size_t charIndex = 0;
    while(charIndex < path2.length() && IsSeparator(path2[charIndex]))
        charIndex++;

    //If not at the end of path2, append it
    if(charIndex < path2.length())
        joined += path2.substr(charIndex, path2.length() - charIndex);

    //Remove relative components
    MakeFullPath(joined);

    return joined;
}

bool OgreMaxUtilities::IsFileReadable(const Ogre::String &path) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    return GetFileAttributesA(path.c_str()) != INVALID_FILE_ATTRIBUTES;
#else
    return access(path.c_str(), R_OK) == 0;
#endif
}

void OgreMaxUtilities::RemoveFile(const Ogre::String &path) {
    remove(path.c_str());
}

bool OgreMaxUtilities::SetDefaultLighting(Ogre::SceneManager *sceneManager, UpAxis upAxis) {
    static const Ogre::String defaultLightName("__defaultDirectionalLight");

    bool defaultLightingNeeded = !sceneManager->hasLight(defaultLightName);
    if(defaultLightingNeeded) {
        //Create a directional light
        Ogre::Light *light = sceneManager->createLight(defaultLightName);
        light->setType(Ogre::Light::LT_DIRECTIONAL);
        light->setDiffuseColour(Ogre::ColourValue((Ogre::Real).9, (Ogre::Real).9, (Ogre::Real).9));
        light->setSpecularColour(Ogre::ColourValue((Ogre::Real).2, (Ogre::Real).2, (Ogre::Real).2));

        //When the viewer faces down the forward axis, the light is angled to the lower right of the view
        Ogre::Vector3 upDirection = (upAxis == UP_AXIS_Y) ? Ogre::Vector3::UNIT_Y : Ogre::Vector3::UNIT_Z;
        Ogre::Vector3 position = upDirection + Ogre::Vector3::NEGATIVE_UNIT_X;
        light->setPosition(position);
        light->setDirection(-position);

        //Set the ambient light if necessary
        if(sceneManager->getAmbientLight() == Ogre::ColourValue::Black)
            sceneManager->setAmbientLight(Ogre::ColourValue((Ogre::Real).2, (Ogre::Real).2, (Ogre::Real).2));
        else
            if(sceneManager->getAmbientLight() == Ogre::ColourValue::White)
                sceneManager->setAmbientLight(Ogre::ColourValue((Ogre::Real).7, (Ogre::Real).7, (Ogre::Real).7));
    }

    return defaultLightingNeeded;
}

bool OgreMaxUtilities::IsInternalResourceGroup(const Ogre::String &resourceGroupName) {
    return
#if OGRE_VERSION_MAJOR == 1 && OGRE_VERSION_MINOR < 7
        resourceGroupName == Ogre::ResourceGroupManager::BOOTSTRAP_RESOURCE_GROUP_NAME ||
#endif
        resourceGroupName == Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME ||
        resourceGroupName == Ogre::ResourceGroupManager::INTERNAL_RESOURCE_GROUP_NAME;
}

void OgreMaxUtilities::ResetResourceGroups(const Ogre::String &createResourceGroup) {
    Ogre::StringVector resourceGroups = Ogre::ResourceGroupManager::getSingleton().getResourceGroups();
    for(size_t index = 0; index < resourceGroups.size(); index++) {
        if(!IsInternalResourceGroup(resourceGroups[index]))
            Ogre::ResourceGroupManager::getSingleton().destroyResourceGroup(resourceGroups[index]);
    }

    //Create the default resource group if necessary
    if(!createResourceGroup.empty())
        Ogre::ResourceGroupManager::getSingleton().createResourceGroup(createResourceGroup);
}

bool OgreMaxUtilities::EndsWithNoCase(const Ogre::String &text, const Ogre::String &endsWith) {
    bool result = false;

    size_t textLength = text.length();
    size_t endsWithLength = endsWith.length();
    if(endsWithLength <= textLength) {
        //Get the end string, lowercase
        Ogre::String end = text.substr(textLength - endsWithLength);
        Ogre::StringUtil::toLowerCase(end);

        //Get the 'ends with' string, lowercase
        Ogre::String endsWithLower = endsWith;
        Ogre::StringUtil::toLowerCase(endsWithLower);

        result = end == endsWithLower;
    }

    return result;
}

const Ogre::Quaternion &OgreMaxUtilities::GetOrientation(UpAxis upAxis) {
    static const Ogre::Quaternion Y_ORIENTATION = Ogre::Quaternion::IDENTITY;
    static const Ogre::Quaternion Z_ORIENTATION((Ogre::Real)0.707107, (Ogre::Real)0.707107, 0, 0);
    return upAxis == UP_AXIS_Y ? Y_ORIENTATION : Z_ORIENTATION;
}

bool OgreMaxUtilities::ImageCodecCanCode(const Ogre::String &name) {
    if(name == "dds" ||
            name == "cut" ||
            name == "g3" ||
            name == "gif" ||
            name == "hdr" ||
            name == "ico" ||
            name == "iff" ||
            name == "jng" ||
            name == "koa" ||
            name == "lbm" ||
            name == "mng" ||
            name == "pcd" ||
            name == "pcx" ||
            name == "psd" ||
            name == "ras" ||
            name == "sgi" ||
            name == "wap" ||
            name == "wbm" ||
            name == "wbmp" ||
            name == "xbm") {
        return false;
    } else
        return true;
}
