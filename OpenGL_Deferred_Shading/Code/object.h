#ifndef OBJECT_H
#define OBJECT_H

#include "mesh.h"

#include <QMatrix4x4>

class Object
{

public:
    Object(QString filename);
    Object(Mesh *meshPointer);
    void setTranslation(float x, float y, float z);
    void setScale(float scale);
    void setRotation(QVector3D rotation);
    void setTexture(GLuint *texturePointer);

    QMatrix4x4 getTransform();
    GLuint* getTexture();

    void bindTexture();
    void draw();
    void destroy();
    ~Object();
private:
    Mesh *mesh;
    QMatrix4x4 meshTransform;
    GLuint *texture;
};

#endif // OBJECT_H
