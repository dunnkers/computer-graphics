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
    makeCurrent();
    debugLogger->stopLogging();
    delete debugLogger;

    qDebug() << "MainView destructor";
    destroyModelBuffers();
    doneCurrent();
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

    // Initialize transformations
    updateProjectionTransform();
    updateModelTransforms();
}

void MainView::createShaderProgram()
{
    // Shader program creation
    // Normal
    normalShaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,
                                           ":/shaders/vertshader_normal.glsl");
    normalShaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment,
                                           ":/shaders/fragshader_normal.glsl");
    normalShaderProgram.link();
    // Normal uniforms
    normalUniformModelViewTransform = normalShaderProgram.uniformLocation("modelViewTransform");
    normalUniformProjectionTransform = normalShaderProgram.uniformLocation("projectionTransform");
    normalUniformNormalTransform = normalShaderProgram.uniformLocation("normalTransform");


    // Gouraud
    gouraudShaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,
                                           ":/shaders/vertshader_gouraud.glsl");
    gouraudShaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment,
                                           ":/shaders/fragshader_gouraud.glsl");
    gouraudShaderProgram.link();
    // Gouraud uniforms
    gouraudUniformModelViewTransform = gouraudShaderProgram.uniformLocation("modelViewTransform");
    gouraudUniformProjectionTransform = gouraudShaderProgram.uniformLocation("projectionTransform");
    gouraudUniformNormalTransform = gouraudShaderProgram.uniformLocation("normalTransform");
    gouraudUniformLightPosition = gouraudShaderProgram.uniformLocation("lightPos");
    gouraudUniformMaterial = gouraudShaderProgram.uniformLocation("material");


    // Phong
    phongShaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,
                                           ":/shaders/vertshader_phong.glsl");
    phongShaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment,
                                           ":/shaders/fragshader_phong.glsl");
    phongShaderProgram.link();
    // Phong uniforms
    phongUniformModelViewTransform = phongShaderProgram.uniformLocation("modelViewTransform");
    phongUniformProjectionTransform = phongShaderProgram.uniformLocation("projectionTransform");
    phongUniformNormalTransform = phongShaderProgram.uniformLocation("normalTransform");
}

void MainView::loadMesh()
{
    // load model & get vertices and normals
    Model model(":/models/cat.obj");
    QVector<QVector3D> vertexCoords = model.getVertices();
    QVector<QVector3D> vertexNormals = model.getNormals();

    QVector<float> meshData;
    meshData.reserve(2 * 3 * vertexCoords.size());

    // should contain equal amount of elements
    assert(vertexCoords.size() == vertexNormals.size());

    for (int i = 0; i != vertexCoords.size(); ++i) {
        QVector3D vertex = vertexCoords.at(i);
        QVector3D normal = vertexNormals.at(i);
        meshData.append(vertex.x());
        meshData.append(vertex.y());
        meshData.append(vertex.z());
        meshData.append(normal.x());
        meshData.append(normal.y());
        meshData.append(normal.z());
    }

    meshSize = vertexCoords.size();

    // Generate VAO
    glGenVertexArrays(1, &meshVAO);
    glBindVertexArray(meshVAO);

    // Generate VBO
    glGenBuffers(1, &meshVBO);
    glBindBuffer(GL_ARRAY_BUFFER, meshVBO);

    // Write the data to the buffer
    glBufferData(GL_ARRAY_BUFFER, meshData.size() * sizeof(float), meshData.data(), GL_STATIC_DRAW);

    // Set vertex coordinates to location 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    // Set colour coordinates to location 1
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
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

    GLint uniformProjectionTransform;
    GLint uniformModelViewTransform;
    GLint uniformNormalTransform;

    QVector3D lightPos = QVector3D(1, 1, 1);
    QVector4D material = QVector4D(0.25f, 0.5f, 0.75f, 5.0f);

    switch (shadingMode) {
        case ShadingMode::PHONG:
            phongShaderProgram.bind();
            uniformProjectionTransform = phongUniformProjectionTransform;
            uniformModelViewTransform = phongUniformModelViewTransform;
            uniformNormalTransform = phongUniformNormalTransform;
            break;
        case ShadingMode::NORMAL:
            normalShaderProgram.bind();
            uniformProjectionTransform = normalUniformProjectionTransform;
            uniformModelViewTransform = normalUniformModelViewTransform;
            uniformNormalTransform = normalUniformNormalTransform;
            break;
        case ShadingMode::GOURAUD:
            gouraudShaderProgram.bind();
            uniformProjectionTransform = gouraudUniformProjectionTransform;
            uniformModelViewTransform = gouraudUniformModelViewTransform;
            uniformNormalTransform = gouraudUniformNormalTransform;
            glUniform3f(gouraudUniformLightPosition, lightPos.x(), lightPos.y(), lightPos.z());
            glUniform4f(gouraudUniformMaterial, material.x(), material.y(), material.z(), material.w());
            break;
    }

    QMatrix3x3 normalTransform = meshTransform.normalMatrix();

    // Set the projection matrix
    glUniformMatrix4fv(uniformProjectionTransform, 1, GL_FALSE, projectionTransform.data());
    glUniformMatrix4fv(uniformModelViewTransform, 1, GL_FALSE, meshTransform.data());
    glUniformMatrix3fv(uniformNormalTransform, 1, GL_FALSE, normalTransform.data());

    glBindVertexArray(meshVAO);
    glDrawArrays(GL_TRIANGLES, 0, meshSize);

    switch (shadingMode) {
        case ShadingMode::NORMAL:
            normalShaderProgram.release();
            break;
        case ShadingMode::GOURAUD:
            gouraudShaderProgram.release();
            break;
        case ShadingMode::PHONG:
            phongShaderProgram.release();
            break;
    }
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

void MainView::updateProjectionTransform()
{
    float aspect_ratio = static_cast<float>(width()) / static_cast<float>(height());
    projectionTransform.setToIdentity();
    projectionTransform.perspective(60, aspect_ratio, 0.2, 20);
}

void MainView::updateModelTransforms()
{
    meshTransform.setToIdentity();
    meshTransform.translate(0, 0, -10);
    meshTransform.scale(scale);
    meshTransform.rotate(QQuaternion::fromEulerAngles(rotation));

    update();
}

// --- OpenGL cleanup helpers

void MainView::destroyModelBuffers()
{
    glDeleteBuffers(1, &meshVBO);
    glDeleteVertexArrays(1, &meshVAO);
}

// --- Public interface

void MainView::setRotation(int rotateX, int rotateY, int rotateZ)
{
    rotation = { static_cast<float>(rotateX), static_cast<float>(rotateY), static_cast<float>(rotateZ) };
    updateModelTransforms();
}

void MainView::setScale(int newScale)
{
    scale = static_cast<float>(newScale) / 25.f;
    updateModelTransforms();
}

void MainView::setShadingMode(ShadingMode shading)
{
    qDebug() << "Changed shading to" << shading;
    shadingMode = shading;
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

