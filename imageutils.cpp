#include "imageutils.h"

#include <QFileInfo>
#include <QImageReader>
#include <QImage>
#include <QIcon>
#include <QScreen>
#include <QGuiApplication>
#include <QDebug>
#include <QDateTime>

#include "TJPEGWrapper.h"
#include "exifinfo.h"
#include "FreeImagePlus.h"

#include <string>
#include <locale>
#include <codecvt>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cassert>
#include <algorithm>
#include <vector>
#include <cctype>
using namespace std;

ImageUtils &ImageUtils::Instance()
{
    static ImageUtils agent;
    return agent;
}

ImageUtils::ImageUtils()
{
    m_largestDimension = 0;
    QList<QScreen *> screenList= QGuiApplication::screens();
    for (auto const &screen : screenList)
    {
        int largerDimension;
        if (screen->size().height() > screen->size().width())
        {
            largerDimension = screen->size().height();
        }
        else
        {
            largerDimension = screen->size().width();
        }

        if (largerDimension > m_largestDimension)
        {
            m_largestDimension = largerDimension;
        }
    }
}


void ImageUtils::loadPixmap(const ReadData &readData)
{
    if (readData.pixmap.isNull())
        return;

 /*   m_loadedPixmap = readData.pixmap;

    // Set file details
    currentFileDetails.isPixmapLoaded = true;
    currentFileDetails.baseImageSize = readData.imageSize;
    currentFileDetails.loadedPixmapSize = loadedPixmap.size();
    if (currentFileDetails.baseImageSize == QSize(-1, -1))
    {
        qInfo() << "QImageReader::size gave an invalid size for " + currentFileDetails.fileInfo.fileName() + ", using size from loaded pixmap";
        currentFileDetails.baseImageSize = currentFileDetails.loadedPixmapSize;
    }

    addToCache(readData);

    // Animation detection
    m_loadedMovie.setFormat("");
    m_loadedMovie.stop();
    m_loadedMovie.setFileName(currentFileDetails.fileInfo.absoluteFilePath());

    // APNG workaround
    if (m_loadedMovie.format() == "png")
    {
        m_loadedMovie.setFormat("apng");
        m_loadedMovie.setFileName(currentFileDetails.fileInfo.absoluteFilePath());
    }

    if (m_loadedMovie.isValid() && m_loadedMoviem_loadedMovie.frameCount() != 1)
        m_loadedMovie.start();

    currentFileDetails.isMovieLoaded = loadedMovie.state() == QMovie::Running;

    if (!currentFileDetails.isMovieLoaded)
        if (auto device = loadedMovie.device())
            device->close();

    currentFileDetails.timeSinceLoaded.start();

    emit fileChanged();

    requestCaching();
    */
}

bool ImageUtils::readFile(const QString &fileName, ReadData& readData)
{
    QImageReader imageReader;
    imageReader.setDecideFormatFromContent(true);
    imageReader.setAutoTransform(true);
    imageReader.setFileName(fileName);

    QFileInfo fileInfo(fileName);
    if (fileInfo.suffix().toLower() == "heic" || fileInfo.suffix().toLower() == "avif")
    {
        int ret = loadHEIC(fileName, readData);
        qDebug()<<"loadHEIC ret"<<ret;
        readData.absoluteFilePath = fileInfo.absoluteFilePath();
        readData.fileSize = fileInfo.size();
        readData.absoluteFilePath = fileInfo.absoluteFilePath();
        if (readData.pixmap.isNull())
        {
            return false;
        }
        return true;
    }
    else if (fileInfo.suffix().toLower() == "jpg"|| fileInfo.suffix().toLower() == "jpeg")
    {
        int begin = QDateTime::currentMSecsSinceEpoch();

        ExifInfo m_exifInfo;
        std::string exifFileName = fileName.toStdString();
        bool ret = m_exifInfo.getExifInfo(exifFileName);
        qDebug()<<"exifInfo.Orientation"<< m_exifInfo.Orientation.c_str();

        wstring str = fileName.toStdWString();

        TurboLoadImage(str, readData, atoi(m_exifInfo.Orientation.c_str()));

       /*QImage readImage = imageReader.read();
       QPixmap readPixmap = QPixmap::fromImage(readImage);
       readData = {
           readPixmap,
           fileInfo.absoluteFilePath(),
           fileInfo.size(),
           imageReader.size()
       };*/

        int end = QDateTime::currentMSecsSinceEpoch();
        qDebug()<<"TurboLoadImage time = "<<end - begin;
        return true;
    }

    //qDebug()<<"imageReader.format() ret"<<imageReader.format();
    QImage readImage;
    if (imageReader.format() == "svg" || imageReader.format() == "svgz")
    {
        // Render vectors into a high resolution
        QIcon icon;
        icon.addFile(fileName);
        readImage = icon.pixmap(m_largestDimension).toImage();
        // If this fails, try reading the normal way so that a proper error message is given
        if (readImage.isNull())
            readImage = imageReader.read();
    }
    else
    {
         readImage = imageReader.read();
    }

    QPixmap readPixmap = QPixmap::fromImage(readImage);
    readData = {
        readPixmap,
        fileInfo.absoluteFilePath(),
        fileInfo.size(),
        imageReader.size()
    };
    // Only error out when not loading for cache
    if (readPixmap.isNull() )
    {
        qDebug()<<"readPixmap.isNull()";
        return false;
    }

    return true;
}

std::wstring utf8_to_wstr(const std::string& src)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.from_bytes(src);
}

std::string wstr_to_utf8(const std::wstring& src)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
    return convert.to_bytes(src);
}

int ImageUtils::loadHEIC(const QString &heicFile, ReadData &readData)
{
    if (heicFile.length() < 4)
       return 0;

    int nErr = 0;
    std::string ff = wstr_to_utf8(heicFile.toStdWString());

    QByteArray qa = heicFile.toUtf8();
    ff = qa.data();
    qDebug()<<"filename"<<ff.c_str();

    heif_context* heif_ctx_ = heif_context_alloc();
    if (!heif_ctx_)
    {
        nErr = -2;
        goto cleanup;
    }
    heif_error error = heif_context_read_from_file(heif_ctx_, ff.c_str(), nullptr);
    if (error.code != heif_error_Ok) {
        nErr = -3;
        goto cleanup;
    }
    heif_image_handle* heif_handle_ = nullptr;
    error = heif_context_get_primary_image_handle(heif_ctx_, &heif_handle_);
    if (error.code != heif_error_Ok) {
        nErr = -4;
        goto cleanup;
    }
    int width_ = heif_image_handle_get_width(heif_handle_);
    int height_ = heif_image_handle_get_height(heif_handle_);

    readData.imageSize = QSize(width_, height_);
    heif_image* heif_img_ = nullptr;
    error = heif_decode_image(heif_handle_, &heif_img_, heif_colorspace_RGB, heif_chroma_interleaved_RGB, nullptr);
    if (error.code !=heif_error_Ok) {
        nErr = -5;
        goto cleanup;
    }
    int stride;
    const uint8_t* data = heif_image_get_plane(heif_img_, heif_channel_interleaved, &stride);
    //下面为使用QLabel进行展示的代码，其他图形框架根据框架要求 自行展示
    {
        QImage img(data,width_,height_,QImage::Format_RGB888);
        readData.pixmap = QPixmap::fromImage(img);
        nErr = 0;
    }

cleanup:
    if (heif_img_)
        heif_image_release(heif_img_);
    if (heif_handle_)
        heif_image_handle_release(heif_handle_);
    if (heif_ctx_)
        heif_context_free(heif_ctx_);
    return nErr;
}


