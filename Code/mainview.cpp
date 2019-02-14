#include "mainview.h"
#include "math.h"

#include <QDateTime>
#include "vertex.h"

MainView::MainView(QWidget *parent) : QOpenGLWidget(parent) {
    qDebug() << "MainView constructor";

    connect(&timer, SIGNAL(timeout()), this, SLOT(update()));
}

MainView::~MainView() {
    debugLogger->stopLogging();

    qDebug() << "MainView destructor";
}

void MainView::initializeGL() {
    qDebug() << ":: Initializing OpenGL";
    initializeOpenGLFunctions();

    debugLogger = new QOpenGLDebugLogger();
    connect( debugLogger, SIGNAL( messageLogged( QOpenGLDebugMessage ) ), this, SLOT( onMessageLogged( QOpenGLDebugMessage ) ), Qt::DirectConnection );

    if ( debugLogger->initialize() ) {
        qDebug() << ":: Logging initialized";
        debugLogger->startLogging( QOpenGLDebugLogger::SynchronousLogging );
        debugLogger->enableMessages();
    }

    QString glVersion;
    glVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    qDebug() << ":: Using OpenGL" << qPrintable(glVersion);

    shader.addShaderFromSourceFile(QOpenGLShader::Vertex,":/shaders/vertshader.glsl");
    shader.addShaderFromSourceFile(QOpenGLShader::Fragment,":/shaders/fragshader.glsl");
    shader.link();
    shader.bind();

    Twodim triangle[3];

    triangle[0].x = -1;
    triangle[0].y = -1;
    triangle[0].red = 1;
    triangle[0].green = 0;
    triangle[0].blue = 0;

    triangle[1].x = 0;
    triangle[1].y = 1;
    triangle[1].red = 0;
    triangle[1].green = 1;
    triangle[1].blue = 0;

    triangle[2].x = 1;
    triangle[2].y = -1;
    triangle[2].red = 0;
    triangle[2].green =0;
    triangle[2].blue = 1;
    //here we create the vbo and vao
    glGenBuffers(1,&vbo);
    glGenVertexArrays(1,&vao);

    //Here we bind the vao and vbo
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER,vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Twodim)*3,&triangle[0], GL_STATIC_DRAW);

    //enable attribute 0 for the postitions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Twodim),0);

    //enable attribute 1 for the colours
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Twodim), (GLvoid*) (2 * sizeof(GLfloat)));

}

void MainView::resizeGL(int newWidth, int newHeight) {

    Q_UNUSED(newWidth)
    Q_UNUSED(newHeight)

}

void MainView::paintGL() {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1,1,1,1);
    glBindVertexArray(GL_ARRAY_BUFFER);
    glDrawArrays(GL_TRIANGLES, 0,3);

}

void MainView::onMessageLogged( QOpenGLDebugMessage Message ) {

    qDebug() << " → Log:" << Message;

}
