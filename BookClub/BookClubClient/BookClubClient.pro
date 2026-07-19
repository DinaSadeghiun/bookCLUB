QT += core gui quick qml widgets
#QT += pdf pdfwidgets

CONFIG += c++17

TARGET = BookClubClient
TEMPLATE = app

SOURCES += \
    main.cpp \
    usermanager.cpp

RESOURCES += \
    resources.qrc


DISTFILES += \
    AdminDashboard.qml \
    BookCard.qml \
    CartView.qml \
    Dashboard.qml \
    ForgotPassword.qml \
    GenreSelection.qml \
    HomeView.qml \
    LibraryView.qml \
    SearchView.qml \
    SettingsView.qml \
    UserPanel.qml \
    main.qml \
    Login.qml \
    SignUp.qml

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    usermanager.h



