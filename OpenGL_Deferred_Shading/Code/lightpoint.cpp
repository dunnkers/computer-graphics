#include "lightpoint.h"

LightPoint::LightPoint(QVector3D position)
{
    init(position, getRandomColor(), 270.0f);
}

LightPoint::LightPoint(QVector3D position, QVector3D color)
{
    init(position, color, 270.0f);
}

LightPoint::LightPoint(QVector3D position, float radius)
{
    init(position, getRandomColor(), radius);
}

LightPoint::LightPoint(QVector3D position, QVector3D color, float radius)
{
    init(position, color, radius);
}

void LightPoint::init(QVector3D position, QVector3D color, float radius)
{
    this->position = position;
    this->color = color;
    this->radius = radius;

    initializeOpenGLFunctions();
}

QVector3D LightPoint::getRandomColor()
{
    float r = (static_cast<float>(rand()) / (RAND_MAX));
    float g = (static_cast<float>(rand()) / (RAND_MAX));
    float b = (static_cast<float>(rand()) / (RAND_MAX));
    return QVector3D(r, g, b);
}

QVector3D LightPoint::getColor()
{
    return color;
}

QVector3D LightPoint::getPosition()
{
    return position;
}

float LightPoint::getRadius()
{
    return radius;
}

void LightPoint::draw(QOpenGLShaderProgram *shaderProgram, GLuint sphereIndexCount)
{
    glUniform1f(shaderProgram->uniformLocation("lightRad"), radius);
    glUniform3f(shaderProgram->uniformLocation("lightPos"), position.x(), position.y(), position.z());
    glUniform3f(shaderProgram->uniformLocation("lightCol"), color.x(), color.y(), color.z());
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(sphereIndexCount), GL_UNSIGNED_INT, nullptr);
}
