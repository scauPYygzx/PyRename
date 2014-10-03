#ifndef FILEMESSAGE_H
#define FILEMESSAGE_H

#include <QString>
#include <QChar>
#include <QDateTime>
#include <QFileInfo>

class FileMessage
{
public:
    FileMessage(QString fp);
    QString getFullPath();
    QString getPath();
    QString getFileName();
    QString getSuffix();
    void setNewName(QString);
    QString getNewName();
    void setFullPath(QString);
    void update();
    QDateTime lastModified;
    QDateTime getLastModified();

    QChar separator;


private:
    QString fullPath;
    QString path;
    QString fileName;
    QString suffix;
    QString newName;
};



#endif // FILEMESSAGE_H
