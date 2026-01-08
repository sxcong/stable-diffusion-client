#ifndef IMAGEFILEMANAGER_H
#define IMAGEFILEMANAGER_H

#include <QObject>
#include <QFileInfo>
#include <QSize>
#include <QElapsedTimer>
#include <QMimeDatabase>
#include <QImageReader>
#include <QDir>
#include <QDateTime>
#include <QCollator>
#include <QFileInfo>
#include <QUrl>
#include <QCache>
#include <vector>
using namespace std;
#include "imageutils.h"

struct CompatibleFile
{
    QString absoluteFilePath;
    QString fileName;
    // Only populated if needed for sorting
    qint64 lastModified;
    qint64 lastCreated;
    qint64 size;
    QString mimeType;
};

enum class GoToFileMode
{
   constant,
   first,
   previous,
   next,
   last
};


class ImageFileManager: public QObject
{
    Q_OBJECT
public:
    static ImageFileManager& Instance();
    ImageFileManager();

    void setFile(const QString& curFile, bool bRefsh);
    QString firstFile();
    QString previousFile();
    QString nextFile();
    QString lastFile();
    QString getCurrentFile(){return m_szCurFile;};
    int getCurrentIndex(){return m_nCurIndex;};
    const vector<QString> &getPlayFileList() const { return m_folderFileList; }
private:
    const QStringList &getFilterList() const { return filterList; }
    const QStringList &getNameFilterList() const { return nameFilterList; }
    const QStringList &getFileExtensionList() const { return fileExtensionList; }
    const QStringList &getMimeTypeNameList() const { return mimeTypeNameList; }
    void defineFilterLists();
    int getFileIndex(const QString &dirPath);
private:
    QStringList filterList;
    QStringList nameFilterList;
    QStringList fileExtensionList;
    QStringList mimeTypeNameList;
    bool allowMimeContentDetection = false;

    int m_nCurIndex;
    QString m_szCurFile;
    vector<QString> m_folderFileList;
    QFileInfoList m_fileInfoList;


    //Natural sorting
    //date modified
    //date created
    //size
    //type
    int m_sortMode = 0;
    bool isLoopFoldersEnabled = false;
    ReadData m_readData;

};

#endif // IMAGEFILEMANAGER_H
