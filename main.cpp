#include <QCoreApplication>
#include <QString>
#include <QTextStream>
#include <QDirIterator>
#include <QFileInfo>
#include <QDateTime>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <stdexcept>
#include "cryptfiledevice.h"
#include <QRandomGenerator>
#include <QDebug>
#include <QTemporaryFile>

QTextStream cin(stdin);
QTextStream cout(stdout);

const QByteArray mark = "ENCRYPTED";
const int SALT_SIZE = 16;

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

bool isFileEncrypted(const QString& path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    if (file.size() < mark.size()) {
        file.close();
        return false;
    }

    QByteArray marker = file.read(mark.size());
    file.close();

    return (marker == mark);
}

void encrypt(const QString& path) {
    if (isFileEncrypted(path)) {
        qDebug() << "Already encrypted. Skipping " << path;
        return;
    }

    QFile toEncrypt(path);
    if (!toEncrypt.open(QIODevice::ReadOnly)) {
        qDebug() << "error opening file to encrypt " << path;
        return;
    }

    QTemporaryFile tempFile;
    if (!tempFile.open()) {
        qDebug() << "error creating temp file " << path;
        return;
    }
    QString tempPath = tempFile.fileName();
    tempFile.close();

    QByteArray salt = generateRandomData(16);

    QFile tempFileDevice(tempPath);
    CryptFileDevice cryptFileDevice(&tempFileDevice, "password", salt);

    if (!cryptFileDevice.open(QIODevice::WriteOnly)) {
        qDebug() << "Cannot open crypt device for:" << path;
        toEncrypt.close();
        return;
    }

    const qint64 BUFFER_SIZE = 8192;
    QByteArray buffer;
    qint64 totalWritten = 0;

    while (!toEncrypt.atEnd()) {
        buffer = toEncrypt.read(BUFFER_SIZE);
        qint64 written = cryptFileDevice.write(buffer);
        if (written != buffer.size()) {
            qDebug() << "error encrypting file " << path;
            toEncrypt.close();
            cryptFileDevice.close();
            return;
        }
        totalWritten += written;
    }

    toEncrypt.close();
    cryptFileDevice.close();

    QFile tempFileForSalt(tempPath);
    if (!tempFileForSalt.open(QIODevice::ReadWrite)) {
        qDebug() << "error opening for salt" << path;
        return;
    }

    QByteArray encryptedData = tempFileForSalt.readAll();
    tempFileForSalt.close();

    if (!tempFileForSalt.open(QIODevice::WriteOnly)) {
        qDebug() << "error reopening for salt" << path;
        return;
    }

    tempFileForSalt.write(mark);
    tempFileForSalt.write(salt);
    tempFileForSalt.write(encryptedData);
    tempFileForSalt.close();

    if (!QFile::remove(path)) {
        qDebug() << "error removing original file";
        return;
    }

    if (!QFile::copy(tempPath, path)) {
        qDebug() << "error copying encrypted file to original";
        return;
    }

    qDebug() << "Successfully encrypted:" << path;

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
    // /Users/liza/Desktop/Тестовая2/

    //return a.exec();
}

