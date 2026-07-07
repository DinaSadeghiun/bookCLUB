QT += core sql network
QT -= gui

CONFIG += c++17 console
CONFIG -= app_bundle

SOURCES += \
    DB/databasemanager.cpp \
    DB/publisherrepository.cpp \
    DB/userrepository.cpp \
    main.cpp

    main.cpp

HEADERS += \
    DB/databasemanager.h \
    DB/publisherrepository.h \
    DB/userrepository.h
    DB/userrepository.h


# بخش گرافیکی موقتاً کامنت شد تا ارورها رفع شوند
# HEADERS += mainwindow.h
# SOURCES += mainwindow.cpp
# FORMS += mainwindow.ui

include(../Common/Common.pri)
