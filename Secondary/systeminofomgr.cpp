#include "systeminofomgr.h"
//#include "syteminfowindowsimpl.h"
#include <windows.h>
#include <QDebug>
#include <QProcess>
#include <QCoreApplication>
SystemInofoMgr::SystemInofoMgr()
{
    init();
}

SystemInofoMgr &SystemInofoMgr::instance()
{
    static SystemInofoMgr tmp;
    return tmp;
}
void SystemInofoMgr::init()
{
    mCpuLoadLastValues = cpuRawData();
}

double SystemInofoMgr::memoryUsed()
{
    //获取内存的使用率
    MEMORYSTATUSEX memoryStatus;
    memoryStatus.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memoryStatus);
    qulonglong memoryPhysicalUsed = memoryStatus.ullTotalPhys - memoryStatus.ullAvailPhys;
    return (double)memoryPhysicalUsed / (double)memoryStatus.ullTotalPhys * 100.0;
}

double SystemInofoMgr::diskUsage()
{
    int total_space= 0;
    int available_space=0;
    foreach (const QStorageInfo &storage, QStorageInfo::mountedVolumes()) {
              if (storage.isValid() && storage.isReady()) {
                  if (!storage.isReadOnly()) {
                      total_space +=storage.bytesTotal()/1024/1024/1024;
                      available_space += storage.bytesAvailable()/1024/1024/1024;
                  }
              }
          }

    return double(total_space-available_space)*100/total_space;
}
double SystemInofoMgr::currentprogramDiskUsage()
{
    int total_space= 0;
    int available_space=0;
     QStorageInfo storage(qApp->applicationDirPath());
     total_space =storage.bytesTotal()/1024/1024/1024;
     available_space =storage.bytesAvailable()/1024/1024/1024;
     return double(total_space-available_space)*100/total_space;
}


QVector<qulonglong> SystemInofoMgr::cpuRawData()
{
    //获取CPU的占用率
    //闲置时间
    FILETIME idleTime;
    //内核使用时间
    FILETIME kernelTime;
    //用户使用时间
    FILETIME userTime;
    GetSystemTimes(&idleTime, &kernelTime, &userTime);
    QVector<qulonglong> rawData;
    rawData.append(convertFileTime(idleTime));
    rawData.append(convertFileTime(kernelTime));
    rawData.append(convertFileTime(userTime));
    return rawData;
}
qulonglong SystemInofoMgr::convertFileTime(const FILETIME& filetime) const
{
    ULARGE_INTEGER largeInteger;
    largeInteger.LowPart = filetime.dwLowDateTime;
    largeInteger.HighPart = filetime.dwHighDateTime;
    return largeInteger.QuadPart;
}

double SystemInofoMgr::cpuLoadAverage()
{
    QVector<qulonglong> firstSample = mCpuLoadLastValues;
    QVector<qulonglong> secondSample = cpuRawData();
    mCpuLoadLastValues = secondSample;

    //获取两个时间点之间的CPU时间
    qulonglong currentIdle = secondSample[0] - firstSample[0];
    qulonglong currentKernel = secondSample[1] - firstSample[1];
    qulonglong currentUser = secondSample[2] - firstSample[2];
    qulonglong currentSystem = currentKernel + currentUser;

    //（总的时间 - 空闲时间）/ 总的时间 = 占用cpu的时间，也就是占用率
    double percent = (currentSystem - currentIdle) * 100.0 /currentSystem ;
    return qBound(0.0, percent, 100.0);
}
