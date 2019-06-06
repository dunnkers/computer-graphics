#include "object.h"

Object::Object(QString filename)
{
    mesh = new Mesh(filename);
    meshTransform.setToIdentity();
}

void Object::setTranslation(float x, float y, float z)
{
    meshTransform.translate(x, y, z);
}

void Object::setScale(float scale)
{
    meshTransform.scale(scale);
}

void Object::setRotation(QVector3D rotation)
{
    meshTransform.rotate(QQuaternion::fromEulerAngles(rotation));
}

QMatrix4x4 Object::getTransform()
{
    return meshTransform;
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
