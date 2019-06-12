#include "mainview.h"


void MainView::loadTexture(QString file, GLuint texturePtr)
{
    loadTexture(file, texturePtr, false);
}

void MainView::loadTexture(QString file, GLuint texturePtr, bool repeat)
{
    // Set texture parameters.
    glBindTexture(GL_TEXTURE_2D, texturePtr);
    if (repeat) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    } else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }

    // Push image data to texture.
    QImage image(file);
    QVector<quint8> imageData = imageToBytes(image);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, image.width(), image.height(),
                 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData.data());

    // bind default again.
    glBindTexture(GL_TEXTURE_2D, 0);
}

QVector<quint8> MainView::imageToBytes(QImage image) {
    // needed since (0,0) is bottom left in OpenGL
    QImage im = image.mirrored();
    QVector<quint8> pixelData;
    pixelData.reserve(im.width()*im.height()*4);

    for (int i = 0; i != im.height(); ++i) {
        for (int j = 0; j != im.width(); ++j) {
            QRgb pixel = im.pixel(j,i);

            // pixel is of format #AARRGGBB (in hexadecimal notation)
            // so with bitshifting and binary AND you can get
            // the values of the different components
            quint8 r = (quint8)((pixel >> 16) & 0xFF); // Red component
            quint8 g = (quint8)((pixel >> 8) & 0xFF); // Green component
            quint8 b = (quint8)(pixel & 0xFF); // Blue component
            quint8 a = (quint8)((pixel >> 24) & 0xFF); // Alpha component

            // Add them to the Vector
            pixelData.append(r);
            pixelData.append(g);
            pixelData.append(b);
            pixelData.append(a);
        }
    }
    return pixelData;
}
