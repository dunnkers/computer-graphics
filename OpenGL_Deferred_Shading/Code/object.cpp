#include "object.h"

Object::Object(QString filename)
{
    mesh = new Mesh(filename);
    meshTransform.setToIdentity();
}

Object::Object(Mesh *meshPointer)
{
    mesh = meshPointer;
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

void Object::setTexture(GLuint *texturePointer)
{
    texture = texturePointer;
}

QMatrix4x4 Object::getTransform()
{
    return meshTransform;
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