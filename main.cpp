#include <QCoreApplication>
#include <QString>
#include <QTextStream>
#include <QDirIterator>

QTextStream cin(stdin);
QTextStream cout(stdout);

void folderTraverse(QDir& folder) {

}

QDir getValidDir() {
    QString path;
    cout << "Введите путь до папки для шифрования: ";
    cout.flush();
    path = cin.readLine();

    QDir folder(path);
    bool valid = false;

    while (!folder.exists()) {
        cout << "Путь введен некорректно или такого пути не существует :(" << Qt::endl;
        cout << "Попробуйте ещё раз: ";
        cout.flush();
        path = cin.readLine();
        folder = QDir(path);
    }

    cout << "Путь найден :)" << Qt::endl;

    return folder;
}

int main()
{
    QDir folder = getValidDir();
    folderTraverse(folder);

    return 0;
    // /Users/liza/Desktop/ТехнЧтение/
    //return a.exec();
}

