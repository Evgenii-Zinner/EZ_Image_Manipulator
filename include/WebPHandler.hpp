#pragma once

#include <QtCore/QString>
#include <QtGui/QImage>

class WebPHandler {
public:
    static bool encode(const QImage& image, const QString& filename, int quality = 90);
    static QImage decode(const QString& filename);

private:
    static uint8_t* convertToRGBA(const QImage& image, int* width, int* height);
    static QImage convertFromRGBA(const uint8_t* data, int width, int height, bool hasAlpha);
};
