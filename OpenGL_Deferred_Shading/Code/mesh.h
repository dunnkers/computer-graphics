#ifndef MESH_H
#define MESH_H

#include <QString>
#include <QOpenGLFunctions_3_3_Core>

class Mesh : protected QOpenGLFunctions_3_3_Core
{
public:
    Mesh(QString filename);

    void draw();
    void destroy();

    ~Mesh();
private:
    GLuint meshVAO;
    GLuint meshVBO;
    GLuint meshSize;
};

#endif // MESH_H
