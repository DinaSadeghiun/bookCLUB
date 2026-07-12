QT += core sql network
QT -= gui

CONFIG += console
CONFIG -= app_bundle

SOURCES += \
    DB/adminrepository.cpp \
    DB/bookrepository.cpp \
    DB/commentrepository.cpp \
    DB/databasemanager.cpp \
    DB/discountrepository.cpp \
    DB/notificationrepository.cpp \
    DB/orderrepository.cpp \
    DB/personallibraryreposiroty.cpp \
    DB/publisherrepository.cpp \
    DB/shoppingcartrepository.cpp \
    DB/userrepository.cpp \
    main.cpp

HEADERS += \
    DB/adminrepository.h \
    DB/bookrepository.h \
    DB/commentrepository.h \
    DB/databasemanager.h \
    DB/discountrepository.h \
    DB/notificationrepository.h \
    DB/orderrepository.h \
    DB/personallibraryreposiroty.h \
    DB/publisherrepository.h \
    DB/shoppingcartrepository.h \
    DB/userrepository.h

include(../Common/Common.pri)
