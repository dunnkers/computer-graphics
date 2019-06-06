#include "framebufferobject.h"

FramebufferObjectInstance::FramebufferObjectInstance(GLsizei width, GLsizei height)
{
    setup(width, height);
}

void FramebufferObjectInstance::setup(GLsizei width, GLsizei height)
{
    initializeOpenGLFunctions();

    // create the gbuffer. first create fbo:
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // RGBA8 color texture-p
    glGenTextures(1, &colorTexture);
    glBindTexture(GL_TEXTURE_2D, colorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                GL_TEXTURE_2D, colorTexture, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    //  RGBA16F normal texture.
    glGenTextures(1, &normalTexture);
    glBindTexture(GL_TEXTURE_2D, normalTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
                                GL_TEXTURE_2D, normalTexture, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    //  RGBA16F position texture.
    glGenTextures(1, &positionTexture);
    glBindTexture(GL_TEXTURE_2D, positionTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2,
                                GL_TEXTURE_2D, positionTexture, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // we need a z-buffer for the gbuffer. but we don't need to read from it.
    // so instead create a renderbuffer.
    glGenRenderbuffers(1, &depthRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);

    // specify that we can render to all three attachments.
    // this is very important! It won't work otherwise.
    GLenum drawBuffers[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, drawBuffers);

    // make sure nothing went wrong:
    GLenum status;
    status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        printf("Framebuffer not complete. Status: %d", status);
        exit(1);
    }

    // bind to default framebuffer again when done.
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FramebufferObjectInstance::bind()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo); // bind g buffer for writing.
}

void FramebufferObjectInstance::unbind(GLuint framebuffer)
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer); // stop writing to gbuffer.
}

// configure a shader for usage in deferred rendering.
void FramebufferObjectInstance::setupDeferredShader(QOpenGLShaderProgram *shader) {
    // bind gbuffer textures.
    glUniform1i(shader->uniformLocation("uniform_colorTexture"), 0);
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, colorTexture);

    glUniform1i(shader->uniformLocation("uniform_normalTexture"), 1);
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, normalTexture);

    glUniform1i(shader->uniformLocation("uniform_positionTexture"), 2);
    glActiveTexture(GL_TEXTURE0 + 2);
    glBindTexture(GL_TEXTURE_2D, positionTexture);
}

void FramebufferObjectInstance::destroy()
{
    glDeleteTextures(1, &colorTexture);
    glDeleteTextures(1, &normalTexture);
    glDeleteTextures(1, &positionTexture);
    glDeleteFramebuffers(1, &fbo);
}

FramebufferObjectInstance::~FramebufferObjectInstance()
{
    destroy();
}
