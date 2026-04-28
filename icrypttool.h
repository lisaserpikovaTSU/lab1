#ifndef ICRYPTTOOL_H
#define ICRYPTTOOL_H

#include <QString>

class ICryptTool {
public:
    virtual ~ICryptTool() = default;

    virtual void encryptFolder(const QString& folderPath, const QString& password) = 0;
    virtual void decryptFolder(const QString& folderPath, const QString& password) = 0;
};

#endif // ICRYPTTOOL_H
