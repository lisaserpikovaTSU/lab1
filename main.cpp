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
#include "aescrypttool.h"

QTextStream cin(stdin);
QTextStream cout(stdout);

//Случай 13: Папка по переданному пути содержит ярлыки
//   Путь для проверки: /Users/liza/lab1/tests/shortcut

//Случай 14: Папка по переданному пути является системной
//   Путь для проверки: /usr/local/bin

//Случай 15: Переданный путь является путем программы
//   Путь для проверки: /Users/liza/lab1/

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QString path;
    cout << "Enter path for folder: ";
    cout.flush();
    path = cin.readLine();

    AesCryptTool& tool = AesCryptTool::Instance();

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

    QString password;
    cout << "Enter password: ";
    cout.flush();
    password = cin.readLine();

    if (action == "1") {
        cout << "Encrypting folder: " << path << Qt::endl;
        tool.encryptFolder(path, password);
    } else {
        cout << "Decrypting folder: " << path << Qt::endl;
        tool.decryptFolder(path, password);
    }

    return 0;
}
