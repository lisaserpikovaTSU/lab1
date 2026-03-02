#include <QCoreApplication>
#include <QString>
#include <QTextStream>
#include <QDirIterator>
#include <QFileInfo>
#include <QDateTime>

QTextStream cin(stdin);
QTextStream cout(stdout);

void print_attribs(QFileInfo& info, QTextStream& stream) {
    if (info.isReadable())
        cout << "R";
    if (info.isWritable())
        cout << "W";
    if (info.isHidden())
        cout << "H";
    if (info.isExecutable())
        cout << "E";
}

QString fileSize(qint64 nSize) {
    qint64 i = 0;
    for (; nSize > 1023; nSize /= 1024, ++i) { }
    return QString().setNum(nSize) + "BKMGT"[i];
}

void folderTraverse(QString& path) {
    QDirIterator itDirs(path, QDir::Dirs);
    while (itDirs.hasNext()) {
        itDirs.next();

        if (itDirs.fileName() == ".") continue;

        QFileInfo info = itDirs.fileInfo();
        cout << "D | " << info.fileName() << " | " << fileSize(info.size())
             << " | " << info.lastModified().toString() << " | ";
        print_attribs(info, cout);
        cout << Qt::endl;
    }

    QDirIterator itFiles(path, QDir::Files);
    while (itFiles.hasNext()) {
        itFiles.next();
        QFileInfo info = itFiles.fileInfo();
        cout << "F | " << info.fileName() << " | " << fileSize(info.size()) << " | "
               << info.lastModified().toString() << " | ";
        print_attribs(info, cout);
        cout << "\n";
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

int main()
{
    QString path = getValidDir();
    folderTraverse(path);

    return 0;
    // /Users/liza/Desktop/Учебные/

    //return a.exec();
}

