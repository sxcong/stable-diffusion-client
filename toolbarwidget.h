#ifndef TOOLBARWIDGET_H
#define TOOLBARWIDGET_H

#include <QWidget>

namespace Ui {
class ToolbarWidget;
}

class ToolbarWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ToolbarWidget(QWidget *parent = nullptr);
    ~ToolbarWidget();
signals:
    void signal_home();
    void signal_prev();
    void signal_next();
    void signal_end();
    void signal_showExifPanel();
    void signal_zoom_originalSize();
    void signal_zoom();
    void signal_zoomOut();
    void signal_fullscreen();

    void signal_flip();
    void signal_mirror();
    void signal_left90();
    void signal_right90();

    void signal_playlist();

private slots:
    void on_pushButton_home_clicked();
    void on_pushButton_prev_clicked();
    void on_pushButton_next_clicked();
    void on_pushButton_end_clicked();
    void on_pushButton_info_clicked();
    void on_pushButton_zoomOut_clicked();
    void on_pushButton_zoom_clicked();
    void on_pushButton_originalsize_clicked();
    void on_pushButton_fullscreen_clicked();
    void on_pushButton_flip_clicked();
    void on_pushButton_mirror_clicked();
    void on_pushButton_rotateright_clicked();
    void on_pushButton_rotateleft_clicked();

    void on_pushButton_playlist_clicked();

private:
    Ui::ToolbarWidget *ui;
};

#endif // TOOLBARWIDGET_H
