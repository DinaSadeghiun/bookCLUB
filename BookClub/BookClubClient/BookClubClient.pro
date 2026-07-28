# Include necessary Qt modules (including widgets and QML/Quick)
QT += core gui quick qml widgets network pdf pdfwidgets quickcontrols2 quick quickcontrols2
#QT += pdf pdfwidgets

CONFIG += c++17

TARGET = BookClubClient
TEMPLATE = app

SOURCES += \
    main.cpp \
    networkmanager.cpp \
    usermanager.cpp

HEADERS += \
    networkmanager.h \
    usermanager.h

RESOURCES += \
    resources.qrc

DISTFILES += \
    AdminDashboard.qml \
    BookPage.qml \
    CartView.qml \
    Dashboard.qml \
    ForgotPassword.qml \
    GenreSelection.qml \
    HomeView.qml \
    LibraryView.qml \
    NotificationToast.qml \
    NotificationView.qml \
    PdfReader.qml \
    PublisherDashboard.qml \
    SearchView.qml \
    SettingsView.qml \
    main.qml \
    Login.qml \
    SignUp.qml

# Deployment rules
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
