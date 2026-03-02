#include <QCoreApplication>
#include <QString>
#include <QTextStream>
#include <QDirIterator>

QTextStream cin(stdin);
QTextStream cout(stdout);

void folderTraverse(QDir& folder) {

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

int main()
{
    QString path = getValidDir();
    folderTraverse(path);

    return 0;
    // /Users/liza/Desktop/ТехнЧтение/
    //return a.exec();
}

