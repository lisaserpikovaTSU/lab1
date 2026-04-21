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

bool isFolderSystem(const QString& path) {
    QString lowerPath = path.toLower();

    if (lowerPath.startsWith("c:") || lowerPath.startsWith("c:/") || lowerPath.startsWith("c:\\") || lowerPath.startsWith("/system")) {
        return true;
    }
    return false;
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
        qDebug() << "error creating temp file";
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

void decrypt(const QString& path) {
    if (!isFileEncrypted(path)) {
        qDebug() << "Is not encrypted. Skipping" << path;
        return;
    }

    QFile toDecrypt(path);
    if (!toDecrypt.open(QIODevice::ReadOnly)) {
        qDebug() << "error opening file to decrypt " << path;
        return;
    }

    QByteArray marker = toDecrypt.read(mark.size());
    QByteArray salt = toDecrypt.read(SALT_SIZE);

    QByteArray encryptedData = toDecrypt.readAll();
    toDecrypt.close();

    QTemporaryFile tempFile;
    tempFile.open();
    tempFile.write(encryptedData);
    tempFile.close();

    CryptFileDevice cryptFileDevice(&tempFile, "password", salt);
    cryptFileDevice.open(QIODevice::ReadOnly);

    const qint64 BUFFER_SIZE = 8192;
    QByteArray decryptedData;
    QByteArray buffer;

    while (!cryptFileDevice.atEnd()) {
        buffer = cryptFileDevice.read(BUFFER_SIZE);
        decryptedData.append(buffer);
    }

    cryptFileDevice.close();

    if (decryptedData.isEmpty()) {
        qDebug() << "Decryption failed (wrong password or corrupted file):" << path;
        return;
    }

    QTemporaryFile tempDecryptedFile;
    tempDecryptedFile.open();
    tempDecryptedFile.write(decryptedData);
    tempDecryptedFile.close();

    QFile::remove(path);
    QFile::copy(tempDecryptedFile.fileName(), path);

    qDebug() << "Successfully decrypted:" << path;

}

void folderTraverse(QString& path, QString& action, int depth = 0) {
    QDirIterator itDirs(path, QDir::Dirs);
    QString tab = QString("  ").repeated(depth);

    QDirIterator itFiles(path, QDir::Files);
    while (itFiles.hasNext()) {
        itFiles.next();
        QFileInfo info = itFiles.fileInfo();
        cout << tab << "F | " << info.fileName();
        cout << "\n";

        if (action == "1") {
            encrypt(info.absoluteFilePath());
        } else {
            decrypt(info.absoluteFilePath());
        }

    }

    while (itDirs.hasNext()) {
        itDirs.next();

        if (itDirs.fileName() == "." || itDirs.fileName() == "..") continue;

        QFileInfo info = itDirs.fileInfo();
        cout << tab << "D | " << info.fileName();
        cout << Qt::endl;

        QString nextPath = info.filePath();
        folderTraverse(nextPath, action, depth+1);
    }
}

QString getValidDir() {
    QString path;
    cout << "Enter path for folder: ";
    cout.flush();
    path = cin.readLine();

    QDir folder(path);

    while (!folder.exists() || isFolderSystem(path)) {
        if (isFolderSystem(path)) {
            cout << "No permission to encrypt system folders >:(" << Qt::endl;
        } else {
            cout << "Path is incorrect or there is no such path :(" << Qt::endl;
        }
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

    cout << "1 - for encryption" << Qt::endl;
    cout << "2 - for decryption" << Qt::endl;
    cout << "Select action(1 or 2): ";
    cout.flush();

    QString action = cin.readLine();

    while (action != "1" && action != "2") {
        cout << "Invalid choice! Please enter 1 or 2: ";
        cout.flush();
        action = cin.readLine();
    }

    folderTraverse(path, action);



    return 0;
    // /Users/liza/Desktop/Тестовая2/
    // /System/Library/Siri/DM/SiriSuggestions/Owners

    //return a.exec();
}

