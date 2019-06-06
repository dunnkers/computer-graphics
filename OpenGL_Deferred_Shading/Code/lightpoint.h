#ifndef LIGHTPOINT_H
#define LIGHTPOINT_H

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
    void draw(QOpenGLShaderProgram *shaderProgram, GLuint sphereIndexCount);
private:
    QVector3D color;
    QVector3D position;
    float radius;
};

#endif // LIGHTPOINT_H
