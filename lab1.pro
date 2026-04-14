QT = core

CONFIG += c++17 cmdline

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp \
        cryptfiledevice.cpp

HEADERS += \
        cryptfiledevice.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

#INCLUDEPATH += C:/Qt/Tools/mingw810_64/opt/include
#LIBS += -LC:/Qt/Tools/mingw810_64/opt/lib -lssl -lcrypto

INCLUDEPATH += /usr/local/opt/openssl@3/include
LIBS += -L/usr/local/opt/openssl@3/lib -lssl -lcrypto
