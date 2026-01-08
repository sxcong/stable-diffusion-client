#-------------------------------------------------
#
# Project created by QtCreator 2023-02-10T09:29:07
#
#-------------------------------------------------

QT       += core gui network charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SDClient
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++17
DEFINES += QT_DEPRECATED_WARNINGS
QMAKE_PROJECT_DEPTH = 0
#MSVC 不识别 GCC 的 -fopenmp，需替换为 /openmp
#cl: 命令行 warning D9002 :忽略未知选项“-fopenmp”

SOURCES += \
    BorderPanel.cpp \
    Canvas.cpp \
    GraphicsItemPosAnimation.cpp \
    Panner.cpp \
    Photo.cpp \
    PhotoDir.cpp \
    PhotoMetaData.cpp \
    PhotoView.cpp \
    PrefetchCache.cpp \
    SensitiveBorder.cpp \
    TJPEGWrapper.cpp \
    TextBorderPanel.cpp \
    exifinfo.cpp \
    filelistdlg.cpp \
    imagefilemanager.cpp \
    imageutils.cpp \
        main.cpp \
    buffer.cpp \
    main.cpp \
    Secondary/msgbox.cpp \
    Secondary/secondarydlg.cpp \
    Secondary/systeminofomgr.cpp \
    global.cpp \
    mainwidget.cpp \
    openwith.cpp \
    qvwin32functions.cpp \
    toolbarwidget.cpp

HEADERS += \
    BorderPanel.h \
    Canvas.h \
    GraphicsItemPosAnimation.h \
    Panner.h \
    Photo.h \
    PhotoDir.h \
    PhotoView.h \
    PrefetchCache.h \
    SensitiveBorder.h \
    TJPEGWrapper.h \
    TextBorderPanel.h \
    evbuffer.h \
    exifinfo.h \
    filelistdlg.h \
    imagefilemanager.h \
    imageutils.h \
    include/stable-diffusion/common.hpp \
    include/stable-diffusion/json.hpp \
    macros.h \
    Secondary/msgbox.h \
    Secondary/secondarydlg.h \
    Secondary/systeminofomgr.h \
    global.h \
    mainwidget.h \
    openwith.h \
    qvwin32functions.h \
    toolbarwidget.h


FORMS += \
    Secondary/secondarydlg.ui \
    Secondary/secondarydlg.ui \
    filelistdlg.ui \
    mainwidget.ui \
    qvopenwithdialog.ui \
    toolbarwidget.ui

# Default rules for deployment.
CONFIG(release, debug|release){
    TARGET = SDClient
    DESTDIR=$$PWD/bin/Release
    LIBS += -L$$PWD/lib/Release -lstable-diffusion -lggml -lggml-base -lggml-cuda -lggml-cpu -llibx265 -lturbojpeg -llibwebp -lheif -lde265 -lFreeImage -lFreeImagePlus -lexiv2

   isEmpty(CUDA_DIR) {
       CUDA_DIR = $$(CUDA_PATH)
   }
   isEmpty(CUDA_DIR) {
       CUDA_DIR = "C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v12.9"
   }
   CUDA_LIB_DIR = $$CUDA_DIR/lib/x64
   LIBS += -L$$CUDA_LIB_DIR \
           -lcudart \
           -lcublas \
           -lcublasLt \
           -lcuda
   LIBS += -lkernel32 \
              -luser32 \
              -lgdi32 \
              -lshlwapi\
              -lwinspool \
              -lshell32 \
              -lole32 \
              -loleaut32 \
              -luuid \
              -lcomdlg32 \
              -ladvapi32
    #;kernel32.lib;user32.lib;gdi32.lib;winspool.lib;shell32.lib;ole32.lib;oleaut32.lib;uuid.lib;comdlg32.lib;advapi32.lib
      #TRANSLATIONS += $$PWD/../Bin/Release/translations/qt_cn.ts
}

CONFIG(debug, debug|release){
    TARGET = SDClientd
    DESTDIR=$$PWD/bin/Debug
    LIBS += -L$$PWD/lib/Debug -lstable-diffusion -lggml -lggml-base -lggml-cuda -lggml-cpu

   isEmpty(CUDA_DIR) {
       CUDA_DIR = $$(CUDA_PATH)
   }
   isEmpty(CUDA_DIR) {
       CUDA_DIR = "C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v12.9"
   }
   CUDA_LIB_DIR = $$CUDA_DIR/lib/x64
   LIBS += -L$$CUDA_LIB_DIR \
           -lcudart \
           -lcublas \
           -lcublasLt \
           -lcuda
   LIBS += -lkernel32 \
              -luser32 \
              -lgdi32 \
              -lshlwapi\
              -lwinspool \
              -lshell32 \
              -lole32 \
              -loleaut32 \
              -luuid \
              -lcomdlg32 \
              -ladvapi32
    #TRANSLATIONS += $$PWD/../Bin/Debug/translations/qt_cn.ts
}


INCLUDEPATH += $$PWD/include/
INCLUDEPATH += $$PWD/Secondary/

msvc {
    QMAKE_CFLAGS += /utf-8
    QMAKE_CXXFLAGS += /utf-8
    QMAKE_CXXFLAGS += /openmp
}

RESOURCES += res.qrc
DISTFILES += res/viewer.rc

OTHER_FILES += res/viewer.rc

QMAKE_LFLAGS_RELEASE    += /MAP
#QMAKE_CFLAGS_RELEASE    += /Zi
QMAKE_LFLAGS_RELEASE    += /debug /opt:ref

