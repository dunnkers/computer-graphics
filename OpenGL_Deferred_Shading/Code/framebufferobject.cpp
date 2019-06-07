#include "framebufferobject.h"

FramebufferObjectInstance::FramebufferObjectInstance(GLsizei width, GLsizei height)
{
    setup(width, height);
}

void FramebufferObjectInstance::setup(GLsizei width, GLsizei height)
{
    initializeOpenGLFunctions();

    // fbo
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // color gbuffer // using RGBA8
    glGenTextures(1, &colorTexture);
    initColorTexture(width, height);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                GL_TEXTURE_2D, colorTexture, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    //  normals gbuffer // using RGBA16F
    glGenTextures(1, &normalTexture);
    initNormalsTexture(width, height);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
                                GL_TEXTURE_2D, normalTexture, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    //  position gbuffer // using RGBA16F
    glGenTextures(1, &positionTexture);
    initPositionTexture(width, height);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2,
                                GL_TEXTURE_2D, positionTexture, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // depth gbuffer // using GL_DEPTH_COMPONENT
    glGenTextures(1, &depthTexture);
    initDepthTexture(width, height);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                GL_TEXTURE_2D, depthTexture, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // specify that we can render to all three attachments.
    // this is very important! It won't work otherwise.
    GLenum drawBuffers[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1,
                              GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, drawBuffers);

    // make sure nothing went wrong:
    GLenum err = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (err != GL_FRAMEBUFFER_COMPLETE) {
        qDebug() << "Something went wrong with framebuffer! Error:" << err;
        exit(1);
    }

    // bind to default framebuffer again when done.
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FramebufferObjectInstance::initColorTexture(GLsizei width, GLsizei height)
{
    glBindTexture(GL_TEXTURE_2D, colorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
}

void FramebufferObjectInstance::initNormalsTexture(GLsizei width, GLsizei height)
{
    glBindTexture(GL_TEXTURE_2D, normalTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0,
                 GL_RGBA, GL_FLOAT, nullptr);
}

void FramebufferObjectInstance::initPositionTexture(GLsizei width, GLsizei height)
{
    glBindTexture(GL_TEXTURE_2D, positionTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0,
                 GL_RGBA, GL_FLOAT, nullptr);
}

void FramebufferObjectInstance::initDepthTexture(GLsizei width, GLsizei height)
{
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0,
                 GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
}

void FramebufferObjectInstance::initTextures(GLsizei width, GLsizei height)
{
    initColorTexture(width, height);
    initNormalsTexture(width, height);
    initPositionTexture(width, height);
    initDepthTexture(width, height);
}

void FramebufferObjectInstance::bind()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
}

void FramebufferObjectInstance::unbind(GLuint framebuffer)
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
}

// update texture uniforms of given shader
void FramebufferObjectInstance::updateShaderUniforms(QOpenGLShaderProgram *shader) {
    glUniform1i(shader->uniformLocation("uniform_colorTexture"), 0);
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, colorTexture);

    glUniform1i(shader->uniformLocation("uniform_normalTexture"), 1);
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, normalTexture);

    glUniform1i(shader->uniformLocation("uniform_positionTexture"), 2);
    glActiveTexture(GL_TEXTURE0 + 2);
    glBindTexture(GL_TEXTURE_2D, positionTexture);

    glUniform1i(shader->uniformLocation("uniform_depthTexture"), 3);
    glActiveTexture(GL_TEXTURE0 + 3);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
}

void FramebufferObjectInstance::destroy()
{
    glDeleteTextures(1, &colorTexture);
    glDeleteTextures(1, &normalTexture);
    glDeleteTextures(1, &positionTexture);
    glDeleteTextures(1, &depthTexture);
    glDeleteFramebuffers(1, &fbo);
}

FramebufferObjectInstance::~FramebufferObjectInstance()
{
    destroy();
}
