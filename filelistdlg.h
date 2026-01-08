#ifndef FILELISTDLG_H
#define FILELISTDLG_H

#include <QTableWidget>
#include <QHeaderView>
#include <QDialog>
#include <vector>
using namespace std;

namespace Ui {
class FileListDlg;
}

class FileListDlg : public QDialog
{
    Q_OBJECT

public:
    explicit FileListDlg(QWidget *parent = nullptr);
    ~FileListDlg();
private:
    void initTableWidget(QTableWidget *pTable);
private:
    Ui::FileListDlg *ui;
    vector<QString> m_playFileList;
};

#endif // FILELISTDLG_H
