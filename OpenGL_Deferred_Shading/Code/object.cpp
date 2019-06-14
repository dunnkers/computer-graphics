#include "object.h"

Object::Object(QString filename)
{
    mesh = new Mesh(filename);
    meshTransform.setToIdentity();
    meshNormalTransform.setToIdentity();
}

Object::Object(Mesh *meshPointer)
{
    mesh = meshPointer;
    meshTransform.setToIdentity();
    meshNormalTransform.setToIdentity();
}

void Object::setTranslation(QVector3D translation)
{
    this->setTranslation(translation.x(), translation.y(), translation.z());
}

void Object::setTranslation(float x, float y, float z)
{
    meshTransform.translate(x, y, z);
    meshNormalTransform = meshTransform.normalMatrix();
}

void Object::setScale(float scale)
{
    meshTransform.scale(scale);
    meshNormalTransform = meshTransform.normalMatrix();
}

void Object::setRotation(QVector3D rotation)
{
    meshTransform.rotate(QQuaternion::fromEulerAngles(rotation));
    meshNormalTransform = meshTransform.normalMatrix();
}

void Object::setTexture(GLuint *texturePointer)
{
    texture = texturePointer;
    hasTextureBool = true;
}

void Object::setMaterialAmbient(QVector3D ambient)
{
    materialAmbient = ambient;
}

QMatrix4x4 Object::getTransform()
{
    return meshTransform;
}

QMatrix3x3 Object::getNormalTransform()
{
    return meshNormalTransform;
}

GLuint* Object::getTexture()
{
    return texture;
}

QVector3D Object::getMaterialAmbient()
{
    return materialAmbient;
}

bool Object::hasTexture()
{
    return hasTextureBool;
}

void Object::draw()
{
    mesh->draw();
}

void Object::destroy()
{
    mesh->destroy();
}

Object::~Object()
{
    destroy();
}
