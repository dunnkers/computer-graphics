#include "mainview.h"
#include "math.h"
#include "vertex.h"

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

    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);

    // Enable backface culling
    glEnable(GL_CULL_FACE);

    // Default is GL_LESS
    glDepthFunc(GL_LEQUAL);

    // Set the color of the screen to be black on clear (new frame)
    glClearColor(0.2f, 0.5f, 0.7f, 0.0f);

    createShaderProgram();

    threeDim cube[36];

    // triangle front 1.

    cube[0].x = -1;
    cube[0].y = -1;
    cube[0].z = -1;
    cube[0].red = 1;
    cube[0].green = 0;
    cube[0].blue = 0;

    cube[1].x = 1;
    cube[1].y = 1;
    cube[1].z = -1;
    cube[1].red = 1;
    cube[1].green = 0;
    cube[1].blue = 0;

    cube[2].x = -1;
    cube[2].y = 1;
    cube[2].z = -1;
    cube[2].red = 1;
    cube[2].green = 0;
    cube[2].blue = 0;




    // triangle front 2

    cube[3].x = -1;
    cube[3].y = -1;
    cube[3].z = -1;
    cube[3].red = 1;
    cube[3].green = 1;
    cube[3].blue = 0;

    cube[4].x = 1;
    cube[4].y = -1;
    cube[4].z = -1;
    cube[4].red = 1;
    cube[4].green = 1;
    cube[4].blue = 0;

    cube[5].x = 1;
    cube[5].y = 1;
    cube[5].z = -1;
    cube[5].red = 1;
    cube[5].green = 1;
    cube[5].blue = 1;

    // triangle down 1

    cube[6].x = -1;
    cube[6].y = -1;
    cube[6].z = -1;
    cube[6].red = 1;
    cube[6].green = 0;
    cube[6].blue = 0;

    cube[7].x = -1;
    cube[7].y = -1;
    cube[7].z = 1;
    cube[7].red = 1;
    cube[7].green = 0;
    cube[7].blue = 0;


    cube[8].x = 1;
    cube[8].y = -1;
    cube[8].z = 1;
    cube[8].red = 1;
    cube[8].green = 0;
    cube[8].blue = 0;

    // triangle down 2

    cube[9].x = -1;
    cube[9].y = -1;
    cube[9].z = -1;
    cube[9].red = 1;
    cube[9].green = 0;
    cube[9].blue = 0;

    cube[10].x = 1;
    cube[10].y = -1;
    cube[10].z = 1;
    cube[10].red = 1;
    cube[10].green = 0;
    cube[10].blue = 0;

    cube[11].x = 1;
    cube[11].y = -1;
    cube[11].z = -1;
    cube[11].red = 1;
    cube[11].green = 0;
    cube[11].blue = 0;

    // triangle left 1

    cube[12].x = -1;
    cube[12].y = 1;
    cube[12].z = 1;
    cube[12].red = 1;
    cube[12].green = 0;
    cube[12].blue = 0;

    cube[13].x = -1;
    cube[13].y = -1;
    cube[13].z = 1;
    cube[13].red = 1;
    cube[13].green = 0;
    cube[13].blue = 0;

    cube[14].x = -1;
    cube[14].y = 1;
    cube[14].z = -1;
    cube[14].red = 1;
    cube[14].green = 0;
    cube[14].blue = 0;

    // triangle left 2

    cube[15].x = -1;
    cube[15].y = 1;
    cube[15].z = -1;
    cube[15].red = 1;
    cube[15].green = 0;
    cube[15].blue = 0;

    cube[16].x = -1;
    cube[16].y = -1;
    cube[16].z = 1;
    cube[16].red = 1;
    cube[16].green = 0;
    cube[16].blue = 0;

    cube[17].x = -1;
    cube[17].y = -1;
    cube[17].z = -1;
    cube[17].red = 1;
    cube[17].green = 0;
    cube[17].blue = 0;

    // triangle right 1

    cube[18].x = 1;
    cube[18].y = -1;
    cube[18].z = 1;
    cube[18].red = 1;
    cube[18].green = 0;
    cube[18].blue = 0;

    cube[19].x = 1;
    cube[19].y = -1;
    cube[19].z = -1;
    cube[19].red = 1;
    cube[19].green = 0;
    cube[19].blue = 0;

    cube[20].x = 1;
    cube[20].y = 1;
    cube[20].z = 1;
    cube[20].red = 1;
    cube[20].green = 0;
    cube[20].blue = 0;

    // triangle

    cube[21].x = 1;
    cube[21].y = -1;
    cube[21].z = -1;
    cube[21].red = 1;
    cube[21].green = 0;
    cube[21].blue = 0;

    cube[22].x = 1;
    cube[22].y = 1;
    cube[22].z = 1;
    cube[22].red = 1;
    cube[22].green = 0;
    cube[22].blue = 0;

    cube[23].x = 1;
    cube[23].y = 1;
    cube[23].z = -1;
    cube[23].red = 1;
    cube[23].green = 0;
    cube[23].blue = 0;


    // triangle
    cube[24].x = -1;
    cube[24].y = 1;
    cube[24].z = 1;
    cube[24].red = 1;
    cube[24].green = 0;
    cube[24].blue = 0;

    cube[25].x = 1;
    cube[25].y = 1;
    cube[25].z = 1;
    cube[25].red = 1;
    cube[25].green = 0;
    cube[25].blue = 0;

    cube[26].x = -1;
    cube[26].y = -1;
    cube[26].z = 1;
    cube[26].red = 1;
    cube[26].green = 0;
    cube[26].blue = 0;

    // triangle

    cube[27].x = -1;
    cube[27].y = -1;
    cube[27].z = 1;
    cube[27].red = 1;
    cube[27].green = 0;
    cube[27].blue = 0;

    cube[28].x = 1;
    cube[28].y = 1;
    cube[28].z = 1;
    cube[28].red = 1;
    cube[28].green = 0;
    cube[28].blue = 0;

    cube[29].x = 1;
    cube[29].y = -1;
    cube[29].z = 1;
    cube[29].red = 1;
    cube[29].green = 0;
    cube[29].blue = 0;

    // triangle

    cube[30].x = -1;
    cube[30].y = 1;
    cube[30].z = -1;
    cube[30].red = 1;
    cube[30].green = 0;
    cube[30].blue = 0;

    cube[31].x = -1;
    cube[31].y = 1;
    cube[31].z = 1;
    cube[31].red = 1;
    cube[31].green = 0;
    cube[31].blue = 0;

    cube[32].x = 1;
    cube[32].y = 1;
    cube[32].z = 1;
    cube[32].red = 1;
    cube[32].green = 0;
    cube[32].blue = 0;

    // triangle

    cube[33].x = -1;
    cube[33].y = 1;
    cube[33].z = -1;
    cube[33].red = 1;
    cube[33].green = 0;
    cube[33].blue = 0;

    cube[34].x = 1;
    cube[34].y = 1;
    cube[34].z = 1;
    cube[34].red = 1;
    cube[34].green = 0;
    cube[34].blue = 0;

    cube[35].x = 1;
    cube[35].y = 1;
    cube[35].z = -1;
    cube[35].red = 1;
    cube[35].green = 0;
    cube[35].blue = 0;

    threeDim pyramid[18];

    // front
    pyramid[0].x = -1;
    pyramid[0].y = -1;
    pyramid[0].z = -1;
    pyramid[0].red = 0;
    pyramid[0].green = 1;
    pyramid[0].blue = 0;

    pyramid[1].x = 1;
    pyramid[1].y = -1;
    pyramid[1].z = -1;
    pyramid[1].red = 1;
    pyramid[1].green = 0;
    pyramid[1].blue = 0;

    pyramid[2].x = 0;
    pyramid[2].y = 1;
    pyramid[2].z = 0;
    pyramid[2].red = 0;
    pyramid[2].green = 1;
    pyramid[2].blue = 0;

    // left

    pyramid[3].x = -1;
    pyramid[3].y = -1;
    pyramid[3].z = 1;
    pyramid[3].red = 0;
    pyramid[3].green = 1;
    pyramid[3].blue = 0;

    pyramid[4].x = 0;
    pyramid[4].y = 1;
    pyramid[4].z = 0;
    pyramid[4].red = 0;
    pyramid[4].green = 1;

            // behind

    pyramid[4].blue = 0;

    pyramid[5].x = -1;
    pyramid[5].y = -1;
    pyramid[5].z = -1;
    pyramid[5].red = 0;
    pyramid[5].green = 1;
    pyramid[5].blue = 0;

    // right

    pyramid[6].x = 1;
    pyramid[6].y = -1;
    pyramid[6].z = 1;
    pyramid[6].red = 0;
    pyramid[6].green = 1;
    pyramid[6].blue = 0;

    pyramid[7].x = 0;
    pyramid[7].y = 1;
    pyramid[7].z = 0;
    pyramid[7].red = 0;
    pyramid[7].green = 1;
    pyramid[7].blue = 0;

    pyramid[8].x = 1;
    pyramid[8].y = -1;
    pyramid[8].z = -1;
    pyramid[8].red = 0;
    pyramid[8].green = 1;
    pyramid[8].blue = 0;


    // behind

    pyramid[9].x = -1;
    pyramid[9].y = -1;
    pyramid[9].z = 1;
    pyramid[9].red = 0;
    pyramid[9].green = 1;
    pyramid[9].blue = 0;

    pyramid[10].x = 0;

    pyramid[10].y = 1;
    pyramid[10].z = 0;
    pyramid[10].red = 0;
    pyramid[10].green = 1;
    pyramid[10].blue = 0;

    pyramid[11].x = 1;
    pyramid[11].y = -1;
    pyramid[11].z = 1;
    pyramid[11].red = 0;
    pyramid[11].green = 1;
    pyramid[11].blue = 0;


    // bottom 1

    pyramid[12].x = -1;
    pyramid[12].y = -1;
    pyramid[12].z = 1;
    pyramid[12].red = 0;
    pyramid[12].green = 1;
    pyramid[12].blue = 0;

    pyramid[13].x = -1;
    pyramid[13].y = -1;
    pyramid[13].z = -1;
    pyramid[13].red = 0;
    pyramid[13].green = 1;
    pyramid[13].blue = 0;

    pyramid[14].x = 1;
    pyramid[14].y = -1;
    pyramid[14].z = 1;
    pyramid[14].red = 0;
    pyramid[14].green = 1;
    pyramid[14].blue = 0;

    // bottom 2

    pyramid[15].x = -1;
    pyramid[15].y = -1;
    pyramid[15].z = -1;
    pyramid[15].red = 0;
    pyramid[15].green = 1;
    pyramid[15].blue = 0;

    pyramid[16].x = 1;
    pyramid[16].y = -1;
    pyramid[16].z = -1;
    pyramid[16].red = 0;
    pyramid[16].green = 1;
    pyramid[16].blue = 0;

    pyramid[17].x = 1;
    pyramid[17].y = -1;
    pyramid[17].z = 1;
    pyramid[17].red = 0;
    pyramid[17].green = 1;
    pyramid[17].blue = 0;

    //here we create the vbo and vao
    glGenBuffers(1,&vbo);
    glGenVertexArrays(1,&vao);

    //Here we bind the vao and vbo
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER,vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(threeDim)*36,&cube[0], GL_STATIC_DRAW);

    //enable attribute 0 for the postitions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(threeDim),0);

    //enable attribute 1 for the colours
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(threeDim), (GLvoid*) (3 * sizeof(GLfloat)));

    // pyramid
    //here we create the vbo and vao
    glGenBuffers(1,&vbo2);
    glGenVertexArrays(1,&vao2);

    //Here we bind the vao and vbo
    glBindVertexArray(vao2);
    glBindBuffer(GL_ARRAY_BUFFER,vbo2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(threeDim)*36,&pyramid[0], GL_STATIC_DRAW);

    //enable attribute 0 for the postitions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(threeDim),0);

    //enable attribute 1 for the colours
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(threeDim), (GLvoid*) (3 * sizeof(GLfloat)));

    cubeTransformation.translate(2,0,-6);
    pyramidTransformation.translate(-2,0,-6);
    projectionTransformation.perspective(60,60,1,1);
    glUniformMatrix4fv(modelTransformLocation,1,GL_FALSE,projectionTransformation.data());

}

void MainView::createShaderProgram()
{

    // Create shader program

    shaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,
                                           ":/shaders/vertshader.glsl");
    shaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment,
                                           ":/shaders/fragshader.glsl");
    shaderProgram.link();

    modelTransformLocation = shaderProgram.uniformLocation("modelTransform");
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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shaderProgram.bind();

//    glBindVertexArray(GL_ARRAY_BUFFER);
    glUniformMatrix4fv(modelTransformLocation,1,GL_FALSE,cubeTransformation.data());
    glUniformMatrix4fv(modelTransformLocation,1,GL_FALSE,pyramidTransformation.data());
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES,0,36);

    glBindVertexArray(vao2);
    glDrawArrays(GL_TRIANGLES,0,18);


    // Draw here

    shaderProgram.release();
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
    // TODO: Update projection to fit the new aspect ratio
    glUniformMatrix4fv(modelTransformLocation,1,GL_FALSE,projectionTransformation.data());
    Q_UNUSED(newWidth)
    Q_UNUSED(newHeight)
}

// --- Public interface

void MainView::setRotation(int rotateX, int rotateY, int rotateZ)
{
    qDebug() << "Rotation changed to (" << rotateX << "," << rotateY << "," << rotateZ << ")";
    Q_UNIMPLEMENTED();
}

void MainView::setScale(int scale)
{
    qDebug() << "Scale changed to " << scale;
    Q_UNIMPLEMENTED();
}

void MainView::setShadingMode(ShadingMode shading)
{
    qDebug() << "Changed shading to" << shading;
    Q_UNIMPLEMENTED();
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
