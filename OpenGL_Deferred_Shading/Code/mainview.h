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
#include <QElapsedTimer>
#include <QVector3D>
#include <QImage>
#include <QVector>
#include <memory>
#include <QMatrix4x4>

class MainView : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core {
    Q_OBJECT

    QOpenGLDebugLogger *debugLogger;
    QTimer timer;

    // geometry shader
    QOpenGLShaderProgram geometryShaderProgram;
    GLint geometryShaderUniform_mvpTransform;
    GLint geometryShaderUniform_normalTransform;
    GLint geometryShaderUniform_textureDiff;

    // light shader - lights (directional & point lights)
    QOpenGLShaderProgram lightShaderProgram;

    // Meshes
    Mesh *mesh_cat;
    Mesh *mesh_cube;
    Mesh *mesh_sphere;

    // Textures
    GLuint texturePtr;
    GLuint texture_rug;
    GLuint texture_earth;
    GLuint texture_jupiter;
    GLuint texture_ground;

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
    QVector<Object *> lightBulbs;

    // Performance
    QElapsedTimer frameTimer; // used for computing performance
    int frameCount = 0; // fps
    bool runningAnalysis = false;
public:
    enum CurrentTexture : GLuint
    {
        ALL = 0, COLOR = 1, NORMALS = 2, POSITION = 3, DEPTH = 4
    };
    const QVector3D rotation_default = QVector3D(-10.0, 0, 0);

    MainView(QWidget *parent = nullptr);
    ~MainView();

    // Functions for widget input events
    void setRotation(int rotateX, int rotateY, int rotateZ);
    void setRotation(QVector3D rotation);
    void setRotation(float rotateX, float rotateY, float rotateZ);
    void setScale(int scale);
    void setCurrentTexture(CurrentTexture currentTexture);
    void toggleSun(bool enabled);
    void toggleLights(bool enabled);
    void toggleAnimation(bool enabled);
    void toggleEmission(bool enabled);
    void toggleAmbient(bool enabled);
    void toggleDiffuse(bool enabled);
    void toggleSpecular(bool enabled);
    void perfAnalysis(bool start);
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
    void loadTexture(QString file, GLuint texturePtr, bool repeat);

    void createObjects();
    void createJupiter(float x, float y, float z, float scale);
    void createEarth(float x, float y, float z, float scale);
    void createLight(QVector3D position);
    void createLight(LightPoint *light);

    void updateProjectionTransform();
    void updateModelTransforms();
    void updateViewMatrix();

    void updateCameraUniform(QOpenGLShaderProgram *shader);
    void updateSettingsUniforms(QOpenGLShaderProgram *shader);
    void updateShaderUniforms(QOpenGLShaderProgram *shader);

    void destroyModelBuffers();

    // Useful utility method to convert image to bytes.
    QVector<quint8> imageToBytes(QImage image);

    // UI settings variables
    CurrentTexture currentTexture = ALL; // The current texture to display
    bool enableSun = true;
    bool enableLights = true;
    bool animate = false;
    bool enableEmission = true;
    bool enableAmbient = true;
    bool enableDiffuse = true;
    bool enableSpecular = true;
};

#endif // MAINVIEW_H
