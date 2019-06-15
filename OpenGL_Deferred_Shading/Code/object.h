#ifndef OBJECT_H
#define OBJECT_H

#include "mesh.h"

#include <QMatrix4x4>

class Object
{

public:
    Object(QString filename);
    Object(Mesh *meshPointer);
    void setTranslation(QVector3D translation);
    void setTranslation(float x, float y, float z);
    void setScale(float scale);
    void setRotation(QVector3D rotation);
    void setTexture(GLuint *texturePointer);
    void setMaterialAmbient(QVector3D ambient);
    void setMaterialEmission(float emission);

    QMatrix4x4 getTransform();
    QMatrix3x3 getNormalTransform();
    GLuint* getTexture();
    QVector3D getMaterialAmbient();
    float getMaterialEmission();
    float getScale();
    bool hasTexture();

    void draw();
    void destroy();
    ~Object();
private:
    Mesh *mesh;
    QMatrix4x4 meshTransform;
    QMatrix3x3 meshNormalTransform;
    GLuint *texture;
    float scale;
    QVector3D materialAmbient = QVector3D(0, 0, 0);
    float materialEmission = 0.0; // emit light
    bool hasTextureBool = false;
};

#endif // OBJECT_H
