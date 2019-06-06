#ifndef OBJECT_H
#define OBJECT_H

#include "mesh.h"

#include <QMatrix4x4>

class Object
{

public:
    Object(QString filename);
    void setTranslation(float x, float y, float z);
    void setScale(float scale);
    void setRotation(QVector3D rotation);
    QMatrix4x4 getTransform();
    void draw();
    void destroy();
    ~Object();
private:
    Mesh *mesh;
    QMatrix4x4 meshTransform;
};

#endif // OBJECT_H
