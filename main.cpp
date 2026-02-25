#include <QCoreApplication>
#include <QString>
#include <QTextStream>
#include <QDirIterator>

int main()
{
    QTextStream cin(stdin);
    QTextStream cout(stdout);

    QString root_inp;
    cout << "Введите путь до папки для шифрования: ";
    cout.flush();
    root_inp = cin.readLine();

    if (QDir(root_inp).exists()) {
        cout << "Путь найден :)";
        //обойти папку
    } else {
        cout << "Путь введен некорректно или такого пути не существует :(";
    }

    return 0;
    //return a.exec();
}

