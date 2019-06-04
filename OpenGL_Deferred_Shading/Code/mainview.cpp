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

    qDebug() << "initializeGL :: createShaderProgram";
    createShaderProgram();
    qDebug() << "initializeGL :: loadMesh";
    loadMesh();
    qDebug() << "initializeGL :: loadTextures";
    loadTextures();
    qDebug() << "initializeGL :: createBuffers";
    createBuffers(800, 600);

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
    uniformFPosition      = phongShaderProgram.uniformLocation("fPosition");
    uniformFNormal      = phongShaderProgram.uniformLocation("fNormal");
    uniformFColor      = phongShaderProgram.uniformLocation("fColour");
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
    qDebug() << "MainView::loadTexture() texturePtr =" << texturePtr;
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
void MainView::createBuffers(int windowWidth, int windowHeight)
{
    qDebug() << "MainView::createBuffers()";

    // Generate Frame Buffer Object
    qDebug() << "glGenFramebuffers FBO...";
    glGenFramebuffers(1, &fbo);
    qDebug() << "glBind FBO...";
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);

    // Generate gBuffers
    /* color gBuffer */
    qDebug() << "createBuffer(colorTexture)...";
    glGenTextures(1, &colorTexture);
    qDebug() << "createBuffer(colorTexture) colorTexture =" << colorTexture;
    glBindTexture(GL_TEXTURE_2D, colorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, windowWidth, windowHeight,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    // disable linear interpolation
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // Associate gBuffers with FBO on GL_DRAW_FRAMEBUFFER target.
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);

    /* normals gBuffer */
    qDebug() << "createBuffer(normalsTexture)...";
    glGenTextures(1, &normalsTexture);
    qDebug() << "createBuffer(normalsTexture) normalsTexture =" << normalsTexture;
    glBindTexture(GL_TEXTURE_2D, normalsTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, windowWidth, windowHeight,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    // disable linear interpolation
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // Associate gBuffers with FBO on GL_DRAW_FRAMEBUFFER target.
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normalsTexture, 0);

    /* depth gBuffer */
    qDebug() << "createBuffer(zBufferTexture)...";
    glGenTextures(1, &zBufferTexture);
    qDebug() << "createBuffer(zBufferTexture) zBufferTexture =" << zBufferTexture;
    glBindTexture(GL_TEXTURE_2D, zBufferTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, windowWidth, windowHeight,
                 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    // disable linear interpolation
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // Associate gBuffers with FBO on GL_DRAW_FRAMEBUFFER target.
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, zBufferTexture, 0);

    // Specify which color attachments to draw to
    qDebug() << "glDrawBuffers()...";
    GLenum drawBuffers[2] = { GL_COLOR_ATTACHMENT0 , GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, drawBuffers);

    // debug
    fb_status("createBuffers func FBO");

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    fb_status("createBuffers func FBO-0");
}

void MainView::initializeTextures()
{
    // @FIXME update on resize.
}

//void MainView::createBuffer(GLuint locTexture)
//{
//    // Generate gBuffer
//    glGenTextures(1, &locTexture);
//    glBindTexture(GL_TEXTURE_2D, locTexture);
//    // disable linear interpolation
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//}

// --- OpenGL drawing

/**
 * @brief MainView::paintGL
 *
 * Actual function used for drawing to the screen
 *
 */
void MainView::paintGL() {
//    qDebug() << "MainView::paintGL()";
    // Clear the screen before rendering
//    glClearColor(0.2f, 0.5f, 0.7f, 0.0f);
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
////    // bind to framebuffer and draw scene as we normally would to color texture
//    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
//    fb_status("paintGL func");

    // store current (default) framebuffer id, cause it's not always 0.
    GLint drawFboId = 0, readFboId = 0;
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &drawFboId);
    glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &readFboId);
//    qDebug() << "MainView::paintGL() draw buff =" << drawFboId << ", read buff =" << readFboId;

//    // ... some stuff
//    glActiveTexture(GL_TEXTURE1);
//    glBindTexture(GL_TEXTURE_2D, colorTexture);


    // Clear the screen before rendering
    glClearColor(0.2f, 0.5f, 0.7f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);

        // render geometry in 1st pass
        QOpenGLShaderProgram *shaderProgram;
        shaderProgram = &deferredShaderProgram;
        shaderProgram->bind();
        updateDeferredUniforms();

        // Set the texture and draw the mesh.
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_2D, texturePtr);

        glBindVertexArray(meshVAO);
        glDrawArrays(GL_TRIANGLES, 0, meshSize);

        shaderProgram->release();

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, drawFboId);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shaderProgram = &phongShaderProgram;
        shaderProgram->bind();


        // starting from GL_TEXTURE1 cause the cat texture has GL_TEXTURE0.
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, colorTexture);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, normalsTexture);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, zBufferTexture);

        updatePhongUniforms();

        renderQuad();

        shaderProgram->release();


//        // 2.5. copy content of geometry's depth buffer to default framebuffer's depth buffer
//        // ----------------------------------------------------------------------------------
//        glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
//        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
//        // blit to default framebuffer. Note that this may or may not work as the internal formats of both the FBO and default framebuffer have to match.
//        // the internal formats are implementation defined. This works on all of my systems, but if it doesn't on yours you'll likely have to write to the
//        // depth buffer in another shader stage (or somehow see to match the default framebuffer's internal format with the FBO's internal format).
//        glBlitFramebuffer(0, 0, windowWidth, windowHeight, 0, 0, windowWidth, windowHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
//        glBindFramebuffer(GL_FRAMEBUFFER, 0);
     // render finished.
}

// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------
unsigned int quadVAO = 0;
unsigned int quadVBO;
void MainView::renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
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
    qDebug() << "MainView::resizeGL(" << newWidth << ", " << newHeight << ")";
    windowWidth = newWidth;
    windowHeight = newHeight;
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

    glUniform1i(uniformTextureSamplerDeferred, 0); // Redundant now, but useful when you have multiple textures.
}

void MainView::updatePhongUniforms()
{
    glUniform1i(uniformFPosition, 1);
    glUniform1i(uniformFNormal, 2);
    glUniform1i(uniformFColor, 3);

//    glUniformMatrix4fv(uniformProjectionTransformPhong, 1, GL_FALSE, projectionTransform.data());
    glUniformMatrix4fv(uniformModelViewTransformPhong, 1, GL_FALSE, meshTransform.data());
//    glUniformMatrix3fv(uniformNormalTransformPhong, 1, GL_FALSE, meshNormalTransform.data());

    glUniform4fv(uniformMaterialPhong, 1, &material[0]);
    glUniform3fv(uniformLightPositionPhong, 1, &lightPosition[0]);
    glUniform3f(uniformLightColourPhong, lightColour.x(), lightColour.y(), lightColour.z());

//    glUniform1i(uniformTextureSamplerPhong, 0);
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

    // deleting gBuffers
    glDeleteTextures(1, &colorTexture);
    glDeleteTextures(1, &normalsTexture);
    glDeleteTextures(1, &zBufferTexture);

    // deleting fbo
    glDeleteFramebuffers(1, &fbo);
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

/**
 * @brief MainView::fb_status
 *
 * Helper function for checking the framebuffer status.
 *
 * @see https://gist.github.com/wrl/5113212
 *
 * @param where Describe where the call came from.
 */
void MainView::fb_status(const char *where)
{
    switch (glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER)) {
    case GL_FRAMEBUFFER_COMPLETE:
        qDebug() << " :: GL_FRAMEBUFFER_COMPLETE in " << where;
        break;

    case GL_FRAMEBUFFER_UNDEFINED:
        qDebug() << " :: GL_FRAMEBUFFER_UNDEFINED in " << where;
        break;

    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
        qDebug() << " :: GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT in " << where;
        break;

    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
        qDebug() << " :: GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT in " << where;
        break;

    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
        qDebug() << " :: GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER in " << where;
        break;

    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
        qDebug() << " :: GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER in " << where;
        break;

    case GL_FRAMEBUFFER_UNSUPPORTED:
        qDebug() << " :: GL_FRAMEBUFFER_UNSUPPORTED in " << where;
        break;

    case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
        qDebug() << " :: GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE in " << where;
        break;

    case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
        qDebug() << " :: GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS in " << where;
        break;
    }
}
