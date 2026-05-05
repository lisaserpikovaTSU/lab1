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

//Тестирование функции ШИФРОВАНИЯ папки (encryptFolder)

//Входные данные - путь до папки, пароль

//Случай 1: Папки по пути не существует
//   Путь для проверки: /Users/liza/lab1/tests/nonexistingfolder

//Случай 2: Папка по переданному пути существует и пуста
//   Путь для проверки: /Users/liza/lab1/tests/emptyfolder

//Случай 3: Папка по переданному пути существует, содержит
//          подпапки и файлы
//   Путь для проверки: /Users/liza/lab1/tests/subdirsandfiles

//Случай 4: Папка содержит файлы разных типов
//   Путь для проверки: /Users/liza/lab1/tests/differentfiles

//Случай 5: Папка содержит пустые файлы
//   Путь для проверки: /Users/liza/lab1/tests/emptyfiles

//Случай 6: Папка является системной
//   Путь для проверки: /System/Library/Siri/DM/SiriSuggestions/Owners

//Случай 7: Папка содержит уже зашифрованные файлы
//   Путь для проверки: /Users/liza/lab1/tests/encrypted

//Случай 8: Пустой пароль
//   Путь для проверки: /Users/liza/lab1/tests/subdirsandfiles
//   Пароль: ""

//Тестирование функции ДЕШИФРОВАНИЯ папки (decryptFolder)

//Входные данные - путь до папки, пароль

//Случай 9: Папка содержит зашифрованные файлы, введен верный пароль
//   Путь для проверки: /Users/liza/lab1/tests/encrypted

//Случай 10: Папка содержит незашифрованные файлы
//   Путь для проверки: /Users/liza/lab1/tests/subdirsandfiles

//Случай 11: Неверный пароль
//   Путь для проверки: /Users/liza/lab1/tests/encrypted
//   Пароль: "wrongpassword"

//Случай 12: Пустой ввод пути
//   Путь для проверки: ""

int main()
{
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
        cout << "Decrypting folder: " << "/Users/liza/Desktop/Тестовая2/" << Qt::endl;
        tool.decryptFolder(path, password);
    }

    return 0;
}
