#ifndef MAINVIEW_H
#define MAINVIEW_H

#include "model.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLDebugLogger>
#include <QOpenGLShaderProgram>
#include <QTimer>
#include <QVector3D>
#include <QMatrix4x4>
#include <memory>


class MainView : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core {
    Q_OBJECT

    QOpenGLDebugLogger *debugLogger;
    QTimer timer; // timer used for animation

    // Normal shader
    QOpenGLShaderProgram normalShaderProgram;
    GLint normalUniformModelViewTransform;
    GLint normalUniformProjectionTransform;
    GLint normalUniformNormalTransform;

    // Gouraud shader
    QOpenGLShaderProgram gouraudShaderProgram;
    GLint gouraudUniformModelViewTransform;
    GLint gouraudUniformProjectionTransform;
    GLint gouraudUniformNormalTransform;
    GLint gouraudUniformLightPosition;
    GLint gouraudUniformMaterial;
    GLint gouraudUniformTextureColor;

    // Phong shader
    QOpenGLShaderProgram phongShaderProgram;
    GLint phongUniformModelViewTransform;
    GLint phongUniformProjectionTransform;
    GLint phongUniformNormalTransform;
    GLint phongUniformLightPosition;
    GLint phongUniformMaterial;
    GLint phongUniformTextureColor;

    // Mesh values
    GLuint meshVAO;
    GLuint meshVBO;
    GLuint meshSize;
    QMatrix4x4 meshTransform;

    // Transforms
    float scale = 1.f;
    QVector3D rotation;
    QMatrix4x4 projectionTransform;

    // Texture
    GLuint texturePtr;

public:
    enum ShadingMode : GLuint
    {
        PHONG = 0, NORMAL, GOURAUD
    };
    // Current shading mode
    ShadingMode shadingMode = ShadingMode::PHONG;

    MainView(QWidget *parent = 0);
    ~MainView();

    // Functions for widget input events
    void setRotation(int rotateX, int rotateY, int rotateZ);
    void setScale(int scale);
    void setShadingMode(ShadingMode shading);

    // Image reading utility function
    QVector<quint8> imageToBytes(QImage image);

protected:
    void initializeGL();
    void resizeGL(int newWidth, int newHeight);
    void paintGL();

    // Functions for keyboard input events
    void keyPressEvent(QKeyEvent *ev);
    void keyReleaseEvent(QKeyEvent *ev);

    // Function for mouse input events
    void mouseDoubleClickEvent(QMouseEvent *ev);
    void mouseMoveEvent(QMouseEvent *ev);
    void mousePressEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);
    void wheelEvent(QWheelEvent *ev);

private slots:
    void onMessageLogged( QOpenGLDebugMessage Message );

private:
    void createShaderProgram();
    void loadMesh();
    void createTextures();
    void loadTexture(QString file, GLuint *texturePtr);

    void destroyModelBuffers();

    void updateProjectionTransform();
    void updateModelTransforms();
    void updateNormalTransforms();
};

#endif // MAINVIEW_H
