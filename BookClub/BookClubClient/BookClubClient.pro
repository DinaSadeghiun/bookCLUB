QT += core gui quick qml widgets

CONFIG += c++17

TARGET = BookClubClient
TEMPLATE = app

SOURCES += \
    main.cpp

RESOURCES += \
    resources.qrc


DISTFILES += \
    main.qml \
    Login.qml \
    SignUp.qml

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


