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
    geometryShaderUniform_normalTransform = geometryShaderProgram
            .uniformLocation("normalTransform");
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
    // create an infamous cat-grid :)
    int w = 8;
    int h = 8;
    int dist = 2;
    for (int i = 0; i < w; ++i) {
        for (int j = 0; j < h; ++j) {
            QVector3D position = QVector3D((-(w / 2)*dist) + i * dist, 0, (-(h / 2)*dist) + j * dist);

            Object* cat = new Object(mesh_cat);
            cat->setTexture(&texturePtr);
            cat->setTranslation(position.x(), position.y(), position.z());
            cat->setRotation(QVector3D(i*i, j*j, 50*i + 40*j));
            objects.push_back(cat);

            // create a light-bulb above every cat.
//            createLight(QVector3D((-(w / 2)*dist) + i * dist, 1.0, (-(h / 2)*dist) + j * dist));

            Object* jupiter = new Object(mesh_sphere);
            jupiter->setTranslation(position.x(), position.y() + 6, position.z());
            jupiter->setTexture(&texture_jupiter);
            jupiter->setScale(2.5f);
            objects.push_back(jupiter);
        }
    }
    // red
    createLight(new LightPoint(QVector3D(-16, 1, -8), QVector3D(1.0, 0.0, 0.0)));
    // g
//    createLight(new LightPoint(QVector3D(-8, 1, 8), QVector3D(0.0, 1.0, 0.0)));
//    // b
//    createLight(new LightPoint(QVector3D(8, 1, -8), QVector3D(0.0, 0.0, 1.0)));
//    // ..
//    createLight(new LightPoint(QVector3D(8, 1, 8), QVector3D(1.0, 1.0, 0.0)));

    // Two random cubes.
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
}

void MainView::createLight(QVector3D position)
{
    createLight(new LightPoint(position, 250.0f));
}

void MainView::createLight(LightPoint *light)
{
    QVector3D position = light->getPosition();
    lights.push_back(light);
//    qDebug() << "Light color:"<<light->getColor();
//    qDebug() << "Light position:"<<light->getPosition();

//    Object* bulb = new Object(mesh_sphere);
//    bulb->setTranslation(position.x(), position.y() + 1, position.z());
//    bulb->setTexture(&texture_jupiter);
//    bulb->setScale(0.3f);
//    lightBulbs.push_back(bulb);

    Object* bulb = new Object(mesh_sphere);
    bulb->setTranslation(position.x(), position.y(), position.z());
//    bulb->setTexture(&texture_bulb);
    bulb->setMaterialAmbient(light->getColor());
    bulb->setScale(0.2f);
    objects.push_back(bulb);
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
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glEnable(GL_CULL_FACE);
    glDisable(GL_BLEND);
    glFrontFace(GL_CCW);

    /**
     * FIRST PASS - RENDER GEOMETRY
     */
    fbo->bind();

    // Clear the screen before rendering
    glViewport(0, 0, width(), height());
    glClearColor(0.2f, 0.5f, 0.7f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Geometry shader
    QOpenGLShaderProgram *shaderProgram;
    shaderProgram = &geometryShaderProgram;
    shaderProgram->bind();

    for (Object* object : objects) {
        // set texture
        glUniform1i(shaderProgram->uniformLocation("hasTextureBool"), object->hasTexture());
        if (object->hasTexture()) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, *object->getTexture());
            glUniform1i(geometryShaderUniform_textureDiff, 0);
        }

        // material - ambient
        QVector3D materialAmbient = object->getMaterialAmbient();
        shaderProgram->setUniformValue(shaderProgram->uniformLocation("materialAmbient"), materialAmbient);

        // update mvp transform uniform
        QMatrix4x4 mvp = projectionTransform * viewMatrix * object->getTransform();
        glUniformMatrix4fv(geometryShaderUniform_mvpTransform, 1, GL_FALSE, mvp.data());

        // update mesh normal transform
        QMatrix3x3 normal = object->getNormalTransform();
        glUniformMatrix3fv(geometryShaderUniform_normalTransform, 1, GL_FALSE, normal.data());

        // draw mesh
        object->draw();
    }

    shaderProgram->release();

    /**
     * SECOND PASS - LIGHTING
     */
    fbo->unbind(defaultFramebufferObject());

    // OpenGL settings
    glDisable(GL_DEPTH_TEST);

    /* SUN LIGHT */
    // Clear the screen before rendering
    glViewport(0, 0, width(), height());
    glClearColor(0.0f, 0.0f, 0.8f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Sun light shader
    shaderProgram = &lightSunShaderProgram;
    shaderProgram->bind();

    // Update uniforms
    fbo->updateShaderUniforms(shaderProgram);
    glUniform1i(shaderProgram->uniformLocation("uniform_enableSun"), enableSun);
    glUniform1i(shaderProgram->uniformLocation("uniform_enableLights"), enableLights);
    updateShaderUniforms(shaderProgram);

    // Update lighting positions and color array uniforms
    const int light_count = 1;
    QVector3D lightPositions[light_count];
    QVector3D lightColors[light_count];
    for (int i = 0; i < light_count; i++)
    {
        lightPositions[i] = lights.at(i)->getPosition();
        lightColors[i] = lights.at(i)->getColor();
    }
    const int uniform_lightPositions = shaderProgram->uniformLocation("lightPositions");
    shaderProgram->setUniformValueArray(uniform_lightPositions, lightPositions, light_count);
    const int uniform_lightColors = shaderProgram->uniformLocation("lightColors");
    shaderProgram->setUniformValueArray(uniform_lightColors, lightColors, light_count);

    QMatrix4x4 mvp_inv = projectionTransform * viewMatrix;
    int loc = shaderProgram->uniformLocation("uniform_mvpTransform_inv");
    shaderProgram->setUniformValue(loc, mvp_inv.inverted());
//    glUniformMatrix4fv(geometryShaderUniform_mvpTransform, 1, GL_FALSE, mvp.data());

    // render a full screen triangle by passing in 3 vertices without attributes. logic in vertex shader.
    glDrawArrays(GL_TRIANGLES, 0, 3);
    shaderProgram->release();


    /* POINT LIGHTS */
    // opengl options
//    glDisable(GL_DEPTH_TEST); // we do this ourselves in the fragshader
            //    glEnable(GL_BLEND); // enable alpha blending
            //    glBlendFunc(GL_ONE, GL_ONE);
            //    glFrontFace(GL_CW); // configure which faces to render. render backfaces.

    // Point light shader
//    shaderProgram = &lightPointShaderProgram;
//    shaderProgram->bind();

    // Update uniforms
//    fbo->updateShaderUniforms(shaderProgram);
//    glUniform1i(shaderProgram->uniformLocation("uniform_enableLights"), enableLights);
//    updateShaderUniforms(shaderProgram);

    // draw light spheres

    // render a full screen triangle by passing in 3 vertices without attributes. logic in vertex shader.
//    glDrawArrays(GL_TRIANGLES, 0, 3);
//    shaderProgram->release();
            //    for (Object *bulb : lightBulbs) {
            //        QMatrix4x4 m = bulb->getTransform();
            //        glUniformMatrix4fv(lightPointShaderUniform_vpTransform, 1, GL_FALSE,
            //                           (projectionTransform * viewMatrix * m).data());
            //        bulb->draw();
            //    }

            //    int i = 0;
            //    for (LightPoint* lightPoint : lights) {
            //        Object *sphere = lightBulbs.at(i);
            //        QVector3D position = lightPoint->getPosition();
            //        QVector3D color = lightPoint->getColor();

            //        // update light bulb uniforms
            //        glUniform1f(shaderProgram->uniformLocation("lightRad"), 15.f);
            //        glUniform3f(shaderProgram->uniformLocation("lightPos"), position.x(), position.y(), position.z());
            //        glUniform3f(shaderProgram->uniformLocation("lightCol"), color.x(), color.y(), color.z());

            //        // update transform uniform
            //        QMatrix4x4 m = sphere->getTransform();
            //        glUniformMatrix4fv(lightPointShaderUniform_vpTransform, 1, GL_FALSE,
            //                           (projectionTransform * viewMatrix * m).data());

            //        sphere->draw();
            //        i ++;
            //    }


    // Performance analysis
    frameCount ++;
   if (runningAnalysis) {
       setRotation(static_cast<int>(rotation.x()),
                   static_cast<int>(rotation.y() + 1),
                   static_cast<int>(rotation.z()));

       // run analysis for 30 seconds.
       if (frameTimer.elapsed() >= 30000) {
           perfAnalysis(false);
       }
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
    fbo->initTextures(newWidth, newHeight);
    updateProjectionTransform();
}

void MainView::updateProjectionTransform()
{
    float aspect_ratio = static_cast<float>(width()) / static_cast<float>(height());
    projectionTransform.setToIdentity();
    projectionTransform.perspective(60, aspect_ratio, static_cast<float>(0.2), 3000);
}

void MainView::updateViewMatrix() {
    // calculate the actual view scale;
    float minscale = 0.003f;
    float maxscale = 0.050f;
    float a = (maxscale - minscale) / 2.00f;
    float b = minscale - (maxscale - minscale) / 200.0f;
    float viewScale = a * scale + b;

    // View matrix
    viewMatrix.setToIdentity();
    viewMatrix.translate(0, 0, -1);
    viewMatrix.rotate(rotation.x(), { -1, 0, 0 });
    viewMatrix.rotate(rotation.y(), {  0, 1, 0 });
    viewMatrix.scale(viewScale);

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

void MainView::updateSettingsUniforms(QOpenGLShaderProgram *shader)
{
    glUniform1i(shader->uniformLocation("uniform_currentTexture"),
                static_cast<GLint>(currentTexture));
}

void MainView::updateShaderUniforms(QOpenGLShaderProgram *shader)
{
    updateCameraUniform(shader);
    updateSettingsUniforms(shader);
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

void MainView::toggleSun(bool enabled)
{
    qDebug() << "Toggling sun enabled to" << enabled;
    enableSun = enabled;
}

void MainView::toggleLights(bool enabled)
{
    qDebug() << "Toggling lights enabled to" << enabled;
    enableLights = enabled;
}

void MainView::perfAnalysis(bool start)
{
    if (start) {
        frameTimer.start(); // timer
    } else { // stop
        // Print average FPS.
        double fps = frameCount / (static_cast<double>(frameTimer.elapsed()) / 1000.0);
        qDebug() << "frames per second = " << fps;
        frameTimer.restart();
        frameCount = 0;
    }

    runningAnalysis = start;
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
