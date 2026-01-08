#include "msgbox.h"
#include <QCoreApplication>
#include <QIcon>
#include <QPainter>
#include <QApplication>
#include <QDesktopWidget>
#include <QLabel>

MsgBox::MsgBox(QWidget *parent) :
    QMessageBox(parent)
{
    m_Parent = parent;
}

MsgBox::MsgBox(QMessageBox::Icon icon, const QString &title, const QString &content, QWidget *parent)
{
    this->setWindowFlags(Qt::CustomizeWindowHint|Qt::WindowTitleHint|Qt::WindowStaysOnTopHint);
    //this->setWindowFlags(this->windowFlags() | Qt::FramelessWindowHint);
    this->setIcon(QMessageBox::NoIcon);
    this->setWindowIcon(QIcon(":/res/title/logo.ico"));
    this->setWindowTitle(title);

    //this->setParent(parent);
    m_Parent = parent;

    this->setText(content);
    this->setStyleSheet("QWidget{border:1px solid white;background:qradialgradient(spread:pad,cx:0.5,cy:0.5,radius:0.5,fx:0.5, fy:0.5, stop:0 rgb(30,30,30),stop:1 rgb(35,58,93));}"
                        "QLabel{border:0px;background:transparent;color:red;font:bold 14pt;}"
                        "QPushButton{min-height:25px;min-width:60px;border-radius:6px;border:1px solid white;background-color:gray;font:bold 12pt;}"
                        "QPushButton:pressed{min-height:25px;min-width:60px;border-radius:6px;border:1px solid white;background-color:qradialgradient(spread:pad,cx:0.5,cy:0.5,radius:0.5,fx:0.5, fy:0.5, stop:0 rgb(255,255,255),stop:1 rgb(200,200,200));font:bold 12pt;}");

    if(QMessageBox::Question == icon)
    {
        this->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        this->setButtonText(QMessageBox::Yes, QObject::tr("Yes"));
        this->setButtonText(QMessageBox::No, QObject::tr("No"));
    }
    else
    {
        this->setStandardButtons(QMessageBox::Yes);
        this->setButtonText(QMessageBox::Yes, QObject::tr("OK"));
    }
}

MsgBox::~MsgBox()
{
}

bool MsgBox::Question(const QString &content, QWidget *parent, const QString &title)
{
    QString newTitle = title;
    if(newTitle.isEmpty())
    {
        newTitle = QObject::tr("Question");
    }
    MsgBox box(QMessageBox::Question, newTitle, content, parent);

    if(MsgBox::Yes == box.exec())
    {
        return true;
    }
    return false;
}

int MsgBox::Warning(const QString &content, QWidget *parent, const QString &title)
{
    QString newTitle = title;
    if(newTitle.isEmpty())
    {
        newTitle = QObject::tr("Warning");
    }
    MsgBox box(QMessageBox::Warning, newTitle, content, parent);
    return box.exec();
}

int MsgBox::Critical(const QString &content, QWidget *parent, const QString &title)
{
    QString newTitle = title;
    if(newTitle.isEmpty())
    {
        newTitle = QObject::tr("Critical");
    }
    MsgBox box(QMessageBox::Critical, newTitle, content, parent);
    return box.exec();
}

int MsgBox::Information(const QString &content, QWidget *parent, const QString &title)
{
    QString newTitle = title;
    if(newTitle.isEmpty())
    {
        newTitle = QObject::tr("Information");
    }
    MsgBox box(QMessageBox::Information, newTitle, content, parent);
    return box.exec();
}

void MsgBox::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)
    this->setModal(true);
}

void MsgBox::resizeEvent(QResizeEvent *event)
{
    if(nullptr != m_Parent)
    {
        int ax = (m_Parent->width() - this->width()) / 2 + m_Parent->x();
        int ay = (m_Parent->height() - this->height()) / 2 + m_Parent->y();
        this->move(ax,ay);
    }

}
