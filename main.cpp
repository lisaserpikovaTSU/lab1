#include <QCoreApplication>
#include <QString>
#include <QTextStream>
#include <QDirIterator>
#include <QFileInfo>
#include <QDateTime>
#include <openssl/aes.h>
#include <stdexcept>
#include "cryptfiledevice.h"
#include <QRandomGenerator>
#include <QDebug>

QTextStream cin(stdin);
QTextStream cout(stdout);

//quint32 generateRandomInt() {
quint16 generateRandomInt() {
    return QRandomGenerator::global()->generate();
}

QByteArray generateRandomData(int size) {
    QByteArray data;
    while (data.size() < size) {
        data += char(generateRandomInt() % 256);
    }
    return data;
}

void encrypt(const QString& path) {
    QFile toEncrypt(path);
    QByteArray salt = generateRandomData(16);
    CryptFileDevice cryptFileDevice(&toEncrypt, "password", salt);

}

void folderTraverse(QString& path, int depth = 0) {
    QDirIterator itDirs(path, QDir::Dirs);
    QString tab = QString("  ").repeated(depth);

    QDirIterator itFiles(path, QDir::Files);
    while (itFiles.hasNext()) {
        itFiles.next();
        QFileInfo info = itFiles.fileInfo();
        cout << tab << "F | " << info.fileName();
        cout << "\n";

        encrypt(info.absoluteFilePath());
    }

    while (itDirs.hasNext()) {
        itDirs.next();

        if (itDirs.fileName() == "." || itDirs.fileName() == "..") continue;

        QFileInfo info = itDirs.fileInfo();
        cout << tab << "D | " << info.fileName();
        cout << Qt::endl;

        QString nextPath = info.filePath();
        folderTraverse(nextPath, depth+1);
    }
}

QString getValidDir() {
    QString path;
    cout << "Enter path for folder to encrypt: ";
    cout.flush();
    path = cin.readLine();

    QDir folder(path);

    while (!folder.exists()) {
        cout << "Path is incorrect or there is no such path :(" << Qt::endl;
        cout << "Try again: ";
        cout.flush();
        path = cin.readLine();
        folder = QDir(path);
    }

    cout << "Path found :)" << Qt::endl;

    return path;
}

int main()
{
    QString path = getValidDir();
    folderTraverse(path);



    return 0;
    // /Users/liza/Desktop/Учебные/

    //return a.exec();
}

