#ifndef AESCRYPTTOOL_H
#define AESCRYPTTOOL_H

#include <QByteArray>
#include <QString>
#include "icrypttool.h"

//Класс для шифрования/дешифрования папок шифром AES
//Релизует интерфейс ICryptTool и использует паттерн Singleton
class AesCryptTool : public ICryptTool
{
public:

    //Статический создающий метод, который используется для получения одиночки
    static AesCryptTool& Instance()
    {
        //"ленивая инициализация", объект создаётся при первом вызове метода
        static AesCryptTool aesCrypter;    //статическая переменная, содержащая одиночный объект
        return aesCrypter;
    }

    //Функция для шифрования указанной папки
    //path - полный путь до необходимой папки
    void encryptFolder(const QString& path, const QString& password) override;
    //Функция для дешифрования указанной папки
    void decryptFolder(const QString& path, const QString& password) override;

private:

    //Приватные конструктор и деструктор класса
    AesCryptTool() = default;
    ~AesCryptTool() = default;

    //Запрет на копирование
    AesCryptTool(AesCryptTool const&);
    AesCryptTool& operator= (AesCryptTool const&);

    //Функция для генерации случайной соли
    QByteArray generateSalt(int size);

    //Проверка на зашифрованность файла
    bool isFileEncrypted(const QString& path);
    //Проверка, является ли папка системной
    bool isFolderSystem(const QString& path);

    //Проверка, является ли файл программным
    bool isProgramFile(const QString& path);
    //Проверка, является ли файл ярлыком или символической ссылкой
    bool isShortcut(const QString& path);

    //Функция шифрования файла
    void encryptFile(const QString& path, const QString& password);
    //Функция дешифрования файла
    void decryptFile(const QString& path, const QString& password);
};

#endif // AESCRYPTTOOL_H
