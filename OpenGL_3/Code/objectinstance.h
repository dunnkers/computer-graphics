#ifndef OBJECTINSTANCE_H
#define OBJECTINSTANCE_H

#include <QVector3D>
#include <QOpenGLFunctions_3_3_Core>

struct ObjectInstance {
    // Positioning and animation
    QVector3D position; // position in 3d space
    QVector3D rotation; // rotation speed in all 3 axis
    QVector3D speed;    // speed in all three directions

    // Mesh
    QString meshFile;
    GLuint meshVAO;
    GLuint meshVBO;
    GLuint meshSize;

    // Texture
    QString textureFile;
    GLuint texturePtr;

    ObjectInstance() : position(), rotation(), speed(){}
    ObjectInstance(QVector3D position, QVector3D rotation, QVector3D speed): position(position), rotation(rotation), speed(speed){}
    ObjectInstance(QString meshFile): meshFile(meshFile), textureFile(){}
    ObjectInstance(QString meshFile, QString textureFile): meshFile(meshFile), textureFile(textureFile){}
};

#endif // OBJECTINSTANCE_H
