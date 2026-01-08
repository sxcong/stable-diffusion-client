#ifndef SYSTEMINOFOMGR_H
#define SYSTEMINOFOMGR_H

#include <QtGlobal>
#include <QVector>
#include <QStorageInfo>

typedef struct _FILETIME FILETIME;
class SystemInofoMgr
{
public:
    SystemInofoMgr();
    static SystemInofoMgr& instance();
public:
    double cpuLoadAverage();

    double memoryUsed();

    double diskUsage();

    double currentprogramDiskUsage();

private:
    void init();
    QVector<qulonglong> cpuRawData();
    qulonglong convertFileTime(const FILETIME& filetime) const;
    QVector<qulonglong> mCpuLoadLastValues;

};

#endif // SYSTEMINOFOMGR_H
