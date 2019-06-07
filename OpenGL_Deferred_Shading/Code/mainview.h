#ifndef MAINVIEW_H
#define MAINVIEW_H

#include "model.h"
#include "mesh.h"
#include "framebufferobject.h"
#include "object.h"
#include "lightpoint.h"

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
    GLint geometryShaderUniform_mvpTransform;
    GLint geometryShaderUniform_textureDiff;

    // light shader - sun lights (directional)
    QOpenGLShaderProgram lightSunShaderProgram;

    // light shader - point lights
    QOpenGLShaderProgram lightPointShaderProgram;
    GLint lightPointShaderUniform_vpTransform;
    GLint lightPointShaderUniform_lightRad;
    GLint lightPointShaderUniform_lightPos;
    GLint lightPointShaderUniform_lightCol;
    // light sphere geometry:
    GLuint spherePositionVbo;
    GLuint sphereIndexVbo;
    GLuint sphereIndexCount;

    // Meshes
    Mesh *mesh_cat;
    Mesh *mesh_cube;
    Mesh *mesh_sphere;

    // Textures
    GLuint texturePtr;
    GLuint texture_rug;
    GLuint texture_earth;
    GLuint texture_jupiter;

    // Objects
    QVector<Object *> objects;

    // Transforms
    float scale = 1.f;
    QVector3D rotation;
    QMatrix4x4 projectionTransform;
    QMatrix4x4 viewMatrix;
    QVector3D cameraPosition;

    // FBO
    FramebufferObjectInstance *fbo;

    // Lights
    QVector<LightPoint *> lights;


public:
    enum CurrentTexture : GLuint
    {
        ALL = 0, COLOR = 1, NORMALS = 2, POSITION = 3, DEPTH = 4
    };

    MainView(QWidget *parent = nullptr);
    ~MainView();

    // Functions for widget input events
    void setRotation(int rotateX, int rotateY, int rotateZ);
    void setScale(int scale);
    void setCurrentTexture(CurrentTexture currentTexture);
    void toggleSun(bool enabled);
    void toggleLights(bool enabled);
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

    void createObjects();
    void createLights();
    void createLight(QVector3D position);
    void createSphere();

    void updateProjectionTransform();
    void updateModelTransforms();
    void updateViewMatrix();

    void updateCameraUniform(QOpenGLShaderProgram *shader);

    void destroyModelBuffers();

    // Useful utility method to convert image to bytes.
    QVector<quint8> imageToBytes(QImage image);

    // UI settings variables
    CurrentTexture currentTexture = ALL; // The current texture to display
    bool enableSun = true;
    bool enableLights = true;
};

#endif // MAINVIEW_H
