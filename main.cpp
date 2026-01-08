#include <QApplication>
#include <QSharedMemory>
#include "secondarydlg.h"
#include <omp.h>
//#include <unistd.h> // 用于获取CPU架构
#include <QDebug>
#include "global.h"

int testOMP()
{
// 1. 打印CPU架构（确认运行在国产CPU上）
#ifdef __aarch64__
        qDebug() << "当前架构：ARM64（鲲鹏/飞腾）";
#elif __loongarch__
        qDebug() << "当前架构：LoongArch（龙芯）";
#elif __sw64__
        qDebug() << "当前架构：SW64（申威）" << endl;
#elif __x86_64__
    qDebug() << "当前架构：x86_64（海光/兆芯）" ;
#endif

// 2. 打印OpenMP线程数
qDebug() << "OpenMP最大线程数：" << omp_get_max_threads() ;

// 3. 并行计算验证
#pragma omp parallel
        {
#pragma omp master//此指令指定只有主线程（Thread 0）执行该代码段。其他线程遇到此段代码时会直接跳过，且不会在末尾等待。
            qDebug() << "实际运行线程数：" << omp_get_num_threads() ;
        }
        // omp_get_max_threads == omp_get_num_threads 则支持
    return 0;
}

//GOOGLE GEMINI
void testOpenMP() {
    // 1. 检查宏定义是否开启 (由编译器开关决定)
#ifdef _OPENMP
    qDebug() << "OpenMP is supported by the compiler. Version:" << _OPENMP;
#else
    qDebug() << "OpenMP is NOT supported or NOT enabled in the compiler settings.";
    return;
#endif

    // 2. 获取 CPU 核心数
    int procs = omp_get_num_procs();
    int max_threads = omp_get_max_threads();
    qDebug() << "Number of processors available:" << procs;
    qDebug() << "Max threads allowed:" << max_threads;

    // 3. 运行并行块测试
    qDebug() << "Starting parallel test...";
#pragma omp parallel
    {
        // 只有在并行块内，才能看到多个线程在工作
        int thread_id = omp_get_thread_num();
#pragma omp critical //此指令确保同一时间只有一个线程能进入该代码段。
        {
            // 使用 critical 保证输出不乱序
            qDebug() << "Hello from thread" << thread_id;
        }
    }
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
   // testOMP();
   // testOpenMP();

    QSharedMemory mem("SDClient");
    if(!mem.create(1))//A single process instance
    {
        a.exit();
        return 0;
    }

    Global::Instance().SDVersion();

    SecondaryDlg secondaryDlg;
    secondaryDlg.show();
    return a.exec();
}
