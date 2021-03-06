#include "mainview.h"
#include "math.h"
#include "vertex.h"

#include <QDateTime>

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
}

void MainView::resizeGL(int newWidth, int newHeight) {

    Q_UNUSED(newWidth)
    Q_UNUSED(newHeight)
}

void MainView::paintGL() {
    Vertex p1;
    p1.x = 0.5;
    p1.y = 0.5;
    p1.r = 1;
    p1.g = 0;
    p1.b = 0;

    float vertices[] = {
       0.0f, 1.0f, 0.0f,
       1.0f, 0.0f, 0.0f,
       -1.0f, 0.0f, 0.0f
    };

    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    GLuint VAO;
    glGenVertexArrays(1, &VAO);



    glBindVertexArray(VAO);

//    glCreateShader();
}

void MainView::onMessageLogged( QOpenGLDebugMessage Message ) {
    qDebug() << " → Log:" << Message;
}
