#ifndef MAINVIEW_H
#define MAINVIEW_H

#include "model.h"
#include "mesh.h"

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

    // geometry shader
    QOpenGLShaderProgram geometryShaderProgram;
    GLint geometryShaderUniform_uVp;
    // directional light shader
    QOpenGLShaderProgram directionalLightShaderProgram;
    GLint directionalLightShaderUniform_uColorTex;
    GLint directionalLightShaderUniform_uNormalTex;
    GLint directionalLightShaderUniform_uPositionTex;
    GLint directionalLightShaderUniform_uCameraPos;
    // point light shader
    QOpenGLShaderProgram pointLightShaderProgram;
    GLint pointLightShaderUniform_uVp;
    // light sphere geometry:
    GLuint spherePositionVbo;
    GLuint sphereIndexVbo;
    GLuint sphereIndexCount;
    // point light uniforms
    GLint pointLightShaderUniform_uLightRadius;
    GLint pointLightShaderUniform_uLightPosition;
    GLint pointLightShaderUniform_uLightColor;

    // Uniforms for the normal shader.
    // GLint uniformModelViewTransformNormal;
    // GLint uniformProjectionTransformNormal;
    // GLint uniformNormalTransformNormal;

    // Meshes
    QVector<Mesh *> meshes;

    // Texture
    GLuint texturePtr;

    // Transforms
    float scale = 1.f;
    QVector3D rotation;
    QMatrix4x4 projectionTransform;
    QMatrix3x3 meshNormalTransform;
    QMatrix4x4 meshTransform;

    // Window
    int windowWidth;
    int windowHeight;

    // Gbuffers.
    GLuint colorTexture;
    GLuint depthRenderbuffer;
    GLuint normalTexture;
    GLuint positionTexture;
    // FBO
    GLuint fbo;
    int fbWidth, fbHeight;
public:
    enum CurrentTexture : GLuint
    {
        COLOR = 0, NORMALS, DEPTH
    };

    MainView(QWidget *parent = nullptr);
    ~MainView();

    // Functions for widget input events
    void setRotation(int rotateX, int rotateY, int rotateZ);
    void setScale(int scale);
    void setCurrentTexture(CurrentTexture currentTexture);

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
    void loadMeshes();

    // Loads texture data into the buffer of texturePtr.
    void loadTextures();
    void loadTexture(QString file, GLuint texturePtr);

    void destroyModelBuffers();

    void updateProjectionTransform();
    void updateModelTransforms();
    // void updateNormalUniforms();

    // Useful utility method to convert image to bytes.
    QVector<quint8> imageToBytes(QImage image);

    // The current texture to display
    CurrentTexture currentTexture = COLOR;

    void setupFBO();
    void setupDeferredDirectionalLightShader();
    void setupDeferredPointLightShader();
    void createSphere();
    void renderPointLight(float radius, const QVector3D position, const QVector3D color);
};

#endif // MAINVIEW_H
