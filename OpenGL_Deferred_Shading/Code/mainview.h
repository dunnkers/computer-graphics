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
#include <QImage>
#include <QVector>
#include <memory>
#include <QMatrix4x4>

class MainView : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core {
    Q_OBJECT

    QOpenGLDebugLogger *debugLogger;
    QTimer timer; // timer used for animation

    QOpenGLShaderProgram normalShaderProgram,
                         deferredShaderProgram,
                         phongShaderProgram;

    // Uniforms for the normal shader.
    GLint uniformModelViewTransformNormal;
    GLint uniformProjectionTransformNormal;
    GLint uniformNormalTransformNormal;

    // Uniforms for the deferred shader.
    GLint uniformModelViewTransformDeferred;
    GLint uniformProjectionTransformDeferred;
    GLint uniformNormalTransformDeferred;

//    GLint uniformMaterialDeferred;
//    GLint uniformLightPositionDeferred;
//    GLint uniformLightColourDeferred;

    GLint uniformTextureSamplerDeferred;

    // Uniforms for the phong shader.
    GLint uniformModelViewTransformPhong;
    GLint uniformProjectionTransformPhong;
    GLint uniformNormalTransformPhong;

    GLint uniformMaterialPhong;
    GLint uniformLightPositionPhong;
    GLint uniformLightColourPhong;

    GLint uniformTextureSamplerPhong;

    // Buffers
    GLuint meshVAO;
    GLuint meshVBO;
    GLuint meshSize;

    // Texture
    GLuint texturePtr;

    // Frame Buffer Object
    GLuint fbo;
    // gBuffers
    GLuint colorTexture;
    GLuint normalsTexture;
    GLuint zBufferTexture;
    GLuint colorBuffer;
    GLuint normalsBuffer;
    GLuint zBufferBuffer;

    // Transforms
    float scale = 1.f;
    QVector3D rotation;
    QMatrix4x4 projectionTransform;
    QMatrix3x3 meshNormalTransform;
    QMatrix4x4 meshTransform;

    // Phong model constants.
    QVector4D material = {0.5, 0.5, 1, 5};
    QVector3D lightPosition = {1, 100, 1};
    QVector3D lightColour = {1, 1, 1};

public:
    enum ShadingMode : GLuint
    {
        PHONG = 0, NORMAL, DEFERRED
    };

    MainView(QWidget *parent = 0);
    ~MainView();

    // Functions for widget input events
    void setRotation(int rotateX, int rotateY, int rotateZ);
    void setScale(int scale);
    void setShadingMode(ShadingMode shading);

protected:
    void initializeGL();
    void resizeGL(int newWidth, int newHeight);
    void paintGL();
    void fb_status(const char *where);

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

    // Loads texture data into the buffer of texturePtr.
    void loadTextures();
    void loadTexture(QString file, GLuint texturePtr);

    // Generate frame buffers
    void createBuffers();
    void createBuffer(GLuint locTexture);

    void destroyModelBuffers();

    void updateProjectionTransform();
    void updateModelTransforms();

    void updateNormalUniforms();
    void updateDeferredUniforms();
    void updatePhongUniforms();

    // Useful utility method to convert image to bytes.
    QVector<quint8> imageToBytes(QImage image);

    // The current shader to use.
    ShadingMode currentShader = PHONG;
};

#endif // MAINVIEW_H
