#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDialog>
#include <QLabel>
#include <QTableWidget>
#include <QPushButton>
#include <QTextEdit>
#include <QCheckBox>
#include <QLineEdit>
#include <QRadioButton>
#include <QGroupBox>

#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>

#include "filemessage.h"
#include <vector>

#include <iostream>
using std::cout;
using std::endl;
using std::vector;

//namespace Ui {
//class MainWindow;
//}

class MainWindow : public QDialog
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    enum STATUS {
        SUCCESS,
        FAILED,
        WAIT,
    };

private:
    //Ui::MainWindow *ui;
    QLabel *folder;
    QLineEdit *renameRule;
    QTableWidget *renameFiles;
    QPushButton *btnFolder;
    QPushButton *btnFiles;
    QPushButton *btnPreview;
    QPushButton *btnRename;
    QPushButton *btnClear;
    vector<FileMessage*> listItemMsg;
    QRadioButton *rBtnCustom;
    QRadioButton *rBtnDate;
    QGroupBox *groupBox;

    void initUI();
    void connectSignalAndSlot();
    void resizeToContents();
    void addRenameFile(FileMessage*);
    void setStatus(int, STATUS);
    void clearRenameFiles();
    void renameByLastModified();

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    //void dragMoveEvent(QDragMoveEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);

public slots:
    void slotPreview();
    void slotRename();
    void slotClear();
    void slotRadioChanged();
    void slotOpenFolder();
    void slotOpenFiles();
};

#endif // MAINWINDOW_H
