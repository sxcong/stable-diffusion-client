#ifndef IMAGEUTILS_H
#define IMAGEUTILS_H

#include <QPixmap>
#include <QMovie>

#include "libheif/heif.h"
#include "libheif/heif_cxx.h"

struct ReadData
{
    QPixmap pixmap;
    QString absoluteFilePath;
    qint64 fileSize;
    QSize imageSize;
    //QColorSpace targetColorSpace;
};


class ImageUtils
{
public:
    //singleton
    static ImageUtils& Instance();
    bool readFile(const QString &fileName, ReadData& readData);
private:
    ImageUtils();
    void loadPixmap(const ReadData &readData);
    int loadHEIC(const QString &heicFile, ReadData &readData);
private:
     int m_largestDimension;
     QPixmap m_loadedPixmap;
     QMovie m_loadedMovie;
};

#endif // IMAGEUTILS_H
