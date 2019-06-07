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
    void updateShaderUniforms(QOpenGLShaderProgram *shader);
    void destroy();
    ~FramebufferObjectInstance();
private:
    // Gbuffers.
    GLuint colorTexture;
    GLuint normalTexture;
    GLuint positionTexture;
    GLuint depthTexture;
    GLuint depthRenderbuffer;
    // FBO
    GLuint fbo;
};

#endif // FRAMEBUFFEROBJECT_H
