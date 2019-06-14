#ifndef LIGHTPOINT_H
#define LIGHTPOINT_H

#include "object.h"

#include <QVector3D>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions_3_3_Core>

class LightPoint : protected QOpenGLFunctions_3_3_Core
{
public:
    LightPoint(QVector3D position);
    LightPoint(QVector3D position, float radius);
    LightPoint(QVector3D position, QVector3D color);
    LightPoint(QVector3D position, QVector3D color, float radius);

    QVector3D getRandomColor();
    void init(QVector3D position, QVector3D color, float radius);

    QVector3D getColor();
    QVector3D getPosition();
    float getRadius();
    void setPosition(QVector3D position);
    void setBulb(Object *bulb);
    Object* getBulb();
    void setAnimationCoefs(QVector3D animationCoefs);
    QVector3D getAnimationCoefs();
    void draw(QOpenGLShaderProgram *shaderProgram, GLuint sphereIndexCount);
private:
    QVector3D color;
    QVector3D position;
    float radius;
    Object *bulb;
    QVector3D animationCoefs = QVector3D(0.0f, 0.03f, 0.0f);
};

#endif // LIGHTPOINT_H
