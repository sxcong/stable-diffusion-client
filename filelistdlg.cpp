#include "filelistdlg.h"
#include "ui_filelistdlg.h"
#include "imagefilemanager.h"
#include <QDebug>


FileListDlg::FileListDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FileListDlg)
{
    ui->setupUi(this);
    //setWindowFlags(this->windowFlags() | Qt::FramelessWindowHint);
    setWindowFlags(windowFlags() & (~Qt::WindowContextHelpButtonHint | Qt::CustomizeWindowHint));
    setWindowModality(Qt::ApplicationModal);
    //setWindowIcon(QIcon(":/res/title/logo.ico"));
    m_playFileList = ImageFileManager::Instance().getPlayFileList();
    int nIndex = ImageFileManager::Instance().getCurrentIndex();
    qDebug()<<"m_playFileList"<<m_playFileList.size();

    initTableWidget(ui->tableWidget);

    if (m_playFileList.size() > 0)
    {
        ui->tableWidget->setRowCount(m_playFileList.size());
        for (uint32_t i = 0; i < m_playFileList.size() ; i++)
        {
            int column = 0;
    /*      auto cell0 = new QTableWidgetItem(QString::number(i));
            cell0->setTextAlignment(Qt::AlignLeft|Qt::AlignVCenter);
            ui->tableWidget->setItem(i, column++, cell0);
    */
            auto cell1 = new QTableWidgetItem(m_playFileList[i]);
            cell1->setTextAlignment(Qt::AlignLeft|Qt::AlignVCenter);
            ui->tableWidget->setItem(i, column++, cell1);

            if (i == nIndex)
            {
                 ui->tableWidget->selectRow(i);
            }
        }
    }
}

FileListDlg::~FileListDlg()
{
    delete ui;
}


void FileListDlg::initTableWidget(QTableWidget *pTable)
{
    if (pTable == nullptr)
        return;

    pTable->setEditTriggers(QAbstractItemView::NoEditTriggers);//设置单元格不可编辑
    pTable->setSelectionMode(QAbstractItemView::SingleSelection);//单选
    pTable->setSelectionBehavior(QAbstractItemView::SelectRows);//选行`
    pTable->horizontalHeader()->setStretchLastSection(true);//最后一个单元格扩展
    pTable->setFocusPolicy(Qt::NoFocus);//解决选中虚框问题
    pTable->setFrameShape(QFrame::Box);//去除边框


    pTable->setAlternatingRowColors(false); // 隔行变色
    //pTable->setPalette(QPalette(QColor(21,46,84))); // 设置隔行变色的颜色  gray灰色
    //pTable->setPalette(QPalette(QColor(255,0,0)));
    //pTable->setStyleSheet("alternate-background-color:rgb(11,231,255);background-color:rgb(21,21,21)");
    pTable->setStyleSheet("alternate-background-color:rgb(11,231,255)");

    pTable->setWindowTitle("");
    //pTable->setSelectionBehavior ( QAbstractItemView::SelectItems); //设置选择行为，以行为单位
    //pTable->setSelectionMode ( QAbstractItemView::NoSelection);

    pTable->verticalHeader()->setVisible(false);   //隐藏列表头
    pTable->horizontalHeader()->setStretchLastSection(true); //设置充满表宽度
    pTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    //pTable->horizontalHeader()->setFixedHeight(35);//设置表头的高度
    pTable->horizontalHeader()->setHighlightSections(false);
    pTable->setWordWrap(false);
//    pTable->hideColumn(pTable->columnCount());

    for(int i = 0; i < pTable->columnCount();++i)
    {
        if(0 == i || 1 == i)
        {
            pTable->horizontalHeader()->setSectionResizeMode(i,QHeaderView::ResizeToContents);
        }
        else
        {
            pTable->horizontalHeader()->setSectionResizeMode(i,QHeaderView::Stretch);
        }
    }
}
