QT += core sql network
QT -= gui

CONFIG += c++17
CONFIG += console
CONFIG -= app_bundle

win64-msvc* {
    QMAKE_CXXFLAGS -= -permissive-
    QMAKE_CXXFLAGS -= /permissive-
    QMAKE_CXXFLAGS += /permissive
}

SOURCES += \
    DB/adminrepository.cpp \
    DB/bookrepository.cpp \
    DB/commentrepository.cpp \
    DB/databasemanager.cpp \
    DB/discountrepository.cpp \
    DB/notificationrepository.cpp \
    DB/orderrepository.cpp \
    DB/personallibraryrepository.cpp \
    DB/publisherrepository.cpp \
    DB/readingprogressrepository.cpp \
    DB/shoppingcartrepository.cpp \
    DB/userrepository.cpp \
    Network/bookclubserver.cpp \
    Network/clienthandler.cpp \
    Services/adminservice.cpp \
    Services/bookservice.cpp \
    Services/commentservice.cpp \
    Services/notificationservice.cpp \
    Services/orderservice.cpp \
    Services/personallibraryservice.cpp \
    Services/publisherservice.cpp \
    Services/shoppingcartservice.cpp \
    Services/userservice.cpp \
    main.cpp

HEADERS += \
    DB/adminrepository.h \
    DB/bookrepository.h \
    DB/commentrepository.h \
    DB/databasemanager.h \
    DB/discountrepository.h \
    DB/notificationrepository.h \
    DB/orderrepository.h \
    DB/personallibraryrepository.h \
    DB/publisherrepository.h \
    DB/readingprogressrepository.h \
    DB/shoppingcartrepository.h \
    DB/userrepository.h \
    Network/bookclubserver.h \
    Network/clienthandler.h \
    Services/adminservice.h \
    Services/bookservice.h \
    Services/commentservice.h \
    Services/notificationservice.h \
    Services/orderservice.h \
    Services/personallibraryservice.h \
    Services/publisherservice.h \
    Services/shoppingcartservice.h \
    Services/userservice.h

INCLUDEPATH += $$PWD \
               $$PWD/DB \
               $$PWD/Network \
               $$PWD/Services

include(../Common/Common.pri)

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
