#ifndef AESCRYPTTOOL_H
#define AESCRYPTTOOL_H

#include <QByteArray>
#include <QString>
#include "icrypttool.h"

class AesCryptTool : public ICryptTool
{
public:
    static AesCryptTool& Instance()
    {
        static AesCryptTool aesCrypter;
        return aesCrypter;
    }

    void encryptFolder(const QString& path, const QString& password) override;
    void decryptFolder(const QString& path, const QString& password) override;

private:
    AesCryptTool() = default;
    ~AesCryptTool() = default;

    AesCryptTool(AesCryptTool const&);
    AesCryptTool& operator= (AesCryptTool const&);

    QByteArray generateSalt(int size);

    bool isFileEncrypted(const QString& path);
    bool isFolderSystem(const QString& path);

    void encryptFile(const QString& path, const QString& password);
    void decryptFile(const QString& path, const QString& password);
};

#endif // AESCRYPTTOOL_H
