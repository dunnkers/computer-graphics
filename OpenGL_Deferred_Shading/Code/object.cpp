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

void Object::bindTexture()
{

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
