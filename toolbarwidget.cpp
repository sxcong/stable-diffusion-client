#include "toolbarwidget.h"
#include "ui_toolbarwidget.h"
#include <QDebug>

QString makeButtonQSS(const QString& image)
{
    QString normal  = QString("QPushButton{ background:rgb(21, 21, 21, 0); background-image: url(%1); color:white; border:none; border-radius:1px;}").arg(image);
    QString hover   = QString("QPushButton:hover{ background:rgb(10, 40, 70); background-image: url(%1); color:white; border:none; border-radius:1px;}").arg(image);
    QString pressed = QString("QPushButton:pressed{ background:qradialgradient(spread:pad,cx:0.5,cy:0.5,radius:0.5,fx:0.5, fy:0.5, stop:0 rgb(10, 40, 70),stop:1 rgb(56, 132, 230));\
                               background-image: url(%1); color:white; border:none; border-radius:1px;}").arg(image);

    QString qss = normal + hover + pressed;
    return qss;
}

ToolbarWidget::ToolbarWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ToolbarWidget)
{
    ui->setupUi(this);

    QPalette pal = palette();
    pal.setColor( QPalette::Base, Qt::black );
    setPalette( pal );
    //setFrameStyle( QFrame::NoFrame );

    QString image = ":/res/icon/24gl-first.png";
    ui->pushButton_home->setStyleSheet(makeButtonQSS(":/res/icon/24gl-first.png"));
    ui->pushButton_prev->setStyleSheet(makeButtonQSS(":/res/icon/24gl-previous.png"));
    ui->pushButton_next->setStyleSheet(makeButtonQSS(":/res/icon/24gl-next.png"));
    ui->pushButton_end->setStyleSheet(makeButtonQSS(":/res/icon/24gl-last.png"));
    ui->pushButton_fullscreen->setStyleSheet(makeButtonQSS(":/res/icon/24gl-fullScreenEnter3.png"));
    ui->pushButton_zoom->setStyleSheet(makeButtonQSS(":/res/icon/24gl-zoomIn.png"));
    ui->pushButton_zoomOut->setStyleSheet(makeButtonQSS(":/res/icon/24gl-zoomOut.png"));
    ui->pushButton_flip->setStyleSheet(makeButtonQSS(":/res/icon/tool_vFlip.png"));
    ui->pushButton_mirror->setStyleSheet(makeButtonQSS(":/res/icon/tool_hFlip.png"));
    ui->pushButton_rotateleft->setStyleSheet(makeButtonQSS(":/res/icon/left-90.png"));
    ui->pushButton_rotateright->setStyleSheet(makeButtonQSS(":/res/icon/right-90.png"));
    ui->pushButton_originalsize->setStyleSheet(makeButtonQSS(":/res/icon/originalSize.png"));
    ui->pushButton_info->setStyleSheet(makeButtonQSS(":/res/icon/information.png"));
    ui->pushButton_playlist->setStyleSheet(makeButtonQSS(":/res/icon/playlist.png"));


    ui->pushButton_home->hide();
    ui->pushButton_prev->hide();
    ui->pushButton_next->hide();
    ui->pushButton_end->hide();
    ui->pushButton_fullscreen->hide();
    ui->pushButton_info->hide();
    ui->pushButton_playlist->hide();
}

ToolbarWidget::~ToolbarWidget()
{
    delete ui;
}

void ToolbarWidget::on_pushButton_home_clicked()
{
    emit signal_home();
}

void ToolbarWidget::on_pushButton_prev_clicked()
{
    emit signal_prev();
}

void ToolbarWidget::on_pushButton_next_clicked()
{
    emit signal_next();
}

void ToolbarWidget::on_pushButton_end_clicked()
{
    emit signal_end();
}

void ToolbarWidget::on_pushButton_info_clicked()
{
    emit signal_showExifPanel();
}

void ToolbarWidget::on_pushButton_zoomOut_clicked()
{
    emit signal_zoomOut();
}

void ToolbarWidget::on_pushButton_zoom_clicked()
{
    emit signal_zoom();
}

void ToolbarWidget::on_pushButton_originalsize_clicked()
{
    emit signal_zoom_originalSize();
}

void ToolbarWidget::on_pushButton_fullscreen_clicked()
{
    emit signal_fullscreen();
}

void ToolbarWidget::on_pushButton_flip_clicked()
{
    emit signal_flip();
}

void ToolbarWidget::on_pushButton_mirror_clicked()
{
    emit signal_mirror();
}

void ToolbarWidget::on_pushButton_rotateright_clicked()
{
    emit signal_right90();
}

void ToolbarWidget::on_pushButton_rotateleft_clicked()
{
    emit signal_left90();
}


void ToolbarWidget::on_pushButton_playlist_clicked()
{
    emit signal_playlist();
}
