QT += core gui network quick qml


CONFIG += c++17

win64-msvc* {
    QMAKE_CXXFLAGS -= -permissive-

    QMAKE_CXXFLAGS += /permissive
}

SOURCES +=

HEADERS +=

FORMS += \

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


include(../Common/Common.pri)

DISTFILES +=

RESOURCES +=

