#include "aescrypttool.h"
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QTemporaryFile>
#include <QDebug>
#include <QRandomGenerator>
#include "cryptfiledevice.h"

void AesCryptTool::encryptFolder(const QString& path, const QString& password)
{
    QDirIterator it(path, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        it.next();

        if (it.fileInfo().isFile()) {
            encryptFile(it.filePath(), password);
        }
    }
}

void AesCryptTool::decryptFolder(const QString& path, const QString& password)
{
    QDirIterator it(path, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        it.next();

        if (it.fileInfo().isFile()) {
            decryptFile(it.filePath(), password);
        }
    }
}

QByteArray AesCryptTool::generateSalt(int size) {
    QByteArray data;
    while (data.size() < size) {
        data += char((QRandomGenerator::global()->generate()) % 256);
    }
    return data;
}

const QByteArray MARK = "ENCRYPTED";
const int SALT_SIZE = 16;

bool AesCryptTool::isFileEncrypted(const QString& path){
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    if (file.size() < MARK.size()) {
        file.close();
        return false;
    }

    QByteArray marker = file.read(MARK.size());
    file.close();

    return (marker == MARK);
}

bool AesCryptTool::isFolderSystem(const QString& path){
    QString lowerPath = path.toLower();

    if (lowerPath.startsWith("c:") || lowerPath.startsWith("c:/") || lowerPath.startsWith("c:\\") || lowerPath.startsWith("/system")) {
        return true;
    }
    return false;
}

void AesCryptTool::encryptFile(const QString& path, const QString& password){
    if (isFileEncrypted(path)) {
        qDebug() << "Already encrypted. Skipping " << path;
        return;
    }

    QFile toEncrypt(path);
    if (!toEncrypt.open(QIODevice::ReadOnly)) {
        qDebug() << "Error opening file to encrypt " << path;
        return;
    }

    if(toEncrypt.size() == 0) {
        qDebug() << "File is empty. Skipping" << path;
        toEncrypt.close();
        return;
    }

    QTemporaryFile tempFile;
    tempFile.open();
    QString tempPath = tempFile.fileName();
    tempFile.close();

    QByteArray salt = generateSalt(SALT_SIZE);

    QFile tempFileDevice(tempPath);
    CryptFileDevice cryptFileDevice(&tempFileDevice, password.toUtf8(), salt);

    if (!cryptFileDevice.open(QIODevice::WriteOnly)) {
        qDebug() << "Cannot open crypt device for:" << path;
        toEncrypt.close();
        return;
    }

    const qint64 BUFFER_SIZE = 8192;
    QByteArray buffer;

    while (!toEncrypt.atEnd()) {
        buffer = toEncrypt.read(BUFFER_SIZE);
        qint64 written = cryptFileDevice.write(buffer);
        if (written != buffer.size()) {
            qDebug() << "Error encrypting file " << path;
            toEncrypt.close();
            cryptFileDevice.close();
            return;
        }
    }

    toEncrypt.close();
    cryptFileDevice.close();

    tempFileDevice.open(QIODevice::ReadWrite);
    QByteArray encryptedData = tempFileDevice.readAll();
    tempFileDevice.close();

    tempFileDevice.open(QIODevice::WriteOnly);
    tempFileDevice.write(MARK);
    tempFileDevice.write(salt);
    tempFileDevice.write(encryptedData);
    tempFileDevice.close();

    QFile::remove(path);
    QFile::copy(tempPath, path);

    qDebug() << "Successfully encrypted:" << path;
}


void AesCryptTool::decryptFile(const QString& path, const QString& password){
    if (!isFileEncrypted(path)) {
        qDebug() << "Is not encrypted. Skipping" << path;
        return;
    }

    QFile toDecrypt(path);
    if (!toDecrypt.open(QIODevice::ReadOnly)) {
        qDebug() << "Error opening file to decrypt " << path;
        return;
    }

    QByteArray marker = toDecrypt.read(MARK.size());
    QByteArray salt = toDecrypt.read(SALT_SIZE);

    QByteArray encryptedData = toDecrypt.readAll();
    toDecrypt.close();

    QTemporaryFile tempFile;
    tempFile.open();
    tempFile.write(encryptedData);
    tempFile.close();

    CryptFileDevice cryptFileDevice(&tempFile, password.toUtf8(), salt);
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
