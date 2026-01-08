#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QShortcut>
#include <QKeySequence>

#include "PhotoView.h"

#include <QFutureWatcher>
#include "openwith.h"
#include "toolbarwidget.h"

namespace Ui {
class MainWidget;
}

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = nullptr);
    ~MainWidget();

    int openImage(const QString& szFile);

public slots:
    void toggleFullscreen();
    void slot_updatedLoadedPixmapItem(QString& szFile);
    void slot_home();
    void slot_prev();
    void slot_next();
    void slot_end();
    void slot_showExifPanel();

    void slot_zoom();
    void slot_zoomOut();
    void slot_zoom_originalSize();

    void slot_fullscreen();
    void slot_flip();
    void slot_mirror();
    void slot_left90();
    void slot_right90();
    void slot_playlist();
protected:
    void contextMenuEvent(QContextMenuEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

private:
    void actionTriggered(QAction *triggeredAction);
    void openAboutDialog();
    void openWelcomeDialog();

    void disableActions();

    void requestPopulateOpenWithMenu();
    void populateOpenWithMenu(const QList<OpenWith::OpenWithItem> openWithItems);
    void openContainingFolder();
private:
    Ui::MainWidget *ui;
    //ImageGraphicsView* m_pImageGraphicsView = nullptr;

    ToolbarWidget* m_pToolbarWidget = nullptr;
    PhotoView* m_pPhotoView = nullptr;
    QShortcut *escShortcut;
    Qt::WindowStates storedWindowState;

    QMenuBar *menuBar;
    QMenu *contextMenu;
    bool isPixmapLoaded = false;
    QTimer *populateOpenWithTimer;
    QFutureWatcher<QList<OpenWith::OpenWithItem>> openWithFutureWatcher;
    QString m_szCurFile;

    bool m_bShowExifPanel = false;
};

#endif // MAINWIDGET_H
