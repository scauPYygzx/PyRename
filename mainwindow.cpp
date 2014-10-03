#include "mainwindow.h"
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMimeData>
#include <QList>
#include <QUrl>
#include <QStringList>
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QByteArray>
#include <QFont>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QMessageBox>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <QMap>
#include <QFileDialog>


MainWindow::MainWindow(QWidget *parent) :
    QDialog(parent)
{
    this->setWindowTitle(tr("PyRename"));
    this->setMaximumSize(800,600);
    this->setMinimumSize(800,600);
    this->setAcceptDrops(true);

    initUI();
    connectSignalAndSlot();
}

MainWindow::~MainWindow()
{
}

void MainWindow::initUI() {
    QFont font;
    folder = new QLabel("");
    font = folder->font();
    font.setPixelSize(11);
    folder->setFont(font);

    renameRule = new QLineEdit(tr("test_%.5d"));

    QStringList header;
    header << tr(" ") << tr("File") << tr("New Name");
    renameFiles = new QTableWidget;
    renameFiles->setColumnCount(3);
    renameFiles->setSelectionBehavior(QAbstractItemView::SelectRows);
    renameFiles->setEditTriggers(QAbstractItemView::NoEditTriggers);
    renameFiles->setHorizontalHeaderLabels(header);
    renameFiles->horizontalHeader()->setStretchLastSection(true);
    renameFiles->horizontalHeader()->resizeSection(0,0);
    font = renameFiles->font();
    font.setPixelSize(10);
    renameFiles->setFont(font);

    btnFolder = new QPushButton(tr("打开文件夹"));
    btnFiles = new QPushButton(tr("打开文件"));
    btnPreview = new QPushButton(tr("预览"));
    btnRename = new QPushButton(tr("重命名"));
    btnRename->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    //btnRename->setStyleSheet("border:2px groove gray;border-radius:10px;padding:2px 4px;");
    btnRename->setEnabled(false);
    btnClear = new QPushButton(tr("清除"));
    rBtnCustom = new QRadioButton(tr("自定义格式"));
    rBtnDate = new QRadioButton(tr("按修改日期"));
    groupBox = new QGroupBox;

    QVBoxLayout *rBtnLayout = new QVBoxLayout;
    rBtnLayout->addWidget(rBtnCustom);
    rBtnLayout->addWidget(rBtnDate);
    groupBox->setLayout(rBtnLayout);
    rBtnCustom->setChecked(true);

    //QGridLayout *gridLayout = new QGridLayout(this);
    QHBoxLayout *hBox = new QHBoxLayout(this);
    QGridLayout *leftLayout = new QGridLayout;
    QGridLayout *rightLayout = new QGridLayout;
    QHBoxLayout *btnLayout = new QHBoxLayout;

    leftLayout->addWidget(renameRule,0,0,1,1);
    leftLayout->addWidget(btnPreview,0,1,1,1);
    leftLayout->addWidget(folder,1,0,1,2);
    leftLayout->addWidget(renameFiles,2,0,20,2);

    btnLayout->addWidget(btnFolder);
    btnLayout->addWidget(btnFiles);
    rightLayout->addLayout(btnLayout,0,0);
    rightLayout->addWidget(groupBox,1,0);
    rightLayout->addWidget(btnClear,2,0,1,1);
    rightLayout->addWidget(btnRename,3,0,1,1);

    hBox->addLayout(leftLayout);
    hBox->addLayout(rightLayout);
    hBox->setStretchFactor(leftLayout,4);
    hBox->setStretchFactor(rightLayout,1);
}

void MainWindow::connectSignalAndSlot() {
    connect(btnPreview,SIGNAL(clicked()),this,SLOT(slotPreview()));
    connect(btnFolder,SIGNAL(clicked()),this,SLOT(slotOpenFolder()));
    connect(btnFiles,SIGNAL(clicked()),this,SLOT(slotOpenFiles()));
    connect(btnRename,SIGNAL(clicked()),this,SLOT(slotRename()));
    connect(btnClear,SIGNAL(clicked()),this,SLOT(slotClear()));
    connect(rBtnCustom,SIGNAL(clicked(bool)),this, SLOT(slotRadioChanged()));
    connect(rBtnDate,SIGNAL(clicked(bool)),this, SLOT(slotRadioChanged()));
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event) {
    const QMimeData *mime = event->mimeData();

    if(mime->hasUrls()) {
        event->acceptProposedAction();
    }
}

//void MainWindow::dragMoveEvent(QDragMoveEvent *event) {
//    const QMimeData *mime = event->mimeData();

//    if(mime->hasUrls()) {
//        event->acceptProposedAction();
//    }
//}

void MainWindow::dropEvent(QDropEvent *event) {
    const QMimeData *mime = event->mimeData();

    if(mime->hasUrls()) {
        clearRenameFiles();

        QList<QUrl> urlList = mime->urls();
        int nCount = urlList.size();

        bool flag = false;
        for(int i = 0; i < nCount; i++) {
            QString url = urlList.at(i).path();
            if(url.endsWith('/')) {
                flag = true;
                break;
            }
        }

        if(flag && nCount > 1) {
            event->ignore();
            return ;
        }

        if(!flag) {
            for(int i = 0; i < nCount; i++) {
                FileMessage *fileMsg = new FileMessage(urlList.at(i).path());
                addRenameFile(fileMsg);
                if(i == 0) {
                    folder->setText(fileMsg->getPath());
                }
            }
        }
        else {
            QString url = urlList.at(0).path();
            folder->setText(url);
            QDir dir(url);
            foreach (QFileInfo info, dir.entryInfoList(QDir::Files/*| QDir::Dirs*/)) {
                QString fileName = info.fileName();
                if(fileName == QString(".") || fileName == QString("..") /*|| info.isDir()*/)
                    continue;
                FileMessage* fileMsg = new FileMessage(dir.absolutePath() + tr("/") + fileName);
                addRenameFile(fileMsg);
            }
        }
        resizeToContents();
    }
    event->acceptProposedAction();
}

void MainWindow::dragLeaveEvent(QDragLeaveEvent *event) {
    event->accept();
}

void MainWindow::resizeToContents() {
    renameFiles->resizeRowsToContents();
    renameFiles->resizeColumnsToContents();
    if(renameFiles->rowCount() > 0) {
//        int size = renameFiles->rowHeight(0);
        renameFiles->horizontalHeader()->resizeSection(0, 10);
    }
}

void MainWindow::addRenameFile(FileMessage* fileMsg) {
    int row = renameFiles->rowCount();
    renameFiles->insertRow(row);
    renameFiles->setItem(row,1,
                         new QTableWidgetItem(fileMsg->getFileName()+fileMsg->getSuffix()));
    QTableWidgetItem *item = new QTableWidgetItem;
    item->setBackground(QBrush(Qt::yellow));
    renameFiles->setItem(row,0,item);
    listItemMsg.push_back(fileMsg);
}

void MainWindow::setStatus(int row, STATUS status) {
    QTableWidgetItem *item = new QTableWidgetItem;
    //QTableWidgetItem *item = renameFiles->item(row,0);
    //log: QTableWidget: cannot insert an item that is already owned by another QTableWidget

    switch(status) {
    case SUCCESS:
        item->setBackground(QBrush(Qt::green));
        break;
    case FAILED:
        item->setBackground(QBrush(Qt::red));
        break;
    case WAIT:
        item->setBackground(QBrush(Qt::yellow));
        break;
    default:
        item->setBackground(QBrush(Qt::black));
    }
    renameFiles->setItem(row, 0, item);
}

void MainWindow::clearRenameFiles() {
    folder->setText(tr(""));
    renameFiles->setRowCount(0);
    for(int i = 0; i < (int)listItemMsg.size(); i++) {
        delete listItemMsg[i];
    }
    listItemMsg.clear();
}

void MainWindow::renameByLastModified() {
    QMap<QString, int> map;
    for(int i = 0; i < (int)listItemMsg.size();i++) {
        FileMessage *item = listItemMsg[i];
        QString newName = item->getLastModified().toString("yyyy-MM-dd_hh-mm-ss");

        if(map.contains(newName+item->getSuffix())) {
            QMap<QString, int>::iterator it;
            map[newName+item->getSuffix()]++;
            newName += QString(tr("(%1)").arg(map[newName+item->getSuffix()]));
        }
        else {
            map.insert(newName+item->getSuffix(),1);
        }

        item->setNewName(newName);
        renameFiles->setItem(i,2,
                             new QTableWidgetItem(item->getNewName()+item->getSuffix()));
        setStatus(i, WAIT);
    }
    resizeToContents();
}

/**************************************************
 * slots
 * ************************************************/
void MainWindow::slotPreview() {
    for(int i = 0; i < (int)listItemMsg.size(); i++) {
        FileMessage* item = listItemMsg[i];
        char buf[100];
        sprintf(buf,renameRule->text().toLatin1().data(),i+1);
        item->setNewName(QString(buf));
        renameFiles->setItem(i,2,
                             new QTableWidgetItem(item->getNewName()+item->getSuffix()));
        setStatus(i, WAIT);
    }
    resizeToContents();
    btnRename->setEnabled(true);
}

void MainWindow::slotRename() {
    bool flag = true;

    for(int i = 0; i < (int)listItemMsg.size(); i++) {
        FileMessage* item = listItemMsg[i];
        QFile file(item->getFullPath());
        QString newName = item->getPath()+item->getNewName()+item->getSuffix();
        flag = file.rename(newName);
        if(!flag) {
            if(file.error() == 10) {
                QMessageBox::information(this,
                                         tr("重命名失败"),
                                         item->getPath()+item->getFileName()+item->getSuffix()+tr(" ,重命名失败!"),
                                         QMessageBox::Ok);
                setStatus(i,FAILED);
            }
            break;
        }
        item->setFullPath(newName);
        renameFiles->setItem(i,1,
                             new QTableWidgetItem(item->getFileName()+item->getSuffix()));
        renameFiles->setItem(i,2,
                             new QTableWidgetItem(""));
        setStatus(i,SUCCESS);
    }
    resizeToContents();

}

void MainWindow::slotClear() {
    clearRenameFiles();
}

void MainWindow::slotRadioChanged() {
    if(sender() == rBtnCustom && rBtnCustom->isChecked()) {
        renameRule->setEnabled(true);
        btnPreview->setEnabled(true);
        btnRename->setEnabled(false);
        if(!renameRule->text().trimmed().isEmpty()) {
            slotPreview();
            btnRename->setEnabled(true);
        }
    }
    else if(sender() == rBtnDate && rBtnDate->isChecked()) {
        renameRule->setEnabled(false);
        btnPreview->setEnabled(false);
        renameByLastModified();
        btnRename->setEnabled(true);
    }
}

void MainWindow::slotOpenFiles() {
    QStringList files = QFileDialog::getOpenFileNames(this,
                                                      tr("选择文件"),
                                                      tr("/"),
                                                      tr("All Files(*.*)"));
    if(files.size() > 0) {
        clearRenameFiles();
        foreach (QString file, files) {
            FileMessage* fileMsg = new FileMessage(file);
            addRenameFile(fileMsg);
        }
        folder->setText(listItemMsg[0]->getPath());
        resizeToContents();
    }
}

void MainWindow::slotOpenFolder() {
    QString directory = QFileDialog::getExistingDirectory(this,
                                                    tr("Folder"),
                                                    tr("/")
                                                    );
    if(!directory.isEmpty())
    {
        QDir dir(directory);
        clearRenameFiles();
        folder->setText(directory);
        foreach (QFileInfo info, dir.entryInfoList(QDir::Files)) {
            QString fileName = info.fileName();
            if(fileName == QString(".") || fileName == QString(".."))
                continue;
            FileMessage* fileMsg = new FileMessage(dir.absolutePath() + tr("/") + fileName);
            addRenameFile(fileMsg);
        }
        resizeToContents();
    }
}
