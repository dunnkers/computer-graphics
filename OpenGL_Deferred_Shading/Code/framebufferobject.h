#ifndef FRAMEBUFFEROBJECT_H
#define FRAMEBUFFEROBJECT_H

#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>

class FramebufferObjectInstance : protected QOpenGLFunctions_3_3_Core
{

public:
    FramebufferObjectInstance(GLsizei width, GLsizei height);
    void setup(GLsizei width, GLsizei height);
    void bind();
    void unbind(GLuint framebuffer);
    void setupDeferredShader(QOpenGLShaderProgram *shader);
    void destroy();
    ~FramebufferObjectInstance();
private:
    // Gbuffers.
    GLuint colorTexture;
    GLuint depthRenderbuffer;
    GLuint normalTexture;
    GLuint positionTexture;
    // FBO
    GLuint fbo;
};

#endif // FRAMEBUFFEROBJECT_H
