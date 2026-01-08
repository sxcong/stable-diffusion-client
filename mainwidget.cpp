#include "mainwidget.h"
#include "ui_mainwidget.h"
#include <QVBoxLayout>
#include <QDebug>
#include <QtConcurrent/QtConcurrentRun>
#include <QDir>
#include <QProcess>

//#include "qvaboutdialog.h"
//#include "qvwelcomedialog.h"
#include "imagefilemanager.h"
#include "filelistdlg.h"

MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget)
{
    ui->setupUi(this);

    m_pToolbarWidget = new ToolbarWidget(this);
    m_pPhotoView = new PhotoView(this);
    ui->widget->layout()->addWidget(m_pToolbarWidget);
    ui->widget->layout()->addWidget(m_pPhotoView);


    // Initialize escape shortcut
    escShortcut = new QShortcut(Qt::Key_Escape, this);
    escShortcut->setKey(Qt::Key_Escape);
    connect(escShortcut, &QShortcut::activated, this, [this]()
    {
        qDebug()<<"Qt::Key_Escape";
        if (windowState() == Qt::WindowFullScreen)
            toggleFullscreen();
    });

    populateOpenWithTimer = new QTimer(this);
    populateOpenWithTimer->setSingleShot(true);
    populateOpenWithTimer->setInterval(250);
    connect(populateOpenWithTimer, &QTimer::timeout, this, &MainWidget::requestPopulateOpenWithMenu);

    // Connection for open with menu population futurewatcher
    connect(&openWithFutureWatcher, &QFutureWatcher<QList<OpenWith::OpenWithItem>>::finished, this, [this](){
        populateOpenWithMenu(openWithFutureWatcher.result());
    });


    connect(m_pPhotoView, &PhotoView::signal_full_screen, this, &MainWidget::toggleFullscreen);
    connect(m_pPhotoView, &PhotoView::signal_updatedLoadedPixmapItem, this, &MainWidget::slot_updatedLoadedPixmapItem);

    connect(m_pToolbarWidget, &ToolbarWidget::signal_home, this, &MainWidget::slot_home);
    connect(m_pToolbarWidget, &ToolbarWidget::signal_prev, this, &MainWidget::slot_prev);
    connect(m_pToolbarWidget, &ToolbarWidget::signal_next, this, &MainWidget::slot_next);
    connect(m_pToolbarWidget, &ToolbarWidget::signal_end,  this, &MainWidget::slot_end);
    connect(m_pToolbarWidget, &ToolbarWidget::signal_showExifPanel, this, &MainWidget::slot_showExifPanel);

    connect(m_pToolbarWidget, &ToolbarWidget::signal_zoom, this, &MainWidget::slot_zoom);
    connect(m_pToolbarWidget, &ToolbarWidget::signal_zoomOut, this, &MainWidget::slot_zoomOut);
    connect(m_pToolbarWidget, &ToolbarWidget::signal_zoom_originalSize, this, &MainWidget::slot_zoom_originalSize);
    connect(m_pToolbarWidget, &ToolbarWidget::signal_fullscreen, this, &MainWidget::slot_fullscreen);

    connect(m_pToolbarWidget, &ToolbarWidget::signal_flip, this, &MainWidget::slot_flip);
    connect(m_pToolbarWidget, &ToolbarWidget::signal_mirror, this, &MainWidget::slot_mirror);
    connect(m_pToolbarWidget, &ToolbarWidget::signal_left90, this, &MainWidget::slot_left90);
    connect(m_pToolbarWidget, &ToolbarWidget::signal_right90, this, &MainWidget::slot_right90);
    connect(m_pToolbarWidget, &ToolbarWidget::signal_playlist, this, &MainWidget::slot_playlist);

}

void MainWidget::populateOpenWithMenu(const QList<OpenWith::OpenWithItem> openWithItems)
{
    /*for (int i = 0; i < ActionManager::Instance().getOpenWithMaxLength(); i++)
    {
        const auto clonedActions = ActionManager::Instance().getAllClonesOfAction("openwith" + QString::number(i), this);
        for (const auto &action : clonedActions)
        {
            // If we are within the bounds of the open with list
            if (i < openWithItems.length())
            {
                auto openWithItem = openWithItems.value(i);

                action->setVisible(true);
                action->setIconVisibleInMenu(false); // Hide icon temporarily to speed up updates in certain cases
                action->setText(openWithItem.name);
                if (!openWithItem.iconName.isEmpty())
                    action->setIcon(QIcon::fromTheme(openWithItem.iconName));
                else
                    action->setIcon(openWithItem.icon);
                auto data = action->data().toList();
                data.replace(1, QVariant::fromValue(openWithItem));
                action->setData(data);
                action->setIconVisibleInMenu(true);
            }
            else
            {
                action->setVisible(false);
            }
        }
    }*/
}

void MainWidget::requestPopulateOpenWithMenu()
{
    openWithFutureWatcher.setFuture(QtConcurrent::run([&]{
        const auto &curFilePath = m_szCurFile;
        qDebug()<<m_szCurFile;
        return OpenWith::getOpenWithItems(curFilePath);
    }));
}

void MainWidget::actionTriggered(QAction *triggeredAction)
{
    // Conditions that will work with a nullptr window passed
    auto key = triggeredAction->data().toStringList().first();
    qDebug()<<key;
    if (key == "about")
    {
        qDebug()<<"about";
        openAboutDialog();
    }
    else if (key == "welcome")
    {
        openWelcomeDialog();
    }
    else if (key == "openwithother") {
        OpenWith::showOpenWithDialog(this);
    }
    else if (key.startsWith("openwith"))
    {
        const auto &openWithItem = triggeredAction->data().toList().at(1).value<OpenWith::OpenWithItem>();
        OpenWith::openWith(m_szCurFile, openWithItem);
    }
    else if (key == "opencontainingfolder") {
        openContainingFolder();
    }
    else if (key == "zoomin")
    {
        m_pPhotoView->zoomIn();
    }
    else if (key == "zoomout")
    {
        m_pPhotoView->zoomOut();
    }
    else if (key == "originalsize")
    {
        m_pPhotoView->setZoomFactor(1.0);
    }
    else if (key == "zoomFitImage")
    {
        m_pPhotoView->setZoomMode(PhotoView::ZoomFitImage);
    }
    else if (key == "zoomFitWidth")
    {
        m_pPhotoView->setZoomMode(PhotoView::ZoomFitWidth);
    }
    else if (key == "zoomFitHeight")
    {
        m_pPhotoView->setZoomMode(PhotoView::ZoomFitHeight);
    }
    else if (key == "zoomFitBest")
    {
        m_pPhotoView->setZoomMode(PhotoView::ZoomFitBest);
    }
    else if (key == "rotateright") {
        m_pPhotoView->rotateImage(90);
    }
    else if (key == "rotateleft") {
        m_pPhotoView->rotateImage(-90);
    }
    else if (key == "mirror") {
        m_pPhotoView->scale(-1, 1);
    }
    else if (key == "flip") {
        m_pPhotoView->scale(1, -1);
    }
}


void MainWidget::openContainingFolder()
{
    if (!isPixmapLoaded)
        return;

    const QFileInfo selectedFileInfo = QFileInfo(m_szCurFile); //getCurrentFileDetails().fileInfo;

#ifdef Q_OS_WIN
    QProcess::startDetached("explorer", QStringList() << "/select," << QDir::toNativeSeparators(selectedFileInfo.absoluteFilePath()));
#elif defined Q_OS_MACOS
    QProcess::execute("open", QStringList() << "-R" << selectedFileInfo.absoluteFilePath());
#else
    QDesktopServices::openUrl(QUrl::fromLocalFile(selectedFileInfo.absolutePath()));
#endif
}


void MainWidget::openAboutDialog()
{

}

void MainWidget::openWelcomeDialog()
{

}

MainWidget::~MainWidget()
{
    delete ui;
}

void MainWidget::slot_home()
{
    QString szFile = ImageFileManager::Instance().firstFile();
    qDebug()<<szFile;
    m_pPhotoView->loadImageFile(szFile, false);
}
void MainWidget::slot_prev()
{
    QString szFile = ImageFileManager::Instance().previousFile();
    qDebug()<<szFile;
    m_pPhotoView->loadImageFile(szFile, false);
}
void MainWidget::slot_next()
{
    QString szFile = ImageFileManager::Instance().nextFile();
    qDebug()<<szFile;
    m_pPhotoView->loadImageFile(szFile, false);
}
void MainWidget::slot_end()
{
    QString szFile = ImageFileManager::Instance().lastFile();
    qDebug()<<szFile;
    m_pPhotoView->loadImageFile(szFile, false);
}

void MainWidget::slot_showExifPanel()
{
    m_bShowExifPanel = !m_bShowExifPanel;
    //m_pPhotoView->showExifPanel(m_bShowExifPanel);
    if (m_bShowExifPanel)
    {
        /*m_pInfoDlg->setInfo(m_pPhotoView->m_fileInfo, m_pPhotoView->m_readData.imageSize.width(), m_pPhotoView->m_readData.imageSize.height(), 0);
        std::string text = m_pPhotoView->m_exifInfo.makeString();
        QString text2 = QString::fromStdString(text);
        m_pInfoDlg->setExifInfo(text2);
        m_pInfoDlg->show();
        m_pInfoDlg->raise();*/
    }

    m_pPhotoView->showExifPanel(m_bShowExifPanel);
}

void MainWidget::slot_zoom()
{
    m_pPhotoView->zoomIn();
}

void MainWidget::slot_zoomOut()
{
    m_pPhotoView->zoomOut();
}

void MainWidget::slot_zoom_originalSize()
{
    m_pPhotoView->noZoom();
}

void MainWidget::slot_fullscreen()
{
    toggleFullscreen();
}

void MainWidget::slot_flip()
{
    m_pPhotoView->scale(1, -1);
}

void MainWidget::slot_mirror()
{
    m_pPhotoView->scale(-1, 1);
}

void MainWidget::slot_left90()
{
    m_pPhotoView->rotateImage(-90);
}

void MainWidget::slot_right90()
{
    m_pPhotoView->rotateImage(90);
}

void MainWidget::toggleFullscreen()
{
    //setWindowState( windowState() ^ Qt::WindowFullScreen );
    if (windowState() == Qt::WindowFullScreen)
    {
        setWindowState(storedWindowState);

    }
    else
    {
        storedWindowState = windowState();
        showFullScreen();
    }
}

void MainWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    //if (event->button() == Qt::MouseButton::LeftButton)
    //    toggleFullscreen();
    QWidget::mouseDoubleClickEvent(event);
}


void MainWidget::contextMenuEvent(QContextMenuEvent *event)
{
    QWidget::contextMenuEvent(event);
    //contextMenu->popup(event->globalPos());
    return;
}

void MainWidget::slot_updatedLoadedPixmapItem(QString& szFile)
{
    m_szCurFile = szFile;
    isPixmapLoaded = true;
    disableActions();
    populateOpenWithTimer->start();
}


void MainWidget::disableActions()
{
   /* const auto &actionLibrary = ActionManager::Instance().getActionLibrary();
    for (const auto &action : actionLibrary)
    {
        const auto &data = action->data().toStringList();
        const auto &clonesOfAction = ActionManager::Instance().getAllClonesOfAction(data.first(), this);

        // Enable this window's actions when a file is loaded
        if (data.last().contains("disable"))
        {
            for (const auto &clone : clonesOfAction)
            {
                const auto &cloneData = clone->data().toStringList();
                if (cloneData.last() == "disable")
                {
                    clone->setEnabled(isPixmapLoaded);
                }
            }
        }
    }

    const auto &openWithMenus = ActionManager::Instance().getAllClonesOfMenu("openwith");
    for (const auto &menu : openWithMenus)
    {
        menu->setEnabled(isPixmapLoaded);
    }*/
}

void MainWidget::slot_playlist()
{
    FileListDlg dlg;
    //QVWelcomeDialog dlg;
    //QVAboutDialog dlg;
    dlg.exec();
}

int MainWidget::openImage(const QString& szFile)
{
     m_pPhotoView->loadImageFile(szFile, false);
    return 0;
}
