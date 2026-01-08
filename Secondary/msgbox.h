#ifndef MESSAGEBOX_H
#define MESSAGEBOX_H

#include <QMessageBox>

class MsgBox : public QMessageBox
{
public:
    explicit MsgBox(QWidget *parent = nullptr);

    MsgBox(Icon icon, const QString &title, const QString &content, QWidget *parent = nullptr);

    ~MsgBox();

    static bool Question(const QString &content, QWidget *parent = nullptr, const QString &title = "");

    static int Warning(const QString &content, QWidget *parent = nullptr,const QString &title = "");

    static int Critical(const QString &content, QWidget *parent = nullptr, const QString &title = "");

    static int Information(const QString &content, QWidget *parent = nullptr, const QString &title = "");

    // QWidget interface
protected:
    void showEvent(QShowEvent *event);
    void resizeEvent(QResizeEvent *event);

    QWidget *m_Parent = nullptr;
};

#endif // MESSAGEBOX_H
