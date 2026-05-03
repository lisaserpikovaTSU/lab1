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

int main()
{
    //QString path = getValidDir();
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

    if (action == "1") {
        //cout << "Encrypting folder: " << path << Qt::endl;
        tool.encryptFolder("/Users/liza/Desktop/Тестовая2/", "password");
        cout << "Encryption completed!" << Qt::endl;
    } else {
        //cout << "Decrypting folder: " << "/Users/liza/Desktop/Тестовая2/" << Qt::endl;
        tool.decryptFolder("/Users/liza/Desktop/Тестовая2/", "password");
        cout << "Decryption completed!" << Qt::endl;
    }

    return 0;
    // /Users/liza/Desktop/Тестовая2/
    // /System/Library/Siri/DM/SiriSuggestions/Owners

    //return a.exec();
}

