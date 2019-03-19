#include "mainview.h"
#include "model.h"
#include "vertex.h"
#include "objectinstance.h"

#include <math.h>
#include <QDateTime>

/**
 * @brief MainView::MainView
 *
 * Constructor of MainView
 *
 * @param parent
 */
MainView::MainView(QWidget *parent) : QOpenGLWidget(parent) {
    qDebug() << "MainView constructor";

    connect(&timer, SIGNAL(timeout()), this, SLOT(update()));
}

/**
 * @brief MainView::~MainView
 *
 * Destructor of MainView
 * This is the last function called, before exit of the program
 * Use this to clean up your variables, buffers etc.
 *
 */
MainView::~MainView() {
    debugLogger->stopLogging();

    qDebug() << "MainView destructor";

    for (ObjectInstance &object : objects)
    {
        glDeleteTextures(1, &object.texturePtr);
    }

    destroyModelBuffers();
}

// --- OpenGL initialization

/**
 * @brief MainView::initializeGL
 *
 * Called upon OpenGL initialization
 * Attaches a debugger and calls other init functions
 */
void MainView::initializeGL() {
    qDebug() << ":: Initializing OpenGL";
    initializeOpenGLFunctions();

    debugLogger = new QOpenGLDebugLogger();
    connect( debugLogger, SIGNAL( messageLogged( QOpenGLDebugMessage ) ),
             this, SLOT( onMessageLogged( QOpenGLDebugMessage ) ), Qt::DirectConnection );

    if ( debugLogger->initialize() ) {
        qDebug() << ":: Logging initialized";
        debugLogger->startLogging( QOpenGLDebugLogger::SynchronousLogging );
        debugLogger->enableMessages();
    }

    QString glVersion;
    glVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    qDebug() << ":: Using OpenGL" << qPrintable(glVersion);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LEQUAL);
    glClearColor(0.0, 1.0, 0.0, 1.0);

    createShaderProgram();
    loadMeshes();
    loadTextures();

    // Initialize transformations
    updateProjectionTransform();
    updateModelTransforms();

    timer.start(1000.0 / 60.0);
    characterJumpTime.start(); // character jump time
}

void MainView::createShaderProgram()
{
    // Create Normal Shader program
    normalShaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,
                                           ":/shaders/vertshader_normal.glsl");
    normalShaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment,
                                           ":/shaders/fragshader_normal.glsl");
    normalShaderProgram.link();

    // Create Gouraud Shader program
    gouraudShaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,
                                           ":/shaders/vertshader_gouraud.glsl");
    gouraudShaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment,
                                           ":/shaders/fragshader_gouraud.glsl");
    gouraudShaderProgram.link();

    // Create Phong Shader program
    phongShaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,
                                           ":/shaders/vertshader_phong.glsl");
    phongShaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment,
                                           ":/shaders/fragshader_phong.glsl");
    phongShaderProgram.link();

    // Get the uniforms for the normal shader.
    uniformModelViewTransformNormal  = normalShaderProgram.uniformLocation("modelViewTransform");
    uniformProjectionTransformNormal = normalShaderProgram.uniformLocation("projectionTransform");
    uniformNormalTransformNormal     = normalShaderProgram.uniformLocation("normalTransform");

    // Get the uniforms for the gouraud shader.
    uniformModelViewTransformGouraud  = gouraudShaderProgram.uniformLocation("modelViewTransform");
    uniformProjectionTransformGouraud = gouraudShaderProgram.uniformLocation("projectionTransform");
    uniformNormalTransformGouraud     = gouraudShaderProgram.uniformLocation("normalTransform");
    uniformMaterialGouraud            = gouraudShaderProgram.uniformLocation("material");
    uniformLightPositionGouraud       = gouraudShaderProgram.uniformLocation("lightPosition");
    uniformLightColourGouraud         = gouraudShaderProgram.uniformLocation("lightColour");
    uniformTextureSamplerGouraud      = gouraudShaderProgram.uniformLocation("textureSampler");

    // Get the uniforms for the phong shader.
    uniformModelViewTransformPhong  = phongShaderProgram.uniformLocation("modelViewTransform");
    uniformProjectionTransformPhong = phongShaderProgram.uniformLocation("projectionTransform");
    uniformNormalTransformPhong     = phongShaderProgram.uniformLocation("normalTransform");
    uniformMaterialPhong            = phongShaderProgram.uniformLocation("material");
    uniformLightPositionPhong       = phongShaderProgram.uniformLocation("lightPosition");
    uniformLightColourPhong         = phongShaderProgram.uniformLocation("lightColour");
    uniformTextureSamplerPhong      = phongShaderProgram.uniformLocation("textureSampler");
}

void MainView::loadMeshes()
{
    ObjectInstance cat(":/models/cat.obj", ":/textures/cat_diff.png");
    cat.position = QVector3D(-2, 0, -8);
    cat.speed = QVector3D(0, 0, 0);
    loadMesh(&cat);
    objects.append(cat);
    characterIdx = 0;

    ObjectInstance cube(":/models/cube.obj", ":/textures/rug_logo.png");
    cube.position = QVector3D(5, 2, -8);
    cube.speed = QVector3D(-0.001, 0, 0);
    loadMesh(&cube);
    objects.append(cube);

    ObjectInstance sphere(":/models/sphere.obj", ":/textures/rug_logo.png");
    sphere.position = QVector3D(6, -2, -8);
    sphere.speed = QVector3D(-0.002, 0, 0);
    sphere.rotationalSpeed = QVector3D(0, 0.2, 0);
    loadMesh(&sphere);
    objects.append(sphere);

    ObjectInstance sphere2(":/models/sphere.obj", ":/textures/rug_logo.png");
    sphere2.position = QVector3D(4, -1, -8);
    sphere2.speed = QVector3D(-0.003, 0, 0);
    sphere2.rotation = QVector3D(0, 180, 0);
    loadMesh(&sphere2);
    objects.append(sphere2);
}

void MainView::loadMesh(ObjectInstance *object)
{
    Model model(object->meshFile);
    model.unitize();
    QVector<float> meshData = model.getVNTInterleaved();

    object->meshSize = model.getVertices().size();

    // Generate VAO
    glGenVertexArrays(1, &object->meshVAO);
    glBindVertexArray(object->meshVAO);

    // Generate VBO
    glGenBuffers(1, &object->meshVBO);
    glBindBuffer(GL_ARRAY_BUFFER, object->meshVBO);

    // Write the data to the buffer
    glBufferData(GL_ARRAY_BUFFER, meshData.size() * sizeof(float), meshData.data(), GL_STATIC_DRAW);

    // Set vertex coordinates to location 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    // Set vertex normals to location 1
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Set vertex texture coordinates to location 2
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void MainView::loadTextures()
{
    for (ObjectInstance &object : objects)
    {
        if (object.textureFile.isNull()) {
            continue;
        }

        glGenTextures(1, &object.texturePtr);
        loadTexture(object.textureFile, object.texturePtr);
    }
}

void MainView::loadTexture(QString file, GLuint texturePtr)
{
    // Set texture parameters.
    glBindTexture(GL_TEXTURE_2D, texturePtr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // Push image data to texture.
    QImage image(file);
    QVector<quint8> imageData = imageToBytes(image);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, image.width(), image.height(),
                 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData.data());
}

// --- OpenGL drawing

/**
 * @brief MainView::paintGL
 *
 * Actual function used for drawing to the screen
 *
 */
void MainView::paintGL() {
    // Clear the screen before rendering
    glClearColor(0.2f, 0.5f, 0.7f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // character jump
    float t = characterJumpTime.elapsed();
    float d = 0.5f * t * t;
    float y = (characterEnergy / 100) - d / 10000000;
    QVector3D jumpVector = QVector3D(0, y, 0);
    objects[characterIdx].speed = jumpVector;
    QVector3D rotationVector = QVector3D(0, 0, characterPitch);
    objects[characterIdx].rotation = rotationVector;
    if (characterPitch > 0) characterPitch -= 0.3f;

    // update object positions
    for (ObjectInstance &object : objects)
    {
        object.position += object.speed;
        object.rotation += object.rotationalSpeed;
    }
    updateModelTransforms();

    // Choose the selected shader.
    QOpenGLShaderProgram *shaderProgram;
    switch (currentShader) {
    case NORMAL:
        shaderProgram = &normalShaderProgram;
        shaderProgram->bind();
        updateNormalUniforms();
        break;
    case GOURAUD:
        shaderProgram = &gouraudShaderProgram;
        shaderProgram->bind();
        updateGouraudUniforms();
        break;
    case PHONG:
        shaderProgram = &phongShaderProgram;
        shaderProgram->bind();
        updatePhongUniforms();
        break;
    }

    // Set the texture and draw the mesh.
    glActiveTexture(GL_TEXTURE0);

    for (ObjectInstance &object : objects)
    {
        updateObjectUniforms(object);
        glBindTexture(GL_TEXTURE_2D, object.texturePtr);
        glBindVertexArray(object.meshVAO);
        glDrawArrays(GL_TRIANGLES, 0, object.meshSize);
    }

    shaderProgram->release();
}

/**
 * @brief MainView::resizeGL
 *
 * Called upon resizing of the screen
 *
 * @param newWidth
 * @param newHeight
 */
void MainView::resizeGL(int newWidth, int newHeight)
{
    Q_UNUSED(newWidth)
    Q_UNUSED(newHeight)

    updateProjectionTransform();
}

void MainView::updateNormalUniforms()
{
    glUniformMatrix4fv(uniformProjectionTransformNormal, 1, GL_FALSE, projectionTransform.data());
}

void MainView::updateGouraudUniforms()
{
    glUniformMatrix4fv(uniformProjectionTransformGouraud, 1, GL_FALSE, projectionTransform.data());

    glUniform4fv(uniformMaterialGouraud, 1, &material[0]);
    glUniform3fv(uniformLightPositionGouraud, 1, &lightPosition[0]);
    glUniform3fv(uniformLightColourGouraud, 1, &lightColour[0]);

    glUniform1i(uniformTextureSamplerGouraud, 0);
}

void MainView::updatePhongUniforms()
{
    glUniformMatrix4fv(uniformProjectionTransformPhong, 1, GL_FALSE, projectionTransform.data());

    glUniform4fv(uniformMaterialPhong, 1, &material[0]);
    glUniform3fv(uniformLightPositionPhong, 1, &lightPosition[0]);
    glUniform3f(uniformLightColourPhong, lightColour.x(), lightColour.y(), lightColour.z());

    glUniform1i(uniformTextureSamplerPhong, 0);
}

void MainView::updateObjectUniforms(ObjectInstance &object)
{
    switch (currentShader) {
    case NORMAL:
        glUniformMatrix4fv(uniformModelViewTransformNormal, 1, GL_FALSE, object.meshTransform.data());
        glUniformMatrix3fv(uniformNormalTransformNormal, 1, GL_FALSE, object.meshNormalTransform.data());
        break;
    case GOURAUD:
        glUniformMatrix4fv(uniformModelViewTransformGouraud, 1, GL_FALSE, object.meshTransform.data());
        glUniformMatrix3fv(uniformNormalTransformGouraud, 1, GL_FALSE, object.meshNormalTransform.data());
        break;
    case PHONG:
        glUniformMatrix4fv(uniformModelViewTransformPhong, 1, GL_FALSE, object.meshTransform.data());
        glUniformMatrix3fv(uniformNormalTransformPhong, 1, GL_FALSE, object.meshNormalTransform.data());
        break;
    }
}

void MainView::updateProjectionTransform()
{
    float aspect_ratio = static_cast<float>(width()) / static_cast<float>(height());
    projectionTransform.setToIdentity();
    projectionTransform.perspective(60, aspect_ratio, 0.2, 20);
}

void MainView::updateModelTransforms()
{
    for (ObjectInstance &object : objects)
    {
        object.meshTransform.setToIdentity();
        object.meshTransform.translate(object.position);
        object.meshTransform.scale(scale);
        object.meshTransform.rotate(QQuaternion::fromEulerAngles(object.rotation));
        object.meshNormalTransform = object.meshTransform.normalMatrix();
    }

    update();
}

// --- OpenGL cleanup helpers

void MainView::destroyModelBuffers()
{
    for (ObjectInstance &object : objects)
    {
        glDeleteBuffers(1, &object.meshVBO);
        glDeleteVertexArrays(1, &object.meshVAO);
    }
}

// --- Public interface

void MainView::setRotation(int rotateX, int rotateY, int rotateZ)
{
    rotation = { static_cast<float>(rotateX), static_cast<float>(rotateY), static_cast<float>(rotateZ) };
    updateModelTransforms();
}

void MainView::setScale(int newScale)
{
    scale = static_cast<float>(newScale) / 100.f;
    updateModelTransforms();
}

void MainView::setAnimationSpeed(int newAnimationSpeed)
{
    animationSpeed = static_cast<float>(newAnimationSpeed) / 100.f;
    updateModelTransforms();
}

void MainView::setShadingMode(ShadingMode shading)
{
    qDebug() << "Changed shading to" << shading;
    currentShader = shading;
}

// --- Private helpers

/**
 * @brief MainView::onMessageLogged
 *
 * OpenGL logging function, do not change
 *
 * @param Message
 */
void MainView::onMessageLogged( QOpenGLDebugMessage Message ) {
    qDebug() << " → Log:" << Message;
}

