#include "WebPHandler.hpp"
#include <webp/encode.h>
#include <webp/decode.h>
#include <webp/mux.h>
#include <QtCore/QFile>
#include <QtCore/QBuffer>
#include <QtCore/QByteArray>

bool WebPHandler::encode(const QImage &image, const QString &filename, int quality)
{
    int width, height;
    uint8_t *rgba = convertToRGBA(image, &width, &height);
    if (!rgba)
        return false;

    // Configure the encoder parameters
    WebPConfig config;
    if (!WebPConfigInit(&config))
    {
        delete[] rgba;
        return false;
    }

    config.quality = static_cast<float>(quality);
    config.method = 6; // Highest compression method

    if (!WebPValidateConfig(&config))
    {
        delete[] rgba;
        return false;
    }

    // Set up the input picture
    WebPPicture pic;
    if (!WebPPictureInit(&pic))
    {
        delete[] rgba;
        return false;
    }

    pic.width = width;
    pic.height = height;
    pic.use_argb = 1;

    if (!WebPPictureImportRGBA(&pic, rgba, width * 4))
    {
        delete[] rgba;
        WebPPictureFree(&pic);
        return false;
    }

    delete[] rgba; // We can delete the input buffer now

    // Set up the output
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly))
    {
        WebPPictureFree(&pic);
        return false;
    }

    WebPMemoryWriter writer;
    WebPMemoryWriterInit(&writer);
    pic.writer = WebPMemoryWrite;
    pic.custom_ptr = &writer;

    // Encode the image
    bool success = WebPEncode(&config, &pic);
    WebPPictureFree(&pic);

    if (success)
    {
        file.write(reinterpret_cast<char *>(writer.mem), writer.size);
        file.close();
    }

    WebPMemoryWriterClear(&writer);
    return success;
}

QImage WebPHandler::decode(const QString &filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
    {
        return QImage();
    }

    QByteArray data = file.readAll();
    file.close();

    // Initialize decoder
    WebPDecoderConfig config;
    if (!WebPInitDecoderConfig(&config))
    {
        return QImage();
    }

    // Get features from the WebP file
    VP8StatusCode status = WebPGetFeatures(
        reinterpret_cast<const uint8_t *>(data.constData()),
        data.size(),
        &config.input);
    if (status != VP8_STATUS_OK)
    {
        return QImage();
    }

    // Set up output buffer format
    config.output.colorspace = MODE_RGBA;
    config.output.is_external_memory = 0;

    // Decode the WebP file
    status = WebPDecode(
        reinterpret_cast<const uint8_t *>(data.constData()),
        data.size(),
        &config);
    if (status != VP8_STATUS_OK)
    {
        return QImage();
    }

    // Get the decoded data
    uint8_t *rgba = config.output.u.RGBA.rgba;

    if (!rgba)
    {
        WebPFreeDecBuffer(&config.output);
        return QImage();
    }

    QImage result = convertFromRGBA(
        rgba,
        config.output.width,
        config.output.height,
        config.input.has_alpha != 0);

    WebPFreeDecBuffer(&config.output);
    return result;
}

uint8_t *WebPHandler::convertToRGBA(const QImage &image, int *width, int *height)
{
    // Convert to RGBA8888 format if needed
    QImage converted;
    if (image.format() != QImage::Format_RGBA8888)
    {
        converted = image.convertToFormat(QImage::Format_RGBA8888);
    }
    else
    {
        converted = image;
    }

    *width = converted.width();
    *height = converted.height();

    // Allocate and copy the image data
    size_t bufferSize = (*width) * (*height) * 4;
    uint8_t *rgba = new uint8_t[bufferSize];
    memcpy(rgba, converted.constBits(), bufferSize);

    return rgba;
}

QImage WebPHandler::convertFromRGBA(const uint8_t *data, int width, int height, bool hasAlpha)
{
    QImage::Format format = QImage::Format_RGBA8888;
    int bytesPerLine = width * 4;

    QImage image(data, width, height, bytesPerLine, format);
    return image.copy(); // Create a deep copy as the data pointer will be freed
}
