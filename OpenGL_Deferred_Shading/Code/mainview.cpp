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
    loadMeshes();
    loadTextures();

    createObjects();
    createSphere(); // create the light sphere geometry.

    fbo = new FramebufferObjectInstance(width(), height());

    // Initialize transformations
    updateProjectionTransform();
    updateViewMatrix();
}

void MainView::createShaderProgram()
{
    // Create Geometry Shader program
    geometryShaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,
                                           ":/shaders/vertshader_geometry.glsl");
    geometryShaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment,
                                           ":/shaders/fragshader_geometry.glsl");
    geometryShaderProgram.link();
    geometryShaderUniform_mvpTransform = geometryShaderProgram
            .uniformLocation("mvpTransform");
    geometryShaderUniform_textureDiff = geometryShaderProgram
            .uniformLocation("textureDiff");

    // Create Sun Light Shader program (directional lighting)
    lightSunShaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,
                                           ":/shaders/vertshader_light_sun.glsl");
    lightSunShaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment,
                                           ":/shaders/fragshader_light_sun.glsl");
    lightSunShaderProgram.link();

    // Create Point Light Shader Program
    lightPointShaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,
                                           ":/shaders/vertshader_light_point.glsl");
    lightPointShaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment,
                                           ":/shaders/fragshader_light_point.glsl");
    lightPointShaderProgram.link();
    lightPointShaderUniform_vpTransform = lightPointShaderProgram
            .uniformLocation("vpTransform");
    lightPointShaderUniform_lightRad = lightPointShaderProgram
            .uniformLocation("lightRad");
    lightPointShaderUniform_lightPos = lightPointShaderProgram
            .uniformLocation("lightPos");
    lightPointShaderUniform_lightCol = lightPointShaderProgram
            .uniformLocation("lightCol");
}

void MainView::loadMeshes()
{
    mesh_cat = new Mesh(":/models/cat.obj");
    mesh_cube = new Mesh(":/models/cube.obj");
    mesh_sphere = new Mesh(":/models/sphere.obj");
}

void MainView::loadTextures()
{
    glGenTextures(1, &texturePtr);
    loadTexture(":/textures/cat_diff.png", texturePtr);

    glGenTextures(1, &texture_rug);
    loadTexture(":/textures/rug_logo.png", texture_rug);

    glGenTextures(1, &texture_earth);
    loadTexture(":/textures/earth.png", texture_earth);

    glGenTextures(1, &texture_jupiter);
    loadTexture(":/textures/jupiter.png", texture_jupiter);
}

void MainView::createObjects()
{
    int w = 20;
    int h = 20;
    int dist = 2;
    for (int i = 0; i < w; ++i) {
        for (int j = 0; j < h; ++j) {
            Object* cat = new Object(mesh_cat);
            cat->setTexture(&texturePtr);
            cat->setTranslation((-(w / 2)*dist) + i * dist, 0, (-(h / 2)*dist) + j * dist);
            objects.push_back(cat);
        }
    }

    {
        Object* cube = new Object(mesh_cube);
        cube->setTexture(&texture_rug);
        cube->setTranslation(9.0f, 10.0f, 0.0);
        objects.push_back(cube);
    }
    {
        Object* cube = new Object(mesh_cube);
        cube->setTexture(&texture_rug);
        cube->setTranslation(-20.0f, 4.0f, 0.0);
        objects.push_back(cube);
    }

    Object* earth = new Object(mesh_sphere);
    earth->setTexture(&texture_earth);
    earth->setTranslation(0, 5, 0);
    objects.push_back(earth);

    Object* jupiter = new Object(mesh_sphere);
    jupiter->setTexture(&texture_jupiter);
    jupiter->setTranslation(5, 0, 0);
    objects.push_back(jupiter);
}

// --- OpenGL drawing

/**
 * @brief MainView::paintGL
 *
 * Actual function used for drawing to the screen
 *
 */
void MainView::paintGL() {
    // setup GL state.
    glEnable(GL_DEPTH_TEST);
    glDepthMask(true);
    glDisable(GL_BLEND);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);

    //
    // In the first pass, we just write to the gbuffer.
    //
    fbo->bind();


    // Clear the screen before rendering
    glViewport(0, 0, width(), height());
    glClearColor(0.2f, 0.5f, 0.7f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // render geometry in 1st pass
    QOpenGLShaderProgram *shaderProgram;
    shaderProgram = &geometryShaderProgram;
    shaderProgram->bind();


    // Set the texture and draw the mesh.
    for (Object* object : objects) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, *object->getTexture());
        glUniform1i(geometryShaderUniform_textureDiff, 0);

        QMatrix4x4 mvp = projectionTransform * viewMatrix * object->getTransform();
        glUniformMatrix4fv(geometryShaderUniform_mvpTransform, 1, GL_FALSE, mvp.data());
        object->draw();
    }

    shaderProgram->release();

    fbo->unbind(defaultFramebufferObject());

    //
    // Now comes the Deferred shading!
    //
    glViewport(0, 0, width(), height());
    glClearColor(0.0f, 0.0f, 0.8f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //
    // first, we render a single directional light, with a fullscreen pass.
    //

    shaderProgram = &lightSunShaderProgram;
    shaderProgram->bind();
    fbo->setupDeferredShader(shaderProgram);
    updateCameraUniform(shaderProgram);

    // we use attribute-less rendering to render a full-screen triangle.
    // so the triangle vertices are basically stored in the vertex shader.
    // see the vertex shader for more details.

    glDrawArrays(GL_TRIANGLES, 0, 3);
    shaderProgram->release();

//    return;
    //
    // Next, we render all the point light soures.
    // We will be doing our own depth testing in frag shader, so disable depth testing.
    // Enable alpha blending. So that the rendered point lights are added to the framebuffer.
    //
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    // We render only the inner faces of the light sphere.
    // In other words, we render the back-faces and not the front-faces of the sphere.
    // If we render the front-faces, the lighting of the light sphere disappears if
    // we are inside the sphere, which is weird. But by rendering the back-faces instead,
    // we solve this problem.
    glFrontFace(GL_CW);

    shaderProgram = &lightPointShaderProgram;
    shaderProgram->bind();
    fbo->setupDeferredShader(shaderProgram);
    updateCameraUniform(shaderProgram);
    glUniformMatrix4fv(lightPointShaderUniform_vpTransform, 1, GL_FALSE,
                       (viewMatrix * projectionTransform).data());
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, spherePositionVbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereIndexVbo);


    {
        QVector3D color(1.0f, 0.0, 0.0);
        QVector3D pos(-20.0, 4.0, 0.0);
        renderPointLight(color, 27.0f, pos);
    }
    {
        QVector3D color(0.0, 1.0f, 0.0);
        QVector3D pos(9.0, 10.0, 0.0);
        renderPointLight(color, 27.0f, pos);
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
    qDebug() << "MainView::resizeGL(" << newWidth << ", " << newHeight << ")";
    updateProjectionTransform();
}

void MainView::updateProjectionTransform()
{
    float aspect_ratio = static_cast<float>(width()) / static_cast<float>(height());
    projectionTransform.setToIdentity();
    projectionTransform.perspective(60, aspect_ratio, static_cast<float>(0.2), 20);
}

void MainView::updateViewMatrix() {
    // calculate the actual view scale;
    float minscale = 0.002f;
    float maxscale = 0.05f;
    float a = (maxscale - minscale) / 1.99f;
    float b = minscale - (maxscale - minscale) / 199.0f;
    float viewScale = a * scale + b;

    // View matrix
    viewMatrix.setToIdentity();
    viewMatrix.translate(0, 0, -1);
    viewMatrix.scale(viewScale);
    viewMatrix.rotate(rotation.x(), { -1, 0, 0 });
    viewMatrix.rotate(rotation.y(), {  0, 1, 0 });

    // Camera position
    QVector4D pos(0, 0, 0, 1);
    pos = viewMatrix.inverted() * pos;
    cameraPosition = QVector3D(pos.x(), pos.y(), pos.z());

    update();
}

void MainView::updateCameraUniform(QOpenGLShaderProgram *shader)
{
    GLint uniform_cameraPosition = shader->uniformLocation("uniform_cameraPosition");
    glUniform3f(uniform_cameraPosition, cameraPosition.x(), cameraPosition.y(), cameraPosition.z());
}

// --- OpenGL cleanup helpers

void MainView::destroyModelBuffers()
{
    for (Object* object : objects) {
        object->destroy();
    }

    // deleting texture buffers
    glDeleteTextures(1, &texturePtr);
    glDeleteTextures(1, &texture_earth);
    glDeleteTextures(1, &texture_jupiter);
    glDeleteTextures(1, &texture_rug);

    // destroy fbo
    fbo->destroy();
}

// --- Public interface

void MainView::setRotation(int rotateX, int rotateY, int rotateZ)
{
    rotation = { static_cast<float>(rotateX), static_cast<float>(rotateY), static_cast<float>(rotateZ) };
    updateViewMatrix();
}

void MainView::setScale(int newScale)
{
    scale = static_cast<float>(newScale) / 100.f;
    updateViewMatrix();
}

void MainView::setCurrentTexture(CurrentTexture texture)
{
    qDebug() << "Changed current texture to" << texture;
    currentTexture = texture;
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

// create simple UV-sphere.
void MainView::createSphere() {
    int stacks = 20;
    int slices = 20;
    const float PI = 3.14f;

    std::vector<float> positions;
    std::vector<GLuint> indices;

    // loop through stacks.
    for (int i = 0; i <= stacks; ++i){

        float V = (float)i / (float)stacks;
        float phi = V * PI;

        // loop through the slices.
        for (int j = 0; j <= slices; ++j){

            float U = (float)j / (float)slices;
            float theta = U * (PI * 2);

            // use spherical coordinates to calculate the positions.
            float x = cos(theta) * sin(phi);
            float y = cos(phi);
            float z = sin(theta) * sin(phi);

            positions.push_back(x);
            positions.push_back(y);
            positions.push_back(z);
        }
    }

    // Calc The Index Positions
    for (int i = 0; i < slices * stacks + slices; ++i){
        indices.push_back(GLuint(i));
        indices.push_back(GLuint(i + slices + 1));
        indices.push_back(GLuint(i + slices));

        indices.push_back(GLuint(i + slices + 1));
        indices.push_back(GLuint(i));
        indices.push_back(GLuint(i + 1));
    }

    // upload geometry to GPU.
    glGenBuffers(1, &spherePositionVbo);
    glBindBuffer(GL_ARRAY_BUFFER, spherePositionVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*positions.size(), positions.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &sphereIndexVbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereIndexVbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*indices.size(), indices.data(), GL_STATIC_DRAW);

    sphereIndexCount = static_cast<GLuint>(indices.size());
}

void MainView::renderPointLight(const QVector3D color, float radius, const QVector3D position) {
    glUniform1f(lightPointShaderUniform_lightRad, radius);
    glUniform3f(lightPointShaderUniform_lightPos, position.x(), position.y(), position.z());
    glUniform3f(lightPointShaderUniform_lightCol, color.x(), color.y(), color.z());
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(sphereIndexCount), GL_UNSIGNED_INT, nullptr);
}
