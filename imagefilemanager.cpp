#include "imagefilemanager.h"

#include <random>
#include <QDebug>
#include <QtGlobal>
#include <QtMath>
#include <qglobal.h>
#include <QDateTime>

ImageFileManager& ImageFileManager::Instance()
{
    static ImageFileManager agent;
    return agent;
}

ImageFileManager::ImageFileManager()
{
    defineFilterLists();
}

void ImageFileManager::setFile(const QString& curFile, bool bRefsh)
{
    QFileInfo fileInfo(curFile);
    if (fileInfo.isFile())
    {
        m_szCurFile = curFile;
        QString path = fileInfo.absolutePath();//C:/Users/Tiger/Pictures
        if (bRefsh)
            ImageFileManager::Instance().getFileIndex(path);
    }
}

void ImageFileManager::defineFilterLists()
{
    const auto &byteArrayFormats = QImageReader::supportedImageFormats();

    auto filterString = tr("Supported Images") + " (";
    filterList.reserve(byteArrayFormats.size()-1);
    fileExtensionList.reserve(byteArrayFormats.size()-1);

    // Build the filterlist, filterstring, and filterregexplist in one loop
    for (const auto &byteArray : byteArrayFormats)
    {
        const auto fileExtension = "." + QString::fromUtf8(byteArray);
        // Qt 5.15 seems to have added pdf support for QImageReader but it is super broken in qView
        if (fileExtension == ".pdf")
            continue;

        filterList << "*" + fileExtension;
        filterString += "*" + fileExtension + " ";
        fileExtensionList << fileExtension;

        // If we support jpg, we actually support the jfif, jfi, and jpe file extensions too almost certainly.
        if (fileExtension == ".jpg")
        {
            filterList << "*.jpe" << "*.jfi" << "*.jfif";
            filterString += "*.jpe *.jfi *.jfif ";
            fileExtensionList << ".jpe" << ".jfi" << ".jfif";
        }
    }
    filterString.chop(1);
    filterString += ")";

    // Build mime type list
    const auto &byteArrayMimeTypes = QImageReader::supportedMimeTypes();
    mimeTypeNameList.reserve(byteArrayMimeTypes.size()-1);
    for (const auto &byteArray : byteArrayMimeTypes)
    {
        // Qt 5.15 seems to have added pdf support for QImageReader but it is super broken in qView
        const QString mime = QString::fromUtf8(byteArray);
        if (mime == "application/pdf")
            continue;

        mimeTypeNameList << mime;
    }

    // Build name filter list for file dialogs
    nameFilterList << filterString;
    nameFilterList << tr("All Files") + " (*)";
}

int ImageFileManager::getFileIndex(const QString &dirPath)
{
    m_fileInfoList.clear();
    m_folderFileList.clear();
    m_nCurIndex = 0;
    const auto &extensions = getFileExtensionList();
    const auto &mimeTypes = getMimeTypeNameList();

    QStringList nameFilters;
    nameFilters << "*.jpg" << "*.jpeg" << "*.JPG" << "*.JPEG";
    nameFilters << "*.png"	<< "*.PNG"
            << "*.gif"	<< "*.GIF"
            << "*.bmp"	<< "*.BMP"
            << "*.tif"	<< "*.TIF"
            << "*.tiff" << "*.TIFF"
            << "*.xpm"	<< "*.XPM"
            << "*.ppm"	<< "*.PPM"
            << "*.pgm"	<< "*.PGM"
            << "*.pbm"	<< "*.PBM";
//".bmp", ".cur", ".gif", ".icns", ".ico", ".jpeg", ".jpg", ".jpe", ".jfi", ".jfif", ".pbm", ".pgm", ".png", ".ppm", ".svg", ".svgz", ".tga", ".tif", ".tiff", ".wbmp", ".webp", ".xbm", ".xpm")

    /*QStringList imageFileNames = QDir(dirPath).entryList( nameFilters,
                                                          QDir::Files,  // wanted type
                                                          QDir::Name ); // sort by
    */

    int begin = QDateTime::currentMSecsSinceEpoch();
    const QFileInfoList currentFolder = QDir(dirPath).entryInfoList(QDir::Files | QDir::Hidden, QDir::Name);
    //qDebug()<<"currentFolder.size"<<currentFolder.size();
    for (const QFileInfo &fileInfo : currentFolder)
    {
        bool matched = false;
        const QString absoluteFilePath = fileInfo.absoluteFilePath();
        const QString fileName = fileInfo.fileName();
        for (const QString &extension : extensions)
        {
            //qDebug()<<"fileName.endsWith(extension"<<fileName<<extension;
            if (fileName.endsWith(extension, Qt::CaseInsensitive))
            {
                matched = true;
                break;
            }
        }
        if (matched)
        {
            m_fileInfoList.push_back(fileInfo);
            m_folderFileList.push_back(fileInfo.absoluteFilePath());
            //qDebug()<<"getFileIndex"<<m_szCurFile<<fileInfo.absoluteFilePath();
            if (m_szCurFile == fileInfo.absoluteFilePath())
            {
                m_nCurIndex = m_folderFileList.size() - 1;
            }
        }
    }
    int end = QDateTime::currentMSecsSinceEpoch();
    qDebug()<<"entryInfoList time = "<<end - begin;
    return m_nCurIndex;
}

QString ImageFileManager::firstFile()
{
    if (m_folderFileList.size() < 1)
        return "";

    m_nCurIndex = 0;
    return m_folderFileList[m_nCurIndex];
}

QString ImageFileManager::previousFile()
{
    if (m_folderFileList.size() < 1)
        return "";

    --m_nCurIndex;
    m_nCurIndex = qBound( 0, (int)m_nCurIndex, (int)(m_folderFileList.size()-1));

    return m_folderFileList.at( m_nCurIndex );
}

QString ImageFileManager::nextFile()
{
    if (m_folderFileList.size() < 1)
        return "";

    ++m_nCurIndex;
    m_nCurIndex = qBound( 0, (int)m_nCurIndex, (int)(m_folderFileList.size()-1));

    return m_folderFileList[m_nCurIndex];
}

QString ImageFileManager::lastFile()
{
    if (m_folderFileList.size() < 1)
        return "";

    m_nCurIndex = m_folderFileList.size() - 1;
    return m_folderFileList[m_nCurIndex];
}
