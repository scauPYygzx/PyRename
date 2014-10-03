#include "filemessage.h"


FileMessage::FileMessage(QString fp)
{
#if defined(Q_OS_WIN32) || defined(Q_OS_WIN64) || defined(Q_OS_WINCE)
    separator = '\\';
#else
    separator = '/';
#endif

    fullPath = fp;
    QFileInfo info = QFileInfo(fp);
    lastModified = info.lastModified();

    update();

}

QString FileMessage::getFullPath() {
    return fullPath;
}

void FileMessage::setFullPath(QString fp) {
    fullPath = fp;
    update();
}

QString FileMessage::getPath() {
    return path;
}

QString FileMessage::getFileName() {
    return fileName;
}

QString FileMessage::getSuffix() {
    return suffix;
}

void FileMessage::setNewName(QString nn) {
    newName = nn;
}

QString FileMessage::getNewName() {
    return newName;
}

void FileMessage::update() {
    int lastDot = fullPath.lastIndexOf('.');
    int lastSlash = fullPath.lastIndexOf(separator);
    if(lastDot < lastSlash) {
        suffix = QString("");
        fileName = fullPath.right(fullPath.length() - lastSlash - 1);
    }
    else {
        suffix = fullPath.right(fullPath.length() - lastDot);
        fileName = fullPath.right(fullPath.length() - lastSlash - 1)
                .left(lastDot - lastSlash - 1);
    }
    path  = fullPath.left(lastSlash+1);
    newName = QString("");
}

QDateTime FileMessage::getLastModified() {
    return lastModified;
}
