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
                         gouraudShaderProgram,
                         phongShaderProgram,
                         waveShaderProgram;

    // Uniforms for the normal shader.
    GLint uniformModelViewTransformNormal;
    GLint uniformProjectionTransformNormal;
    GLint uniformNormalTransformNormal;

    // Uniforms for the gouraud shader.
    GLint uniformModelViewTransformGouraud;
    GLint uniformProjectionTransformGouraud;
    GLint uniformNormalTransformGouraud;

    GLint uniformMaterialGouraud;
    GLint uniformLightPositionGouraud;
    GLint uniformLightColourGouraud;

    GLint uniformTextureSamplerGouraud;

    // Uniforms for the phong shader.
    GLint uniformModelViewTransformPhong;
    GLint uniformProjectionTransformPhong;
    GLint uniformNormalTransformPhong;

    GLint uniformMaterialPhong;
    GLint uniformLightPositionPhong;
    GLint uniformLightColourPhong;

    GLint uniformTextureSamplerPhong;

    // Uniforms for the wave shader.
    GLint uniformModelViewTransformWave;
    GLint uniformProjectionTransformWave;
    GLint uniformNormalTransformWave;

    GLint uniformMaterialWave;
    GLint uniformLightPositionWave;
    GLint uniformLightColourWave;
    // ... extra wave properties
    GLint uniformAmountWave;
    GLint uniformSpeedWave;
    GLint uniformAmplitudeWave;
    GLint uniformPhaseWave;
    GLint uniformFrequencyWave;
    GLint uniformColorWave;

    // Buffers
    GLuint meshVAO;
    GLuint meshVBO;
    GLuint meshSize;

    // Texture
    GLuint texturePtr;

    // Transforms
    float scale = 1.5f;
    // set default rotation such that wave is nicely visible
    QVector3D rotation = { -45, 0, 0};
    QMatrix4x4 projectionTransform;
    QMatrix3x3 meshNormalTransform;
    QMatrix4x4 meshTransform;

    // Phong model constants.
    QVector4D material      = {0.5, 0.5, 1, 5};
    QVector3D lightPosition = {1, 100, 1};
    QVector3D lightColour   = {1, 1, 1};

    // Wave animation
    int         waveAmount          = 8;
    float       waveSpeed           = 0;
    GLfloat     waveAmplitude[8]    = { 0.05f, 0.03f, 0.02f, 0.04f, 0.01f, 0.03f, 0.00f, 0.07f  };
    GLfloat     waveFrequency[8]    = { 2.0f, 6.0f, 4.0f, 5.0f, 2.0f, 4.0f, 3.0f, 2.0f  };
    GLfloat     wavePhase[8]        = { 1.0f, 0.1f, 0.4f, 0.0f, 0.4f, 0.1f, 1.0f, 0.4f  };
    QVector3D   waveColor           = {0.5, 0.5, 1};
    // specify wave own material instead of using `global` one.
    QVector4D   waveMaterial        = { 0.5f, 0.5f, 1.0f, 1.0f  };
public:
    enum ShadingMode : GLuint
    {
        PHONG = 0, NORMAL, GOURAUD
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

    void destroyModelBuffers();

    void updateProjectionTransform();
    void updateModelTransforms();

    void updateNormalUniforms();
    void updateGouraudUniforms();
    void updatePhongUniforms();
    void updateWaveUniforms();

    // Useful utility method to convert image to bytes.
    QVector<quint8> imageToBytes(QImage image);

    // The current shader to use.
    ShadingMode currentShader = PHONG;
};

#endif // MAINVIEW_H
