#ifndef SECONDARYDLG_H
#define SECONDARYDLG_H

#include <QDialog>
#include <QTableWidget>
#include <QMouseEvent>
#include "mainwidget.h"

namespace Ui {
class SecondaryDlg;
}

class SecondaryDlg : public QDialog
{
    Q_OBJECT

public:
    explicit SecondaryDlg(QWidget *parent = nullptr);
    ~SecondaryDlg();

public slots:
    void slot_FullBand(const QVector<float> &qVec);
signals:
    void sig_FullBand(const QVector<float> &qVec);
private slots:
    void on_tBtn_Close_clicked();


    void on_tableWidget_doubleClicked(const QModelIndex &index);

    void on_pushButton_clicked();

    void on_btnImgGen_clicked();

    void on_btnOutput_clicked();

private:
    Ui::SecondaryDlg *ui;
    int m_offset = 0;
    void init();

    void initTableWidget(QTableWidget* pTable);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

    bool m_bLeftButtonPressed = false;
    QPoint m_currentPos;
    QPoint m_currentWidgetPos;
    QString m_szLastPath;
    MainWidget* m_pMainWidget = nullptr;
    // QWidget interface
protected:
    void showEvent(QShowEvent *event);
    void timerEvent(QTimerEvent *event);
};

#endif // SECONDARYDLG_H
