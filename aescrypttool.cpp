#include <QCoreApplication>
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
    QDir dir(path);

    if (dir.isEmpty()) {
        qDebug() << "Directory cannot be empty!";
        return;
    }

    if (!dir.exists()) {
        qDebug() << "Directory does not exist.";
        return;
    }

    if (isFolderSystem(path)) {
        qDebug() << "System folder cannot be encrypted.";
        return;
    }

    if (password.isEmpty()) {
        qDebug() << "Password cannot be empty!";
        return;
    }

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
    QDir dir(path);

    if (dir.isEmpty()) {
        qDebug() << "Directory cannot be empty!";
        return;
    }

    if (!dir.exists()) {
        qDebug() << "Directory does not exist.";
        return;
    }

    if (isFolderSystem(path)) {
        qDebug() << "System folder cannot be decrypted.";
        return;
    }

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

bool AesCryptTool::isFolderSystem(const QString& path) {
    QString lowerPath = path.toLower();
    QFileInfo pathInfo(path);
    QString canonicalPath = pathInfo.canonicalFilePath().toLower();

    // Windows
    if (canonicalPath.startsWith("c:/windows") ||
        canonicalPath.startsWith("c:\\windows") ||
        canonicalPath.startsWith("c:/program files") ||
        canonicalPath.startsWith("c:\\program files") ||
        canonicalPath.startsWith("c:/program files (x86)") ||
        canonicalPath.startsWith("c:\\program files (x86)") ||
        canonicalPath.startsWith("c:/programdata") ||
        canonicalPath.startsWith("c:\\programdata")) {
        return true;
    }

    // macOS
    if (canonicalPath.startsWith("/system") ||
        canonicalPath.startsWith("/library") ||
        canonicalPath.startsWith("/usr") ||
        canonicalPath.startsWith("/bin") ||
        canonicalPath.startsWith("/sbin") ||
        canonicalPath.startsWith("/private") ||
        canonicalPath.startsWith("/var") ||
        canonicalPath.startsWith("/tmp")) {
        return true;
    }

    // Linux/Unix
    if (canonicalPath == "/" ||
        canonicalPath.startsWith("/boot") ||
        canonicalPath.startsWith("/dev") ||
        canonicalPath.startsWith("/proc") ||
        canonicalPath.startsWith("/sys") ||
        canonicalPath.startsWith("/etc") ||
        canonicalPath.startsWith("/root") ||
        canonicalPath.startsWith("/lost+found")) {
        return true;
    }

    return false;
}

bool AesCryptTool::isProgramFile(const QString& path) {
    QFileInfo file(path);
    QString suffix = file.suffix().toLower();
    QString fileName = file.fileName().toLower();

    QStringList executableExtensions = {
        "exe", "dll", "so", "dylib", "app", "msi", "deb", "rpm",
        "jar", "py", "pyc", "pyo", "class", "bin", "sh", "bash",
        "zsh", "fish", "csh", "tcsh", "ksh", "rb", "pl", "php",
        "js", "ts", "go", "rs", "cpp", "c", "h", "hpp", "swift"
    };

    QStringList configFiles = {
        "makefile", "cmakelists.txt", "configure", "setup.py",
        "package.json", "cargo.toml", "pom.xml", "build.gradle",
        "dockerfile", "docker-compose.yml", "vagrantfile",
        "rakefile", "gruntfile.js", "gulpfile.js", "webpack.config.js"
    };

    if (executableExtensions.contains(suffix)) {
        QString canonicalPath = file.canonicalFilePath();
        if (canonicalPath.startsWith("/bin/") ||
            canonicalPath.startsWith("/usr/bin/") ||
            canonicalPath.startsWith("/usr/local/bin/") ||
            canonicalPath.startsWith("/opt/")) {
            return true;
        }
        return true;
    }

    if (configFiles.contains(fileName)) {
        return true;
    }

    QString executablePath = QCoreApplication::applicationFilePath();
    QFileInfo exeInfo(executablePath);
    QString exeDir = exeInfo.canonicalPath();

    QFileInfo currentFileInfo(path);
    QString currentCanonicalPath = currentFileInfo.canonicalFilePath();

    if (currentCanonicalPath.startsWith(exeDir)) {
        return true;
    }

    return false;
}

bool AesCryptTool::isShortcut(const QString &path){

    QFileInfo file(path);

    if (file.isSymLink()) {
        return true;
    }

    if (file.suffix().toLower() == "lnk"
        || fileInfo.suffix().toLower() == "url"
        || fileInfo.suffix().toLower() == "alias"
        || fileInfo.fileName().endsWith(".alias")
        || fileInfo.suffix().toLower() == "desktop")
    {
        return true;
    }

    return false;
}

void AesCryptTool::encryptFile(const QString& path, const QString& password){
    if (isProgramFile(path)) {
        qDebug() << "System file cannot be encrypted. Skipping " << path;
        return;
    }

    if (isShortcut(path)) {
        qDebug() << "Shortcut or symlink cannot be encrypted. Skipping " << path;
        return;
    }

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
    if (isProgramFile(path)) {
        qDebug() << "System file cannot be decrypted. Skipping " << path;
        return;
    }

    if (isShortcut(path)) {
        qDebug() << "Shortcut or symlink cannot be decrypted. Skipping " << path;
        return;
    }

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
    if (!cryptFileDevice.open(QIODevice::ReadOnly)) {
        qDebug() << "Password is incorrect. Decryption is not available!";
        return;
    }

    const qint64 BUFFER_SIZE = 8192;
    QByteArray decryptedData;
    QByteArray buffer;

    while (!cryptFileDevice.atEnd()) {
        buffer = cryptFileDevice.read(BUFFER_SIZE);
        decryptedData.append(buffer);
    }

    cryptFileDevice.close();

    if (decryptedData.isEmpty()) {
        qDebug() << "Decryption failed (corrupted file):" << path;
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
