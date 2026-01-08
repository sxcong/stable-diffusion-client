#include "global.h"
#include "secondarydlg.h"
#include "ui_secondarydlg.h"
#include <QFileInfo>
#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>

SecondaryDlg::SecondaryDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SecondaryDlg)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint/* | Qt::WindowStaysOnTopHint*/);
    init();

    ui->dateTimeEdit_2->hide();
    ui->lineEdit->setText("a black-and-white Border Collie");
}

SecondaryDlg::~SecondaryDlg()
{
    delete ui;
}

void SecondaryDlg::slot_FullBand(const QVector<float> &qVec)
{
    emit sig_FullBand(qVec);
}

void SecondaryDlg::on_tBtn_Close_clicked()
{
    this->close();
}

void SecondaryDlg::init()
{
    //on_btn_SinglePhase_clicked(true);
    //on_Btn_FullBand_clicked(true);
    initTableWidget(ui->tableWidget);

    m_pMainWidget = new MainWidget(this);
    ui->verticalLayout->addWidget(m_pMainWidget);
}

void SecondaryDlg::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_currentWidgetPos = event->pos();
        m_currentPos = event->pos();

        if(event->pos().y() < 60)
        {
            m_bLeftButtonPressed = true;
        }
    }

    QDialog::mousePressEvent(event);
}

void SecondaryDlg::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_bLeftButtonPressed = false;
    }

    QDialog::mouseReleaseEvent(event);
}

void SecondaryDlg::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() == Qt::LeftButton)
    {
        if (m_bLeftButtonPressed)
        {
            resize(this->width(),this->height());
            move(event->globalPos() - m_currentWidgetPos);
            return;
        }
    }

    QDialog::mouseMoveEvent(event);
}

void SecondaryDlg::showEvent(QShowEvent *event)
{
    this->activateWindow();
    this->raise();
}


void SecondaryDlg::initTableWidget(QTableWidget *pTable)
{
    if (pTable == nullptr)
        return;

    pTable->setAlternatingRowColors(true);
    //pTable->setPalette(QPalette(QColor(255,255,255))); //

    pTable->setWindowTitle("");
    pTable->setSelectionBehavior ( QAbstractItemView::SelectRows);
    pTable->setSelectionMode ( QAbstractItemView::SingleSelection);

    pTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    pTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    pTable->verticalHeader()->setVisible(false);
    pTable->horizontalHeader()->setVisible(true);
    pTable->horizontalHeader()->setHighlightSections(false);

    pTable->setWordWrap(false);
    pTable->horizontalHeader()->setStretchLastSection(true);
}


void SecondaryDlg::on_tableWidget_doubleClicked(const QModelIndex &index)
{
}


void SecondaryDlg::timerEvent(QTimerEvent *event)
{
}


std::string formatBytes(uint64_t bytes) {
    const char* units[] = {"B", "K", "M", "G", "T"};
    int unitIndex = 0;
    double size = static_cast<double>(bytes);

    while (size >= 1024 && unitIndex < 4) {
        size /= 1024;
        unitIndex++;
    }

    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%.2f%s", size, units[unitIndex]);
    return std::string(buffer);
}

void SecondaryDlg::on_pushButton_clicked()
{
     QDir dir(m_szLastPath);
     QString str;
     if(dir.exists())
     {
         str = m_szLastPath;
     }
     QStringList fileNames = QFileDialog::getOpenFileNames(this, "Select File", str,"safetensors File(*.safetensors)");
     if (fileNames.size() > 0)
     {
         ui->tableWidget->clearContents();
         ui->tableWidget->setRowCount(fileNames.size());
         for (int i = 0; i < fileNames.size(); i++)
         {
             QFileInfo info(fileNames[i]);
             QString fileName = info.fileName();
             m_szLastPath = info.path();

             auto cell0 = new QTableWidgetItem(fileNames[i]);
             cell0->setTextAlignment(Qt::AlignLeft|Qt::AlignVCenter);
             ui->tableWidget->setItem(i, 0, cell0);

             std::string str = formatBytes(info.size());
             auto cell1 = new QTableWidgetItem(str.c_str());
             cell1->setTextAlignment(Qt::AlignLeft|Qt::AlignVCenter);
             ui->tableWidget->setItem(i, 1, cell1);

             ui->tableWidget->selectRow(i);
             break; //only one
         }
     }
}

void SecondaryDlg::on_btnImgGen_clicked()
{
    QString path ;
    QString prompt;
    QString outputPath;
    QList<QTableWidgetItem*> list = ui->tableWidget->selectedItems();
    if (list.size() < 1)
    {
        QMessageBox::information(this, "", "Please select a model file!");
        return;
    }

     path = list[0]->text();
     prompt = ui->lineEdit->text();
     outputPath = ui->lineEdit_2->text();

    if (prompt.size() < 1)
    {
        QMessageBox::information(this, "", "Prompt cannot be empty.");
        return;
    }
    if (outputPath.size() < 1)
    {
        QMessageBox::information(this, "", "OutputPath cannot be empty.");
        return;
    }

    int ret = Global::Instance().testSD(prompt.toStdString(), path.toStdString(), outputPath.toStdString());
    if (ret == 0)
    {
        m_pMainWidget->openImage(outputPath);
    }
}

void SecondaryDlg::on_btnOutput_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(
        this,
        tr("Save Image"),
        "",
        tr("PNG Files (*.png);;All Files (*)")
        );

    if (!fileName.isEmpty()) {
        if (!fileName.toLower().endsWith(".png")) {
            fileName += ".png";
        }
        ui->lineEdit_2->setText(fileName);
    }
}

