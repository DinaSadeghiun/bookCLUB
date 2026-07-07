QT += core gui widgets network


CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    #../Common/Models/book.cpp \
    #../Common/Models/Person.cpp \
    #../Common/Models/discount.cpp \
    #../Common/Models/shoppingcart.cpp \
    #../Common/Models/personallibrary.cpp \
    #../Common/Models/comment.cpp \
    #../Common/Models/publisher.cpp \
    #../Common/Models/order.cpp \
    #../Common/Models/notification.cpp \
    #../Common/Models/admin.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    #../Common/Models/book.h \
    #../Common/Models/Person.h \
    #../Common/Models/discount.h \
    #../Common/Models/shoppingcart.h \
    #../Common/Models/personallibrary.h \
    #../Common/Models/comment.h \
    #../Common/Models/publisher.h \
    #../Common/Models/order.h \
    #../Common/Models/notification.h \
    #../Common/Models/admin.h \
    mainwindow.h

FORMS += \
   # mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


include(../Common/Common.pri)

