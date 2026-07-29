#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "networkmanager.h"
#include <QQuickStyle>


int main(int argc, char *argv[])
{   QQuickStyle::setStyle("Basic");
    QApplication app(argc, argv);

    // 1. Create QML engine
    QQmlApplicationEngine engine;

    // 2. Create network manager
    NetworkManager* netManager = new NetworkManager(&app);

    // 3. Register networkManager in QML context
    engine.rootContext()->setContextProperty("networkManager", netManager);

    // 4. Load QML with error handling
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
                         if (!obj && url == objUrl)
                             QCoreApplication::exit(-1);
                     }, Qt::QueuedConnection);
    engine.load(url);

    // 5. Connect to server AFTER QML is loaded
    netManager->connectToServer("127.0.0.1", 8080); //


    return app.exec();
}