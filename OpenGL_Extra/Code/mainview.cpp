#include "mainview.h"
#include "model.h"
#include "vertex.h"

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

    glDeleteTextures(1, &texturePtr);

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
    loadMesh();
    loadTextures();
    createBuffers();

    // Initialize transformations
    updateProjectionTransform();
    updateModelTransforms();
}

void MainView::createShaderProgram()
{
    // Create Normal Shader program
    normalShaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,
                                           ":/shaders/vertshader_normal.glsl");
    normalShaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment,
                                           ":/shaders/fragshader_normal.glsl");
    normalShaderProgram.link();

    // Create Deferred Shader program
    deferredShaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,
                                           ":/shaders/vertshader_deferred.glsl");
    deferredShaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment,
                                           ":/shaders/fragshader_deferred.glsl");
    deferredShaderProgram.link();

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

    // Get the uniforms for the deferred shader.
    uniformModelViewTransformDeferred  = deferredShaderProgram.uniformLocation("modelViewTransform");
    uniformProjectionTransformDeferred = deferredShaderProgram.uniformLocation("projectionTransform");
    uniformNormalTransformDeferred     = deferredShaderProgram.uniformLocation("normalTransform");
//    uniformMaterialDeferred            = deferredShaderProgram.uniformLocation("material");
//    uniformLightPositionDeferred       = deferredShaderProgram.uniformLocation("lightPosition");
//    uniformLightColourDeferred         = deferredShaderProgram.uniformLocation("lightColour");
    uniformTextureSamplerDeferred      = deferredShaderProgram.uniformLocation("textureSampler");

    // Get the uniforms for the phong shader.
    uniformModelViewTransformPhong  = phongShaderProgram.uniformLocation("modelViewTransform");
    uniformProjectionTransformPhong = phongShaderProgram.uniformLocation("projectionTransform");
    uniformNormalTransformPhong     = phongShaderProgram.uniformLocation("normalTransform");
    uniformMaterialPhong            = phongShaderProgram.uniformLocation("material");
    uniformLightPositionPhong       = phongShaderProgram.uniformLocation("lightPosition");
    uniformLightColourPhong         = phongShaderProgram.uniformLocation("lightColour");
    uniformTextureSamplerPhong      = phongShaderProgram.uniformLocation("textureSampler");
}

void MainView::loadMesh()
{
    Model model(":/models/cat.obj");
    model.unitize();
    QVector<float> meshData = model.getVNTInterleaved();

    this->meshSize = model.getVertices().size();

    // Generate VAO
    glGenVertexArrays(1, &meshVAO);
    glBindVertexArray(meshVAO);

    // Generate VBO
    glGenBuffers(1, &meshVBO);
    glBindBuffer(GL_ARRAY_BUFFER, meshVBO);

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
    glGenTextures(1, &texturePtr);
    loadTexture(":/textures/cat_diff.png", texturePtr);
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

// Creating the framebuffer object.
void MainView::createBuffers()
{
    // @note shouldn't we glTextImage2D first, then bind? We bind first now.
    // [12:21] Color for Gouraud & Phong gets black with white stripes when we Init first, then bind.

    // Generate gBuffers
    createBuffer(colorTexture);
    createBuffer(normalsTexture);
    createBuffer(zBufferTexture);

    // Initialize textures
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 150, 150, // @FIXME should be window size. @see resizeGL
                 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 150, 150, // @FIXME should be window size. @see resizeGL
                 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 150, 150, // @FIXME should be window size. @see resizeGL
                 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    // Generate Frame Buffer Object
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);

    // Associate gBuffers with FBO on GL_DRAW_FRAMEBUFFER target.
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normalsTexture, 0);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, zBufferTexture, 0);

    // Specify which color attachments to draw to
    GLenum drawBuffers[2] = { GL_COLOR_ATTACHMENT0 , GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, drawBuffers);
}

void MainView::createBuffer(GLuint locTexture)
{
    // Generate gBuffer
    glGenTextures(1, &locTexture);
    glBindTexture(GL_TEXTURE_2D, locTexture);
    // disable linear interpolation
    glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_MAG_FILTER ,
        GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_MIN_FILTER ,
        GL_NEAREST);
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

    // Choose the selected shader.
    QOpenGLShaderProgram *shaderProgram;
    switch (currentShader) {
    case NORMAL:
        shaderProgram = &normalShaderProgram;
        shaderProgram->bind();
        updateNormalUniforms();
        break;
    case DEFERRED:
        shaderProgram = &deferredShaderProgram;
        shaderProgram->bind();
        updateDeferredUniforms();
        break;
    case PHONG:
        shaderProgram = &phongShaderProgram;
        shaderProgram->bind();
        updatePhongUniforms();
        break;
    }

    // Set the texture and draw the mesh.
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texturePtr);
//    glBindFramebuffer(GL_TEXTURE_2D, fbo);

    glBindVertexArray(meshVAO);
    glDrawArrays(GL_TRIANGLES, 0, meshSize);

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
    glUniformMatrix4fv(uniformModelViewTransformNormal, 1, GL_FALSE, meshTransform.data());
    glUniformMatrix3fv(uniformNormalTransformNormal, 1, GL_FALSE, meshNormalTransform.data());
}

void MainView::updateDeferredUniforms()
{
    glUniformMatrix4fv(uniformProjectionTransformDeferred, 1, GL_FALSE, projectionTransform.data());
    glUniformMatrix4fv(uniformModelViewTransformDeferred, 1, GL_FALSE, meshTransform.data());
    glUniformMatrix3fv(uniformNormalTransformDeferred, 1, GL_FALSE, meshNormalTransform.data());

//    glUniform4fv(uniformMaterialDeferred, 1, &material[0]);
//    glUniform3fv(uniformLightPositionDeferred, 1, &lightPosition[0]);
//    glUniform3fv(uniformLightColourDeferred, 1, &lightColour[0]);

    //glUniform1i(uniformTextureSamplerDeferred, 0); // Redundant now, but useful when you have multiple textures.
}

void MainView::updatePhongUniforms()
{
    glUniformMatrix4fv(uniformProjectionTransformPhong, 1, GL_FALSE, projectionTransform.data());
    glUniformMatrix4fv(uniformModelViewTransformPhong, 1, GL_FALSE, meshTransform.data());
    glUniformMatrix3fv(uniformNormalTransformPhong, 1, GL_FALSE, meshNormalTransform.data());

    glUniform4fv(uniformMaterialPhong, 1, &material[0]);
    glUniform3fv(uniformLightPositionPhong, 1, &lightPosition[0]);
    glUniform3f(uniformLightColourPhong, lightColour.x(), lightColour.y(), lightColour.z());

    glUniform1i(uniformTextureSamplerPhong, 0);
}

void MainView::updateProjectionTransform()
{
    float aspect_ratio = static_cast<float>(width()) / static_cast<float>(height());
    projectionTransform.setToIdentity();
    projectionTransform.perspective(60, aspect_ratio, 0.2, 20);
}

void MainView::updateModelTransforms()
{
    meshTransform.setToIdentity();
    meshTransform.translate(0, 0, -4);
    meshTransform.scale(scale);
    meshTransform.rotate(QQuaternion::fromEulerAngles(rotation));
    meshNormalTransform = meshTransform.normalMatrix();

    update();
}

// --- OpenGL cleanup helpers

void MainView::destroyModelBuffers()
{
    glDeleteBuffers(1, &meshVBO);
    glDeleteVertexArrays(1, &meshVAO);

    // deleting texture buffers
    glDeleteTextures(1, &texturePtr);

    // deleting fbo
    glDeleteFramebuffers(1, &fbo);

    // deleting gBuffers
    glDeleteTextures(1, &colorBuffer);
    glDeleteTextures(1, &normalsBuffer);
    glDeleteTextures(1, &zBufferBuffer);
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

