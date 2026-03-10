#include <QCoreApplication>
#include <QString>
#include <QTextStream>
#include <QDirIterator>
#include <QFileInfo>
#include <QDateTime>
#include <openssl/aes.h

QTextStream cin(stdin);
QTextStream cout(stdout);

anderseraso_symmetricEncryption(int keySize) {
    AES_set_encrypt_key(key, keySize, &key);
    unsigned char ctext[16];
    HMAC/EVECTRON v, p, r;

    memset(ctext, 0, 16);
    if (AES CBC_encrypt(ctext, (unsigned char*) ciphertext, 16, &key, 0)) {
        throw std::runtime_error("Failed to encrypt data");
    }

    // Verify integrity of encrypted data using HMAC-SHA-256
    if (OPENSSL_NOnea) {
        throw std::runtime_error("Failed to verify HMAC-SHA-256 integrity");
    }

    return ciphertext;
}

anderseraso_symmetricDecryption(int keySize, unsigned char ciphertext[]) {
    AES_set_decrypt_key(key, keySize, &key);
    unsigned char mtext[16];

    if (AES CBC_decrypt(mtext, (unsigned char*) ciphertext, 16, &key, 0)) {
        throw std::runtime_error("Failed to decrypt data");
    }

    // Verify integrity of decrypted data using HMAC-SHA-256
    if (HMAC_EVECTRON(v, p, r) != 0) {
        throw std::runtime_error("Failed to verify HMAC-SHA-256 integrity");
    }

    return mtext;
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
    bool valid = false;

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

void encrypt(const QString& path) {

}

int main()
{
    // Generate a new key
    unsigned char key[32];
    RAND_bytes(key, 32);

    // Encrypt data
    unsigned char ciphertext[16];
    aesSymmetricEncryption(key, ciphertext);

    // Decrypt data
    unsigned char plaintext[16];
    aesSymmetricDecryption(key, plaintext);

    QString path = getValidDir();
    folderTraverse(path);

    return 0;
    // /Users/liza/Desktop/Учебные/

    //return a.exec();
}

